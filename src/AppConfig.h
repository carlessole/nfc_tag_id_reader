#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "yaml-cpp/yaml.h"

#if __cplusplus

struct MQTTConfig
{
    std::string server_address;
    std::string client_id;
};
namespace YAML
{
    template <>
    struct convert<MQTTConfig>
    {
        static bool decode(const Node &node, MQTTConfig &config)
        {
            if (!node["server_address"] || !node["client_id"])
                return false;

            config.server_address = node["server_address"].as<std::string>();
            config.client_id = node["client_id"].as<std::string>();
            return true;
        }
    };
}

struct NFCConfig
{
    bool debug;
    bool read_data;
};
namespace YAML
{
    template <>
    struct convert<NFCConfig>
    {
        static bool decode(const Node &node, NFCConfig &config)
        {
            if (!node["debug"] || !node["read_data"])
                return false;

            config.debug = node["debug"].as<bool>();
            config.read_data = node["read_data"].as<bool>();
            return true;
        }
    };
}

struct AppConfig
{
    std::string nfc_tag_topic;
    MQTTConfig mqtt;
    NFCConfig nfc;
};
namespace YAML
{
    template <>
    struct convert<AppConfig>
    {
        static bool decode(const Node &node, AppConfig &config)
        {
            if (!node["nfc_tag_topic"] || !node["mqtt"] || !node["nfc"])
                return false;

            config.nfc_tag_topic = node["nfc_tag_topic"].as<std::string>();
            config.mqtt = node["mqtt"].as<MQTTConfig>();
            config.nfc = node["nfc"].as<NFCConfig>();
            return true;
        }
    };
};

#endif

#endif // APPCONFIG_H