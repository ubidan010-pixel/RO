#include "precompiled_core.h"

#include "core/UUID.h"

//#include "core/AdaptersInterfaces/Adapter_SafeRandom.h"
#include "core/Seeder.h"


namespace ITF
{

    namespace
    {
        bool getCryptoRandomU32s(u32* _integers, size_t _nbIntegers)
        {
            //if (ITF_VERIFY_MSG(SAFERANDOM_ADAPTER != nullptr, "Safe random adapter not initialised"))
            //{
            //    Adapter_SafeRandom::ErrorCode safeRandomResult = SAFERANDOM_ADAPTER->generate(_nbIntegers * sizeof(u32), reinterpret_cast<u8*>(_integers));
            //    if (ITF_VERIFY(safeRandomResult == Adapter_SafeRandom::Error_Ok))
            //    {
            //        return true;
            //    }
            //}

            return false;
        }
    }


    // The nil UUID (all zeros)
    const UUID UUID::Nil;


    bool UUID::operator <(const UUID& _rhs) const
    {
        return (this->m_highPart < _rhs.m_highPart ||
                (this->m_highPart == _rhs.m_highPart && this->m_lowPart < _rhs.m_lowPart));
    }


    bool UUID::operator >(const UUID& _rhs) const
    {
        return (this->m_highPart > _rhs.m_highPart ||
                (this->m_highPart == _rhs.m_highPart && this->m_lowPart > _rhs.m_lowPart));
    }

    void UUID::makeRandom(bool _fastRandom)
    {
        u32 randomU32s [4];
        if (_fastRandom || !getCryptoRandomU32s(randomU32s, ITF_ARRAY_SIZE(randomU32s)))
        {
            Seeder& seeder = Seeder::getSharedSeeder();
            for (size_t integerCount = 0; integerCount < ITF_ARRAY_SIZE(randomU32s); integerCount++)
            {
                randomU32s[integerCount] = seeder.GetU32();
            }
        }

        // Set variant (see § 4.1.1 in RFC 4122)
        randomU32s[2] = ((randomU32s[2] & 0x3FFFFFFFU) | 0x80000000U);

        // Set version (see § 4.1.3 in RFC 4122)
        const u32 version = 4;  // = random UUID
        randomU32s[1] = ((randomU32s[1] & 0xFFFF0FFFU) | (version << 12));

        m_highPart = ((static_cast<u64>(randomU32s[0]) << 32) | randomU32s[1]);
        m_lowPart  = ((static_cast<u64>(randomU32s[2]) << 32) | randomU32s[3]);
    }


    UUID::UUID(const char* _uuid)
    {
        ITF_VERIFY(fromString(_uuid));
    }


    UUID::UUID(const String8& _uuidString)
    {
        ITF_VERIFY(fromString(_uuidString));
    }


    UUID UUID::operator =(const char* _rhs)
    {
        if (!(fromString(_rhs)))
        {
            makeNil();
        }

        return *this;
    }


    bool UUID::fromString(const char* _rhs)
    {
        if (_rhs != nullptr)
        {
            // If the UUID is between brackets, skip the 1st character
            if (_rhs[0] == '{')
            {
                _rhs++;
            }

            u32 part1 = 0;
            u16 part2 = 0;
            u16 part3 = 0;
            u16 part4 = 0;
            u16 part5_1 = 0;
            u32 part5_2 = 0;
            if (sscanf(_rhs, "%8x-%4hx-%4hx-%4hx-%4hx%8x", &part1, &part2, &part3, &part4, &part5_1, &part5_2) == 6)
            {
                m_highPart = ((static_cast<u64>(part1) << 32) | (static_cast<u64>(part2) << 16) | part3);
                m_lowPart = ((static_cast<u64>(part4) << 48) | (static_cast<u64>(part5_1) << 32) | part5_2);
                return true;
            }
        }

        return false;
    }


    String8 UUID::toString() const
    {
        u32 part1 = static_cast<u32>(m_highPart >> 32);
        u16 part2 = static_cast<u16>(m_highPart >> 16);
        u16 part3 = static_cast<u16>(m_highPart);
        u16 part4 = static_cast<u16>(m_lowPart >> 48);
        u16 part5_1 = static_cast<u16>(m_lowPart >> 32);
        u32 part5_2 = static_cast<u32>(m_lowPart);

        String8 strUuid;
        strUuid.setTextFormat("%08x-%04hx-%04hx-%04hx-%04hx%08x", part1, part2, part3, part4, part5_1, part5_2);

        return strUuid;
    }


}
