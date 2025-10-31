#ifndef _ITF_WAR_GAMEMATERIAL_H_
#define _ITF_WAR_GAMEMATERIAL_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

namespace ITF
{
    class War_GameMaterial : public GameMaterial
    {
    public:
        enum BounceDirection
        {
            BounceDirection_Opposite,
            BounceDirection_Same,
            BounceDirection_Left, 
            BounceDirection_Right,
            ENUM_FORCE_SIZE_32(0)
        };

                War_GameMaterial() : GameMaterial() {};
        bbool   getCanClimb() const {return testFloat(getCanClimbTag());}
        bbool   isSolidHanging() const {return testFloat(getSolidHangingTag());}
        u32     getDangerousLevel() const {return static_cast<u32>(getFloat(getDangerousTag()));}
        bbool   getBounceUseObjectNormal() const {return testFloat(getBounceUseObjectNormalTag());}
        f32     getBounceFactor() const {return getFloat(getBounceFactorTag());}
        BounceDirection getBounceDirection() const {return (BounceDirection)((i32)getFloat(getBounceDirectionTag()));}
        bbool   isClimbVertical() const { return testFloat(climbVerticalTag());}

        static u32 getSizeTag()  {return m_SizeTag.GetValue();}
        static u32 getBounceFactorTag() {return m_BounceFactorTag.GetValue();}
        static u32 getBounceDirectionTag() {return m_BounceDirectionTag.GetValue();}
        static u32 getDangerousTag() {return m_DangerousTag.GetValue();}
        static u32 getBounceUseObjectNormalTag()  {return m_BounceUseObjectNormalTag.GetValue();}
        static u32 getCanClimbTag() {return m_CanClimbTag.GetValue();}
        static u32 getSolidHangingTag() {return m_SolidHangingTag.GetValue();}
        static u32 climbVerticalTag() { return m_climbVerticalTag.GetValue(); }

    private:


        static StringID m_SizeTag;
        static StringID m_BounceFactorTag;
        static StringID m_BounceDirectionTag;
        static StringID m_DangerousTag;
        static StringID m_BounceUseObjectNormalTag;
        static StringID m_CanClimbTag;
        static StringID m_SolidHangingTag;
        static StringID m_climbVerticalTag;
    };

}

#endif //_ITF_WAR_GAMEMATERIAL_H_

