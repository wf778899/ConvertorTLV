#include "TLVObject.h"

#include <fstream>
#include <iostream>


TLVObject::TLVObject(TLVObject&& src) noexcept
    : m_bytes(std::move(src.m_bytes))
{}


TLVObject& TLVObject::operator=(TLVObject&& src) noexcept
{
    m_bytes = std::move(src.m_bytes);
    return *this;
}


bool TLVObject::WriteBool(bool val)
{
    m_bytes.push_back(static_cast<char>(val ? Tag::Bool_T : Tag::Bool_F));          // Tag (just tag - it's enough for boolean)
    return true;
}


bool TLVObject::WriteString(const std::string& str)
{
    m_bytes.push_back(static_cast<char>(Tag::String));

    if (!WriteLength(str.length()))
    {
        return false;
    }
    m_bytes.insert(m_bytes.end(), str.begin(), str.end());
    return true;
}


bool TLVObject::WriteLength(uint32_t length)
{
    if (length > s_lenLimit) {
        return false;
    }

    // If len is [0 ... 0x7F] - its value will be in the 1st octet as it
    if (length <= s_lenWidth_1Byte)
    {
        m_bytes.push_back(static_cast<char>(length));
    }
    // If len is [0x80 ... 0xFF] - then 1st octet indicates that length is stored in 2nd octet
    else if (length <= 0xFF)
    {
        m_bytes.push_back(static_cast<char>(s_lenWidth_2Byte));
        m_bytes.push_back(static_cast<char>(length));
    }
    // If len is [0x0100 ... 0xFFFF] - then 1st octet indicates that length is stored in 2nd and 3rd octets
    else if (length <= 0xFFFF)
    {
        m_bytes.push_back(static_cast<char>(s_lenWidth_3Byte));
        m_bytes.push_back(static_cast<char>(length >> 8));
        m_bytes.push_back(static_cast<char>(length & 0x00FF));
    }
    // If len is [0x010000 ... 0xFFFFFF] - then 1st octet indicates that length is stored in 2nd, 3rd and 4th octets
    else
    {
        m_bytes.push_back(static_cast<char>(s_lenWidth_4Byte));
        m_bytes.push_back(static_cast<char>(length >> 16));
        m_bytes.push_back(static_cast<char>((length >> 8) & 0x0000FF));
        m_bytes.push_back(static_cast<char>(length & 0x00FF));
    }
    return true;
}


bool TLVObject::Dump(const std::string& filePath)
{
    std::ofstream out(filePath, std::ios::out | std::ios::binary);

    if (!out.is_open()) {
        std::cout << "Unable to open the file for record" << std::endl;
        return false;
    }

    out.write(m_bytes.data(), m_bytes.size());
    out.close();
    return true;
}

void TLVObject::Clear()
{
    m_bytes.clear();
}