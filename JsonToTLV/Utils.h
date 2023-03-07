#include <string>


/*  Converts one JSON line to appropriate binaries
 *
 *  'jsonString'        - valid JSON string
 *  'recordFileName'    - filepath the binary record will be generated to
 *  'dictFileName'      - filepath the binary dictionary will be generated to
 *_____________________________________________________________________________________________________________________________*/
bool ConvertToTLV(const std::string& jsonString, const std::string& recordFileName, const std::string& dictFileName);