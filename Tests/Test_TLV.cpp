#include <TLV/TLVObject.h>
#include <JsonToTLV/Utils.h>
#include <gtest/gtest.h>

#include <fstream>


// Friendly fixture for access to TLVObject's private fields
class TLVTester : public ::testing::Test
{
public:
    using Bytes = std::vector<uint8_t>;

    TLVTester() { tlv2.m_bytes = bytes; }

    const Bytes& Tlv1Bytes() const         { return tlv1.m_bytes; }
    const Bytes& Tlv2Bytes() const         { return tlv2.m_bytes; }
    bool         Tlv1WriteLength(size_t l) { return tlv1.WriteLength(l); }

public:
    TLVObject   tlv1;                       // Empty
    TLVObject   tlv2;                       // Keeps bytes
    const Bytes bytes { 'T','E','S','T' };  // Emm... bytes!
};

// Fixture for JSON to TLV convertion test
class ConvertionTester : public testing::Test
{
public:
    using Bytes = std::vector<uint8_t>;

    std::string recordFileName = "record";
    std::string dictFileName = "dictionary";
    Bytes record;
    Bytes dict;
};


TEST_F(TLVTester, DefaultConstruction)
{
    EXPECT_TRUE(tlv1.Empty());
}

TEST_F(TLVTester, MoveConstructionAndAssignment)
{
    TLVObject tlv(std::move(tlv2));
    EXPECT_TRUE(tlv2.Empty());
    EXPECT_EQ(tlv.Size(), bytes.size());

    tlv2 = std::move(tlv);
    EXPECT_TRUE(tlv.Empty());
    EXPECT_EQ(Tlv2Bytes(), bytes);
}

TEST_F(TLVTester, CheckBytesAPI)
{
    EXPECT_EQ(tlv2.Size(), 4);
    tlv2.Clear();
    EXPECT_TRUE(tlv2.Empty());
    EXPECT_EQ(tlv2.Size(), 0);
}

TEST_F(TLVTester, WriteBool)
{
    EXPECT_TRUE( tlv1.WriteBool( true ));
    EXPECT_TRUE( tlv1.WriteBool( false ));

    Bytes expected
    {
        static_cast<uint8_t>(TLVObject::Tag::Bool_T),
        static_cast<uint8_t>(TLVObject::Tag::Bool_F)
    };
    EXPECT_EQ( Tlv1Bytes(), expected );
}

TEST_F(TLVTester, WriteLengthTooBig)
{
    EXPECT_FALSE( Tlv1WriteLength( 0x1000000 ));    // Next length value after Max
    EXPECT_TRUE( Tlv1Bytes().empty());
}

TEST_F(TLVTester, WriteLength7bit)
{
    EXPECT_TRUE(Tlv1WriteLength(0));                // Min length value is zero
    EXPECT_EQ(Tlv1Bytes(), Bytes{ 0x00 });
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0x52));             // Some abstract 7-bit length
    EXPECT_EQ(Tlv1Bytes(), Bytes{ 0x52 });
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0x7F));             // Max 7-bit length is 0x7F
    EXPECT_EQ(Tlv1Bytes(), Bytes{ 0x7F });
}

TEST_F(TLVTester, WriteLength1byte)
{
    Bytes expected;
    EXPECT_TRUE(Tlv1WriteLength(0x80));             // Min 1-byte length value is 0x80
    expected = { 0x81, 0x80 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0xA5));             // Some abstract 1-byte length
    expected = { 0x81, 0xA5 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0xFF));             // Max 1-byte length is 0xFF
    expected = { 0x81, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteLength2byte)
{
    Bytes expected;
    EXPECT_TRUE(Tlv1WriteLength(0x100));            // Min 2-byte length value is 0x100
    expected = { 0x82, 0x01, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0x51A5));           // Some abstract 2-byte length
    expected = { 0x82, 0x51, 0xA5 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0xFFFF));           // Max 2-byte length is 0xFFFF
    expected = { 0x82, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteLength3byte)
{
    Bytes expected;
    EXPECT_TRUE(Tlv1WriteLength(0x10000));          // Min 3-byte length value is 0x10000
    expected = { 0x83, 0x01, 0x00, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0x7F48B2));         // Some abstract 3-byte length
    expected = { 0x83, 0x7F, 0x48, 0xB2 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(Tlv1WriteLength(0xFFFFFF));         // Max 3-byte length is 0xFFFFFF
    expected = { 0x83, 0xFF, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteInteger8)
{
    Bytes expected;
    uint8_t tagS8 = static_cast<uint8_t>(TLVObject::Tag::Integer_S8);
    uint8_t tagU8 = static_cast<uint8_t>(TLVObject::Tag::Integer_U8);

    EXPECT_TRUE(tlv1.WriteInteger(int8_t( 0x80 )));     // Min S8 value is 0x80 (-128)
    expected = { tagS8, 0x80 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int8_t( 0xCA )));     // Some abstract S8 value (-54)
    expected = { tagS8, 0xCA };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int8_t( 0x7F )));     // Max S8 value is 0x7F (127)
    expected = { tagS8, 0x7F };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint8_t( 0x00 )));    // Min U8 value is 0
    expected = { tagU8, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint8_t( 0xEF )));    // Some abstract U8 value (239)
    expected = { tagU8, 0xEF };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint8_t( 0xFF )));    // Max U8 value is 0xFF (255)
    expected = { tagU8, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteInteger16)
{
    Bytes expected;
    uint8_t tagS16 = static_cast<uint8_t>(TLVObject::Tag::Integer_S16);
    uint8_t tagU16 = static_cast<uint8_t>(TLVObject::Tag::Integer_U16);

    EXPECT_TRUE(tlv1.WriteInteger(int16_t( 0x8000 )));  // Min S16 value is 0x8000 (-32768)
    expected = { tagS16, 0x80, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int16_t( 0xEAAB )));  // Some abstract S16 value (-5461)
    expected = { tagS16, 0xEA, 0xAB };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int16_t( 0x7FFF )));  // Max S16 value is 0x7FFF (32767)
    expected = { tagS16, 0x7F, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint16_t( 0x0000 ))); // Min U16 value is 0
    expected = { tagU16, 0x00, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint16_t( 0x7294 ))); // Some abstract U16 value (29332)
    expected = { tagU16, 0x72, 0x94 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint16_t( 0xFFFF ))); // Max U16 value is 0xFFFF (65535)
    expected = { tagU16, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteInteger32)
{
    Bytes expected;
    uint8_t tagS32 = static_cast<uint8_t>(TLVObject::Tag::Integer_S32);
    uint8_t tagU32 = static_cast<uint8_t>(TLVObject::Tag::Integer_U32);

    EXPECT_TRUE(tlv1.WriteInteger(int32_t(0x80000000)));    // Min S32 value is 0x80000000 (-2147483648)
    expected = { tagS32, 0x80, 0x00, 0x00, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int32_t(0xF1C71C72)));    // Some abstract S32 value (-238609294)
    expected = { tagS32, 0xF1, 0xC7, 0x1C, 0x72 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int32_t(0x7FFFFFFF)));    // Max S32 value is 0x7FFFFFFF (2147483647)
    expected = { tagS32, 0x7F, 0xFF, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint32_t(0x00000000)));   // Min U32 value is 0
    expected = { tagU32, 0x00, 0x00, 0x00, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint32_t(0x2B21C7FF)));   // Some abstract U32 value (723634175)
    expected = { tagU32, 0x2B, 0x21, 0xC7, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint32_t(0xFFFFFFFF)));   // Max U32 value is 0xFFFF (4294967295)
    expected = { tagU32, 0xFF, 0xFF, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteInteger64)
{
    Bytes expected;
    uint8_t tagS64 = static_cast<uint8_t>(TLVObject::Tag::Integer_S64);
    uint8_t tagU64 = static_cast<uint8_t>(TLVObject::Tag::Integer_U64);

    EXPECT_TRUE(tlv1.WriteInteger(int64_t(0x8000000000000000)));            // Min S64 value is 0x8000000000000000 (-9223372036854775808)
    expected = { tagS64, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int64_t(0xFDA12F684BDA12F7)));            // Some abstract S64 value (-170803185867681033)
    expected = { tagS64, 0xFD, 0xA1, 0x2F, 0x68, 0x4B, 0xDA, 0x12, 0xF7 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(int64_t(0x7FFFFFFFFFFFFFFF)));            // Max S64 value is 0x7FFFFFFFFFFFFFFF (9223372036854775807)
    expected = { tagS64, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint64_t(0x0000000000000000)));           // Min U64 value is 0
    expected = { tagU64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint64_t(0x101E573AC901E490)));           // Some abstract U64 value (1161461663900230800)
    expected = { tagU64, 0x10, 0x1E, 0x57, 0x3A, 0xC9, 0x01, 0xE4, 0x90 };
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    EXPECT_TRUE(tlv1.WriteInteger(uint64_t(0xFFFFFFFFFFFFFFFF)));           // Max U64 value is 0xFFFF (18446744073709551615)
    expected = { tagU64, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteEmptyString)
{
    Bytes expected = { static_cast<uint8_t>(TLVObject::Tag::String), 0x00 };
    EXPECT_TRUE(tlv1.WriteString(""));
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, WriteSomeString)
{
    Bytes expected;
    uint8_t tag = static_cast<uint8_t>(TLVObject::Tag::String);

    EXPECT_TRUE(tlv1.WriteString("Mein Herz Brennt"));
    expected = { tag, 0x10, 'M','e','i','n',' ','H','e','r','z',' ','B','r','e','n','n','t'};
    EXPECT_EQ(Tlv1Bytes(), expected);
    tlv1.Clear();

    const char* str = "Nun liebe Kinder gebt fein acht\nIch bin die Stimme aus dem Kissen\nIch hab euch etwas mitgebracht\nHab es aus meiner Brust gerissen" \
                      "Mit diesem Herz hab ich die Macht\nDie Augenlider zu erpressen\nIch singe bis der Tag erwacht\nEin heller Schein am Firmament" \
                      "Mein Herz brennt!!!";
    EXPECT_TRUE(tlv1.WriteString(str));
    expected = { tag, 0x82, 0x01, 0x0E };   // expected length is 270, it's representation in TLV is 0x82010E
    std::copy(str, str + strlen(str), std::back_inserter(expected));
    EXPECT_EQ(Tlv1Bytes(), expected);
}

TEST_F(TLVTester, DumpCheck)
{
    for (uint8_t i = 0; i < UINT8_MAX; ++i)
    {
        tlv1.WriteInteger(i);
    }
    EXPECT_TRUE(tlv1.Dump("binary"));

    std::ifstream in("binary", std::ios::binary | std::ios::in);
    ASSERT_TRUE(in.is_open());

    Bytes expected;
    std::copy(std::istreambuf_iterator<char>(in), {}, std::back_inserter(expected));
    in.close();
    std::remove("binary");

    EXPECT_EQ(Tlv1Bytes(), expected);
}


// Check Records and Dictionary for JSON with integers converted to TLV
TEST_F(ConvertionTester, ConvertIntegers)
{
    std::vector<std::string> jsonStrings = {
         "{\"i8_min\" :                -128}", "{\"i8_any\" :                -15}", "{\"u8_any\" :                 74}", "{\"u8_max\" :                 255}",
         "{\"i16_min\":              -32768}", "{\"i16_any\":             -16752}", "{\"u16_any\":              35864}", "{\"u16_max\":               65535}",
         "{\"i32_min\":         -2147483648}", "{\"i32_any\":         -147633681}", "{\"u32_any\":         2769364793}", "{\"u32_max\":          4294967295}",
         "{\"i64_min\":-9223372036854775808}", "{\"i64_any\":-223300436883615895}", "{\"u64_any\":9663751073709473064}", "{\"u64_max\":18446744073709551615}"
    };
    std::vector<Bytes> expectedRecords   = {
        {0x07,0x01,0x03,0x80},                {0x07,0x01,0x03,0xF1},                {0x07,0x01,0x07,0x4A},                {0x07,0x01,0x07,0xFF},
        {0x07,0x01,0x04,0x80,0x00},           {0x07,0x01,0x04,0xBE,0x90},           {0x07,0x01,0x08,0x8C,0x18},           {0x07,0x01,0x08,0xFF,0xFF},
        {0x07,0x01,0x05,0x80,0x00,0x00,0x00}, {0x07,0x01,0x05,0xF7,0x33,0x49,0xEF}, {0x07,0x01,0x09,0xA5,0x11,0x27,0x39}, {0x07,0x01,0x09,0xFF,0xFF,0xFF,0xFF},
        {0x07,0x01,0x06,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00},                   {0x07,0x01,0x06,0xFC,0xE6,0xAD,0x6F,0x8D,0x5C,0xCF,0x69},
        {0x07,0x01,0x0A,0x86,0x1C,0x8A,0x66,0x9A,0x40,0xBD,0x28},                   {0x07,0x01,0x0A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
    };
    std::vector<Bytes> expectedDicts     = {
        {0x0B,0x06,0x69,0x38,0x5F,0x6D,0x69,0x6E,0x07,0x01},      {0x0B,0x06,0x69,0x38,0x5F,0x61,0x6E,0x79,0x07,0x01},
        {0x0B,0x06,0x75,0x38,0x5F,0x61,0x6E,0x79,0x07,0x01},      {0x0B,0x06,0x75,0x38,0x5F,0x6D,0x61,0x78,0x07,0x01},
        {0x0B,0x07,0x69,0x31,0x36,0x5F,0x6D,0x69,0x6E,0x07,0x01}, {0x0B,0x07,0x69,0x31,0x36,0x5F,0x61,0x6E,0x79,0x07,0x01},
        {0x0B,0x07,0x75,0x31,0x36,0x5F,0x61,0x6E,0x79,0x07,0x01}, {0x0B,0x07,0x75,0x31,0x36,0x5F,0x6D,0x61,0x78,0x07,0x01},
        {0x0B,0x07,0x69,0x33,0x32,0x5F,0x6D,0x69,0x6E,0x07,0x01}, {0x0B,0x07,0x69,0x33,0x32,0x5F,0x61,0x6E,0x79,0x07,0x01},
        {0x0B,0x07,0x75,0x33,0x32,0x5F,0x61,0x6E,0x79,0x07,0x01}, {0x0B,0x07,0x75,0x33,0x32,0x5F,0x6D,0x61,0x78,0x07,0x01},
        {0x0B,0x07,0x69,0x36,0x34,0x5F,0x6D,0x69,0x6E,0x07,0x01}, {0x0B,0x07,0x69,0x36,0x34,0x5F,0x61,0x6E,0x79,0x07,0x01},
        {0x0B,0x07,0x75,0x36,0x34,0x5F,0x61,0x6E,0x79,0x07,0x01}, {0x0B,0x07,0x75,0x36,0x34,0x5F,0x6D,0x61,0x78,0x07,0x01}
    };

    for (int i = 0; i < jsonStrings.size(); ++i)
    {
        EXPECT_TRUE(ConvertToTLV(jsonStrings[i], recordFileName, dictFileName));
        std::ifstream recordFile(recordFileName, std::ios::binary | std::ios::in);
        std::ifstream dictFile(dictFileName, std::ios::binary | std::ios::in);
        ASSERT_TRUE(recordFile.is_open());
        ASSERT_TRUE(dictFile.is_open());
        std::copy(std::istreambuf_iterator<char>(recordFile), {}, std::back_inserter(record));
        std::copy(std::istreambuf_iterator<char>(dictFile), {}, std::back_inserter(dict));
        recordFile.close();
        dictFile.close();
        std::remove(recordFileName.c_str());
        std::remove(dictFileName.c_str());
        EXPECT_EQ(record, expectedRecords[i]);
        EXPECT_EQ(dict, expectedDicts[i]);
        record.clear();
        dict.clear();
    }
}