#pragma once
#include <stdint.h>
#include <string>
#include <vector>

class TLVTester;

/*  Basic implementation of encoding the data to TLV format.  In current state supports standard types for encode:  bool, strings
 *  and 2- 4- 8-byte signed/unsigned integers. Encoding rules has some difference depending from the type to encode:
 *  -- For booleans: just 'Tag'  field is used  ('Length' and 'Value' are dropped),  because its enougth to have 2 kind of tags -
 *     Bool_T and Bool_F - to interpret logic true or false;
 *  -- For integers we're using different 'Tags' depending from the integer's  length, so that there is no necessary in a'Length'
 *     field - just 'Tag' and 'Value'.
 *  -- String uses all the TLV fields.
 *  As said earlier, for the standard types we have predefined Tags - see enum TLVObject::Tag.
 *
 *  TLV supports maximum value of the Length field: 0xFFFFFF; To encode the Length field we're using the next rules:
 *  -- If length is 7bit value (e.g. 0...127) - it will be written as one byte as it is (15 => 0x0F).
 *  -- If length is 1-byte value (e.g. 0x80...0xFF) - first byte will be 0x81, and next one - the length (0xE8 => 0x81, 0xE8).
 *  -- If length is 2-byte value (e.g. 0x0100...0xFFFF) - first byte will be 0x82, and two next ones - the length
 *     (0x75A2 => 0x82, 0x75, 0xA2)
 *  -- If length is 3-byte value (e.g. 0x010000...0xFFFFFF) - first byte will be 0x0x83, and three next bytes - the length
 *     (0x53A9C7 => 0x83, 0x53, 0xA9, 0xC7)
 */
class TLVObject
{
    friend class TLVTester;

    static const size_t  s_lenLimit;
    static const uint8_t s_lenWidth_1Byte;
    static const uint8_t s_lenWidth_2Byte;
    static const uint8_t s_lenWidth_3Byte;
    static const uint8_t s_lenWidth_4Byte;

public:
    // Predefined Tags for standard types
    enum class Tag : uint8_t {
        Bool_T = 1,
        Bool_F,
        Integer_S8,
        Integer_S16,
        Integer_S32,
        Integer_S64,
        Integer_U8,
        Integer_U16,
        Integer_U32,
        Integer_U64,
        String,
        Invalid
    };

public:
    TLVObject() = default;

    ~TLVObject() = default;

    TLVObject(const TLVObject&) = delete;

    TLVObject& operator=(const TLVObject&) = delete;

    TLVObject(TLVObject&& src) noexcept;

    TLVObject& operator=(TLVObject&& src) noexcept;

    /*  Encodes the boolean val */
    bool WriteBool(bool val);

    /*  Encodes the integer val. Supports signed/unsigned integers up to 8-byte size */
    template<class T>
    bool WriteInteger(T val);

    /*  Encodes the string str */
    bool WriteString(const std::string& str);

    /*  Dumps to the file 'filePath' the binary data encoded with 'Write*' calls */
    bool Dump(const std::string& filePath);

    /*  Clears internal binary buffer, returning the TLVObject to its initial state */
    void Clear()        { m_bytes.clear(); }

    /*  Checks whether TLV has the data encoded */
    bool Empty() const  { return m_bytes.empty(); }

    /*  Gets the size of encoded data */
    size_t Size() const { return m_bytes.size(); }

private:
    /*  Separate method to write the 'Length'  field where it's necessary  (for strings, for example).   Length is encoded by the
     *  rules are described above, in the class description */
    bool WriteLength(size_t length);

    std::vector<uint8_t> m_bytes;
};


/*  Encodes the integer. Supports signed/unsigned integers up to 8-byte size */
template<class T>
bool TLVObject::WriteInteger(T val)
{
    static_assert(std::is_integral<T>::value && "Supposed to be used with an integer types");

    constexpr bool isSigned = std::is_signed<T>::value;
    constexpr uint8_t length = sizeof(T);

    Tag tag;
    switch (length) {
        case 1:  tag = isSigned ? Tag::Integer_S8  : Tag::Integer_U8;  break;
        case 2:  tag = isSigned ? Tag::Integer_S16 : Tag::Integer_U16; break;
        case 4:  tag = isSigned ? Tag::Integer_S32 : Tag::Integer_U32; break;
        case 8:  tag = isSigned ? Tag::Integer_S64 : Tag::Integer_U64; break;
        default:
            return false;
    }
    m_bytes.push_back(static_cast<uint8_t>(tag));    // Such a tag represents both the type and length - so next field is a value

    // Big Endian is used to represent integers, e.g. for value '0xA58F2301' the 1st octet will be written is 'A5'
    for (uint8_t i = 1; i <= length; ++i)
    {
        uint8_t shift = 8 * (length - i);
        uint8_t byte = 0xFF & (val >> shift);
        m_bytes.push_back(byte);
    }
    return true;
}