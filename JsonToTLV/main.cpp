#include <fstream>
#include <iostream>

#include "TLVObject.h"
#include "Utils.h"

/*  According to the task here  we expect to receive  the /path/to/json/file.txt.  Each JSON new line will be handled separately.
 *  Appropriate 'record_x' and 'dict_x' files will be generated for them,  where 'x' is the line number.   For example, this JSON
 *  {"key1":11,"key2":true}
 *  {"key3":11,"key4":true}         will be converted to binaries: 'record_0', 'record_1', 'dict_0', 'dict_1' files.
 *
 *  From example above: 'record_0' will be built from the source like "{1:11,2:true}", and 'dict_0' - from "{key1:1},{key2:2}".
 *_____________________________________________________________________________________________________________________________*/
int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Expected the name of the file with valid JSON to convert" << std::endl;
        return -1;
    }

    std::string jsonFileName = argv[1];

    std::ifstream input(jsonFileName, std::ios::in);
    if (!input.is_open())
    {
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }

    uint64_t record_number = 0;         // This is to distinguish the records/dictionaries (as much as many lines in JSON)
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