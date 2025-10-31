#include "precompiled_core.h"


#if ITF_UNITTEST > 0


namespace ITF
{

static void    fillU8 (char* _p, u32 _amount)
{
    u32 i;
    char value = 1;
    for (i = 0; i < _amount; i++)
    {
        if (value == 0)
            value++;
        _p[i] = value++;
    }
    _p[i-1] = 0;
}

static void    fillU16 (u16* _p, u32 _amount)
{
    u32 i;
    u16 value = 1;
    for (i = 0; i < _amount; i++)
    {
        if (value == 0)
            value++;
        _p[i] = value++;
    }
    _p[i-1] = 0;
}

static void StringUnitTest_testLength()
{
    ITF_ASSERT(String::getStringLength("12345678") == 8);

    char localc[27];
    fillU8(localc, 27);
    ITF_ASSERT(String::getStringLength(localc) == 26);

    u16 local[16];
    fillU16(local, 16);
    ITF_ASSERT(String::getStringLength(local) == 15);
}

static void StringUnitTest_testOperators()
{
    {
        String s;
        s += "12";
        ITF_ASSERT ( s == "12");

        char localc[27];
        fillU8(localc, 27);
        u16 local[16];
        fillU16(local, 16);

        s += localc;
        ITF_ASSERT(s.getLen() == 2 + 26);
        s += local;
        ITF_ASSERT(s.getLen() == 2 + 26 + 15);
    }

    { 
        String s  = "12";
        ITF_ASSERT (s == "12");
        ITF_ASSERT (s == String("12"));
        String o;
        o.setText("12");
        ITF_ASSERT (s == o);
    }
}

void String::unitTest()
{
    StringUnitTest_testLength();
    StringUnitTest_testOperators();
}

} // namespace ITF

#endif // ITF_UNITTEST > 0
