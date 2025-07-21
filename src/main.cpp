#include <iostream>

#include "NFCHandler.h"
#include "MQTTHandler.h"

MQTTHandler::Conf mqtt_config = {
    .server_address = "tcp://localhost:1883",
    .client_id = "nfc_tag_id_reader_cpp_client"};
MQTTHandler mqtt_handler(mqtt_config);

class CustomNFCHandlerHooks : public NFCHandlerHooks
{
public:
    bool publish_tag_id(const std::string &tag_id) override
    {
        bool ok = mqtt_handler.publish("selrac/nfc_handler/tags", tag_id);
        return ok;
    }
};
CustomNFCHandlerHooks custom_nfc_handler_hooks;

NFCHandler::Conf config = {.debug = true, .read_data = false};
NFCHandler nfc_handler(config, custom_nfc_handler_hooks);

int main()
{
    std::cout << "Let's read NFC tags!" << std::endl;

    mqtt_handler.init();

    nfc_handler.init();
    nfc_handler.tic();

    std::cout << "Exiting program." << std::endl;

    return 0;
}
