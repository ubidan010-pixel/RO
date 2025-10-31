

#include "precompiled_engine.h"

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "../actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(FxBankComponent_Template)
    BEGIN_SERIALIZATION(FxBankComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("Fx",m_FxList);
        SERIALIZE_CONTAINER_OBJECT("inputs",m_inputList);
    END_SERIALIZATION()

    FxBankComponent_Template::FxBankComponent_Template()
    {
    }

    FxBankComponent_Template::~FxBankComponent_Template()
    {
        if (getActorTemplate())
        {
            for ( u32 i = 0 ; i < m_FxList.size() ; ++i )
            {
                m_FxList[i].removeFromResourceGroup(getActorTemplate()->getTemplateResourceGroup());
            }
        }
    }

	void FxBankComponent_Template::onTemplateLoaded()
	{
		Super::onTemplateLoaded();

        for ( u32 i = 0 ; i < m_FxList.size() ; ++i )
        {
            m_FxList[i].onTemplateLoaded();
            m_FxList[i].addToResourceGroup(getActorTemplate()->getTemplateResourceGroup());
        }
	}

    IMPLEMENT_OBJECT_RTTI(FxBankComponent)
    BEGIN_SERIALIZATION_CHILD(FxBankComponent)
    END_SERIALIZATION()
    
    FxBankComponent::FxBankComponent()
    : Super()
    , m_fxInputUpdate(NULL)
    , m_draw2D(bfalse)
    //, m_DBGActive(0)
    //, m_DBGRejected(0)
    //, m_DBGMaxReached(0)
    {
    }


    FxBankComponent::~FxBankComponent()
    {
        clear();
    }

    void FxBankComponent::computeDraw2D()
    {
        bbool has3D = bfalse;
        ITF_UNUSED(has3D);
        bbool has2D = bfalse;

        for (u32 i=0; i<m_FxList.size(); i++)
        {
            FxDescriptor &descriptor = m_FxList[i];
            if (descriptor.getTemplate()->getDraw2D())
                has2D = btrue;
            else
                has3D = btrue;
        }
        m_draw2D = bfalse;
        ITF_WARNING_CATEGORY(GPP,m_actor, !(has2D && has3D), "FX bank component has both 2D & 3D descriptors!");
        m_draw2D = has2D;
    }

    void FxBankComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Inputs
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

        //FX

        u32 i = 0;
        m_FxList.resize(getTemplate()->getFxList().size());

        FxBankComponent_Template::FxList::const_iterator itTemplate = getTemplate()->getFxList().begin();
        for (; itTemplate != getTemplate()->getFxList().end(); itTemplate++ )
        {
            FxDescriptor* newDescriptor = &m_FxList[i];
            newDescriptor->onActorLoaded(&(*itTemplate));

            FxMap::const_iterator itmap = m_Fx.find( (*itTemplate).GetName());
            if (itmap != m_Fx.end())
            {
                ITF_WARNING(NULL, NULL, "fx %s already defined in FXBankComponent (%s)",(*itTemplate).GetName().getDebugString(), m_actor->getUserFriendly().cStr());
            }
            else
            {
                m_Fx[(*itTemplate).GetName()] = i;
            }
            ++i;
        }

        //
        computeDraw2D();

        //Register this component for events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetFloatInput,808064645),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetUintInput,1614618903),this);
    }

    bbool FxBankComponent::keepAlive()
    {
        u32 size = m_instance.size();
        for (u32 i=0; i<size; i++)
        {
            if (m_instance[i].m_gen)
                return btrue;
        }
        return bfalse;
    }

	void FxBankComponent::onBecomeInactive()
	{
        u32 size = m_instance.size();
        for (u32 i = 0; i < size; i++)
        {
            FxInstance& pFxInstance = m_instance[i];
            if (pFxInstance.m_gen != NULL)
            {
                stopFx(i);
                clearInstance(i);
            }
        }

		FEEDBACKFX_MANAGER->releaseFxDescriptors(m_actor);
		m_feedbackFxs.clear();
	}

    void FxBankComponent::clearInstance(u32 _index)
    {
        m_instance[_index].clear();

        // DEBUG
        //m_DBGActive--;
    }

    void FxBankComponent::Update( f32 _deltaTime )
    {
        ITF_ASSERT(!m_pause);

        if ( m_fxInputUpdate )
        {
            m_fxInputUpdate->updateFxInput();
        }

        //const Layer* layer = LAYER_MANAGER->findLayer(GetActor()->getLayerID());
        u32 size = m_instance.size();
        for (u32 i = 0; i < size; i++)
        {
            FxInstance& pFxInstance = m_instance[i];
            if (pFxInstance.m_gen != NULL)
            {
                ITF_WARNING(m_actor, pFxInstance.m_gen->checkGeneratorValidity(), "generator: %s : '%s'", pFxInstance.m_descriptor->GetName().getDebugString(), StringToUTF8(pFxInstance.m_gen->getValidityMessage()).get());

                //pFxInstance.m_gen->applyAmbiantColor(layer->getAmbientLight().m_color);

                if (pFxInstance.m_state == FxInstance::InstanceState_Stopping && 
                    ((pFxInstance.m_gen->getParameters()->getGeneratorMode() == PARGEN_MODE_MANUAL) || !isPlaying(i)))
                {
                    clearInstance(i);
                    continue;
                }

                updateInstance(pFxInstance,_deltaTime);
            }
        }

        //String tmp;
        //tmp.setStringFormat("%ls : FX pool -> %d/%d",);
        //GFX_ADAPTER->drawDBGText(tmp,20,20,1.0f,0.0f,0.0f);
    }

    void FxBankComponent::setLifeTimeMultiplier(u32 _index, f32 _multiplier)
    {
        ITF_ASSERT(_index != U32_INVALID);

        if ( _index < m_instance.size() && m_instance[_index].m_gen != NULL)
        {
            m_instance[_index].m_gen->getParameters()->setLifeTimeMultiplier(_multiplier);
        }
    }

    void FxBankComponent::addLight(const LightInfo &_light)
    {
        u32 size = m_instance.size();
        for (u32 i = 0; i < size; i++)
        {
            if (m_instance[i].m_gen != NULL)
            {
                if (!getDisableLight())
					m_instance[i].m_gen->addlight(_light);
            }
        }
    }

    void FxBankComponent::Draw()
    {
        u32 size = m_instance.size();
        for (u32 i = 0; i < size; i++)
        {
            if (m_instance[i].m_gen != NULL)
            {
				m_instance[i].m_gen->getParameters()->setDisableLight(getDisableLight());
				m_instance[i].m_gen->setColorFactor(getColorFactor());
                m_instance[i].m_gen->setAlpha(getAlpha());
                m_instance[i].m_gen->setFog(getColorFog());
                m_instance[i].m_gen->setRenderInTarget(m_renderInTarget);
                m_instance[i].m_gen->render();
            }
        }
    }

    void FxBankComponent::Draw2D()
    {
        u32 size = m_instance.size();
        for (u32 i = 0; i < size; i++)
        {
            if (m_instance[i].m_gen != NULL)
            {
                m_instance[i].m_gen->setColorFactor(getColorFactor());
                m_instance[i].m_gen->setAlpha(getAlpha());
                m_instance[i].m_gen->setFog(getColorFog());
                m_instance[i].m_gen->setRenderInTarget(m_renderInTarget);
                m_instance[i].m_gen->render();
            }
        }
    }

    void FxBankComponent::clear()
    {
        for (FxList::iterator it = m_FxList.begin(); it != m_FxList.end(); ++it)
        {
            it->clear();
        }

        m_FxList.clear();
        m_Fx.clear();

        u32 size = m_instance.size();

        for (u32 i = 0; i < size; ++i)
        {
            clearInstance(i);
        }
        
        FEEDBACKFX_MANAGER->releaseFxDescriptors(m_actor);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    u32 FxBankComponent::playFx(StringID _name, u32 _boneIndex, bbool _useBoneOrientation, bbool _emitFromBase, bbool _useActorSpeed, bbool _useActorOrientation, f32 _lifeTimeMult,  const FxDescriptor_Template * _FxDesc )
    {
		FxDescriptor * desc = NULL;

		// A FX descriptor has been specified, then check feedback list first to get descriptor
		if ( _FxDesc )
		{
			ITF_MAP<StringID,FxDescriptor *>::const_iterator feedback_it = m_feedbackFxs.find(_name);

			// A slot has already been taken from FeedbackFXManager ?
			if ( feedback_it != m_feedbackFxs.end() )
			{
				desc = feedback_it->second;
			}
			// Ask a free sound descriptor slot to FeedbackFXManager, and get SoundDescriptor
			else
			{
				desc = FEEDBACKFX_MANAGER->getNewFxDescriptor( m_actor, _FxDesc );
				if ( desc )
				{
					m_feedbackFxs[_name] = desc;
				}
			}
		}
		else
		{
			FxMap::const_iterator it = m_Fx.find(_name);
			desc = it != m_Fx.end() ? &m_FxList[it->second] : NULL;
		}

        if (desc)
        {
            u32 i = 0;
            u32 size = m_instance.size();
            for (i = 0; i < size; ++i)
            {
                if (m_instance[i].m_gen == NULL)
                    break;
            }

            ITF_ASSERT(i<=size);

            if ( i== size && size <= MAX_NUM_FX_INSTANCES )
            {
                m_instance.push_back(FxInstance());
                size++;
            }

            if (i < size)
            {
                FxInstance & instance = m_instance[i];

                ITF_ParticleGenerator * gen = ITF_ParticleGenerator::getGenerator(instance.m_generatorIndexInPool);

                if (!gen)
                    return U32_INVALID;

                gen->init(&desc->getGen());
                gen->getParameters()->setTexture(desc->getResource());
                gen->getParameters()->setRenderPrio(gen->getParameters()->getRenderPrio());

                // Set instance play mode
                if ( _boneIndex != U32_INVALID )
                {
                    instance.m_playMode = FXPlayMode_AttachedToBone;
                }
                else if ( _emitFromBase )
                {
                   instance.m_playMode = FXPlayMode_ActorBase;
                }
                else
                {
                    instance.m_playMode = FXPlayMode_Actor;
                }


                instance.m_descriptor = desc;
                instance.m_gen = gen;
                instance.m_state = FxInstance::InstanceState_Playing;
                instance.m_emitFromBase = _emitFromBase;
                instance.m_useActorSpeed = _useActorSpeed;
                instance.m_useActorOrientation = _useActorOrientation;
                instance.m_gen->getParameters()->setLifeTimeMultiplier(_lifeTimeMult);
                instance.m_boneIndex = _boneIndex;
                instance.m_useBoneOrientation = _useBoneOrientation;
                instance.m_timer = Seeder::getSharedSeeder().GetFloat(desc->getMinDelay(),desc->getMaxDelay());
                instance.m_name = _name;

                // DEBUG
                //m_DBGActive++;
                //m_DBGRejected = 0;
                setPosAndAngle(instance);

                return i;
            }
            // DEBUG
            //else
            //    m_DBGRejected++;
        }
        return U32_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void FxBankComponent::stopFx(u32 _index)
    {
        if (_index < m_instance.size())
        {
            if (m_instance[_index].m_gen != NULL && m_instance[_index].m_state != FxInstance::InstanceState_Unused)
            {
                m_instance[_index].m_state = FxInstance::InstanceState_Stopping;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    ITF_ParticleGenerator* FxBankComponent::getParticleGenerator(u32 _index)
    {
        if (_index < m_instance.size())
        {
            if (m_instance[_index].m_gen != NULL && m_instance[_index].m_state != FxInstance::InstanceState_Unused)
            {
                return m_instance[_index].m_gen;
            }
        }

        return NULL;
    }

     const StringID& FxBankComponent::getInstanceName(u32 _index)
    {
        if (_index < m_instance.size())
        {
            if (m_instance[_index].m_gen != NULL && m_instance[_index].m_state != FxInstance::InstanceState_Unused)
            {
                return m_instance[_index].m_name;
            }
        }

        return StringID::Invalid;
    }

    void FxBankComponent::setFXPos(u32 _index, const Vec3d& _pos)
    {
        if (_index < m_instance.size())
        {
            if (m_instance[_index].m_gen != NULL && m_instance[_index].m_state != FxInstance::InstanceState_Unused)
            {
                m_instance[_index].m_pos = _pos;
                m_instance[_index].m_playMode = FXPlayMode_Position;
            }
        }
    }

    void FxBankComponent::setFXAngle(u32 _index, f32 _angle)
    {
        if (_index < m_instance.size())
        {
            if (m_instance[_index].m_gen != NULL && m_instance[_index].m_state != FxInstance::InstanceState_Unused)
            {
                m_instance[_index].m_angle = _angle;
            }
        }
    }

    void FxBankComponent::attachToBone(u32 _index, u32 _boneIndex)
    {
        if (_index < m_instance.size())
        {
            if (m_instance[_index].m_gen != NULL && m_instance[_index].m_state != FxInstance::InstanceState_Unused)
            {
                m_instance[_index].m_boneIndex = _boneIndex;
                m_instance[_index].m_playMode = FXPlayMode_AttachedToBone;
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    bbool FxBankComponent::isPlaying(u32 _index) const
    {
        ITF_ASSERT(_index != U32_INVALID);
        ITF_ASSERT(_index < m_instance.size());
        //return SOUND_ADAPTER->isPlaying(m_instance[_index].m_handle);
        const FxInstance & instance = m_instance[_index];
        return instance.m_gen != NULL && (instance.m_gen->getNumGeneratedParticles() == 0 || instance.m_gen->getActive() || instance.m_timer != 0.0f);
    }

    void FxBankComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        u32 size = m_instance.size();
        if (_event->IsClassCRC(EventDie::GetClassCRCStatic()))
        {
            for (u32 i=0; i<size; i++)
            {
                stopFx(i);
            }
        }
        // Reset ?
        else if ( _event->IsClassCRC(EventReset::GetClassCRCStatic()) )
        {
            // Make sure all spawned particles are correctly removed
            for (u32 i = 0; i < size; ++i)
            {
                clearInstance(i);
            }
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

    void FxBankComponent::updateInstance( FxInstance& _instance, f32 _deltaTime )
    {
        if (_instance.m_timer == 0.0f)
        {
            setPosAndAngle(_instance);

            setInputs(_instance, _deltaTime);


            //UPDATE PARTICLE
            _instance.m_gen->update(_deltaTime, _instance.m_state == FxInstance::InstanceState_Stopping);
        }


        updateAABB(_instance);

        _instance.m_timer = Max(_instance.m_timer-_deltaTime,0.0f);
    }

    void FxBankComponent::setPosAndAngle( FxInstance &_instance )
    {
        f32 angle = m_actor->getAngle();
        AnimLightComponent* anim = GetActor()->GetComponent<AnimLightComponent>();

        bbool fliped =  m_actor->isFlipped();

        angle += _instance.m_descriptor->getAngleOffset();

        Vec3d pos;
        if (anim && (_instance.m_playMode == FXPlayMode_AttachedToBone) && (_instance.m_boneIndex != U32_INVALID))
        {
            pos = m_actor->getPos();
            AnimBoneDyn * bone = anim->getBone(_instance.m_boneIndex);
            //ITF_WARNING_CATEGORY(GPP,m_actor,bone,"Trying to play a VFX at bone index %d, but the animation is not ready or not even playing",_instance.m_boneIndex);
            if (bone != NULL)
            {
                pos = bone->m_Pos.to3d(m_actor->getDepth());

                if ( _instance.m_useBoneOrientation )
                {
                    f32 offsetAngle = _instance.m_descriptor->getAngleOffset();



                    f32 boneAngle = bone->getAngle(); //world angle

                    if (fliped && _instance.m_gen->getParameters()->canFlipAngleOffset())
                        boneAngle += offsetAngle;
                    else
                        boneAngle -= offsetAngle;

                    _instance.m_gen->getParameters()->setAngle(Angle(bfalse,boneAngle));

                    angle = boneAngle;
                }
            }
        }
        else if ( _instance.m_playMode == FXPlayMode_ActorBase)
        {
            pos = m_actor->getBase();
        }
        else if (_instance.m_playMode == FXPlayMode_Position )
        {
            pos = _instance.m_pos;
        }
        else
        {
            pos = m_actor->getPos();
        }

        pos.m_z = m_actor->getDepth();

        _instance.m_gen->getParameters()->setPosition(pos);

        if (_instance.m_useActorSpeed)
        {
            PhysComponent * phys = GetActor()->GetComponent<PhysComponent>();
            if (phys)
            {
                _instance.m_gen->getParameters()->setVelocityNorm(_instance.m_descriptor->getGen().getParameters().getVelocityNorm() * phys->getSpeed().norm());
            }
        }

        // Apply angle from instance
        if ( _instance.m_angle != 0.0f )
        {
            angle += _instance.m_angle;
            _instance.m_gen->getParameters()->setAngle(Angle(bfalse,angle));
        }
        // Apply angle from actor
        else if ( _instance.m_useActorOrientation ) 
        {
            _instance.m_gen->getParameters()->setAngle(Angle(bfalse,angle));
        }


        if (_instance.m_gen->getParameters()->m_useMatrix)
        {
            _instance.m_gen->setLocalRotationZ(angle);
            _instance.m_gen->setLocalScale(m_actor->getScale());
            angle = 0.f;
        }

        Vec3d velocity;
        f32_CosSinOpt(angle, &velocity.m_x, &velocity.m_y);
        velocity.m_z = 0.f;

        _instance.m_gen->getParameters()->setVelocity(velocity);

        _instance.m_gen->getParameters()->setFlip(fliped);
    }

    void FxBankComponent::setInputs( FxInstance &_instance, f32 _deltaTime )
    {
        // Inputs
        //-------------------------------------------------------------

        if (_instance.m_descriptor->getFrequencyInput().isValid())
        {
            f32 freq = f32_Inv(_instance.m_gen->getParameters()->getFrequency());
            freq = _instance.m_descriptor->getFrequencyInput().getValue(freq, _deltaTime, m_inputList);
            f32 invFreq = freq ? f32_Inv(freq) : 0.f;
            _instance.m_gen->getParameters()->setFrequency(invFreq);
        }

        if (_instance.m_descriptor->getEmitCountInput().isValid() )
        {
            f32 emitCount = (f32)_instance.m_gen->getParameters()->getNumToEmit();
            emitCount = _instance.m_descriptor->getEmitCountInput().getValue(emitCount, _deltaTime, m_inputList);
            _instance.m_gen->getParameters()->setNumToEmit((u32)emitCount);
        }

        if ( _instance.m_descriptor->getVelocityInput().isValid() )
        {
            f32 velocity = _instance.m_gen->getParameters()->getVelocityNorm();
            velocity = _instance.m_descriptor->getVelocityInput().getValue(velocity, _deltaTime, m_inputList);
            _instance.m_gen->getParameters()->setVelocityNorm(velocity);
        }

        if ( _instance.m_descriptor->getVelocityDeltaInput().isValid() )
        {
            f32 velocityVar = _instance.m_gen->getParameters()->getVelocityVar();
            velocityVar = _instance.m_descriptor->getVelocityDeltaInput().getValue(velocityVar, _deltaTime, m_inputList);
            _instance.m_gen->getParameters()->setVelocityVar(velocityVar);
        }

        if ( _instance.m_descriptor->getAngularSpeedInput().isValid() )
        {
            Angle angularSpeed = _instance.m_gen->getParameters()->getAngularSpeed();
            angularSpeed.SetDegrees(_instance.m_descriptor->getAngularSpeedInput().getValue(angularSpeed.ToDegrees(), _deltaTime, m_inputList));
            _instance.m_gen->getParameters()->setAngularSpeed(angularSpeed);
        }

        if ( _instance.m_descriptor->getAngularSpeedDeltaInput().isValid() )
        {
            Angle angularSpeedDelta = _instance.m_gen->getParameters()->getAngularSpeedDelta();
            angularSpeedDelta.SetDegrees(_instance.m_descriptor->getAngularSpeedDeltaInput().getValue(angularSpeedDelta.ToDegrees(), _deltaTime, m_inputList));
            _instance.m_gen->getParameters()->setAngularSpeedDelta(angularSpeedDelta);
        }
    }

    void FxBankComponent::updateAABB( FxInstance &_instance )
    {
        const AABB& fxAABB = _instance.m_gen->getParameters()->getBoundingBox();

        if (_instance.m_descriptor->getGen().getComputeAABB())
        {
            m_actor->growRelativeAABBFromAbsolute(fxAABB);
        }
        else
        {
            AABB aabb(fxAABB.getMin() * m_actor->getScale());
            aabb.grow(fxAABB.getMax() * m_actor->getScale());
            m_actor->growRelativeAABB(aabb);
        }
    }


    void FxInstance::clear()
    {
        if (m_generatorIndexInPool  >= 0)
        {
            ITF_ParticleGenerator::freeGenerator(m_gen, m_generatorIndexInPool);
            m_gen = NULL;
            m_generatorIndexInPool = -1;
        }
        else
        {
            SF_DEL(m_gen);
        }
        m_descriptor            = NULL;
        m_state                 = InstanceState_Unused;
        m_emitFromBase          = bfalse;
        m_useActorSpeed         = btrue;
        m_useActorOrientation   = bfalse;
        m_angle                 = 0.0f;
        m_boneIndex             = U32_INVALID;
        m_useBoneOrientation    = bfalse;
        m_pos                   = Vec3d::Zero;
        m_timer                 = 0.0f;
        m_name                  = StringID::Invalid;
        m_playMode              = 0;
    }

}

