#include "TLVObject.h"

#include <gtest/gtest.h>


TEST(TLVObjectTests, Test_WriteBool)
{
    TLVObject tlv;
    auto expected = true;
    ASSERT_EQ( tlv.WriteBool(true), expected );
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}