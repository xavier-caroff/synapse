///
/// @file NlohmannEnum.h
///
/// Declaration of the JSON_SERIALIZE_ENUM macro.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <stdexcept>

#include <nlohmann/json.hpp>

namespace nlohmann {

// clang-format off
/// Helper for JSON serialization of enum.
///
/// @param ENUM_TYPE The enumeration to be serialized.
/// @param ... The table of literal/string pairs.
///
/// This macro can be used as the NLOHMANN_JSON_SERIALIZE_ENUM macro but exception are thrown
/// when an entry is missing in the table (literal or string).
#define JSON_SERIALIZE_ENUM(ENUM_TYPE, ...)                                                     \
    template<typename BasicJsonType>                                                            \
    inline void to_json(BasicJsonType& j, const ENUM_TYPE& e)                                   \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [e](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool  \
        {                                                                                       \
            return ej_pair.first == e;                                                          \
        });                                                                                     \
        if (it == std::end(m))                                                                  \
        {                                                                                       \
            throw std::logic_error("literal missing in JSON_SERIALIZE_ENUM");                   \
        }                                                                                       \
        j = it->second;                                                                         \
    }                                                                                           \
    template<typename BasicJsonType>                                                            \
    inline void from_json(const BasicJsonType& j, ENUM_TYPE& e)                                 \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [&j](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool \
        {                                                                                       \
            return ej_pair.second == j;                                                         \
        });                                                                                     \
        if (it == std::end(m))                                                                  \
        {                                                                                       \
           throw std::runtime_error("unsupported enumeration literal");                         \
        }                                                                                       \
        e = it->first;                                                                          \
    }

// clang-format on

} // namespace nlohmann