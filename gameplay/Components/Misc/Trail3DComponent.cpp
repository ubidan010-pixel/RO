#include "precompiled_gameplay.h"

#ifndef _ITF_TRAIL3DCOMPONENT_H_
#include "gameplay/components/misc/Trail3DComponent.h"
#endif //_ITF_TRAIL3DCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Trail3DComponent)


    BEGIN_SERIALIZATION_CHILD(Trail3DComponent)
    END_SERIALIZATION()



    Trail3DComponent::Trail3DComponent()
    : m_trailActive(btrue)
    , m_trailFaiding(bfalse)
    {
    }


    Trail3DComponent::~Trail3DComponent()
    {
        clear();
    }

    void Trail3DComponent::clear()
    {
        if (m_actor)
        {
            ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
            for (; trailIter != m_trailList.end(); ++trailIter)
            {
                ResourceID & res = (*trailIter).getTexture();
                m_actor->removeResource(res);
                res.invalidateResourceId();
            }
        }
    }

    bbool   Trail3DComponent::needsDraw() const 
    { 
        return !getTemplate()->getDraw2D(); 
    }
    bbool   Trail3DComponent::needsDraw2D() const 
    { 
        return getTemplate()->getDraw2D(); 
    }


    void Trail3DComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_trailActive = getTemplate()->getStartActive();
        m_attachedBoneList.resize(getTemplate()->getTrailList().size());
        m_trailList.resize(getTemplate()->getTrailList().size());
        ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
        for (u32 i=0; trailIter != m_trailList.end(); ++trailIter, ++i)
        {
            Trail & trail = (*trailIter);

            trail.setTemplate(&getTemplate()->getTrailList()[i]);
            trail.setIs2D(getTemplate()->getDraw2D());
            if(!trail.getTextureFile().isEmpty())
            {
                ResourceID res = m_actor->addResource(Resource::ResourceType_Texture, trail.getTextureFile());
                trail.setTexture(res);
                m_attachedBoneList[i] = BoneID(trail.getAttachedBone().GetValue());
            }
            trail.initData();
#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
#else
            trail.initMesh();
#endif
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrail,2898640431),this);

        clearDepth();
    }

    void Trail3DComponent::onBecomeActive()
    {
        if (m_initDepth)
        {
            m_depth = m_actor->getDepth();
            m_initDepth = bfalse;
        }
    }

    void Trail3DComponent::Update( f32 _deltaTime )
    {
        if (m_trailFaiding)
        {
            bbool stopUpdate = btrue;
            ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
            for (; trailIter != m_trailList.end(); ++trailIter)
                stopUpdate &= !(*trailIter).fade(_deltaTime);
            if (stopUpdate)
            {
                m_trailActive = bfalse;
                m_trailFaiding = bfalse;
                return;
            }
        }

        if (m_trailActive)
        {
            ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
            ITF_VECTOR<BoneID>::iterator boneIter = m_attachedBoneList.begin();
            for (; trailIter != m_trailList.end(); ++trailIter, ++boneIter)
            {
                Vec3d   pos;
                getAttachPos(pos, (*boneIter));
                
                (*trailIter).addPoint(pos, m_actor->getAngle());
            }
        }

            AABB aabb = m_actor->getAABB();
            ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
            for (; trailIter != m_trailList.end(); ++trailIter)
            {
                Trail & trail = (*trailIter);
                trail.setAlphaMultiplier(getAlpha());
                trail.update();
                if (m_trailActive || m_trailFaiding)
                    trail.updateAABB(aabb);
            }
            if (m_trailActive || m_trailFaiding)
            {
                m_actor->growRelativeAABBFromAbsolute(aabb);
            }
    }


    void Trail3DComponent::Draw()
    { 
        if (m_initDepth)
        {
            m_depth = m_actor->getDepth();
            m_initDepth = bfalse;
        }
        f32 epsilonDepth = 0.f;
        ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
        for (; trailIter != m_trailList.end(); ++trailIter, epsilonDepth+=MTH_EPSILON)
        {
            Trail & trail = (*trailIter);
#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
            GFX_ADAPTER->addPrimitiveInZList((void*)&trail, GFX_ITF_TRAIL, NULL, m_depth+epsilonDepth,0,m_actor->getRef());
#else
            if(trail.directDraw()) // with non shader only compute mesh
                GFX_ADAPTER->addPrimitiveInZList((void*)trail.getMesh(), GFX_ITF_MESH, NULL, m_depth+epsilonDepth,0,m_actor->getRef());
#endif
        }
    }


    void Trail3DComponent::Draw2D()
    { 
        if (m_initDepth)
        {
            m_depth = m_actor->getDepth();
            m_initDepth = bfalse;
        }
        f32 epsilonDepth = 0.f;
        ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
        for (; trailIter != m_trailList.end(); ++trailIter, epsilonDepth+=MTH_EPSILON)
        {
            Trail & trail = (*trailIter);
#if defined(ANIMUSESHADER) || defined(ANIMUSEGEOMETRYSHADER)
            GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, 0,GFX_ITF_TRAIL,&trail);
#else
            trail.directDraw(); // with non shader only compute mesh
            GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, 0,GFX_ITF_MESH,trail.getMesh());
            
#endif
        }
    }

    void Trail3DComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if (EventTrail* trailing = _event->DynamicCast<EventTrail>(ITF_GET_STRINGID_CRC(EventTrail,2898640431)))
        {
            if ( trailing->getUseSetColor() )
            {
                ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
                for (; trailIter != m_trailList.end(); ++trailIter)
                    (*trailIter).setColor(trailing->getSetColor());
            }

            if (trailing->getLaunch())
            {
                if ( !m_trailActive )
                {
                    m_trailActive   = btrue;
                    m_trailFaiding  = bfalse;
                    ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
                    for (; trailIter != m_trailList.end(); ++trailIter)
                        (*trailIter).clear();
                }
            }
            else
            {
                if ( trailing->getForceNoFade() )
                {
                    if ( m_trailActive || m_trailFaiding )
                    {
                        m_trailFaiding = bfalse;
                        m_trailActive = bfalse;
                        
                        ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
                        for (; trailIter != m_trailList.end(); ++trailIter)
                            (*trailIter).clear();
                    }
                }
                else
                {
                    if ( !m_trailFaiding && m_trailActive )
                    {
                        m_trailFaiding      = btrue;
                        ITF_VECTOR<Trail>::iterator trailIter = m_trailList.begin();
                        for (; trailIter != m_trailList.end(); ++trailIter)
                            (*trailIter).startFade();
                    }
                }
            }
        }
    }

    void Trail3DComponent::onResourceReady()
    {
        Super::onResourceReady();

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        if (m_animComponent)
        {
            ITF_VECTOR<BoneID>::iterator boneIter = m_attachedBoneList.begin();
            for (; boneIter != m_attachedBoneList.end(); ++boneIter)
                (*boneIter).init(m_animComponent);
        }
    }

    void Trail3DComponent::getAttachPos( Vec3d& _pos, BoneID & attachedBone )
    {
        if (m_animComponent && attachedBone.isValid())
        {
            m_animComponent->getBonePos(attachedBone.getIndex(), _pos);
        }
        else
        {
            _pos = m_actor->getPos();
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Trail3DComponent::onEditorMove(bbool _modifyInitialPos)
    {
        clearDepth();
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Trail3DComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Trail3DComponent_Template)

        SERIALIZE_CONTAINER_OBJECT("trailList", m_trailList);
        SERIALIZE_MEMBER("startActive", m_startActive);
        SERIALIZE_MEMBER("draw2D",m_draw2D); 

    END_SERIALIZATION()

    Trail3DComponent_Template::Trail3DComponent_Template()
    : m_startActive(btrue)
    , m_draw2D(bfalse)
    {
    }

}
