#include "precompiled_engine.h"

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_OBJBINDING_H_
#include "engine/actors/ObjBinding.h"
#endif //_ITF_OBJBINDING_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_


namespace ITF
{
    BEGIN_SERIALIZATION(ActorBind)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data | ESerialize_Editor)
            SERIALIZE_MEMBER("parentPath",          m_parent);
            SERIALIZE_MEMBER("offsetPos",           m_initialPositionOffset);
            SERIALIZE_MEMBER("offsetAngle",         m_initialAngleOffset);
            SERIALIZE_ENUM_BEGIN("type",            m_type);
                SERIALIZE_ENUM_VAR(Root);
                SERIALIZE_ENUM_VAR(BoneIndex);
                SERIALIZE_ENUM_VAR(BoneName);
            SERIALIZE_ENUM_END();
            SERIALIZE_MEMBER("typeData",            m_typeData);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable | ESerialize_Editor)
            SERIALIZE_MEMBER("useParentFlip",       m_useParentFlip);
            SERIALIZE_MEMBER("useParentScale",      m_useParentScale);
            SERIALIZE_MEMBER("removeWithParent",    m_removeWithParent);
        END_CONDITION_BLOCK()
        
        SERIALIZE_FUNCTION(postLoadInstance, ESerialize_Data_Load);
    END_SERIALIZATION()

    ActorBind::ActorBind()
    : m_parent()
    , m_useParentScale(bfalse)
    , m_useParentFlip(bfalse)
    , m_removeWithParent(bfalse)
    , m_type(Root)
    , m_typeData(0)
    , m_runtimeParent()
    , m_initialPositionOffset(Vec3d::Zero)
    , m_positionOffset(Vec3d::Zero)
    , m_initialAngleOffset(0.0f)
    , m_angleOffset(0.0f)
    , m_runtimeDisabled(bfalse)
    , m_temporaryBind( bfalse )
    {
    }

    ActorBindHandler::ActorBindHandler( )
    : m_owner(NULL)
    {
    }

    void ActorBindHandler::setOwner( Actor* _owner )
    {
        ITF_ASSERT_CRASH(_owner, "Owner can't be NULL");

        m_owner = _owner;
    }

    bbool ActorBindHandler::hasChild(Actor* _child) const
    {
        return (m_children.find(_child->getRef()) >= 0);
    }

    void ActorBindHandler::addChild( Actor* _child )
    {
        ObjectRef childRef = _child->getRef();

        ITF_ASSERT_MSG(Pickable::HotReloadType_None == _child->getHotReloadingStatus(), "please don't restore links when hot reloading");

        ITF_ASSERT_CRASH(!hasChild(_child), "Can't add twice the same child");
        // insert the child at the beginning or at the end of the list depending on its group priority
        u32 prio = _child->getObjectGroupIndex();
        if (prio == 0)
            m_children.insert(childRef, 0);
        else
        {
            ITF_ASSERT_MSG(ObjectGroup::GroupIndex_Max == 2, "only works for 2 priorities");
            m_children.push_back(childRef); // if more than 2 priorities, a scan if needed here
        }

        ActorBind* pChildBindToParent = _child->getParentBind();
        ITF_ASSERT_CRASH(pChildBindToParent, "Wrong child");
        pChildBindToParent->m_runtimeParent = m_owner->getRef();

        if(pChildBindToParent->m_useParentFlip)
            _child->setFlipped(m_owner->isFlipped());

        if(pChildBindToParent->m_useParentScale)
            _child->setScale(m_owner->getScale());

        // Add object bind to m_owner so children's update is called by parent
        m_owner->getBinding()->bindChild(childRef);

        updateWorldCoordinates(_child, pChildBindToParent);

        _child->setWorldInitialPos(_child->getPos());
        _child->setWorldInitialRot(_child->getAngle());
    }

    void ActorBindHandler::removeChild( Actor* _child )
    {
        ObjectRef childRef = _child->getRef();

        i32 index = m_children.find(childRef);
        ITF_ASSERT_CRASH(index >= 0, "Can't delete unknown child");
        m_children.eraseKeepOrder(index); // keep order because children are ordered by priority
                

        ITF_ASSERT_CRASH(_child->getParentBind(), "Wrong child");
        _child->getParentBind()->m_runtimeParent.invalidate();

        m_owner->getBinding()->unbindChild(childRef);
    }

    bbool convertBoneIndexToBoneName(Actor* _pOwner, ActorBind& _actorBind)
    {
        if(_actorBind.m_type == ActorBind::BoneIndex)
        {
            if ( AnimLightComponent* pAnimLightComponent = _pOwner->GetComponent<AnimLightComponent>() )
            {
                if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                {
                    if(AnimSkeleton *skeleton = pAnimLightComponent->getCurSubAnim(0)->getSkeleton())
                    {
                        if(AnimMeshScene* pMeshScene = pAnimLightComponent->getAnimMeshScene())
                        {
                            u32 index = _actorBind.m_typeData;
                            const SafeArray<AnimBoneDyn>& boneList = pMeshScene->m_AnimInfo.getCurrentBoneList();

                            if(index < boneList.size())
                            {
                                if(const AnimBone* pBone = skeleton->getBoneAt(index))
                                {
                                    _actorBind.m_type = ActorBind::BoneName;
                                    _actorBind.m_typeData = pBone->m_Name.GetValue();
                                    return btrue;
                                }
                            }
                        }
                    }
                }
            }
        }
        return bfalse;
    }

    void ActorBindHandler::update( f32 /*_dt*/ )
    {
        ITF_ASSERT_CRASH(m_owner, "Owner can't be NULL");

        AABB ownerAABB = m_owner->getAABB();

        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Actor* pGenericChild = static_cast<Actor*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(pGenericChild, "A child was deleted but not removed from parent bind list");

            ActorBind* pBind = pGenericChild->getParentBind();
            ITF_ASSERT_CRASH(pBind, "Invalid child, not bind information");
            
            if (pBind->isRuntimeDisabled())
            {
                continue;
            }

            if(pBind->m_useParentScale)
                pGenericChild->setScale(m_owner->getScale());

            if(pBind->m_useParentFlip)
                pGenericChild->setFlipped(m_owner->isFlipped());

            // This code could be removed when all maps saved before 20/04/2011 or version < 12956
            // are resaved after
            bbool updateCoord = btrue;
            if(pBind->m_type == ActorBind::BoneIndex)
                updateCoord = convertBoneIndexToBoneName(m_owner, *pBind);
            // End of convertion code

            if(updateCoord)
                updateWorldCoordinates(pGenericChild, pBind);

            ownerAABB.grow(pGenericChild->getAABB());
        }

        // Include children's AABB inside owner's AABB
        m_owner->changeRelativeAABBFromAbsolute(ownerAABB);
    }

    void ActorBindHandler::updateWorldCoordinates( Actor* _childActor, ActorBind* _childBind ) const
    {
        Vec3d pos;
        f32 angle;

        bbool bApply = computeWorldCoordinates(
            _childBind, _childBind->getPosOffset(), _childBind->getAngleOffset(),
            pos, angle);

        if(bApply)
        {
            _childActor->setAngle(angle);
            _childActor->setPos(pos);
        }
    }

    void ActorBindHandler::replaceReinitDataByCurrent()
    {
        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Actor* pGenericChild = static_cast<Actor*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(pGenericChild, "A child was deleted but not removed from parent bind list");

            pGenericChild->replaceReinitDataByCurrent();
        }
    }

    void ActorBindHandler::clear()
    {
        SafeArray<Actor*> children;
        for(u32 iChild = 0; iChild < m_children.size(); ++iChild)
        {
            if(Actor* pChild = static_cast<Actor*>(m_children[iChild].getObject()))
            {
                children.push_back(pChild);
            }
        }

        for(u32 iChild = 0; iChild < children.size(); ++iChild)
            removeChild(children[iChild]);

        m_children.clear();
    }

    void ActorBindHandler::requestChildrenDestruction()
    {
        const u32 uChildrenCount = m_children.size();
        for(u32 iChild = 0; iChild < uChildrenCount; ++iChild)
        {
            if(Actor* pChild = static_cast<Actor*>(m_children[iChild].getObject()))
            {
                if(ActorBind* pParentBind = pChild->getParentBind())
                {
                    if (!pParentBind->isRuntimeDisabled() &&
                        pParentBind->m_removeWithParent)
                    {
                        pChild->requestDestruction();
                    }
                }
            }
        }
    }

    bbool ActorBindHandler::getTransform( const ActorBind* _childBind, Vec3d& _pos, f32& _angle ) const
    {
        _pos = Vec3d::Zero;
        _angle = 0.f;


        switch(_childBind->m_type)
        {
        case ActorBind::Root:
            {
                _pos = m_owner->getPos();
                _angle = m_owner->getAngle();
                return btrue;
            }
            break;

        case ActorBind::BoneName:
            {
                if ( AnimLightComponent* pAnimLightComponent = m_owner->GetComponent<AnimLightComponent>() )
                {
                    if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                    {
                        const StringID boneName(_childBind->m_typeData);
                        i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                        if(idx != U32_INVALID)
                        {
                            if ( AnimBoneDyn* bone = pAnimLightComponent->getBone(idx) )
                            {
                                _pos = Vec3d(bone->m_Pos.m_x, bone->m_Pos.m_y, m_owner->getDepth());
                                _angle = bone->getAngle();
                                return btrue;
                            }
                        }
                    }
                }
            }
            break;

        default:
            ITF_ASSERT_MSG(0, "Not supported yet");
            break;
        }

        return bfalse;
    }

    void computeWorldAngle( f32 _angle, f32 _parentAngle, f32& _result )
    {
        _result = _parentAngle + _angle;
    }

    void computeWorldPosition( const Vec3d& _position, const Vec3d& _parentPos, f32 _parentAngle, const Vec2d& _parentScale, bbool _flip, Vec3d& _result )
    {
        _result = _position;
        if(_flip)
            _result.m_x = -_result.m_x;

        _result.m_x *= _parentScale.m_x;
        _result.m_y *= _parentScale.m_y;

        Vec3d_Rotate(&_result, _parentAngle);
        _result += _parentPos;
    }

    bbool ActorBindHandler::computeWorldCoordinates( const ActorBind* _childBind,
        const Vec3d& _localPos, const f32 _localAngle,  // input
        Vec3d& _worldPos, f32& _worldAngle ) const            // output
    {
        switch(_childBind->m_type)
        {
        case ActorBind::Root:
            {
                const bbool bFlip = _childBind->m_useParentFlip && m_owner->isFlipped();

                computeWorldAngle(_localAngle, m_owner->getAngle(), _worldAngle);
                computeWorldPosition( _localPos, m_owner->getPos(), m_owner->getAngle(), m_owner->getScale(), bFlip, _worldPos );

                return btrue;
            }
            break;

        case ActorBind::BoneName:
            {
                if ( AnimLightComponent* pAnimLightComponent = m_owner->GetComponent<AnimLightComponent>() )
                {
                    if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                    {
                        const StringID boneName(_childBind->m_typeData);
                        i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                        if(idx != U32_INVALID)
                        {
                            if ( AnimBoneDyn* bone = pAnimLightComponent->getBone(idx) )
                            {
                                const bbool bFlip   = _childBind->m_useParentFlip && m_owner->isFlipped();
                                const f32 fZ        = m_owner->getDepth();
                                Vec3d localPos = _localPos;
                                f32 localAngle = _localAngle;
                               
                                if ( bFlip )
                                {
                                    localPos = Vec3d( -_localPos.m_x, -_localPos.m_y, _localPos.m_z );
                                    localAngle = MTH_PI - localAngle;
                                }
                                computeWorldAngle( localAngle, bone->getAngle(), _worldAngle);
                                computeWorldPosition( localPos, bone->m_Pos.to3d(fZ), bone->getAngle(), m_owner->getScale(), bFlip, _worldPos);
                                return btrue;
                            }
                        }
                    }
                }

                return bfalse;
            }
            break;

        default:
            ITF_ASSERT_MSG(0, "Not supported yet");
            break;
        }

        return bfalse;
    }
    
    bbool ActorBindHandler::computeLocalCoordinates( const ActorBind* _childBind, const Vec3d& _childWorldPos, const f32 _childWorldAngle, Vec3d& _localPos, f32& _localAngle ) const
    {
        Vec3d parentWorldPos;
        f32 parentWorldAngle;
        if(getTransform( _childBind, parentWorldPos, parentWorldAngle ))
        {
            _localAngle = _childWorldAngle - parentWorldAngle;
            _localPos   = _childWorldPos - parentWorldPos;
            Vec3d_Rotate(&_localPos, -parentWorldAngle);
            _localPos.m_x /= m_owner->getScale().m_x;
            _localPos.m_y /= m_owner->getScale().m_y;
            
            if(_childBind->m_useParentFlip && m_owner->isFlipped())
                _localPos.m_x = -_localPos.m_x;

            return btrue;

// WIP 
#if 0
            switch(_childBind->m_type)
            {
            case ActorBind::Root:
                {
                    if(_childBind->m_useParentFlip && m_owner->isFlipped())
                        _localPos.m_x = -_localPos.m_x;

                    return btrue;
                }
                break;

            case ActorBind::BoneName:
                {
                    if ( AnimLightComponent* pAnimLightComponent = m_owner->GetComponent<AnimLightComponent>() )
                    {
                        if(pAnimLightComponent->isLoaded() && pAnimLightComponent->getNumPlayingSubAnims())
                        {
                            const StringID boneName(_childBind->m_typeData);
                            i32 idx = pAnimLightComponent->getBoneIndex(boneName);

                            if(idx != U32_INVALID)
                            {
                                if ( AnimBoneDyn* bone = pAnimLightComponent->getBone(idx) )
                                {
                                    if(_childBind->m_useParentFlip && pAnimLightComponent->getFlipped())
                                    {
                                        _localAngle = parentWorldAngle - _childWorldAngle;
                                        _localPos.m_x = -_localPos.m_x;
                                    }
                                    return btrue;
                                }
                            }
                        }
                    }

                }
                break;

            default:
                ITF_ASSERT_MSG(0, "Not supported yet");
                break;
            }
#endif // 0
        }
        return bfalse;
    }

#ifdef ITF_SUPPORT_EDITOR
    void ActorBindHandler::onEditorMove(bbool _modifyInitialPos)
    {
        update(0.0f);

        const u32 childCount = m_children.size();
        for(u32 iChild = 0; iChild < childCount; ++iChild)
        {
            Pickable* pGenericChild = static_cast<Pickable*>(m_children[iChild].getObject());
            ITF_ASSERT_CRASH(pGenericChild, "A child was deleted but not removed from parent bind list");

            pGenericChild->storeCurrentToInitial();
            // pGenericChild->onEditorMove(_modifyInitialPos); // it's called in ObjBinding
        }
    }
#endif // ITF_SUPPORT_EDITOR

} //namespace ITF