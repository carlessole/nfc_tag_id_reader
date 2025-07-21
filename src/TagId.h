#ifndef TAGID_H
#define TAGID_H

#include <string>

#include <nlohmann/json.hpp>

#if __cplusplus

struct TagId
{
    std::string type;
    std::string id;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TagId, type, id)
};

#endif // __cplusplus

#endif // TAGID_H