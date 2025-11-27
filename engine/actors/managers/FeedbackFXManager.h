#ifndef _ITF_FEEDBACKFXMANAGER_H_
#define _ITF_FEEDBACKFXMANAGER_H_

#include "engine/sound/SoundDescriptor.h"

#ifndef _ITF_FXDESCRIPTOR_H_
#include "engine/display/FxDescriptor.h"
#endif //_ITF_FXDESCRIPTOR_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

namespace ITF
{
    //forward
    class HitStim;

	///////////////////////////////////////////////////////////////////////////////////////////
	//	FXCONTROL
	///////////////////////////////////////////////////////////////////////////////////////////
	struct FXControl
	{
		DECLARE_SERIALIZE()
		static const u32 flag_sound     = 0x1;
		static const u32 flag_material  = 0x2;
		static const u32 flag_particle  = 0x4;

		FXControl() : 
		m_fxStopOnEndAnim(bfalse)
			, m_fxPlayOnce(bfalse)
			, m_fxEmitFromBase(btrue)
			, m_fxUseActorSpeed(btrue)
            , m_fxUseActorOrientation(bfalse)
			, m_fxBoneIndex(U32_INVALID)
            , m_fxUseBoneOrientation(bfalse)
			, m_playContact(bfalse)
			, m_fxFlags(0)
            , m_owner(StringID::Invalid)
            , m_fxInstanceOnce(bfalse)
		{
		}

        // Specific to feedback
        StringID    m_owner;

		StringID    m_name;
		bbool       m_fxStopOnEndAnim;
		bbool       m_fxPlayOnce;
        bbool       m_fxInstanceOnce;
		bbool       m_fxEmitFromBase;
		u32         m_fxFlags;
		bbool       m_fxUseActorSpeed;
        bbool       m_fxUseActorOrientation;
		StringID    m_fxBoneName;
		u32         m_fxBoneIndex;
        bbool       m_fxUseBoneOrientation;
		bbool       m_playContact;

		StringID    m_sound;
		StringID    m_particle;


		SafeArray<StringID> m_sounds;
		SafeArray<StringID> m_particles;

		void init();
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//	ACTIONMAP
	///////////////////////////////////////////////////////////////////////////////////////////
	class Action
	{
	public:
		DECLARE_SERIALIZE()

		Action();
		~Action();

		typedef ITF_MAP<StringID, FXControl *>		ActionMap;

		FXControl *	getFeedbacks(const StringID& _action);
		ActionMap*	getActionMap() { return &m_actions; }

		const FXControl *	getFeedbacks(const StringID& _action) const;
		const ActionMap*	getActionMap() const { return &m_actions; }

	private:

		ActionMap	m_actions;
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//	TARGETMAP
	///////////////////////////////////////////////////////////////////////////////////////////
	class Target
	{
	public:

		DECLARE_SERIALIZE()

		Target();
		~Target();

		typedef ITF_MAP<StringID, Action>		TargetMap;

		Action*			getActions(const StringID& _target);
		TargetMap*		getTargetMap() { return &m_targets; }

		const Action*		getActions (const StringID& _target) const;
		const TargetMap*	getTargetMap() const { return &m_targets; }

	private:

		TargetMap	m_targets;
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//  FEEDBACK FX MANAGER
	///////////////////////////////////////////////////////////////////////////////////////////
	class FeedbackFXManager
	{
	public:

		DECLARE_SERIALIZE()

		FeedbackFXManager();
		~FeedbackFXManager();

		typedef ITF_MAP<StringID, Target>			ActorMap;

		typedef ITF_VECTOR<StringID>				ActionIDs;
		typedef ITF_MAP<StringID, ActionIDs>		ActionsPerActorMap;
		typedef ITF_MAP<StringID, ActionIDs>		ActionsPerTargetMap;
        typedef ITF_VECTOR<FXControl *>             FXControls;
        typedef ITF_MAP<StringID, FXControls>       OwnerMap;

		typedef ITF_VECTOR<ResourceID>				ResourceList;

		void				init();

		const FXControl*	getFeedback( const StringID& _actor
										, const StringID& _actorArchetype
                                        , const StringID& _defaultActor
										, const StringID& _action
										, const StringID& _target
										, const StringID& _targetArchetype
                                        , const StringID& _defaultTarget);

        void                addOwner( const StringID& _archetype, const StringID& _type, ResourceGroup* _resourceGroup);

        // For bundle process
        void                getDependencies( const StringID& _archetype, const StringID& _type,  ITF_VECTOR<Path>& _dependencies );

		bbool				hasFeedback( const StringID& _actor
										, const StringID& _actorArchetype
                                        , const StringID& _defaultActor
										, const StringID& _action
										, const StringID& _target
										, const StringID& _targetArchetype
                                        , const StringID& _defaultTarget );

		ITF_VECTOR<SoundDescriptor_Template *>*			getSoundDescriptorTemplates() { return &m_soundDescriptorTemplates; }
		const ITF_VECTOR<SoundDescriptor_Template *>*	getSoundDescriptorTemplates() const { return &m_soundDescriptorTemplates; }
		ITF_VECTOR<FxDescriptor_Template *>*			getFxDescriptorTemplates() { return &m_FxDescriptorTemplates; }
		const ITF_VECTOR<FxDescriptor_Template *>*		getFxDescriptorTemplates() const { return &m_FxDescriptorTemplates; }

		SoundDescriptor *		getNewSoundDescriptor( const Actor * _owner, const SoundDescriptor_Template * _soundDesc, class SoundComponent * _component);
		void					releaseSoundDescriptors( const Actor * _owner, SoundDescriptor * _soundDesc = NULL );

		FxDescriptor *			getNewFxDescriptor( const Actor * _owner, const FxDescriptor_Template * _FxDesc);
		void					releaseFxDescriptors( const Actor * _owner, FxDescriptor * _FxDesc = NULL );

		SoundDescriptor_Template*	getSDTemplate(const StringID& _name);
		FxDescriptor_Template*		getFXTemplate(const StringID& _name);

        const StringID&             getDefaultMaterial() const {return m_defaultMaterial;}
        const StringID&             getDefaultActor() const {return m_defaultActor;}

        const StringID&             getCustomCategory(const StringID& _type);

        void forceLoad();

	private:

		// Structures used for descriptors pools
		struct SoundDescriptorPoolElem
		{
			SoundDescriptorPoolElem()
				: m_isFree(true)
				, m_owner(NULL)
			{}

			bbool				m_isFree;
			const Actor*		m_owner;

			SoundDescriptor		m_descriptor;
		};

		struct FxDescriptorPoolElem
		{
			FxDescriptorPoolElem()
				: m_isFree(true)
				, m_owner(NULL)
			{}

			bbool			m_isFree;
			const Actor*	m_owner;

			FxDescriptor	m_descriptor;
		};

		typedef ITF_VECTOR<SoundDescriptorPoolElem>	SoundDescriptorsPool;
		typedef ITF_VECTOR<FxDescriptorPoolElem>	FxDescriptorsPool;

        void                    addFXToResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup);
        void                    removeFXFromResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup);
        void                    addSoundToResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup);
        void                    removeSoundFromResourceGroup(FXControl * _fxControl, ResourceGroup* _resourceGroup);

		bbool					hasFeedbackImpl( const StringID& _actor
												, const StringID& _action
												, const StringID& _target);

		const FXControl*		getFeedbackImpl( const StringID& _actor
													, const StringID& _action
													, const StringID& _target);

		void					releaseSoundDescriptor(u32 _index);
		void					releaseFxDescriptor(u32 _index);

		void					initInternal();
		void					updateInternal();

        void                    addInOwnerTable(FXControl * _fxControl);

        // For bundle process
        void                    getVFXPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths);
        void                    getSoundPaths(FXControl * _fxControl, ITF_VECTOR<Path>& _paths);

		ITF_VECTOR<SoundDescriptor_Template *>	m_soundDescriptorTemplates;
		ITF_VECTOR<FxDescriptor_Template *>		m_FxDescriptorTemplates;

		// Used to quickly access descriptors templates
		ITF_MAP<StringID, u32>	m_SDTemplateMap;
		ITF_MAP<StringID, u32>	m_FXTemplateMap;

		static u32				s_groupCount;

		ActorMap				m_actors;
        OwnerMap                m_owners;
		ActionsPerActorMap		m_actionsPerActors;
		ActionsPerTargetMap		m_actionsPerTargets;

		SoundDescriptorsPool	m_soundDescPool;
		FxDescriptorsPool		m_FxDescPool;

        StringID          m_defaultMaterial;
        StringID          m_defaultActor;

        ITF_MAP<StringID,StringID>      m_busMap;
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	//	FEEDBACKFXMANAGER_TEMPLATE
	///////////////////////////////////////////////////////////////////////////////////////////
	class FeedbackFXManager_Template : public TemplateObj
	{
		DECLARE_OBJECT_CHILD_RTTI(FeedbackFXManager_Template, TemplateObj,152627411);
		DECLARE_SERIALIZE()

	public:

        struct buses
        {
            DECLARE_SERIALIZE()
            StringID m_actorType;
            StringID m_bus;
        };

		FeedbackFXManager_Template()
			: Super()
		{}
		FeedbackFXManager_Template(const Path& _path)
			: Super(_path)
		{}

		~FeedbackFXManager_Template();


		virtual bbool onTemplateLoaded();

        const FeedbackFXManager::ActorMap&				getActorsMap() const { return m_actors; }
		const ITF_VECTOR<SoundDescriptor_Template *>&	getSDTemplates() const { return m_soundDescriptorTemplates; }
		const ITF_VECTOR<FxDescriptor_Template *>&		getFXTemplates() const { return m_FxDescriptorTemplates; }
        const ITF_VECTOR<buses>&                        getBusList() const { return m_busList; }

	private:

        ITF_VECTOR<buses>                       m_busList;

        FeedbackFXManager::ActorMap				m_actors;
		ITF_VECTOR<SoundDescriptor_Template *>	m_soundDescriptorTemplates;
		ITF_VECTOR<FxDescriptor_Template *>		m_FxDescriptorTemplates;
	};
}
#endif // _ITF_FEEDBACKFXMANAGER_H_
