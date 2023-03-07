#include "Utils.h"
#include "TLVObject.h"

#include "json.hpp"

#include <iostream>

using namespace nlohmann::detail;
using namespace nlohmann;


/*  Converts one JSON line to appropriate binaries
 *
 *  'jsonString'        - valid JSON string
 *  'recordFileName'    - filepath the binary record will be generated to
 *  'dictFileName'      - filepath the binary dictionary will be generated to
 *_____________________________________________________________________________________________________________________________*/
bool ConvertToTLV(const std::string& jsonString, const std::string& recordFileName, const std::string& dictFileName)
{
    std::unordered_map<std::string, uint8_t> dict;  // {"key1":1, "qwe":2, "keyEE":3...}
    TLVObject tlv_dict, tlv_record;
    json j;
    uint8_t k = 1;                                   // This is to distinguish the keys inside one record (1:"qqq", 2:true, 3...)
    bool ok = true;

    try {
        j = json::parse(jsonString);
    }
    catch (const nlohmann::detail::exception& e) {
        std::cout << e.what();
        return false;
    }

    for (const auto& el : j.items())
    {
        dict[el.key()] = k;
        auto val = el.value();

        if (!(ok &= tlv_record.WriteInteger(k++))) {
            break;
        }
        switch (val.type()) {
            case value_t::boolean:              ok &= tlv_record.WriteBool(val.get<bool>());            break;
            case value_t::string:               ok &= tlv_record.WriteString(val.get<std::string>());   break;

            // Despite the JSON returns 64bit integers - we do narrow cast if possible to save tlv size
            case value_t::number_integer:
            {
                int64_t num = val.get<int64_t>();
                if      (num >= INT8_MIN)       ok &= tlv_record.WriteInteger(static_cast<int8_t>(num));
                else if (num >= INT16_MIN)      ok &= tlv_record.WriteInteger(static_cast<int16_t>(num));
                else if (num >= INT32_MIN)      ok &= tlv_record.WriteInteger(static_cast<int32_t>(num));
                else if (num >= INT64_MIN)      ok &= tlv_record.WriteInteger(static_cast<int64_t>(num));
                break;
            }
            case value_t::number_unsigned:
            {
                uint64_t num = val.get<uint64_t>();
                if      (num <= UINT8_MAX)      ok &= tlv_record.WriteInteger(static_cast<uint8_t>(num));
                else if (num <= UINT16_MAX)     ok &= tlv_record.WriteInteger(static_cast<uint16_t>(num));
                else if (num <= UINT32_MAX)     ok &= tlv_record.WriteInteger(static_cast<uint32_t>(num));
                else if (num <= UINT64_MAX)     ok &= tlv_record.WriteInteger(static_cast<uint64_t>(num));
                break;
            }
            default:
                ok &= false;
        }
        if (!ok) {
            break;
        }
    }
    if (ok && !dict.empty())
    {
        for (const auto& pair : dict)
        {
            ok &= tlv_dict.WriteString(pair.first);
            ok &= tlv_dict.WriteInteger(pair.second);
            if (!ok) {
                return false;
            }
        }
        tlv_record.Dump(recordFileName);
        tlv_dict.Dump(dictFileName);
        return true;
    }
    return false;
}