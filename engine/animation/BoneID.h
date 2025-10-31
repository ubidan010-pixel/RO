#ifndef _ITF_BONEID_H_
#define _ITF_BONEID_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

namespace ITF
{
    class AnimLightComponent;

    class BoneID : public StringID
    {
    public:

        BoneID()
            : m_index(U32_INVALID) {}
        BoneID( const char* _str )
            : StringID(_str),  m_index(U32_INVALID) {}
        BoneID( const ITF_STDSTRING& _str )
            : StringID(_str),  m_index(U32_INVALID) {}
        BoneID( const String& _str )
            : StringID(_str),  m_index(U32_INVALID) {}
        BoneID( const String8& _str )
            : StringID(_str),  m_index(U32_INVALID) {}
        BoneID( const StringIdValueType _value )
            : StringID(_value),  m_index(U32_INVALID) {}


        void init(AnimLightComponent* _anim);

        ITF_INLINE u32 getIndex() const
        {
            ITF_ASSERT_MSG(StringID::isValid(), "the bone index is invalid!");
            return m_index;
        }

        ITF_INLINE bbool isValid() const
        {
            return StringID::isValid() && m_index != U32_INVALID;
        }

        void setFromStringID( const StringID& _id ) { m_index = _id.GetValue(); }

    private:

        u32 m_index;
    };
}

#endif //_ITF_BONEID_H_

