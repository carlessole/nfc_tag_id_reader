#ifndef NFCHANDLER_H
#define NFCHANDLER_H

#include <vector>

#include <nfc/nfc.h>

#if __cplusplus

class NFCHandler
{
public:
    struct Conf
    {
        bool debug;
        bool read_data;
    };

public:
    // Default constructor
    NFCHandler() = default;

    // Parameterized constructor
    NFCHandler(const Conf &config) : _config(config) {}

    // Copy constructor
    NFCHandler(const NFCHandler &other) = delete; // Deleted to prevent copying
    // Copy assignment operator
    NFCHandler &operator=(const NFCHandler &other) = delete; // Deleted to prevent copying

    // Move constructor
    NFCHandler(NFCHandler &&other) noexcept = delete; // Deleted to prevent moving
    // Move assignment operator
    NFCHandler &operator=(NFCHandler &&other) noexcept = delete; // Deleted to prevent moving

    // Destructor
    ~NFCHandler() = default;

    // Example member function
    bool init();
    bool tic();

private:
    bool send_apdu(const nfc_target &target, const std::vector<uint8_t> &apdu, uint32_t timeout_ms,
                   std::vector<uint8_t> &response);
    void read_data_mifare_classic(const nfc_target &target);
    void read_data_nfc_type4(const nfc_target &target);

    nfc_context *_context = nullptr;
    nfc_device *_device = nullptr;

    Conf _config = {.debug = true, .read_data = true};
};

#endif // __cplusplus

#endif // NFCHANDLER_H