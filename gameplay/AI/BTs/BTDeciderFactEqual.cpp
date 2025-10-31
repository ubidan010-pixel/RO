
#include "precompiled_gameplay.h"

#ifndef _ITF_BTDECIDERFACTEQUAL_H_
#include "gameplay/ai/BTs/BTDeciderFactEqual.h"
#endif // _ITF_BTDECIDERFACTEQUAL_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTDeciderFactEqual);

BTDeciderFactEqual::BTDeciderFactEqual()
{
}

BTDeciderFactEqual::~BTDeciderFactEqual()
{
}

bbool BTDeciderFactEqual::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getFact().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"fact",getTemplate()->getFact().isValid(),"%s doesn't specify an fact to remove",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( getTemplate()->getType() < EValueType_Boolean ||
         getTemplate()->getType() > EValueType_Vec3 )
    {
        ITF_WARNING_PARAM(_actor,"type",0,"%s doesn't specify a valid type of fact",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool BTDeciderFactEqual::decide( f32 _dt )
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

            bool fact;
            
            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    case EValueType_Integer32:
        {
            i32 factValue = atoi(getTemplate()->getValue().cStr());
            i32 fact;

            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    case EValueType_UInteger32:
        {
            u32 factValue = atol(getTemplate()->getValue().cStr());
            u32 fact;

            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    case EValueType_Float:
        {
            f32 factValue = static_cast<f32>(atof(getTemplate()->getValue().cStr()));
            f32 fact;

            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    case EValueType_StringId:
        {
            StringID factValue = getTemplate()->getValue();
            StringID fact;

            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    case EValueType_Vec2:
        {
            Vec2d factValue = Vec2d::Zero;

            if ( !getTemplate()->getValue().isEmpty() )
            {
                SSCANF_S(getTemplate()->getValue().cStr(),"%f %f",&factValue.m_x,&factValue.m_y);
            }

            Vec2d fact;

            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    case EValueType_Vec3:
        {
            Vec3d factValue = Vec3d::Zero;

            if ( !getTemplate()->getValue().isEmpty() )
            {
                SSCANF_S(getTemplate()->getValue().cStr(),"%f %f %f",&factValue.m_x,&factValue.m_y,&factValue.m_z);
            }

            Vec3d fact;

            if ( m_tree->getBlackBoard().getFact(getTemplate()->getFact(),fact) )
            {
                return fact == factValue;
            }
        }
        break;
    }

    return bfalse;
}

IMPLEMENT_OBJECT_RTTI(BTDeciderFactEqual_Template);

BEGIN_SERIALIZATION_CHILD(BTDeciderFactEqual_Template)

    SERIALIZE_MEMBER("fact",m_fact);
    SERIALIZE_MEMBER("value",m_value);
    SERIALIZE_FACTVALUETYPE("type",m_type);

END_SERIALIZATION()

}
