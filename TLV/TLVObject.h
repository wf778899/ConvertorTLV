#pragma once
#include <stdint.h>
#include <string>
#include <vector>

class TLVTester;

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

    TLVObject(const TLVObject&) = delete;

    TLVObject& operator=(const TLVObject&) = delete;

    TLVObject(TLVObject&& src) noexcept;

    TLVObject& operator=(TLVObject&& src) noexcept;

    bool WriteBool(bool val);

    template<class T>
    bool WriteInteger(T val);

    bool WriteString(const std::string& str);

    bool Dump(const std::string& filePath);

    void Clear()        { m_bytes.clear(); }

    bool Empty() const  { return m_bytes.empty(); }

    size_t Size() const { return m_bytes.size(); }

private:
    bool WriteLength(size_t length);

    std::vector<uint8_t> m_bytes;
};


template<class T>
bool TLVObject::WriteInteger(T val)
{
    static_assert(std::is_integral<T>::value && "Supposed to be used with an integer types");

    constexpr bool isSigned = std::is_signed<T>::value;
    constexpr uint8_t length = sizeof(T);

    Tag tag;
    switch (length) {
        case 1:  tag = isSigned ? Tag::Integer_S8 :  Tag::Integer_U8;  break;
        case 2:  tag = isSigned ? Tag::Integer_S16 : Tag::Integer_U16; break;
        case 4:  tag = isSigned ? Tag::Integer_S32 : Tag::Integer_U32; break;
        case 8:  tag = isSigned ? Tag::Integer_S64 : Tag::Integer_U64; break;
        default:
            return false;
    }
    m_bytes.push_back(static_cast<uint8_t>(tag));         // Such a tag represents both the type and length - so next field is a value

    // Big Endian is used to represent integers, e.g. for value '0xA58F2301' the 1st octet will be written is 'A5'
    for (uint8_t i = 1; i <= length; ++i)
    {
        uint8_t shift = 8 * (length - i);
        uint8_t byte = 0xFF & (val >> shift);
        m_bytes.push_back(byte);
    }
    return true;
}