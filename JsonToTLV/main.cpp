#include <fstream>
#include <iostream>

#include "TLVObject.h"
#include "Utils.h"


int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "RTFM!" << std::endl;
        return -1;
    }

    std::string jsonFileName = argv[1];

    std::ifstream input(jsonFileName, std::ios::in);
    if (!input.is_open())
    {
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }

    uint64_t record_number = 0;                     // This is to distinguish the records (as much as many lines in JSON)
    std::string line;
    while (std::getline(input, line))
    {
        std::string recordName = "record_" + std::to_string(record_number);
        std::string dictName = "dict_" + std::to_string(record_number);

        if (!ConvertToTLV(line, recordName, dictName))
            break;
        ++record_number;
    }
    input.close();
    return 0;
}