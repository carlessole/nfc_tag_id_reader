#include <iostream>

#include "NFCHandler.h"
#include "MQTTHandler.h"

#include "AppConfig.h"
#include "TagId.h"

#include "yaml-cpp/yaml.h"

AppConfig _app_config;

std::unique_ptr<MQTTHandler> mqtt_handler = nullptr;

class CustomNFCHandlerHooks : public NFCHandlerHooks
{
public:
    bool publish_tag_id(const std::string &tag_id) override
    {
        bool ok = false;

        if (mqtt_handler != nullptr)
        {
            std::string payload;
            TagId tag_id_obj = {.type = "ISO14443A", .id = tag_id};
            nlohmann::json tag_id_json = tag_id_obj;
            payload = tag_id_json.dump();

            ok = mqtt_handler->publish(_app_config.nfc_tag_topic, payload);
        }

        return ok;
    }
};
CustomNFCHandlerHooks custom_nfc_handler_hooks;

std::unique_ptr<NFCHandler> nfc_handler = nullptr;

bool load_config()
{
    bool ok = true;

    YAML::Node config = YAML::LoadFile("config/config.yaml");
    try
    {
        _app_config = config.as<AppConfig>();

        std::cout << "App config loaded successfully" << std::endl;
    }
    catch (const YAML::Exception &e)
    {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        ok = false;
    }

    return ok;
}

int main()
{
    bool proceed = true;

    std::cout << "Let's read NFC tags!" << std::endl;

    if (!load_config())
    {
        std::cerr << "Failed to load configuration." << std::endl;
        proceed = false;
    }

    if (proceed)
    {
        MQTTHandler::Conf mqtt_config = {
            .server_address = _app_config.mqtt.server_address,
            .client_id = _app_config.mqtt.client_id};
        mqtt_handler = std::make_unique<MQTTHandler>(mqtt_config);

        NFCHandler::Conf nfc_config = {
            .debug = _app_config.nfc.debug,
            .read_data = _app_config.nfc.read_data};
        nfc_handler = std::make_unique<NFCHandler>(nfc_config, custom_nfc_handler_hooks);

        if (mqtt_handler == nullptr || nfc_handler == nullptr)
        {
            std::cerr << "Failed to create handlers." << std::endl;
            proceed = false;
        }
    }

    if (proceed)
    {
        mqtt_handler->init();

        nfc_handler->init();
        nfc_handler->tic();
    }

    std::cout << "Exiting program." << std::endl;

    return 0;
}
