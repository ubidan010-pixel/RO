#include "precompiled_engine.h"

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(AnimatedComponent)
    
BEGIN_SERIALIZATION_CHILD(AnimatedComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AnimatedComponent)
    VALIDATE_COMPONENT_PARAM("", m_animTree.validate(), "Anim Tree has errors");
END_VALIDATE_COMPONENT()

const f32 AnimatedComponent::s_MagicBoxLockTimer = 0.1f;

AnimatedComponent::AnimatedComponent()
: Super()
, m_animInputUpdate(NULL)
, m_lockMagicBox(bfalse)
, m_lockMagicBoxPosFrom(Vec3d::Zero)
, m_lockMagicBoxPosTo(Vec3d::Zero)
, m_lockMagicBoxTimer(0.f)
, m_lockMagicBoxTotalTime(0.f)
, m_lockMagicBoxResetFrom(bfalse)
, m_magicBoxIndex(U32_INVALID)
, m_disableInputUpdate(bfalse)
{
}

//////////////////////////////////////////////////////////////////////////
AnimatedComponent::~AnimatedComponent()
    //////////////////////////////////////////////////////////////////////////
{
    clear();
}

void AnimatedComponent::clear()
{
    m_animTree.clear();
}

void AnimatedComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_inputList.resize(getTemplate()->getInputList().size());

    for ( u32 i = 0; i < m_inputList.size(); i++ )
    {
        Input* input = &m_inputList[i];
        const InputDesc& inputDesc = getTemplate()->getInputList()[i];

        input->setId(inputDesc.getId());
        input->setType(inputDesc.getType());

        if ( inputDesc.getType() == InputType_F32 )
        {
            input->setValue(0.f);
        }
        else
        {
            input->setValue(static_cast<u32>(0));
        }
    }

    if (!m_vertexs.size())
    {
        for (u32 i=0;i<4;i++)
        {
            Vertex v;
            m_vertexs.push_back(v);
        }
    }

    AnimTreeInitData initData;

    initData.m_inputs = &m_inputList;

    m_animTree.init(&initData,&getTemplate()->getAnimTreeTemplate());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903),this);
}

void AnimatedComponent::setFirstAnim()
{
    if (!m_action.isValid())
    {
        if ( !getTemplate()->getDefaultAnim().isValid() )
        {
            if ( m_animTree.getNumNodes() )
            {
                setAnim(m_animTree.getNodeID(0));
            }
            else if ( getTemplate()->getAnimTreeTemplate().getNodeList().size() )
            {
                setAnim(getTemplate()->getAnimTreeTemplate().getNodeList()[0]->getId());
            }
        }
        else
        {
            setAnim(getTemplate()->getDefaultAnim());
        }
    }
    else
    {
        setAnim(m_action);
    }
}

void AnimatedComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( EventReset* eventReset = _event->DynamicCast<EventReset>(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
    {
        resetLockMagicBox();
    }
    else if (EventSetFloatInput * onSetInput = _event->DynamicCast<EventSetFloatInput>(ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645)))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
    else if (EventSetUintInput * onSetInput = _event->DynamicCast<EventSetUintInput>(ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903)))
    {
        setInput(onSetInput->getInputName(),onSetInput->getInputValue());
    }
}

void AnimatedComponent::onResourceLoaded()
{
    Super::onResourceLoaded();

    m_magicBoxIndex = getBoneIndex(ITF_GET_STRINGID_CRC(B_SnapOn,18328722));

    AnimTreeLoadResource resData;

    resData.m_subAnimSet = &m_subAnimSet;

    m_animTree.onLoadResource(&resData);
}

bool AnimatedComponent::updateData()
{
    if ( m_animInputUpdate && m_currentDt && !m_disableInputUpdate )
    {
        m_animInputUpdate->updateAnimInput();
    }

    return Super::updateData();
}

void AnimatedComponent::resetTree()
{
    m_animTree.reset();
}

void AnimatedComponent::resetTransition()
{
    m_animTree.resetTransition();
}

bool AnimatedComponent::postUpdateData()
{
    if ( isLockMagicBox() )
    {
        updateLockMagicBox();
    }
    return btrue;
}

void AnimatedComponent::processAnim()
{
    if ( m_action.isValid() )
    {
        m_animTree.playNode(m_action);
        m_action.invalidate();
    }
}

f32 AnimatedComponent::getPrevBlending() const
{
    if ( m_AnimMeshScene )
    {
        const AnimInfo& animInfo = m_AnimMeshScene->m_AnimInfo;
        return animInfo.m_prevPercent;
    }
    else
    {
        return 0.f;
    }
}

void AnimatedComponent::updateAnimTime( f32 _dt )
{
    if (m_forceDirectUpdate)
    {
        Super::updateAnimTime( _dt );
        return;
    }

    StringID previousAnimsBuff[32];
    AnimsPlayingContainer previousAnims(sizeof(previousAnimsBuff)/sizeof(StringID), MemoryId::mId_Temporary, previousAnimsBuff);
    getResultSubAnimNames(*m_animTree.getResult(), previousAnims);

    if ( _dt )
    {
        m_animTree.update(_dt,m_inputList);
    }

    if ( m_animTree.getBlendFramesResult() != U32_INVALID )
    {
        setCurrentBlendFrames(m_animTree.getBlendFramesResult());
    }

    StringID currentAnimsBuff[32];    
    AnimsPlayingContainer currentAnims(sizeof(currentAnimsBuff)/sizeof(StringID), MemoryId::mId_Temporary, currentAnimsBuff);
    const AnimTreeResult* result = m_animTree.getResult();
    getResultSubAnimNames(*result, currentAnims);

    u32 numPrevAnims = previousAnims.size();
    for ( u32 i = 0; i < numPrevAnims; i++ )
    {
        const StringID& strId = previousAnims[i];

        if ( currentAnims.find(strId) == U32_INVALID )
        {
            EventAnimChanged onAnimChanged(strId);
            m_actor->onEvent(&onAnimChanged);
        }
    }

    u32 numAnims = result->m_anims.size();

    m_frameEvents.clear();

    resetSubAnimsToPlay();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimTreeResultLeaf & anim = result->m_anims[i];

        addSubAnimToPlay(*anim.m_subAnim, anim.m_weight, anim.m_usePatches);

        if ( anim.m_useEvents )
        {
            u32 numEvents = anim.m_events.size();
            #if defined(ITF_WINDOWS) && !defined(ITF_FINAL) 
                if (numEvents > MAX_EVENTPROCESSCONTAINER_EVENTS)
                {
                    ITF_WARNING(m_actor, bfalse, "too many events in animation");
                }
            #endif
            #ifndef ITF_FINAL
            if (numEvents > MAX_EVENTPROCESSCONTAINER_EVENTS)
                numEvents = MAX_EVENTPROCESSCONTAINER_EVENTS;
            #endif
            for ( u32 j = 0; j < numEvents; j++ )
            {
                m_frameEvents.push_back(anim.m_events[j]);
            }
        }
    }

    commitSubAnimsToPlay();
}

bool AnimatedComponent::getMagicBox(Vec3d & _pos, bbool _local )
{
    if (!isLoaded())
        return false;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (m_magicBoxIndex == U32_INVALID || boneDynList.size() == 0)
        return false;

    ITF_ASSERT(m_magicBoxIndex<boneDynList.size());
    Vec2d pos2d = boneDynList[m_magicBoxIndex].m_Pos;

    if ( _local )
    {
        if (m_rootIndex == U32_INVALID) return false;

        Vec2d root     = boneDynList[m_rootIndex].m_Pos;

        pos2d -= root;
    }

    _pos = pos2d.to3d(m_actor->getPos().m_z);

    return true;
}

bbool AnimatedComponent::getRoot( Vec3d& _pos ) const
{
    if ( m_rootIndex == U32_INVALID )
    {
        return bfalse;
    }

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    Vec2d root = boneDynList[m_rootIndex].m_Pos;

    _pos = root.to3d(m_actor->getPos().m_z);

    return btrue;
}

void AnimatedComponent::lockMagicBox( const Vec3d& _pos, f32 _interpolationTime )
{
    if ( !m_lockMagicBox )
    {
        m_lockMagicBox = btrue;
        m_lockMagicBoxTotalTime = m_lockMagicBoxTimer = _interpolationTime;
        resetLockMagicBoxFrom();
    }

    m_lockMagicBoxPosTo = _pos;
}

void AnimatedComponent::updateLockMagicBox()
{
    if ( m_lockMagicBoxResetFrom )
    {
        getMagicBox(m_lockMagicBoxPosFrom,bfalse);
        m_lockMagicBoxResetFrom = bfalse;
    }

    m_lockMagicBoxTimer = Max(m_lockMagicBoxTimer-m_currentDt,0.f);

    f32 t = 1.f - (m_lockMagicBoxTimer/m_lockMagicBoxTotalTime);

    t = Clamp(t,0.f,1.f);

    Vec3d destMagicBox = Interpolate(m_lockMagicBoxPosFrom,m_lockMagicBoxPosTo,t);

    Vec3d currentMagicBox;

    getMagicBox(currentMagicBox,bfalse);

    Vec3d dif = destMagicBox - currentMagicBox;

    m_actor->updateComponentPosAngle(m_actor->getPos()+dif,m_actor->getAngle(),this);

    getCurSubAnim(0)->getSkeleton()->Translate(&m_AnimMeshScene->m_AnimInfo.getCurrentBoneList(), dif.truncateTo2D(), m_AnimMeshScene->m_AnimInfo.m_globalData, !isVisiblePrediction() && !getDisableVisiblePrediction());
    m_AnimMeshScene->m_AnimInfo.m_Pos = getOffsettedPos();
}

u32 AnimatedComponent::findInputIndex( StringID _id ) const
{
    for ( u32 i = 0; i < m_inputList.size(); i++ )
    {
        const Input& input = m_inputList[i];

        if ( input.getId() == _id )
        {
            return i;
        }
    }

    return U32_INVALID;
}

void AnimatedComponent::getResultSubAnimNames(const AnimTreeResult & _result, AnimsPlayingContainer& _anims)
{
    u32 numAnims = _result.m_anims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimTreeResultLeaf & anim = _result.m_anims[i];
        _anims.push_back(anim.m_subAnim->getSubAnim()->getFriendlyName());
    }
}

void AnimatedComponent::dbgGetCurRule(String8 & _text)
{
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
    AnimTreeResult* res = m_animTree.getResult();
    u32 numNodes = res->m_playingNodes.size();

    for ( u32 i = 0; i < numNodes; i++ )
    {
        AnimTreeNode* node = res->m_playingNodes[i];

        String8 rulename;
        if (node->getId().isValid())
        {
            rulename = node->getId().getDebugString();
        }
        else
        {
            rulename = "unnamed";
        }
        rulename += " - ";
        _text = rulename + _text;
    }
#endif
}

void AnimatedComponent::resetCurTime()
{
    AnimTreeResult * result = m_animTree.getResult();
    if (result)
    {
        u32 numAnims = result->m_anims.size();

        for ( u32 i = 0; i < numAnims; i++ )
        {
            result->m_anims[i].m_subAnim->resetCurTime();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(AnimatedComponent_Template)
BEGIN_SERIALIZATION_CHILD(AnimatedComponent_Template)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
        SERIALIZE_OBJECT("tree",m_animTree);
    END_CONDITION_BLOCK()
END_SERIALIZATION()

AnimatedComponent_Template::AnimatedComponent_Template()
{
    m_useBase = btrue;
}

AnimatedComponent_Template::~AnimatedComponent_Template()
{
}

} //namespace ITF

