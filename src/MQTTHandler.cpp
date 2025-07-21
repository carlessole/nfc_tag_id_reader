#include <iostream>

#include "MQTTHandler.h"

// This code has been adapted to use the Paho MQTT C++ library.
// Reference example: https://github.com/eclipse-paho/paho.mqtt.cpp/blob/master/examples/async_publish.cpp

/// Public methods
MQTTHandler::~MQTTHandler()
{
    disconnect();
}

bool MQTTHandler::init()
{
    bool ok = true;

    if (!connect())
    {
        std::cerr << "Failed to connect to MQTT broker." << std::endl;
        ok = false;
    }

    return ok;
}

bool MQTTHandler::publish(std::string topic, std::string payload, int qos)
{
    bool ok = true;

    if (!_connected)
    {
        std::cerr << "Not connected to MQTT broker. Cannot publish message." << std::endl;
        return ok = false;
    }

    if (ok)
    {
        try
        {
            std::cout << "Publishing message: " << payload << std::endl;
            auto msg = mqtt::make_message(topic, payload);
            msg->set_qos(qos);
            _client.publish(msg)->wait();

            std::cout << "Message published successfully!" << std::endl;
        }
        catch (const mqtt::exception &e)
        {
            std::cerr << "MQTT Error: " << e.what() << std::endl;
            ok = false;
        }
    }

    return ok;
}

///_Public methods

/// Protected methods
///_Protected methods

/// Private methods

bool MQTTHandler::connect()
{
    bool ok = true;

    try
    {
        std::cout << "Connecting to MQTT broker..." << std::endl;
        _client.connect()->wait();
        std::cout << "Connected successfully!" << std::endl;
        _connected = true;
    }
    catch (const mqtt::exception &e)
    {
        std::cerr << "MQTT Error: " << e.what() << std::endl;
        ok = false;
    }

    return ok;
}

bool MQTTHandler::disconnect()
{
    bool ok = true;

    try
    {
        std::cout << "Disconnecting from MQTT broker..." << std::endl;
        _client.disconnect()->wait();
        std::cout << "Disconnected successfully!" << std::endl;
        _connected = false;
    }
    catch (const mqtt::exception &e)
    {
        std::cerr << "MQTT Error: " << e.what() << std::endl;
        ok = false;
    }

    return ok;
}
///_Private methods