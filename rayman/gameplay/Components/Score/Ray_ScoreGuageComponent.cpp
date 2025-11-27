#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SCOREGUAGECOMPONENT_H_
#include "rayman/gameplay/Components/score/Ray_ScoreGuageComponent.h"
#endif //_ITF_RAY_SCOREGUAGECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_


#define STATE_RECAP_DISABLED	0
#define STATE_RECAP_INTRO		1
#define STATE_RECAP_RELEASING	2
#define STATE_RECAP_FINISH		3

#define RECAP_TIME_FADE_BONE	0.5f
#define RECAP_OFFSET_LUMS_FLASH 5

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ScoreGuageComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ScoreGuageComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ScoreGuageComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_ScoreGuageComponent requires an AnimLightComponent/AnimatedComponent.");
    END_VALIDATE_COMPONENT()

	// ****************************************************************************

	// Link tag
	const StringID nameCineTag = ITF_GET_STRINGID_CRC(Cine,1585859472);
	const StringID nameScoreTag = ITF_GET_STRINGID_CRC(Score,3852336941);
	const StringID nameFiestaTag = ITF_GET_STRINGID_CRC(Fiesta,71747355);
	const StringID nameTrophyTag = ITF_GET_STRINGID_CRC(Trophy,952164198);
	const StringID nameDiscoTag = ITF_GET_STRINGID_CRC(Disco,1658684750);

	// Anim
	const StringID animArrived = ITF_GET_STRINGID_CRC(Arrived,3510154912);
	const StringID animArrived_WithoutDrk = ITF_GET_STRINGID_CRC(Arrived_WithoutDrk,2764271190);

	const StringID animIdle = ITF_GET_STRINGID_CRC(Idle,1633200266);
	const StringID animIdle_WithoutDrk = ITF_GET_STRINGID_CRC(Idle_WithoutDrk,2351405621);

	const StringID animReward01 = ITF_GET_STRINGID_CRC(Reward01,4048379015);
	const StringID animReward02 = ITF_GET_STRINGID_CRC(Reward02,506707471);
	const StringID animReward02_Bis = ITF_GET_STRINGID_CRC(Reward02_Bis,4073341280);
	const StringID animReward03 = ITF_GET_STRINGID_CRC(Reward03,1927956828);

	// Sound
    const StringID s_MRK_Lum_Jauge = ITF_GET_STRINGID_CRC(MRK_Lum_Jauge,2542590978);

	const StringID soundWait = ITF_GET_STRINGID_CRC(EndOfMap_LoopWait,2843522991);
	const StringID soundMusic1 = ITF_GET_STRINGID_CRC(EndOfMap_LoopFill_Part1,217874386);
	const StringID soundMusic2 = ITF_GET_STRINGID_CRC(EndOfMap_LoopFill_Part2,2243090504);
	const StringID soundMusic3 = ITF_GET_STRINGID_CRC(EndOfMap_LoopFill_Part3,2573263261);
	const StringID soundFlash = ITF_GET_STRINGID_CRC(EndOfMap_HitCountLums,2537847575);
	const StringID soundDisco = ITF_GET_STRINGID_CRC(EndOfMap_Disco,3879289714);

	// Bones
	const StringID boneCounterA_1 = ITF_GET_STRINGID_CRC(Indicator_A,515217362);
	const StringID boneCounterA_2 = ITF_GET_STRINGID_CRC(Arrow_A,1710702511);
	const StringID boneCounterA_3 = ITF_GET_STRINGID_CRC(Capacity_B,3663230776);

	const StringID boneCounterB_1 = ITF_GET_STRINGID_CRC(Indicator_B,2433189039);
	const StringID boneCounterB_2 = ITF_GET_STRINGID_CRC(Arrow_B,3098166560);
	const StringID boneCounterB_3 = ITF_GET_STRINGID_CRC(Capacity_C,2285436900);

	const StringID boneCounterC_1 = ITF_GET_STRINGID_CRC(Indicator_C,2299577750);
	const StringID boneCounterC_2 = ITF_GET_STRINGID_CRC(Arrow_C,775896084);
	const StringID boneCounterC_3 = ITF_GET_STRINGID_CRC(Capacity_D,3233638897);

	const StringID boneGlassCut_A = ITF_GET_STRINGID_CRC(B_GlassCut01,595174002);
	const StringID boneGlassCut_B = ITF_GET_STRINGID_CRC(B_GlassCut02,3445731091);
	const StringID boneGlassCut_C = ITF_GET_STRINGID_CRC(Tube_Back,923183304);

	// Fx
	const StringID fxNameFlash = ITF_GET_STRINGID_CRC(Flash_scorerecap,167988053);

    // ****************************************************************************

    Ray_ScoreGuageComponent::Ray_ScoreGuageComponent()
    : m_animComponent(NULL)
    , m_soundComponent(NULL)
    , m_uiTextBoxesComponent(NULL)
	, m_linkComponent(NULL)
	, m_fxControllerComponent(NULL)
    , m_firstUpdate(btrue)
    , m_arrivalInProgress(bfalse)
    , m_listenerRegistered(bfalse)
    , m_menuValidPressed(bfalse)
    , m_menuBackPressed(bfalse)
    , m_lumCount (0)
    , m_maxLumCount(0)
    , m_lumAdd(0)
    , m_thresholdB(0)
    , m_thresholdC(0)
    , m_thresholdD(0)
    , m_scoreStarted(bfalse)
    , m_scoresFinished(0)
    , m_waitingTime(0.0f)
    , m_soundHandle(ITF::SoundHandle::getInvalidHandle())
    , m_lumCursorSmooth(0.0f)
	, m_lumCursor(0.0f)
    , m_thresholdBCursor(0.0f)
    , m_thresholdCCursor(0.0f)
    , m_thresholdDCursor(0.0f)
	, m_initRef(bfalse)
	, m_scorePlayersDiplayed(bfalse)
	, m_tubeP1(Vec2d::Zero)
	, m_tubeP2(Vec2d::Zero)
	, m_tubeP3(Vec2d::Zero)
	, m_tubeP4(Vec2d::Zero)
	, m_trame(0)
	, m_indexBone_Lums(U32_INVALID)
	, m_rewardIndex(0)
	, m_rewardPlaying(bfalse)
	, m_mrDarkIsDead(bfalse)
	, m_state(STATE_RECAP_DISABLED)
	, m_totalLumsWin(0)
    , m_countAtlasUv(0)
	, m_fadingCounter_A(bfalse)
	, m_timeFadingCounter_A(0.0f)
	, m_fadingCounter_B(bfalse)
	, m_timeFadingCounter_B(0.0f)
	, m_fadingCounter_C(bfalse)
	, m_timeFadingCounter_C(0.0f)
	, m_indexboneCounterA_1(U32_INVALID)
	, m_indexboneCounterA_2(U32_INVALID)
	, m_indexboneCounterA_3(U32_INVALID)
	, m_indexboneCounterB_1(U32_INVALID)
	, m_indexboneCounterB_2(U32_INVALID)
	, m_indexboneCounterB_3(U32_INVALID)
	, m_indexboneCounterC_1(U32_INVALID)
	, m_indexboneCounterC_2(U32_INVALID)
	, m_indexboneCounterC_3(U32_INVALID)
	, m_alphaCounter_A(1.0f)
	, m_alphaCounter_B(1.0f)
	, m_alphaCounter_C(1.0f)
	, m_indexboneGlassCut_A(U32_INVALID)
	, m_indexboneGlassCut_B(U32_INVALID)
	, m_indexboneGlassCut_C(U32_INVALID)
	, m_actionButtonAlreadyPressed(bfalse)

	, m_handleSoundWait(ITF::SoundHandle::getInvalidHandle())
	, m_handleSoundMusic1(ITF::SoundHandle::getInvalidHandle())
	, m_handleSoundMusic2(ITF::SoundHandle::getInvalidHandle())
	, m_handleSoundMusic3(ITF::SoundHandle::getInvalidHandle())
	, m_handleSoundDisco(ITF::SoundHandle::getInvalidHandle())

	{
        clearListener();
    }
 
    // ****************************************************************************

    Ray_ScoreGuageComponent::~Ray_ScoreGuageComponent()
    {

        // Reset values set for this sequence on CinematicManager
        CINEMATICMANAGER->resetPlayersChannels();

        if(m_listenerRegistered)
        {
            m_listenerRegistered = bfalse;
            GAMEMANAGER->setInputModeForMenu(bfalse);
            GAMEMANAGER->removeInputListener(this);
            CONTEXTICONSMANAGER->hide();
        }
    }

    // ****************************************************************************

    void Ray_ScoreGuageComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get Components
        m_animComponent = m_actor->GetComponent<AnimatedComponent>();
        m_soundComponent = m_actor->GetComponent<SoundComponent>();
		ITF_ASSERT(m_soundComponent);

        m_uiTextBoxesComponent = m_actor->GetComponent<UITextBoxesComponent>();
		m_linkComponent = m_actor->GetComponent<LinkComponent>();
		m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();


        m_firstUpdate = btrue;
        m_arrivalInProgress = bfalse;
        m_scoresFinished = 0;
		m_rewardIndex = 0;

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventScoreGuageAddLum,2833738877),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventScoreSetup,427760862),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventMedalNonCinematicEnded,694861796),this);

		initAtlasLums();

		// Check if the Dark teensy is dead !
		if (getTemplate()->getCheckMapForMrDark() == RAY_GAMEMANAGER->getCurrentLevelName())
		{
			m_mrDarkIsDead = btrue;
		} 

    }

	// ****************************************************************************

	void Ray_ScoreGuageComponent::stopAllSound()
	{
		if(m_soundComponent)
		{
			m_soundComponent->stopSound(m_handleSoundWait);
			m_handleSoundWait = ITF::SoundHandle::getInvalidHandle();

			m_soundComponent->stopSound(m_handleSoundMusic1);
			m_handleSoundMusic1 = ITF::SoundHandle::getInvalidHandle();

			m_soundComponent->stopSound(m_handleSoundMusic2);
			m_handleSoundMusic2 = ITF::SoundHandle::getInvalidHandle();

			m_soundComponent->stopSound(m_handleSoundMusic3);
			m_handleSoundMusic3 = ITF::SoundHandle::getInvalidHandle();

			m_soundComponent->stopSound(m_handleSoundDisco);
			m_handleSoundDisco = ITF::SoundHandle::getInvalidHandle();
		}
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::initAtlasLums()
	{
		// Init Texture Atlas
		const u32 countLums = getTemplate()->getCountLums();
		m_atlas.initialize(m_actor->getResourceGroup(), getTemplate()->getTextureLums(), countLums);

		Texture * texture = m_atlas.getAtlasTexture();
		if(texture)
		{
			const UVAtlas* uvAtlas = texture->getUVAtlas();

			if (uvAtlas)
			{
				m_countAtlasUv = uvAtlas->getNumberUV();

				m_listUV.clear();
				Vec2d* listUV = &m_listUV.incrSize(m_countAtlasUv * 4);
				for (u32 i = 0; i < m_countAtlasUv; i++)
				{
					uvAtlas->get4UVAt(i, listUV+i*4);
				}
			}
		}

		// Init lums
		for (u32 i = 0; i < countLums; i++)
		{
			ParticleLumsData newLums;

			f32 dirX = Seeder::getSharedSeeder().GetFloat(-1.0f, 1.0f);
			f32 dirY = Seeder::getSharedSeeder().GetFloat(-1.0f, 1.0f);

			newLums.m_speed	= Vec2d(dirX, dirY);
			newLums.m_pos	= Vec2d::Up * (getTemplate()->getGaugeLumsLimits_Y_Min() + 0.001f);
			newLums.m_randomOffsetSpeed =  Seeder::getSharedSeeder().GetFloat(0.8f, 1.2f);

			m_listLums.push_back(newLums);
		}
	}

	// ****************************************************************************

    void Ray_ScoreGuageComponent::onBecomeActive()
    {
		// Init Bones
		if (m_animComponent)
		{
			const StringID boneNameLums = ITF_GET_STRINGID_CRC(B_Lums01,775989530);
			m_indexBone_Lums  = m_animComponent->getBoneIndex(boneNameLums);
			ITF_ASSERT(m_indexBone_Lums != U32_INVALID);
		}

		// Inputs
		if(!m_listenerRegistered)
        {
            m_listenerRegistered = btrue;
            GAMEMANAGER->setInputModeForMenu(btrue);
            GAMEMANAGER->addMenuInputListener(this);
        }

		// Start Cine
		if (m_cineRef.isValid())
		{
			if (Actor* cineAct = m_cineRef.getActor())
			{
                // set channels on cinematic manager depending on players lums score
                sortPlayerByLumScoreForCinematic();

				EventTrigger eventTrig;
				eventTrig.setSender( m_actor->getRef() );
				eventTrig.setActivated(btrue);
				eventTrig.setActivator(m_actor->getRef());
				cineAct->onEvent(&eventTrig);
			}
		}

		if (m_soundComponent)
		{
			m_handleSoundWait = m_soundComponent->playSound(soundWait);
		}


    }

	//*****************************************************************************

	void Ray_ScoreGuageComponent::onResourceReady()
	{
		if(!m_initRef)
		{
			initRef();
		}

		// init bones

		if (m_animComponent)
		{
			m_indexboneCounterA_1  = m_animComponent->getBoneIndex(boneCounterA_1);
			ITF_ASSERT(m_indexboneCounterA_1 != U32_INVALID);
		
			m_indexboneCounterA_2  = m_animComponent->getBoneIndex(boneCounterA_2);
			ITF_ASSERT(m_indexboneCounterA_2 != U32_INVALID);

			m_indexboneCounterA_3  = m_animComponent->getBoneIndex(boneCounterA_3);
			ITF_ASSERT(m_indexboneCounterA_3 != U32_INVALID);

			m_indexboneCounterB_1  = m_animComponent->getBoneIndex(boneCounterB_1);
			ITF_ASSERT(m_indexboneCounterB_1 != U32_INVALID);

			m_indexboneCounterB_2  = m_animComponent->getBoneIndex(boneCounterB_2);
			ITF_ASSERT(m_indexboneCounterB_2 != U32_INVALID);

			m_indexboneCounterB_3  = m_animComponent->getBoneIndex(boneCounterB_3);
			ITF_ASSERT(m_indexboneCounterB_3 != U32_INVALID);

			m_indexboneCounterC_1  = m_animComponent->getBoneIndex(boneCounterC_1);
			ITF_ASSERT(m_indexboneCounterC_1 != U32_INVALID);

			m_indexboneCounterC_2  = m_animComponent->getBoneIndex(boneCounterC_2);
			ITF_ASSERT(m_indexboneCounterC_2 != U32_INVALID);

			m_indexboneCounterC_3  = m_animComponent->getBoneIndex(boneCounterC_3);
			ITF_ASSERT(m_indexboneCounterC_3 != U32_INVALID);

			//
			m_indexboneGlassCut_A  = m_animComponent->getBoneIndex(boneGlassCut_A);
			ITF_ASSERT(m_indexboneGlassCut_A != U32_INVALID);

			m_indexboneGlassCut_B  = m_animComponent->getBoneIndex(boneGlassCut_B);
			ITF_ASSERT(m_indexboneGlassCut_B != U32_INVALID);

			m_indexboneGlassCut_C  = m_animComponent->getBoneIndex(boneGlassCut_C);
			ITF_ASSERT(m_indexboneGlassCut_C != U32_INVALID);
	

		}

	}

	//*****************************************************************************

    void Ray_ScoreGuageComponent::clearListener()
    {
        m_menuValidPressed = bfalse;
        m_menuBackPressed = bfalse;
    }

    //***************************************************************************** 

    void Ray_ScoreGuageComponent::Receive( u32 deviceID /* player */, f32 axis, const StringID& action )
    {
        ITF_ASSERT(m_listenerRegistered);
        if (!m_listenerRegistered)
        {
            return;
        }

        if (GAMEMANAGER->getMainIndexPlayer() != deviceID || GAMEMANAGER->isInPause())
        {
            return; 
        }

        const StringID s_MenuValid = ITF_GET_STRINGID_CRC(MENU_VALID,1084313942);
        const StringID s_MenuBack = ITF_GET_STRINGID_CRC(MENU_BACK,2477582220);


		if (m_scoreStarted && m_scoresFinished >= 4)
		{
			if (action==s_MenuValid)   
			{	
				m_menuValidPressed = btrue;
			}
			else if (action==s_MenuBack)
			{
				m_menuBackPressed = btrue;
			}
		}

    }

    // ****************************************************************************
	
	void Ray_ScoreGuageComponent::startFiesta()
	{
		// Start fiesta !

		m_rewardIndex++;
		m_rewardPlaying = btrue;

		EventTrigger trigger;
		trigger.setSender(m_actor->getRef());
		trigger.setActivated(btrue);

		switch(m_rewardIndex)
		{
		case 1:
			{
				if (!m_mrDarkIsDead)
				{
					playAnim(animReward01);
				}
				m_fadingCounter_A = btrue;
				m_timeFadingCounter_A = RECAP_TIME_FADE_BONE;

				// Sound
				m_soundComponent->stopSound(m_handleSoundMusic1);
				m_handleSoundMusic1 = ITF::SoundHandle::getInvalidHandle();
				m_handleSoundMusic2 = m_soundComponent->playSound(soundMusic2);

				break;
			}
		case 2:
			{
				if (!m_mrDarkIsDead)
				{
					if  (m_totalLumsWin < m_thresholdD)
					{
						playAnim(animReward02_Bis);
					}
					else
					{
						playAnim(animReward02);
					}

				}

				m_fadingCounter_B = btrue;
				m_timeFadingCounter_B = RECAP_TIME_FADE_BONE;

				// Sound
				m_soundComponent->stopSound(m_handleSoundMusic2);
				m_handleSoundMusic2 = ITF::SoundHandle::getInvalidHandle();;
				m_handleSoundMusic3 = m_soundComponent->playSound(soundMusic3);

				break;
			}

		case 3:
			{
				if (!m_mrDarkIsDead)
				{
					playAnim(animReward03);
				}
				m_fadingCounter_C = btrue;
				m_timeFadingCounter_C = RECAP_TIME_FADE_BONE;

				// Sound
				m_soundComponent->stopSound(m_handleSoundMusic3);
				m_handleSoundMusic3 = ITF::SoundHandle::getInvalidHandle();;
				m_handleSoundDisco = m_soundComponent->playSound(soundDisco);

				// Send to disco actor
				for (u32 i = 0; i < m_discoRef.size(); i++)
				{
					if (Actor* actDisco = m_discoRef[i].getActor())
					{
						actDisco->onEvent(&trigger);
					}
				}

				// Send to cine actor
				if (Actor* actCine = m_cineRef.getActor())
				{
					actCine->onEvent(&trigger);
				}

				break;
			}

		}


		// Send to fiesta actor
		for (u32 i = 0; i < m_fiestaRef.size(); i++)
		{
			if (Actor* actFiesta = m_fiestaRef[i].getActor())
			{
				actFiesta->onEvent(&trigger);
			}
		}



	}

	// ****************************************************************************

    void Ray_ScoreGuageComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

		m_trame++;

		// Display player score
		if (!m_scorePlayersDiplayed)
		{
			const StringID eventDisplayScore = ITF_GET_STRINGID_CRC(DisplayScore,955041297);

			const u32 scoreSize = m_scores.size();
			for (u32 i = 0; i < scoreSize; ++i)
			{
				if (Actor* actorScore = m_scores[i].m_scoreRef.getActor())
				{
					EventGeneric eventGeneric;
					eventGeneric.setSender(m_actor->getRef());
					eventGeneric.setId(eventDisplayScore);

					actorScore->onEvent(&eventGeneric);
				}
			}
			m_scorePlayersDiplayed = btrue;
		}

		updateParticles(_dt);

		updateState(_dt);

		if (m_scoreStarted)
		{
			updateAlphaBones(_dt);
		}

        updateAnimInputs();

		updateReward();

		if (m_scoresFinished >= 4)
        {
            if (m_soundHandle != ITF::SoundHandle::getInvalidHandle())
            {
				CONTEXTICONSMANAGER->show(ContextIcon_Retry, ContextIcon_Continue);

				m_soundComponent->stopSound(m_soundHandle);
                m_soundHandle = ITF::SoundHandle::getInvalidHandle();

				if  (m_totalLumsWin < m_thresholdD)
				{
					stopAllSound();
					m_handleSoundWait = m_soundComponent->playSound(soundWait);
				}

            }

            m_waitingTime += _dt;

			if (!m_actionButtonAlreadyPressed)
			{
				if (m_menuValidPressed)
				{

					m_actionButtonAlreadyPressed = btrue;

					stopAllSound();
                    RAY_GAMEMANAGER->commitAndSaveGameState();

					// Special case for level ending : going next will lead you to credits instead of worldmap
					if ( !RAY_GAMEMANAGER->launchCredits() )
					{
						RAY_GAMEMANAGER->resetToWorldMap();
					}

				}
				else if (m_menuBackPressed)
				{

					m_actionButtonAlreadyPressed = btrue;

					stopAllSound();

					// if inside end sequence, then stop it now
					RAY_GAMEMANAGER->onRestartLevelDuringEndSequence();

					// Save game
					RAY_GAMEMANAGER->commitAndSaveGameState();

					StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
					Path path = RAY_GAMEMANAGER->getLevelPath(tag);
					ITF_VERIFY(GAMEMANAGER->loadGameplayMap(path, btrue, btrue));

				}
			}

            clearListener();
        }

        m_lumAdd = 0;

    }


	// ****************************************************************************

	void Ray_ScoreGuageComponent::updateState( f32 _dt )
	{
		switch(m_state)
		{
		case STATE_RECAP_INTRO:
			{
				if (m_animComponent && m_animComponent->isMainNodeFinished())
				{
					if (m_mrDarkIsDead)
					{
						playAnim(animIdle_WithoutDrk);
					}
					else
					{
						playAnim(animIdle);
					}

					startRecap();

					changeState(STATE_RECAP_RELEASING);
				}

				break;
			}
		}

	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::changeState( u32 _newState )
	{
		switch(_newState)
		{
		case STATE_RECAP_INTRO:
			{
				// Play arrived anim
				if (m_mrDarkIsDead)
				{
					playAnim(animArrived_WithoutDrk);
				}
				else
				{
					playAnim(animArrived);
				}
				break;
			}		
		}

		m_state = _newState;

	}

	// ****************************************************************************


	void Ray_ScoreGuageComponent::playAnim( const StringID _anim )
	{
		if (m_animComponent)
		{
			m_animComponent->setAnim(_anim);
		}
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::playAnimOtherAct( ActorRef _actorRef, const StringID _anim )
	{
		if (_actorRef.isValid())
		{
			if (Actor* act = _actorRef.getActor())
			{
				if (AnimLightComponent* animCompo = act->GetComponent<AnimLightComponent>())
				{
					animCompo->setAnim(_anim);
				}  
			}
		}
	}

	// ****************************************************************************

	const Transform2d Ray_ScoreGuageComponent::getTransform2d()
	{
		// Get info bones
		Vec2d posLumsBone(Vec2d::Zero);
		f32   angleLumsBone = 0.0f;

		if (AnimBoneDyn* bone = m_animComponent->getBone(m_indexBone_Lums))
		{
			posLumsBone = bone->m_Pos;
			angleLumsBone = bone->getAngle() - MTH_PIBY2;

			//DebugDraw::circle(posLumsBone, m_actor->getDepth(), 0.1f, Color::red());
			//DebugDraw::circle(posLumsBone, m_actor->getDepth(), 0.3f, Color::red());
		}

		Transform2d worldTransform(posLumsBone, angleLumsBone);

		return worldTransform;
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::updateParticles( f32 _dt )
	{
		const Vec2d actorPos = m_actor->get2DPos();
        //const Transform2d worldTransform = getWorldTransform2d(m_actor);

		f32 maxSpeed = getTemplate()->getMaxSpeedLums();

		//p4 ----- p3
		//|        |
		//|        |
		//|        |
		//|        |
		//|  TUBE  |
		//|        |
		//|        |
		//|        |
		//|        |
		//|        |
		//p1 ----- p2

		const Transform2d worldTransform = getTransform2d();

		m_tubeP1 = Vec2d::Right * getTemplate()->getGaugeLumsLimits_X_Min() + Vec2d::Up * getTemplate()->getGaugeLumsLimits_Y_Min();
		m_tubeP2 = Vec2d::Right * getTemplate()->getGaugeLumsLimits_X_Max() + Vec2d::Up * getTemplate()->getGaugeLumsLimits_Y_Min();
		m_tubeP3 = Vec2d::Right * getTemplate()->getGaugeLumsLimits_X_Max() + Vec2d::Up * getTemplate()->getGaugeLumsLimits_Y_Min() + Vec2d::Up * (getTemplate()->getGaugeLumsLimits_Y_Max() * m_lumCursor);
		m_tubeP4 = Vec2d::Right * getTemplate()->getGaugeLumsLimits_X_Min() + Vec2d::Up * getTemplate()->getGaugeLumsLimits_Y_Min() + Vec2d::Up * (getTemplate()->getGaugeLumsLimits_Y_Max() * m_lumCursor);

		m_tubeP1 = worldTransform.transformPos(m_tubeP1);
		m_tubeP2 = worldTransform.transformPos(m_tubeP2);
		m_tubeP3 = worldTransform.transformPos(m_tubeP3);
		m_tubeP4 = worldTransform.transformPos(m_tubeP4);

		f32 height = (m_tubeP3 - m_tubeP2).norm();
		f32 width  = (m_tubeP3 - m_tubeP4).norm();

		Vec2d right = (m_tubeP3 - m_tubeP2).normalize();
		Vec2d left  = (m_tubeP4 - m_tubeP1).normalize();
		Vec2d up    = (m_tubeP3 - m_tubeP4).normalize();
		Vec2d down  = (m_tubeP2 - m_tubeP1).normalize();

#ifdef ITF_SUPPORT_EDITOR
// DEBUG
		//DebugDraw::circle(m_tubeP1, m_actor->getDepth(), 0.1f, Color::red());
		//DebugDraw::circle(m_tubeP2, m_actor->getDepth(), 0.1f, Color::green());
		//DebugDraw::circle(m_tubeP3, m_actor->getDepth(), 0.1f, Color::blue());
		//DebugDraw::circle(m_tubeP4, m_actor->getDepth(), 0.1f, Color::yellow());

		//DebugDraw::line2D(m_tubeP1, m_tubeP2, m_actor->getDepth(), Color::red(), 3.0f);
		//DebugDraw::line2D(m_tubeP2, m_tubeP3, m_actor->getDepth(), Color::red(), 3.0f);
		//DebugDraw::line2D(m_tubeP3, m_tubeP4, m_actor->getDepth(), Color::red(), 3.0f);
		//DebugDraw::line2D(m_tubeP4, m_tubeP1, m_actor->getDepth(), Color::red(), 3.0f);

// DEBUG
#endif

		// Update animation trame ?
		u32 mod = m_trame%2;
		u8 addIndexAtlas = 0;
		if (mod == 0)
		{
			addIndexAtlas = 1;
		}

		const u32 countObj = m_listLums.size();
		for (u32 i = 0; i < countObj; i++)
		{
			ParticleLumsData& curObj = m_listLums[i];

			// Rotate
			curObj.m_angle += MTH_PI * 0.05f; // piBy20

			// Max speed
			f32 curMaxSpeed = maxSpeed * curObj.m_randomOffsetSpeed;

			// Update animation
			curObj.m_curIndexAtlas += addIndexAtlas;
			curObj.m_curIndexAtlas = curObj.m_curIndexAtlas%m_countAtlasUv;

			if (curObj.m_speed.sqrnorm() > curMaxSpeed * curMaxSpeed)
			{
			    curObj.m_speed = curObj.m_speed.normalize() * curMaxSpeed;
			}

			Vec2d posTemp = curObj.m_pos + curObj.m_speed;
			
			posTemp = worldTransform.transformPos(posTemp);

			// UP / DOWN
			Vec2d p2ToPos = posTemp - m_tubeP2;
			f32 dotRight = right.dot(p2ToPos);
			if (dotRight <= 0.0f || dotRight >= height)
			{
				curObj.m_speed.m_y = -curObj.m_speed.m_y;
			}

			// RIGHT / LEFT
			Vec2d p1ToPos = posTemp - m_tubeP1;
			f32 dotDown = down.dot(p1ToPos);
			if (dotDown <= 0.0f || dotDown >= width)
			{
				curObj.m_speed.m_x = -curObj.m_speed.m_x;
			}

			if (curObj.m_speed.sqrnorm() > curMaxSpeed * curMaxSpeed)
			{
				curObj.m_speed = curObj.m_speed.normalize() * curMaxSpeed;
			}		

			curObj.m_pos += curObj.m_speed;
		}

	}

	// ****************************************************************************

    void Ray_ScoreGuageComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (Ray_EventScoreGuageAddLum * onAddLums = _event->DynamicCast<Ray_EventScoreGuageAddLum>(ITF_GET_STRINGID_CRC(Ray_EventScoreGuageAddLum,2833738877)))
        {
            m_lumAdd += onAddLums->getNumLums();
        }
        else if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
			if (m_scoreStarted)
			{
				m_scoresFinished++;
			}

			if (m_state == STATE_RECAP_DISABLED)
			{
				changeState(STATE_RECAP_INTRO);
			}
        }
		else if (Ray_EventMedalNonCinematicEnded * onMedalCineEnd = _event->DynamicCast<Ray_EventMedalNonCinematicEnded>(ITF_GET_STRINGID_CRC(Ray_EventMedalNonCinematicEnded,694861796)))
		{
			startFiesta();
		}
	

    }

	// ****************************************************************************

	void Ray_ScoreGuageComponent::startRecap()
	{

		u32 lumCount = 0;
		RewardManager::GameSession *gameSession = REWARD_MANAGER->Session_GetPrevious();
		if (gameSession)
		{
			//compute lum count
			for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
			{
				if ( REWARD_MANAGER->GetIsPlayerActif(gameSession, i) )
				{
                    lumCount += RAY_GAMEMANAGER->getCurrentScore().getLums(i);
				}
			}
		}

		m_totalLumsWin = lumCount;

		u32 bestLumAttack = RAY_GAMEMANAGER->getCurrentLevelBestLumAttack();
		if ((i32)bestLumAttack<lumCount)
		{
			RAY_GAMEMANAGER->setCurrentLevelBestLumAttack((u32)lumCount);
		}

		if (!m_scoreStarted)
		{
			m_scoreStarted = btrue;

			m_lumCount = 0;
			m_maxLumCount = 0;
			m_lumAdd = 0;
			m_thresholdB = 0;
			m_thresholdC = 0;
			m_thresholdD = 0;
			m_scoresFinished = 0;

			init();

			startArrival();

			const u32 scoreSize = m_scores.size();
			for (u32 i = 0; i < scoreSize; ++i)
			{
				EventTrigger trigger;
				trigger.setSender(m_actor->getRef());
				trigger.setActivated(btrue);

				Actor * actor = m_scores[i].m_scoreRef.getActor();
				if (actor)
				{
					actor->onEvent(&trigger);
				}
			}
		}

	}
	// ****************************************************************************

	void Ray_ScoreGuageComponent::pauseLumsReleasing()
	{
		const StringID eventPauseReleasing = ITF_GET_STRINGID_CRC(PauseReleasing,3938703612);

        m_soundComponent->stopSound(m_soundHandle);

		const u32 scoreSize = m_scores.size();
		for (u32 i = 0; i < scoreSize; ++i)
		{
			EventGeneric evtGen;
			evtGen.setSender(m_actor->getRef());
			evtGen.setId(eventPauseReleasing);
			
			if (Actor* actor = m_scores[i].m_scoreRef.getActor())
			{
				actor->onEvent(&evtGen);
			}
		}
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::resumeLumsReleasing()
	{
		const StringID eventResumeReleasing = ITF_GET_STRINGID_CRC(ResumeReleasing,26409933);

		m_soundHandle = m_soundComponent->playSound(s_MRK_Lum_Jauge);

		const u32 scoreSize = m_scores.size();
		for (u32 i = 0; i < scoreSize; ++i)
		{
			EventGeneric evtGen;
			evtGen.setSender(m_actor->getRef());
			evtGen.setId(eventResumeReleasing);

			if (Actor* actor = m_scores[i].m_scoreRef.getActor())
			{
				actor->onEvent(&evtGen);
			}
		}
	}

    // ****************************************************************************

    void Ray_ScoreGuageComponent::sortPlayerByLumScoreForCinematic()
    {
        ITF_VECTOR< std::pair<u32,i32> > sortedPlayers;
        for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
        {
            if ( RAY_GAMEMANAGER->getPlayer(i) && RAY_GAMEMANAGER->getPlayer(i)->getActiveAndPersistent() )
                sortedPlayers.push_back(std::pair<u32,i32>(i,static_cast<i32>(RAY_GAMEMANAGER->getCurrentScore().getLums(i))));
            else
                sortedPlayers.push_back(std::pair<u32,i32>(i, -1));
        }

        // Sort players by lums score
        std::sort(sortedPlayers.begin(), sortedPlayers.end(), AIUtils::sortPlayersByScore);

        for ( u32 i = 0 ; i < sortedPlayers.size() ; ++i )
        {
            CINEMATICMANAGER->setPlayerChannel(sortedPlayers[i].first, i);
        }
    }

	// ****************************************************************************

    void Ray_ScoreGuageComponent::startArrival()
    {
        m_arrivalInProgress = btrue;

		if (m_soundComponent)
		{
			m_soundHandle = m_soundComponent->playSound(s_MRK_Lum_Jauge);
			m_soundComponent->stopSound(m_handleSoundWait);
			m_handleSoundWait = ITF::SoundHandle::getInvalidHandle();	

			m_handleSoundMusic1 = m_soundComponent->playSound(soundMusic1);
		
		}
    }

	// ****************************************************************************

    void Ray_ScoreGuageComponent::init()
    {
        World *world = CURRENTWORLD;
        if (!world)
            return;
        Scene *scene = world->getRootScene();
        if (!scene)
            return;

        StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
        StringID type = RAY_GAMEMANAGER->getMapConfigType(tag);
#ifndef ITF_FINAL
        if ( !type.isValid() )
            type = "platformer";
#endif // !ITF_FINAL


            m_thresholdB = RAY_GAMEMANAGER->getLumAttackThreshold(tag, 0);
            m_thresholdC = RAY_GAMEMANAGER->getLumAttackThreshold(tag, 1);
            m_thresholdD = RAY_GAMEMANAGER->getLumAttackThreshold(tag, 2);

            m_maxLumCount = m_thresholdD;
            m_lumCount = 0;

            const StringID s_thresholdB = ITF_GET_STRINGID_CRC(Capacity_B,3663230776);
            const StringID s_thresholdC = ITF_GET_STRINGID_CRC(Capacity_C,2285436900);
            const StringID s_thresholdD = ITF_GET_STRINGID_CRC(Capacity_D,3233638897);

            String8 str;
            str.setTextFormat("%d",m_thresholdB);
            m_uiTextBoxesComponent->setText(s_thresholdB,str);
            str.setTextFormat("%d",m_thresholdC);
            m_uiTextBoxesComponent->setText(s_thresholdC,str);
            str.setTextFormat("%d",m_thresholdD);
            m_uiTextBoxesComponent->setText(s_thresholdD,str);
    }

	// ****************************************************************************

	void Ray_ScoreGuageComponent::initRef()
	{
		// Init other actor ref
		StringID valueTag; 

		if (m_linkComponent)
		{
			const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
			for (u32 i = 0; i < children.size(); i++)
			{
				Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

				if ( pickable )
				{
					Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

					if (childActor)
					{
						if (children[i].getTagValue(nameCineTag, valueTag) && !m_cineRef.isValid())
						{
							m_cineRef = childActor->getRef();                            
						}    
						if (children[i].getTagValue(nameTrophyTag, valueTag) && !m_trophyRef.isValid())
						{
							m_trophyRef = childActor->getRef();                            
						}  
						if (children[i].getTagValue(nameDiscoTag, valueTag))
						{
							m_discoRef.push_back(childActor->getRef());
						}  
						if (children[i].getTagValue(nameScoreTag, valueTag))
						{
							ScorePlayer newScorePlayer;
							newScorePlayer.m_scoreRef = childActor->getRef();

							m_scores.push_back(newScorePlayer);
						}    
						if (children[i].getTagValue(nameFiestaTag, valueTag))
						{
							m_fiestaRef.push_back(childActor->getRef());
						}    
					}
				}
			}
		}
		
		m_initRef = btrue;
	}

	// ****************************************************************************

    void Ray_ScoreGuageComponent::activateMedal(u32 _stage)
    {
        {
            Ray_EventShowElectoonMedal showMedal;

            switch(_stage)
            {
           case 0: //LUM ATTACK 1. WILL MAKE LOADS OF LUM
                {
                    ITF_ASSERT(REWARD_MANAGER->Session_GetPrevious());
                    showMedal.m_forBrokenCage = bfalse;
                    showMedal.m_forLumAndTimeAttacks = btrue;
                    showMedal.m_dontSetLumAttack1 = bfalse;
                    showMedal.m_dontSetLumAttack2 = btrue;
                    showMedal.m_dontSetTimeAttack = btrue;
                    showMedal.m_currentlyEmphasizedChallenge = CHALLENGE_LUMATTACK1;
                } break;
           case 1: //LUM ATTACK 2. WILL MAKE LOADS OF LUM
               {
                   ITF_ASSERT(REWARD_MANAGER->Session_GetPrevious());
                   showMedal.m_forBrokenCage = bfalse;
                   showMedal.m_forLumAndTimeAttacks = btrue;
                   showMedal.m_dontSetLumAttack2 = bfalse;
                   showMedal.m_dontSetLumAttack1 = btrue;
                   showMedal.m_dontSetTimeAttack = btrue;
                   showMedal.m_currentlyEmphasizedChallenge = CHALLENGE_LUMATTACK2;

               } break;
           case 2: //
               {
                    //TODO MANAGE SPECIAL MEDAL

                   //ITF_ASSERT(REWARD_MANAGER->Session_GetPrevious());
                   //REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetLums, 0, 500);
                   //showMedal.m_forBrokenCage = bfalse;
                   //showMedal.m_forLumAndTimeAttacks = btrue;
                   //showMedal.m_dontSetLumAttack2 = bfalse;
                   //showMedal.m_dontSetLumAttack1 = btrue;
                   //showMedal.m_dontSetTimeAttack = btrue;
                   //showMedal.m_currentlyEmphasizedChallenge = CHALLENGE_LUMATTACK2;
               } break;
            }

            showMedal.setSender(m_actor->getRef());
            EVENTMANAGER->broadcastEvent(&showMedal);

        }
    }

	// ****************************************************************************

	void Ray_ScoreGuageComponent::setAlphaBone(u32 _indexBone, f32 _alpha)
	{
		if (m_animComponent)
		{
			if (AnimBoneDyn* bone = m_animComponent->getBone(_indexBone))
			{
				bone->m_Alpha = _alpha;
			}
		}
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::updateAlphaBones(f32 _dt)
	{
		f32 multTimeTotal = 1.0f / RECAP_TIME_FADE_BONE;

		if (m_fadingCounter_A)
		{		
			m_timeFadingCounter_A -= _dt;

			if (m_timeFadingCounter_A > 0.0f)
			{
				m_alphaCounter_A = m_timeFadingCounter_A * multTimeTotal;
			}
			else
			{
				m_alphaCounter_A = 0.0f;
				m_fadingCounter_A = bfalse;
			}
		}

		if (m_fadingCounter_B)
		{
			m_timeFadingCounter_B -= _dt;

			if (m_timeFadingCounter_B > 0.0f)
			{
				m_alphaCounter_B = m_timeFadingCounter_B * multTimeTotal;
			}
			else
			{
				m_alphaCounter_B = 0.0f;
				m_fadingCounter_B = bfalse;
			}
		}

		if (m_fadingCounter_C)
		{
			m_timeFadingCounter_C -= _dt;

			if (m_timeFadingCounter_C > 0.0f)
			{
				m_alphaCounter_C = m_timeFadingCounter_C * multTimeTotal;
			}
			else
			{
				m_alphaCounter_C = 0.0f;
				m_fadingCounter_C = bfalse;
			}
		}
	
		setAlphaBone(m_indexboneCounterA_1, m_alphaCounter_A);
		setAlphaBone(m_indexboneCounterA_2, m_alphaCounter_A);
		setAlphaBone(m_indexboneCounterA_3, m_alphaCounter_A);

		setAlphaBone(m_indexboneCounterB_1, m_alphaCounter_B);
		setAlphaBone(m_indexboneCounterB_2, m_alphaCounter_B);
		setAlphaBone(m_indexboneCounterB_3, m_alphaCounter_B);

		setAlphaBone(m_indexboneCounterC_1, m_alphaCounter_C);
		setAlphaBone(m_indexboneCounterC_2, m_alphaCounter_C);
		setAlphaBone(m_indexboneCounterC_3, m_alphaCounter_C);
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::updateReward()
	{
		// Flashs
		if (m_lumCount < m_thresholdB - RECAP_OFFSET_LUMS_FLASH && ((m_lumCount + m_lumAdd) >= m_thresholdB - RECAP_OFFSET_LUMS_FLASH))
		{
			playFxFlash(m_indexboneGlassCut_A);
		}
		else if (m_lumCount < m_thresholdC - RECAP_OFFSET_LUMS_FLASH && ((m_lumCount + m_lumAdd) >= m_thresholdC - RECAP_OFFSET_LUMS_FLASH))
		{
			playFxFlash(m_indexboneGlassCut_B);
		}
		else if (m_lumCount < m_thresholdD - RECAP_OFFSET_LUMS_FLASH && ((m_lumCount + m_lumAdd) >= m_thresholdD - RECAP_OFFSET_LUMS_FLASH))
		{
			playFxFlash(m_indexboneGlassCut_C);
		}


		// Rewards
		if (m_lumCount < m_thresholdB && ((m_lumCount + m_lumAdd) >= m_thresholdB))
		{
			activateMedal(0);
		}
		else if (m_lumCount < m_thresholdC && ((m_lumCount + m_lumAdd) >= m_thresholdC))
		{
			activateMedal(1);
		}
		else if (m_lumCount < m_thresholdD && ((m_lumCount + m_lumAdd) >= m_thresholdD))
		{
			RAY_GAMEMANAGER->setChallengeWon(CHALLENGE_LUMATTACK3);
			const StringID animAppearTrophy = ITF_GET_STRINGID_CRC(Appear,1975175726);
			playAnimOtherAct(m_trophyRef, animAppearTrophy);

			startFiesta();
		}

		// Indicator
		const StringID s_indicator = ITF_GET_STRINGID_CRC(Indicator_D,969763102);
		m_lumCount += m_lumAdd;
		String8 str;
		if (m_lumCount == 0)
		{
			str.setTextFormat("");
		}
		else 
		{
			str.setTextFormat("%d",m_lumCount);
		}

		m_uiTextBoxesComponent->setText(s_indicator,str);
	}

	// ****************************************************************************

	void Ray_ScoreGuageComponent::playFxFlash(u32 _indexBone)
	{
		f32 zActor = m_actor->getDepth();
		Vec2d posFlash(Vec2d::Zero);

		if (m_animComponent)
		{
			if (AnimBoneDyn* bone = m_animComponent->getBone(_indexBone))
			{
				posFlash = bone->m_Pos;
				u32 handleFx = m_fxControllerComponent->playFX(fxNameFlash);
				m_fxControllerComponent->setFXPosFromHandle(handleFx, posFlash.to3d(zActor));

				if (m_soundComponent)
				{
					auto handleSound =  m_soundComponent->playSound(soundFlash);

					m_soundComponent->setSoundPos(handleSound, posFlash.to3d(zActor));
				}
			}
		}
	}

	// ****************************************************************************

    void Ray_ScoreGuageComponent::updateAnimInputs()
    {
        if (m_maxLumCount != 0)
        {
            const StringID s_lums("lums");

            const StringID s_threshold_b = ITF_GET_STRINGID_CRC(threshold_b,467123950);
            const StringID s_threshold_c = ITF_GET_STRINGID_CRC(threshold_c,927270300);
            const StringID s_threshold_d = ITF_GET_STRINGID_CRC(threshold_d,2918143512);

			const StringID s_glasscut_a = ITF_GET_STRINGID_CRC(glasscut_a,3871191716);
			const StringID s_glasscut_b = ITF_GET_STRINGID_CRC(glasscut_b,2227474587);


            m_lumCursor = static_cast<f32>(f32(m_lumCount)/f32(m_maxLumCount));
			m_lumCursor = Clamp(m_lumCursor, 0.0f, 1.0f);

            m_thresholdBCursor = m_thresholdBCursor +( static_cast<f32>(f32(m_thresholdB)/f32(m_maxLumCount)) - m_thresholdBCursor) * getTemplate()->getGuageSmooth();
            m_thresholdCCursor = m_thresholdCCursor +( static_cast<f32>(f32(m_thresholdC)/f32(m_maxLumCount)) - m_thresholdCCursor) * getTemplate()->getGuageSmooth();
            m_thresholdDCursor = m_thresholdDCursor +( static_cast<f32>(f32(m_thresholdD)/f32(m_maxLumCount)) - m_thresholdDCursor) * getTemplate()->getGuageSmooth();

            m_soundComponent->setInput(s_lums,m_lumCursor);


			//m_lumCursorSmooth = m_lumCursorSmooth + (m_lumCursor - m_lumCursorSmooth) * getTemplate()->getGuageSmooth();
            		
			m_animComponent->setInput(s_lums, m_lumCursor);
            			
			m_animComponent->setInput(s_threshold_b,m_thresholdBCursor);
            m_animComponent->setInput(s_threshold_c,m_thresholdCCursor);
            m_animComponent->setInput(s_threshold_d,m_thresholdDCursor);
			m_animComponent->setInput(s_glasscut_a,m_thresholdBCursor);
			m_animComponent->setInput(s_glasscut_b,m_thresholdCCursor);

        }
    }

    //*****************************************************************************

    void Ray_ScoreGuageComponent::Draw()
    {
        Super::Draw();

		if (m_lumCursor == 0.0f)
		{
			return;
		}

		const Transform2d worldTransform = getTransform2d();

		const u32 countObj = m_listLums.size();

		const Vec2d posActor = m_actor->get2DPos();
		const f32 zActor = m_actor->getDepth();

		f32 semiWidthEdge = getTemplate()->getSizeLums() * 0.5f;
		f32 semiHeightEdge = getTemplate()->getSizeLums() * 0.5f;

		Vec2d decalPoint1(-semiWidthEdge, semiHeightEdge);
		Vec2d decalPoint2(-semiWidthEdge, -semiHeightEdge);
		Vec2d decalPoint3(semiWidthEdge, -semiHeightEdge);
		Vec2d decalPoint4(semiWidthEdge, semiHeightEdge);

		for (u32 i = 0; i < countObj; i++)
		{
			const ParticleLumsData& curObj = m_listLums[i];

			//p1 ----- p4
			//|        |
			//|        |
			//p2 ----- p3

			Vec2d curObjPos = worldTransform.transformPos(curObj.m_pos);

			Vec2d p1 = curObjPos + decalPoint1; 
			Vec2d p2 = curObjPos + decalPoint2; 
			Vec2d p3 = curObjPos + decalPoint3; 
			Vec2d p4 = curObjPos + decalPoint4; 

			// Orientation
			f32 c,s;
			f32_CosSin(curObj.m_angle,&c,&s);
			p1 = p1.RotateAroundCS(curObjPos, c,s );
			p2 = p2.RotateAroundCS(curObjPos, c,s );
			p3 = p3.RotateAroundCS(curObjPos, c,s );
			p4 = p4.RotateAroundCS(curObjPos, c,s );

			m_atlas.addQuad(curObj.m_curIndexAtlas, p4, p3, p2, p1, 0.0f); 

		}

		m_atlas.draw(zActor - 0.01f);

    }


#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_ScoreGuageComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
    }
#endif // ITF_SUPPORT_EDITOR


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_ScoreGuageComponent_Template)

    BEGIN_SERIALIZATION(Ray_ScoreGuageComponent_Template)
		SERIALIZE_MEMBER("textureLums", m_textureLums);

		SERIALIZE_MEMBER("countLums", m_countLums);
		SERIALIZE_MEMBER("gaugeLumsLimits_X_Min", m_gaugeLumsLimits_X_Min);
		SERIALIZE_MEMBER("gaugeLumsLimits_X_Max", m_gaugeLumsLimits_X_Max);
		SERIALIZE_MEMBER("gaugeLumsLimits_Y_Min", m_gaugeLumsLimits_Y_Min);
		SERIALIZE_MEMBER("gaugeLumsLimits_Y_Max", m_gaugeLumsLimits_Y_Max);
		SERIALIZE_MEMBER("sizeLums", m_sizeLums);
		SERIALIZE_MEMBER("maxSpeedLums", m_maxSpeedLums);
		SERIALIZE_MEMBER("gaugeOffsetY", m_gaugeOffsetY);
		
		SERIALIZE_MEMBER("arrivalAnim",	m_arrivalAnim);
		SERIALIZE_MEMBER("idleAnim",	m_idleAnim);
		SERIALIZE_MEMBER("guageSmooth",	m_guageSmooth);
		SERIALIZE_MEMBER("waitingTime",	m_waitingTime);

		SERIALIZE_MEMBER("checkMapForMrDark",	m_checkMapForMrDark);


    END_SERIALIZATION()

    //*****************************************************************************

    Ray_ScoreGuageComponent_Template::Ray_ScoreGuageComponent_Template()
    : m_guageSmooth(0.1f)
    , m_waitingTime(5.0f)
	, m_countLums(10)
	, m_gaugeLumsLimits_X_Min(-0.5f)
	, m_gaugeLumsLimits_X_Max(0.5f)
	, m_gaugeLumsLimits_Y_Min(0.0f)
	, m_gaugeLumsLimits_Y_Max(4.0f)
	, m_sizeLums(0.3f)
	, m_maxSpeedLums(0.05f)
	, m_gaugeOffsetY(0.0f)
    {
        // none
    }
    
    //*****************************************************************************
    
    Ray_ScoreGuageComponent_Template::~Ray_ScoreGuageComponent_Template()
    {
        // none
    }

    //*****************************************************************************

};
