#include "precompiled_engine.h"

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#define DESCRIPTOR_POOL_MAX_SIZE	128

namespace ITF
{
	u32 FeedbackFXManager::s_groupCount = 0;

	///////////////////////////////////////////////////////////////////////////////////////////
	//	FXCONTROL
	///////////////////////////////////////////////////////////////////////////////////////////

	BEGIN_SERIALIZATION(FXControl)
		SERIALIZE_MEMBER("name",m_name);
		SERIALIZE_MEMBER("fxStopOnEndAnim",m_fxStopOnEndAnim);
		SERIALIZE_MEMBER("fxPlayOnce",m_fxPlayOnce);
		SERIALIZE_MEMBER("fxEmitFromBase",m_fxEmitFromBase);
		SERIALIZE_MEMBER("fxUseActorSpeed",m_fxUseActorSpeed);
        SERIALIZE_MEMBER("fxUseActorOrientation",m_fxUseActorOrientation);
		SERIALIZE_MEMBER("fxBoneName",m_fxBoneName);
        SERIALIZE_MEMBER("fxUseBoneOrientation",m_fxUseBoneOrientation);
		SERIALIZE_MEMBER("sound",m_sound);
		SERIALIZE_MEMBER("particle",m_particle);
		SERIALIZE_MEMBER("playContact",m_playContact);
		SERIALIZE_CONTAINER("sounds",m_sounds);
        SERIALIZE_CONTAINER("particles",m_particles);
        SERIALIZE_MEMBER("owner",m_owner);
	END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////

    void FXControl::init()
    {
        m_fxFlags           = 0;

        if (m_sound.isValid())
        {
            m_fxFlags |= FXControl::flag_sound;
        }
        else
        {
            for (u32 i = 0; i < m_sounds.size(); ++i)
            {
                if (m_sounds[i].isValid())
				{
					m_fxFlags |= FXControl::flag_sound;
                    break;
				}
            }
        }
        if (m_particle.isValid())
        {
            m_fxFlags |= FXControl::flag_particle;
        }
        else
        {
            for (u32 i = 0; i < m_particles.size(); ++i)
            {
                if (m_particles[i].isValid())
				{
                    m_fxFlags |= FXControl::flag_particle;
                    break;
				}
			 }
        }
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	//	ACTIONMAP
	///////////////////////////////////////////////////////////////////////////////////////////

	BEGIN_SERIALIZATION(Action)
		SERIALIZE_CONTAINER_OBJECT("actions",m_actions);
	END_SERIALIZATION()
	Action::Action()
	{}

	Action::~Action()
	{
        // DO NOT CALL SF_DEL() on actionMap members as it's done inside FeedbackFXManager::~FeedbackFXManager()
		m_actions.clear();
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	FXControl* Action::getFeedbacks(const StringID& _action)
	{
		ITF_MAP<StringID, FXControl *>::iterator action_it = m_actions.find(_action);
		if ( action_it == m_actions.end() )
			return NULL;

		return action_it->second;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	const FXControl * Action::getFeedbacks(const StringID& _action) const
	{
		ITF_MAP<StringID, FXControl *>::const_iterator action_it = m_actions.find(_action);
		if ( action_it == m_actions.end() )
			return NULL;

		return action_it->second;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	//	TARGETMAP
	///////////////////////////////////////////////////////////////////////////////////////////
	BEGIN_SERIALIZATION(Target)
		SERIALIZE_CONTAINER_OBJECT("targets",m_targets);
	END_SERIALIZATION()
	Target::Target()
	{}

	Target::~Target()
	{
		m_targets.clear();
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	Action* Target::getActions(const StringID& _target)
	{
		ITF_MAP<StringID, Action>::iterator target_it = m_targets.find(_target);
		if ( target_it == m_targets.end() )
			return NULL;

		return &(target_it->second);
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	const Action* Target::getActions (const StringID& _target) const
	{
		ITF_MAP<StringID, Action>::const_iterator target_it = m_targets.find(_target);
		if ( target_it == m_targets.end() )
			return NULL;

		return &(target_it->second);
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	//  FEEDBACK FX MANAGER TEMPLATE
	///////////////////////////////////////////////////////////////////////////////////////////

	IMPLEMENT_OBJECT_RTTI(FeedbackFXManager_Template)

	BEGIN_SERIALIZATION(FeedbackFXManager_Template)
		SERIALIZE_CONTAINER_OBJECT("soundDescriptors",m_soundDescriptorTemplates);
		SERIALIZE_CONTAINER_OBJECT("FxDescriptors",m_FxDescriptorTemplates);
        SERIALIZE_CONTAINER_OBJECT("actors",m_actors);
        SERIALIZE_CONTAINER_OBJECT("busList",m_busList);
	END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(FeedbackFXManager_Template,buses)
        SERIALIZE_MEMBER("actorType",m_actorType);
        SERIALIZE_MEMBER("bus",m_bus);
    END_SERIALIZATION()

	FeedbackFXManager_Template::~FeedbackFXManager_Template()
	{
		for( u32 i = 0 ; i < m_soundDescriptorTemplates.size() ; ++i )
		{
			SF_DEL(m_soundDescriptorTemplates[i]);
		}
		for( u32 i = 0 ; i < m_FxDescriptorTemplates.size() ; ++i )
		{
			SF_DEL(m_FxDescriptorTemplates[i]);
		}

		m_soundDescriptorTemplates.clear();
		m_FxDescriptorTemplates.clear();
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	bbool FeedbackFXManager_Template::onTemplateLoaded()
	{
        for( u32 i = 0 ; i < m_soundDescriptorTemplates.size() ; i++ )
            m_soundDescriptorTemplates[i]->onTemplateLoaded();

        for( u32 i = 0 ; i < m_FxDescriptorTemplates.size() ; i++ )
            m_FxDescriptorTemplates[i]->onTemplateLoaded();

		return Super::onTemplateLoaded();
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	//  FEEDBACK FX MANAGER
	///////////////////////////////////////////////////////////////////////////////////////////

	FeedbackFXManager::FeedbackFXManager ()
	{
		m_soundDescPool.resize(DESCRIPTOR_POOL_MAX_SIZE);
		m_FxDescPool.resize(DESCRIPTOR_POOL_MAX_SIZE);

        m_defaultMaterial   = StringID::StrToCRC("DefaultMaterial");
        m_defaultActor      = StringID::StrToCRC("DefaultActor");
	}

	FeedbackFXManager::~FeedbackFXManager ()
	{
        //if (TEMPLATEDATABASE)
        //{
        //    const Path FRTFile(GETPATH_ALIAS("archetypes"));
        //    TEMPLATEDATABASE->removeTemplateClient(FRTFile.getStringID(), this);
        //}

		m_soundDescriptorTemplates.clear();
		m_FxDescriptorTemplates.clear();

		m_actionsPerActors.clear();
		m_actionsPerTargets.clear();

        // Delete all actions
        FeedbackFXManager::ActorMap::iterator actor_it = m_actors.begin();
        // Check all actors
        for (  ; actor_it != m_actors.end() ; actor_it++ )
        {
            Target::TargetMap* pFRTTargetMap	= actor_it->second.getTargetMap();

            // Check all targets
            Target::TargetMap::iterator target_it = pFRTTargetMap->begin();
            for (  ; target_it != pFRTTargetMap->end() ; target_it++ )
            {
                Action::ActionMap* pFRTActionMap	= target_it->second.getActionMap();

                // Check all actions
                Action::ActionMap::iterator action_it = pFRTActionMap->begin();
                for ( ; action_it != pFRTActionMap->end() ; action_it++ )
                {
                    SF_DEL(action_it->second);
                }
            }
        }

        m_owners.clear();
		m_actors.clear();
		m_SDTemplateMap.clear();
		m_FXTemplateMap.clear();
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::init()
	{
        const Path FRTFile(GETPATH_ALIAS("archetypes"));

        TEMPLATEDATABASE->addTemplateClient(FRTFile.getStringID(), this);
		const FeedbackFXManager_Template* config = TEMPLATEDATABASE->getTemplate<FeedbackFXManager_Template>(this, FRTFile);
		if (!config)
		{
			ITF_FATAL_ERROR("Couldn't load Archetype table config: %s", FRTFile.getString8().cStr());
            return;
		}

        m_actors = config->getActorsMap(); // copy
        const ITF_VECTOR<FeedbackFXManager_Template::buses>& busList = config->getBusList();
        for( u32 i = 0 ; i < busList.size() ; ++i )
        {
            m_busMap[busList[i].m_actorType] = busList[i].m_bus;
        }
		m_soundDescriptorTemplates = config->getSDTemplates();
		m_FxDescriptorTemplates = config->getFXTemplates();

		initInternal();
	}


    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::addOwner( const StringID& _archetype, const StringID& _type, ResourceGroup* _resourceGroup)
    {
        // Gather resources that this archetype is the owner
        FeedbackFXManager::OwnerMap::const_iterator owner_it = m_owners.find(_type);
        if ( owner_it == m_owners.end() )
            owner_it = m_owners.find(_archetype);

        if ( owner_it != m_owners.end() )
        {
            FeedbackFXManager::FXControls::const_iterator fxControl_it = owner_it->second.begin();
            for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
            {

                // Add SFX to owner resource group
                addSoundToResourceGroup(*fxControl_it, _resourceGroup);

                // Add VFX to owner resource group
                addFXToResourceGroup(*fxControl_it, _resourceGroup);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

	bbool FeedbackFXManager::hasFeedback( const StringID& _actor
										, const StringID& _actorArchetype
                                        , const StringID& _defaultActor
										, const StringID& _action
										, const StringID& _target
										, const StringID& _targetArchetype
                                        , const StringID& _defaultTarget)
	{
		// actor vs target
        bbool ret = hasFeedbackImpl(_actor, _action, _target);
        // actor archetype vs target
		if (!ret)
			ret = hasFeedbackImpl(_actorArchetype, _action, _target);
        // actor vs target archetype
		if (!ret)
			ret = hasFeedbackImpl(_actor, _action, _targetArchetype);
        // actor archetype vs target archetype
		if (!ret)
			ret = hasFeedbackImpl(_actorArchetype, _action, _targetArchetype);

        // TESTING DEFAULT
        // actor vs default target
        if (!ret)
            ret = hasFeedbackImpl(_actor, _action, _defaultTarget);
        // actor archetype vs default target
        if (!ret)
            ret = hasFeedbackImpl(_actorArchetype, _action, _defaultTarget);
        // default actor vs target
        if (!ret)
            ret = hasFeedbackImpl(_defaultActor, _action, _target);
        // default actor vs target archetype
        if (!ret)
            ret = hasFeedbackImpl(_defaultActor, _action, _targetArchetype);
        // default actor vs default target
        if (!ret)
            ret = hasFeedbackImpl(_defaultActor, _action, _defaultTarget);

		return ret;
	}
	
    ///////////////////////////////////////////////////////////////////////////////////////////

	bbool FeedbackFXManager::hasFeedbackImpl( const StringID& _actor, const StringID& _action, const StringID& _target )
	{
        if ( _actor == StringID::Invalid || _action == StringID::Invalid ||_target == StringID::Invalid )
            return bfalse;

		FeedbackFXManager::ActionsPerActorMap::const_iterator actor_it = m_actionsPerActors.find(_actor);
		if (actor_it != m_actionsPerActors.end())
		{
			FeedbackFXManager::ActionsPerTargetMap::const_iterator target_it = m_actionsPerTargets.find(_target);
			if (target_it != m_actionsPerTargets.end())
			{
                // Find action in actor's action list
                bbool bFoundActionInActor = bfalse;
				for (u32 i = 0 ; i < actor_it->second.size() && !bFoundActionInActor ; ++i)
				{
					if ( actor_it->second[i] == _action )
						bFoundActionInActor = btrue;
				}

                // Find action in target's action list
                bbool bFoundActionInTarget = bfalse;
                for (u32 i = 0 ; i < target_it->second.size() && !bFoundActionInTarget ; ++i)
                {
                    if ( target_it->second[i] == _action )
                        bFoundActionInTarget = btrue;
                }

                return (bFoundActionInActor && bFoundActionInTarget);
			}
		}

		return bfalse;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	const FXControl * FeedbackFXManager::getFeedback( const StringID& _actor
													, const StringID& _actorArchetype
                                                    , const StringID& _defaultActor
													, const StringID& _action
													, const StringID& _target
													, const StringID& _targetArchetype
                                                    , const StringID& _defaultTarget)
	{
        const FXControl * ret = NULL;
        if ( hasFeedbackImpl(_actor, _action, _target) )
            ret = getFeedbackImpl(_actor, _action, _target);
		if (!ret && hasFeedbackImpl(_actorArchetype, _action, _target))
			ret = getFeedbackImpl(_actorArchetype, _action, _target);
		if (!ret && hasFeedbackImpl(_actor, _action, _targetArchetype))
			ret = getFeedbackImpl(_actor, _action, _targetArchetype);
		if (!ret && hasFeedbackImpl(_actorArchetype, _action, _targetArchetype))
			ret = getFeedbackImpl(_actorArchetype, _action, _targetArchetype);

        // WITH DEFAULT
        if (!ret && hasFeedbackImpl(_actor, _action, _defaultTarget) )
            ret = getFeedbackImpl(_actor, _action, _defaultTarget);
        if (!ret && hasFeedbackImpl(_actorArchetype, _action, _defaultTarget))
            ret = getFeedbackImpl(_actorArchetype, _action, _defaultTarget);
        if (!ret && hasFeedbackImpl(_defaultActor, _action, _target))
            ret = getFeedbackImpl(_defaultActor, _action, _target);
        if (!ret && hasFeedbackImpl(_defaultActor, _action, _targetArchetype))
            ret = getFeedbackImpl(_defaultActor, _action, _targetArchetype);
        if (!ret && hasFeedbackImpl(_defaultActor, _action, _defaultTarget))
            ret = getFeedbackImpl(_defaultActor, _action, _defaultTarget);

		return ret;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	const FXControl* FeedbackFXManager::getFeedbackImpl( const StringID& _actor
														, const StringID& _action
														, const StringID& _target )
	{
		// Get targets associated to given actor
		FeedbackFXManager::ActorMap::iterator actor_it = m_actors.find(_actor);
		if (actor_it == m_actors.end())
			return NULL;

		Target* pTarget = &(actor_it->second);

		// Get actions associated to given target
		Action* pAction = pTarget->getActions(_target);
		if (!pAction)
			return NULL;

		// Get list of feedbacks associated to given action
		return pAction->getFeedbacks(_action);
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	SoundDescriptor * FeedbackFXManager::getNewSoundDescriptor( const Actor * _owner, const SoundDescriptor_Template * _soundDesc, class SoundComponent * _component)
	{
		if ( !(_owner && _soundDesc) )
			return NULL;

		ITF_ASSERT_MSG( m_soundDescPool.size() > 0,"Sound descriptor pool has not been correctly initialized !");

		// Look for a free slot in sound descriptor pool
		for ( u32 i = 0 ; i < m_soundDescPool.size() ; ++i )
		{
			if ( m_soundDescPool[i].m_isFree )
			{
				m_soundDescPool[i].m_isFree		= false;
				m_soundDescPool[i].m_owner		= _owner;
				// Associate template to this descriptor so that it can have access to resources already loaded
				m_soundDescPool[i].m_descriptor.init(_soundDesc, _component);

				return &(m_soundDescPool[i].m_descriptor);
			}
		}

		ITF_ASSERT_MSG( 0,"Found no free slot in sound descriptor pool. Need to increase its size ?");

		return NULL;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::releaseSoundDescriptors( const Actor * _owner, SoundDescriptor * _soundDesc )
	{
		for ( u32 i = 0 ; i < m_soundDescPool.size() ; ++i )
		{
			if ( !m_soundDescPool[i].m_isFree
				&& m_soundDescPool[i].m_owner == _owner
				&& ( (_soundDesc && &(m_soundDescPool[i].m_descriptor) == _soundDesc)
					|| !_soundDesc ) )
			{
				releaseSoundDescriptor(i);
			}
		}
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::releaseSoundDescriptor(u32 _index)
	{
		ITF_ASSERT_MSG(_index <  m_soundDescPool.size(), "Index out of range !");

		m_soundDescPool[_index].m_isFree	= true;
		m_soundDescPool[_index].m_owner		= NULL;
		m_soundDescPool[_index].m_descriptor.uninit();

		//ITF_VECTOR<ResourceID>& descResourceList = m_soundDescPool[_index].m_descriptor.getResourceList();
		//descResourceList.clear();
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	FxDescriptor * FeedbackFXManager::getNewFxDescriptor( const Actor * _owner, const FxDescriptor_Template * _FxDesc)
	{
		if ( !(_owner && _FxDesc) )
			return NULL;

		ITF_ASSERT_MSG( m_FxDescPool.size() > 0,"FX descriptor pool has not been correctly initialized !");

			// Look for a free slot in sound descriptor pool
			for ( u32 i = 0 ; i < m_FxDescPool.size() ; ++i )
			{
				if ( m_FxDescPool[i].m_isFree )
				{
					m_FxDescPool[i].m_isFree	= false;
					m_FxDescPool[i].m_owner		= _owner;
					
					// Associate template to this descriptor so that it can have access to resources already loaded
					m_FxDescPool[i].m_descriptor.onActorLoaded(_FxDesc);

					return &(m_FxDescPool[i].m_descriptor);
				}
			}

			ITF_ASSERT_MSG( 0,"Found no free slot in sound descriptor pool. Need to increase its size ?");

				return NULL;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::releaseFxDescriptors( const Actor * _owner, FxDescriptor * _FxDesc )
	{
		for ( u32 i = 0 ; i < m_FxDescPool.size() ; ++i )
		{
			if ( !m_FxDescPool[i].m_isFree
				&& m_FxDescPool[i].m_owner == _owner
				&& ( (_FxDesc && &(m_FxDescPool[i].m_descriptor) == _FxDesc)
				|| !_FxDesc ) )
			{
				releaseFxDescriptor(i);
			}
		}
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::releaseFxDescriptor(u32 _index)
	{
		ITF_ASSERT_MSG(_index <  m_FxDescPool.size(), "Index out of range !");

		m_FxDescPool[_index].m_isFree	= true;
		m_FxDescPool[_index].m_owner	= NULL;
		m_FxDescPool[_index].m_descriptor.clear();
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::initInternal()
	{
		// Init Descriptors templates maps
		ITF_VECTOR<SoundDescriptor_Template *>::iterator sound_it = m_soundDescriptorTemplates.begin();
		u32 i = 0;
		for ( ; sound_it != m_soundDescriptorTemplates.end() ; ++sound_it )
		{
			const StringID& soundDescID = (*sound_it)->GetName();

			ITF_ASSERT(soundDescID.isValid());

			if ( soundDescID.isValid() )
			{
				// Update sound descriptor template map
				m_SDTemplateMap[soundDescID] = i;
			}
			++i;
		}

		ITF_VECTOR<FxDescriptor_Template *>::iterator FX_it = m_FxDescriptorTemplates.begin();
		i = 0;
		for ( ; FX_it != m_FxDescriptorTemplates.end() ; ++FX_it )
		{
			const StringID& FXDescID = (*FX_it)->GetName();

			ITF_ASSERT(FXDescID.isValid());

			if ( FXDescID.isValid() )
			{
				// Update FX descriptor template map
				m_FXTemplateMap[FXDescID] = i;
			}
			++i;
		}

		updateInternal();
	}

    ///////////////////////////////////////////////////////////////////////////////////////////

	void FeedbackFXManager::updateInternal()
	{
		FeedbackFXManager::ActorMap::iterator actor_it = m_actors.begin();

		// For each actors
		for (  ; actor_it != m_actors.end() ; actor_it++ )
		{
			ActionIDs& actionPerActorIDs = m_actionsPerActors[actor_it->first];

			Target::TargetMap* pFRTTargetMap	= actor_it->second.getTargetMap();

			// for each targes
			Target::TargetMap::iterator target_it = pFRTTargetMap->begin();
			for (  ; target_it != pFRTTargetMap->end() ; target_it++ )
			{
				ActionIDs& actionPerTargetIDs = m_actionsPerTargets[target_it->first];

				Action::ActionMap* pFRTActionMap	= target_it->second.getActionMap();

				// for each actions
				Action::ActionMap::iterator action_it = pFRTActionMap->begin();
				for ( ; action_it != pFRTActionMap->end() ; action_it++ )
				{
					u32 i = 0;
					bbool b_actorFound = false;
					// add actor to action per actor map if it's not already done
					for ( i = 0 ; i < actionPerActorIDs.size() ; i++ )
					{
						if ( actionPerActorIDs[i] == action_it->first )
						{
							b_actorFound = true;
							// This action is already referenced, no need to continue then
							break;
						}
					}
					// This action is not referenced for this actor yet
					if ( !b_actorFound )
					{
						actionPerActorIDs.push_back(action_it->first);
					}

					bbool b_targetFound = false;
					// add actor to action per target map if it's not already done
					for ( i = 0 ; i < actionPerTargetIDs.size() ; i++ )
					{
						if ( actionPerTargetIDs[i] == action_it->first )
						{
							b_targetFound = true;
							// This action is already referenced, no need to continue then
							break;
						}
					}
					// This action is not referenced for this actor yet
					if ( !b_targetFound )
					{
						actionPerTargetIDs.push_back(action_it->first);
					}

					// Init FXcontrol data
					action_it->second->init();

                    // Add this fxcontrol to owner table
                    addInOwnerTable(action_it->second);
				}
			}
		}
	}

    void FeedbackFXManager::addInOwnerTable( FXControl * _fxControl )
    {
        ITF_ASSERT(_fxControl);

        if( _fxControl )
        {

            // There is a owner defined ?
            ITF_WARNING_CATEGORY(GPP,NULL,_fxControl->m_owner.isValid(),"FXControl %s has no ownerset. Check with sound designers / FX",_fxControl->m_name.getDebugString());
            if ( _fxControl->m_owner.isValid() )
            {
                FeedbackFXManager::OwnerMap::iterator owner_it = m_owners.find(_fxControl->m_owner);
                // This owner already exists
                bbool bExists = bfalse;
                if ( owner_it != m_owners.end() )
                {
                    ITF_VECTOR<FXControl *>& fxControls = owner_it->second;
                    // Make sure _fxControl has not been previously added (which is bad !)
                    for ( u32 i = 0 ; i < fxControls.size() || bExists ; i++ )
                    {
                        ITF_ASSERT(fxControls[i] != _fxControl);
                        if ( fxControls[i] == _fxControl )
                            bExists = btrue;
                    }
                }

                // Add the new one
                if ( !bExists )
                    m_owners[_fxControl->m_owner].push_back(_fxControl);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::addFXToResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup)
    {
        for ( u32 i = 0 ; i < _fxControl->m_particles.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator fxDesc_it = m_FXTemplateMap.find(_fxControl->m_particles[i]);
            if ( fxDesc_it != m_FXTemplateMap.end() )
            {
                m_FxDescriptorTemplates[fxDesc_it->second]->addToResourceGroup(_resourceGroup);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::removeFXFromResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup)
    {
        for ( u32 i = 0 ; i < _fxControl->m_particles.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator fxDesc_it = m_FXTemplateMap.find(_fxControl->m_particles[i]);
            if ( fxDesc_it != m_FXTemplateMap.end() )
            {
                m_FxDescriptorTemplates[fxDesc_it->second]->removeFromResourceGroup(_resourceGroup);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::addSoundToResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup)
    {
        for ( u32 i = 0 ; i < _fxControl->m_sounds.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator soundDesc_it = m_SDTemplateMap.find(_fxControl->m_sounds[i]);
            if ( soundDesc_it != m_SDTemplateMap.end() )
            {
                m_soundDescriptorTemplates[soundDesc_it->second]->addToResourceGroup(_resourceGroup);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void FeedbackFXManager::removeSoundFromResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup)
    {
        for ( u32 i = 0 ; i < _fxControl->m_sounds.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator soundDesc_it = m_SDTemplateMap.find(_fxControl->m_sounds[i]);
            if ( soundDesc_it != m_SDTemplateMap.end() )
            {
                m_soundDescriptorTemplates[soundDesc_it->second]->removeFromResourceGroup(_resourceGroup);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    const StringID& FeedbackFXManager::getCustomCategory(const StringID& _type)
    {
        ITF_MAP<StringID,StringID>::const_iterator  it = m_busMap.find(_type);

        if ( it != m_busMap.end() )
            return it->second;

        return StringID::Invalid;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

	SoundDescriptor_Template*	FeedbackFXManager::getSDTemplate(const StringID& _name)
	{
		ITF_MAP<StringID, u32>::iterator SDIndex_it = m_SDTemplateMap.find(_name);
		if ( SDIndex_it != m_SDTemplateMap.end() )
			return m_soundDescriptorTemplates[SDIndex_it->second];

		return NULL;
	}
	
    ///////////////////////////////////////////////////////////////////////////////////////////

	FxDescriptor_Template*		FeedbackFXManager::getFXTemplate(const StringID& _name)
	{
		ITF_MAP<StringID, u32>::iterator FXIndex_it = m_FXTemplateMap.find(_name);
		if ( FXIndex_it != m_FXTemplateMap.end() )
			return m_FxDescriptorTemplates[FXIndex_it->second];

		return NULL;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    // BUNDLE
    ///////////////////////////////////////////////////////////////////////////////////////////

    void FeedbackFXManager::forceLoad()
    {


        for ( u32 i = 0; i < m_soundDescriptorTemplates.size(); ++i )
        {
            const StringID& soundDescID = m_soundDescriptorTemplates[i]->GetName();

            ITF_ASSERT(soundDescID.isValid());

            if ( soundDescID.isValid() )
            {
                const ITF_VECTOR< Path > files = m_soundDescriptorTemplates[i]->getFiles();

                for ( u32 j = 0; j < files.size(); ++j )
                {
                    const ResourceID& id = RESOURCE_MANAGER->newResourceIDFromFile( Resource::ResourceType_Sound, files[j] );

                    id.getResource()->setStatic( btrue );
                    RESOURCE_MANAGER->queryAddUserToResourcePhysicalData( id );
                }
            }
        }

        for ( u32 i = 0; i < m_FxDescriptorTemplates.size(); ++i )
        {
            const StringID& FXDescID = m_FxDescriptorTemplates[i]->GetName();

            ITF_ASSERT(FXDescID.isValid());

            if ( FXDescID.isValid() )
            {
                const ResourceID& id = RESOURCE_MANAGER->newResourceIDFromFile( Resource::ResourceType_Texture, m_FxDescriptorTemplates[i]->getTexture() );

                id.getResource()->setStatic( btrue );
                RESOURCE_MANAGER->queryAddUserToResourcePhysicalData( id );
            }
        }
    }

    void FeedbackFXManager::getDependencies( const StringID& _archetype, const StringID& _type,  ITF_VECTOR<Path>& _dependencies )
    {
        // Gather resources that this archetype / type is the owner
        FeedbackFXManager::OwnerMap::const_iterator owner_it = m_owners.find(_type);
        if ( owner_it == m_owners.end() )
            owner_it = m_owners.find(_archetype);

        if ( owner_it != m_owners.end() )
        {
            FeedbackFXManager::FXControls::const_iterator fxControl_it = owner_it->second.begin();
            for ( ; fxControl_it != owner_it->second.end() ; fxControl_it++ )
            {

                // Gather all sound paths
                getSoundPaths(*fxControl_it, _dependencies);

                // Gather all VFX paths
                getVFXPaths(*fxControl_it, _dependencies);
            }
        }
    }

    void FeedbackFXManager::getVFXPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths)
    {
        for ( u32 i = 0 ; i < _fxControl->m_particles.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator fxDesc_it = m_FXTemplateMap.find(_fxControl->m_particles[i]);
            if ( fxDesc_it != m_FXTemplateMap.end() )
            {
                _paths.push_back(m_FxDescriptorTemplates[fxDesc_it->second]->getTexture());
            }
        }
    }

    void FeedbackFXManager::getSoundPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths)
    {
        for ( u32 i = 0 ; i < _fxControl->m_sounds.size() ; i++ )
        {
            ITF_MAP<StringID, u32>::const_iterator soundDesc_it = m_SDTemplateMap.find(_fxControl->m_sounds[i]);
            if ( soundDesc_it != m_SDTemplateMap.end() )
            {
                const ITF_VECTOR<Path> soundList = m_soundDescriptorTemplates[soundDesc_it->second]->getFiles();
                for ( u32 j = 0 ; j < soundList.size() ; j++ )
                {
                    _paths.push_back(soundList[j]);
                }
            }
        }
    }

}
