#include <iostream>
#include <fstream>
#include <unordered_map>

#include "json.hpp"

#include "TLVObject.h"

using namespace nlohmann::detail;
using namespace nlohmann;


int main()
{
    std::ifstream input("some.json", std::ios::in);
    if (!input.is_open()) {
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }
    std::unordered_map<std::string, uint8_t> dict;  // {"key1":1, "qwe":2, "keyEE":3...}
    TLVObject tlv_dict;
    TLVObject tlv_record;
    uint64_t record_number = 0;                     // This is to distinguish the records (as much as many lines in JSON)
    std::string line;
    json j;

    while (std::getline(input, line))
    {
        j = json::parse(line);
        uint8_t k = 1;                              // This is to distinguish the keys inside one record (1:"qqq", 2:true, 3...)
        bool ok = true;;

        for (const auto& el : j.items())
        {
            auto key = el.key();
            auto val = el.value();

            if (dict.insert({ key, k }).second) {
                k++;
            }
            ok &= tlv_record.WriteInteger(dict[key]);                                                                   //

            switch (val.type()) {
                case value_t::boolean:          if (ok) ok &= tlv_record.WriteBool(val.get<bool>());            break;  //
                case value_t::number_integer:   if (ok) ok &= tlv_record.WriteInteger(val.get<int64_t>());      break;  //
                case value_t::number_unsigned:  if (ok) ok &= tlv_record.WriteInteger(val.get<uint64_t>());     break;  //
                case value_t::string:           if (ok) ok &= tlv_record.WriteString(val.get<std::string>());   break;  //
                default:                        ok &= false;                                                            //
            }
            if (!ok) {
                std::cout << "Error in TLV writing for record. Element: " << el << std::endl;
                break;
            }
        }
        if (ok) {
            for (const auto& pair : dict)
            {
                ok &= tlv_dict.WriteString(pair.first);
                ok &= tlv_dict.WriteInteger(pair.second);

                if (!ok) {
                    std::cout << "Error in TLV writing for dictionary. Pair: [" << pair.first << ":" << pair.second << "]" << std::endl;
                    break;
                }
            }
            if (ok) ok &= tlv_record.Dump("Record_" + std::to_string(record_number));
            if (ok) ok &= tlv_dict.Dump("Dictionary_" + std::to_string(record_number));
        }
        tlv_record.Clear();
        tlv_dict.Clear();
        dict.clear();
        ++record_number;
    }
    input.close();
    return 0;
}