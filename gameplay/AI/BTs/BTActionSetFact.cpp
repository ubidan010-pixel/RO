
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONSETFACT_H_
#include "gameplay/ai/BTs/BTActionSetFact.h"
#endif // _ITF_BTACTIONSETFACT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionSetFact);

BTActionSetFact::BTActionSetFact()
{
}

bbool BTActionSetFact::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getFact().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"fact",getTemplate()->getFact().isValid(),"%s doesn't specify an fact to set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( getTemplate()->getType() < EValueType_Boolean &&
         getTemplate()->getType() > EValueType_Vec3 )
    {
        ITF_WARNING_PARAM(_actor,"type",0,"%s doesn't specify a valid type of fact",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void BTActionSetFact::onActivate()
{
    switch(getTemplate()->getType())
    {
        case EValueType_Boolean:
            {
                bool factValue = false;

                if ( !getTemplate()->getValue().isEmpty() )
                {
                    factValue = getTemplate()->getValue().cStr()[0] != 0;
                }
                else
                {
                    factValue = false;
                }

                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
        case EValueType_Integer32:
            {
                i32 factValue = atoi(getTemplate()->getValue().cStr());
                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
        case EValueType_UInteger32:
            {
                u32 factValue = atol(getTemplate()->getValue().cStr());
                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
        case EValueType_Float:
            {
                f32 factValue = static_cast<f32>(atof(getTemplate()->getValue().cStr()));
                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
        case EValueType_StringId:
            {
                StringID factValue = getTemplate()->getValue();
                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
        case EValueType_Vec2:
            {
                Vec2d factValue = Vec2d::Zero;

                if ( !getTemplate()->getValue().isEmpty() )
                {
                    SSCANF_S(getTemplate()->getValue().cStr(),"%f %f",&factValue.m_x,&factValue.m_y);
                }

                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
        case EValueType_Vec3:
            {
                Vec3d factValue = Vec3d::Zero;

                if ( !getTemplate()->getValue().isEmpty() )
                {
                    SSCANF_S(getTemplate()->getValue().cStr(),"%f %f %f",&factValue.m_x,&factValue.m_y,&factValue.m_z);
                }

                m_tree->getBlackBoard().setFact(getTemplate()->getFact(),factValue);
            }
            break;
    }
}

bbool BTActionSetFact::update( f32 _dt )
{
    Super::update(_dt);

    return bfalse;
}

IMPLEMENT_OBJECT_RTTI(BTActionSetFact_Template);

BEGIN_SERIALIZATION_CHILD(BTActionSetFact_Template)

    SERIALIZE_MEMBER("fact",m_fact);
    SERIALIZE_MEMBER("value",m_value);
    SERIALIZE_FACTVALUETYPE("type",m_type);

END_SERIALIZATION()

BTActionSetFact_Template::BTActionSetFact_Template()
: m_type(EValueType_StringId)
{
}

}