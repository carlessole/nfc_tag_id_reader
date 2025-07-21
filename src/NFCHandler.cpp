#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "NFCHandler.h"

/// Public methods
bool NFCHandler::init()
{
    bool ok = true;

    nfc_init(&_context);
    if (!_context)
    {
        std::cerr << "Failed to initialize libnfc." << std::endl;
        ok = false;
    }

    if (ok)
    {
        _device = nfc_open(_context, nullptr);
        if (!_device)
        {
            nfc_exit(_context);
            std::cerr << "Could not open NFC device." << std::endl;
            ok = false;
        }
    }

    if (ok)
    {
        if (nfc_initiator_init(_device) < 0)
        {
            nfc_close(_device);
            nfc_exit(_context);
            std::cerr << "Could not set NFC device as initiator." << std::endl;
            ok = false;
        }
    }

    if (ok)
    {
        std::cout << "NFC device initialized using libnfc: " << nfc_device_get_name(_device) << std::endl;
    }

    return ok;
}

bool NFCHandler::tic()
{
    const nfc_modulation mod = {.nmt = NMT_ISO14443A, .nbr = NBR_106};

    std::string last_uid = "";
    std::chrono::steady_clock::time_point last_seen = std::chrono::steady_clock::now();

    while (true)
    {
        nfc_target target;
        int res = nfc_initiator_select_passive_target(_device, mod, nullptr, 0, &target);

        if (res > 0)
        {
            std::stringstream uid_str;
            for (size_t i = 0; i < target.nti.nai.szUidLen; ++i)
                uid_str << std::hex << std::setw(2) << std::setfill('0') << (int)target.nti.nai.abtUid[i];

            const std::string uid = uid_str.str();

            if (uid != last_uid)
            {
                last_uid = uid;
                last_seen = std::chrono::steady_clock::now();

                if (_config.debug)
                {
                    std::cout << "Detected tag UID: " << uid << std::endl;
                    // std::cout << "Publishing RFID token: " << everest::staging::helpers::redact(token) << std::endl;
                }

                _hooks.publish_tag_id(uid);

                if (_config.read_data)
                {
                    const uint8_t sak = target.nti.nai.btSak;
                    switch (sak)
                    {
                    case 0x08: // MIFARE Classic
                        std::cout << "Detected MIFARE Classic tag with NFCID: " << uid << std::endl;
                        read_data_mifare_classic(target);
                        break;

                    case 0x20: // DESFire or NFC Type 4
                        std::cout << "Tag appears to be NFC Type 4 (SEL_RES=0x20) with NFCID: " << uid << std::endl;
                        read_data_nfc_type4(target);
                        break;

                    default:
                        std::cout << "Unknown SEL_RES value: 0x" << std::hex << (int)sak << ". Skipping read." << std::endl;
                        break;
                    }
                }
            }
            else
            {
                // Optional: log tag still present (debug)
            }
        }
        else
        {
            // If tag was present but now is gone, reset
            if (!last_uid.empty() && std::chrono::steady_clock::now() - last_seen > std::chrono::seconds(1))
            {
                if (_config.debug)
                {
                    std::cout << "Tag removed" << std::endl;
                }
                last_uid.clear();
            }
        }

        // Very short sleep to avoid CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

///_Public methods

/// Protected methods
///_Protected methods

/// Private methods
bool NFCHandler::send_apdu(const nfc_target &target, const std::vector<uint8_t> &apdu, uint32_t timeout_ms,
                           std::vector<uint8_t> &response)
{
    bool valid = false;

    response.resize(256); // Allocate enough space for response

    int res =
        nfc_initiator_transceive_bytes(_device, apdu.data(), apdu.size(), response.data(), response.size(), timeout_ms);
    if (res < 0)
    {
        std::cerr << "APDU exchange failed" << std::endl;
        response.clear();
    }
    else
    {
        response.resize(res);
        valid = true;
    }

    return valid;
}

void NFCHandler::read_data_mifare_classic(const nfc_target &target)
{
    const uint8_t block = 0x04; // Ex: block 4 (sector 1)
    const std::vector<uint8_t> default_key_a = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    std::vector<uint8_t> response_buffer;

    // Format: [0x60 | block | key (6 bytes) | UID (4 bytes)]
    std::vector<uint8_t> auth_cmd = {0x60,
                                     block, // Authenticate with key A
                                     default_key_a[0],
                                     default_key_a[1],
                                     default_key_a[2],
                                     default_key_a[3],
                                     default_key_a[4],
                                     default_key_a[5]};

    // Add UID (4 bytes)
    for (int i = 0; i < 4; ++i)
        auth_cmd.push_back(target.nti.nai.abtUid[i]);

    bool auth_resp_valid = send_apdu(target, auth_cmd, 500, response_buffer);
    if (!auth_resp_valid)
    {
        std::cerr << "Authentication failed for MIFARE Classic block 0x04" << std::endl;
        return;
    }

    std::cout << "Authentication successful. Reading block 0x04" << std::endl;

    std::vector<uint8_t> read_cmd = {0x30, block}; // Read block 0x04
    bool read_resp_valid = send_apdu(target, read_cmd, 500, response_buffer);
    if (!read_resp_valid)
    {
        std::cerr << "Failed to read data from MIFARE block 0x04" << std::endl;
        return;
    }

    std::stringstream ss;
    ss << "Data from block 0x04: ";
    for (auto b : response_buffer)
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    std::cout << ss.str() << std::endl;
}

void NFCHandler::read_data_nfc_type4(const nfc_target &target)
{
    std::vector<uint8_t> response_buffer;

    bool resp_valid = send_apdu(target, {0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00},
                                500, response_buffer);
    if (response_buffer.size() < 2 || response_buffer[response_buffer.size() - 2] != 0x90 ||
        response_buffer[response_buffer.size() - 1] != 0x00)
    {
        std::cerr << "SELECT NDEF failed" << std::endl;
        return;
    }

    resp_valid = send_apdu(target, {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xE1, 0x03}, 500, response_buffer); // Select CC
    if (response_buffer.size() < 2 || response_buffer[response_buffer.size() - 2] != 0x90)
    {
        std::cerr << "SELECT CC file failed" << std::endl;
        return;
    }

    resp_valid = send_apdu(target, {0x00, 0xB0, 0x00, 0x00, 0x0F}, 500, response_buffer); // Read CC file
    if (response_buffer.size() < 15)
    {
        std::cerr << "CC file too short" << std::endl;
        return;
    }

    uint16_t ndef_file_id = (response_buffer[9] << 8) | response_buffer[10];
    uint16_t ndef_max_size = (response_buffer[11] << 8) | response_buffer[12];
    std::cout << "Found NDEF File ID: 0x" << std::hex << ndef_file_id << ", max size: " << std::dec << ndef_max_size << std::endl;

    resp_valid = send_apdu(target,
                           {0x00, 0xA4, 0x00, 0x0C, 0x02, static_cast<uint8_t>(ndef_file_id >> 8),
                            static_cast<uint8_t>(ndef_file_id & 0xFF)},
                           500, response_buffer);
    if (response_buffer.size() < 2 || response_buffer[response_buffer.size() - 2] != 0x90)
    {
        std::cerr << "SELECT NDEF file failed" << std::endl;
        return;
    }

    // Read NDEF length
    resp_valid = send_apdu(target, {0x00, 0xB0, 0x00, 0x00, 0x02}, 500, response_buffer);
    if (response_buffer.size() < 4)
    {
        std::cerr << "Failed to read NDEF length" << std::endl;
        return;
    }

    uint16_t ndef_len = (response_buffer[0] << 8) | response_buffer[1];
    std::cout << "NDEF content length: " << ndef_len << std::endl;

    // Read full NDEF content
    resp_valid = send_apdu(target, {0x00, 0xB0, 0x00, 0x02, static_cast<uint8_t>(ndef_len)}, 1000, response_buffer);
    if (response_buffer.empty())
    {
        std::cerr << "Failed to read NDEF content" << std::endl;
        return;
    }

    std::stringstream ss;
    ss << "NDEF content: ";
    for (const auto &b : response_buffer)
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    std::cout << ss.str() << std::endl;
}
///_Private methods
