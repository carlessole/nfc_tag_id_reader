#ifndef MQTTHANDLER_H
#define MQTTHANDLER_H

#include <mqtt/async_client.h>

#include <string>

#if __cplusplus

class MQTTHandler
{
public:
    struct Conf
    {
        std::string server_address;
        std::string client_id;
    };

public:
    // Parameterized constructor
    MQTTHandler(const Conf &config) : _config(config), _client(config.server_address, config.client_id), _connected(false) {};

    // Copy constructor
    MQTTHandler(const MQTTHandler &other) = delete; // Deleted to prevent copying
    // Copy assignment operator
    MQTTHandler &operator=(const MQTTHandler &other) = delete; // Deleted to prevent copying

    // Move constructor
    MQTTHandler(MQTTHandler &&other) noexcept = delete; // Deleted to prevent moving
    // Move assignment operator
    MQTTHandler &operator=(MQTTHandler &&other) noexcept = delete; // Deleted to prevent moving

    // Destructor
    virtual ~MQTTHandler();

    bool init();

    bool publish(std::string topic, std::string payload, int qos = 1);

private:
    bool connect();
    bool disconnect();

    Conf _config = {.server_address = "tcp://localhost:1883",
                    .client_id = "nfc_tag_id_reader_cpp_client"};

    mqtt::async_client _client;
    bool _connected = false;
};

#endif // __cplusplus

#endif // MQTTHANDLER_H