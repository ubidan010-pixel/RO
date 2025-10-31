#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ELEVATORAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorAIComponent.h"
#endif //_ITF_Ray_ElevatorAIComponent_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_


#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#include "engine/actors/components/SoundComponent.h"

// States
#define STATE_STAND           0

#define STATE_EXPLODE_01      1
#define STATE_STAND_01        2

#define STATE_EXPLODE_02      3
#define STATE_STAND_02        4

#define STATE_EXPLODE_03      5
#define STATE_STAND_03        6

#define STATE_FULL_BREAK      7

// Defines
#define EPSILON_ELEVATOR            0.5f
#define EPSILON_NOT_ANIM_SHAKE      0.01f

 
namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_ElevatorAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ElevatorAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ElevatorAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "Ray_ElevatorAIComponent requires an AnimLightComponent/AnimatedComponent.");
        VALIDATE_COMPONENT_PARAM("", m_soundComponent,    "Ray_ElevatorAIComponent requires an SoundComponent.");
        VALIDATE_COMPONENT_PARAM("", m_polylineComponent, "Ray_ElevatorAIComponent requires an PolylineComponent.");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_ElevatorAIComponent::Ray_ElevatorAIComponent()
    :
    m_animatedComponent(NULL),
    m_soundComponent(NULL),
    m_polylineComponent(NULL),
    m_currentProgress(0.0f),
    m_currentProgressOld(0.0f),
    m_minClampProgress(0.0f),
    m_force(0.0f),
    m_hitRecieved(bfalse),
    m_state(STATE_STAND),
    m_isJustTopDown(bfalse),
    m_currentWeight(0.0f)
	, m_stuckTop(bfalse)
	, m_timeStuckTop(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_ElevatorAIComponent::~Ray_ElevatorAIComponent()
    {
        // none
    }

    //*****************************************************************************
 
    void Ray_ElevatorAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);

        // Get components
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_soundComponent    = m_actor->GetComponent<SoundComponent>();
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();

        
        m_state = STATE_STAND;
    }

    // ****************************************************************************

    void Ray_ElevatorAIComponent::onResourceReady()
    {
        Super::onResourceReady();

        // Get bone for Monster detection distance.
        m_boneDetect = BoneID(ITF_GET_STRINGID_CRC(B_Detection,3114309993));
        m_boneDetect.init(m_animatedComponent);

    }

    //*****************************************************************************

    void Ray_ElevatorAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

		if (m_stuckTop)
		{
			m_timeStuckTop -= _dt;
			if (m_timeStuckTop <= 0.0f)
			{
				m_stuckTop = bfalse;
			}
		}


        // Stand Anim
        const StringID animStand        = ITF_GET_STRINGID_CRC(Stand,2617849515);
        const StringID animStand01      = ITF_GET_STRINGID_CRC(Stand01,147266583);
        const StringID animStand02      = ITF_GET_STRINGID_CRC(Stand02,55218973);

        // Explode Anim
        const StringID animExplode01    = ITF_GET_STRINGID_CRC(Explode01,3931734884);
        const StringID animExplode02    = ITF_GET_STRINGID_CRC(Explode02,2144425855);
        const StringID animExplode03    = ITF_GET_STRINGID_CRC(Explode03,1192668445);

        // Shake Anim
        const StringID animShake        = ITF_GET_STRINGID_CRC(Shake,3500416584);
        const StringID animShake01      = ITF_GET_STRINGID_CRC(Shake01,896417442);
        const StringID animShake02      = ITF_GET_STRINGID_CRC(Shake02,2198143426);

        // Landing Anim
        const StringID animLanding      = ITF_GET_STRINGID_CRC(Landing,3985928854);
        const StringID animLanding01    = ITF_GET_STRINGID_CRC(Landing01,3142153242);
        const StringID animLanding02    = ITF_GET_STRINGID_CRC(Landing02,3710940058);


        const StringID inputSound       = ITF_GET_STRINGID_CRC(Speed,4218325830);
        const StringID s_Quake1         = ITF_GET_STRINGID_CRC(Quake1,3041659572);

        f32 weightOnElevator = m_polylineComponent->getWeight();

        // ### Update animation state
        switch(m_state)
        {
            case STATE_STAND:
                if (m_hitRecieved)
                {
                    m_animatedComponent->setAnim(animExplode01);
                    CAMERACONTROLLERMANAGER->addShake(s_Quake1);
                    m_state = STATE_EXPLODE_01;
                }
                else if(m_isJustTopDown)
                {
                    m_animatedComponent->resetCurTime();
                    m_animatedComponent->setAnim(animShake);
                    m_isJustTopDown = bfalse; 
                }
                else if (weightOnElevator > m_currentWeight)
                {
                    m_animatedComponent->resetCurTime();
                    m_animatedComponent->setAnim(animLanding);  
                }
                break;

            //------------------------------------------------------
                
            case STATE_EXPLODE_01:
                if (m_animatedComponent->isMainNodeFinished())
                {
                    m_animatedComponent->setAnim(animStand01);
                    m_state = STATE_STAND_01;
                }
                break;

            //------------------------------------------------------

            case STATE_STAND_01:
                if (m_hitRecieved)
                {
                    m_animatedComponent->setAnim(animExplode02);
                    CAMERACONTROLLERMANAGER->addShake(s_Quake1);
                    m_state = STATE_EXPLODE_02;
                }
                else if(m_isJustTopDown)
                {
                    m_animatedComponent->resetCurTime();
                    m_animatedComponent->setAnim(animShake01);
                    m_isJustTopDown = bfalse;
                }
                else if (weightOnElevator > m_currentWeight)
                {
                    m_animatedComponent->resetCurTime();
                    m_animatedComponent->setAnim(animLanding01);  
                }
                break;

            //------------------------------------------------------

            case STATE_EXPLODE_02:
                if (m_animatedComponent->isMainNodeFinished())
                {
                    m_animatedComponent->setAnim(animStand02);
                    m_state = STATE_STAND_02;
                }
                break;

            //------------------------------------------------------

            case STATE_STAND_02:
                if (m_hitRecieved)
                {
                    m_animatedComponent->setAnim(animExplode03);
                    CAMERACONTROLLERMANAGER->addShake(s_Quake1);
                    m_state = STATE_EXPLODE_03;
                }
                else if(m_isJustTopDown)
                {
                    m_animatedComponent->resetCurTime();
                    m_animatedComponent->setAnim(animShake02);
                    m_isJustTopDown = bfalse;
                }
                else if (weightOnElevator > m_currentWeight)
                {
                    m_animatedComponent->resetCurTime();
                    m_animatedComponent->setAnim(animLanding02);  
                }
                break;

            //------------------------------------------------------

            case STATE_EXPLODE_03:
                if (m_animatedComponent->isMainNodeFinished())
                {
                    m_state = STATE_FULL_BREAK;  
                    m_force = 0.0f;
                    m_currentProgress = 0.0f;
                    m_soundComponent->setInput(inputSound, 0.0f);
                }
                break;
        }

        m_currentWeight = weightOnElevator;

        if (m_state != STATE_FULL_BREAK)
        {

            if (m_hitRecieved)
            {
                m_force = getTemplate()->getForceHitMonster();
                m_hitRecieved = bfalse;
            }

            // ### Update vertical progression
            // Force Up
            //if (m_countForce > 0)
            //{
            //    m_force += m_countForce * getTemplate()->getForceUpPerHit();
            //    m_countForce = 0;
            //}

            // Force Down (Gravity)
            if(m_currentProgress > 0 && !m_stuckTop)
            { 
                m_force -= getTemplate()->getForceGravity();
            }

            // Clamp force

            m_force = Clamp(m_force, getTemplate()->getClampForceMin(), getTemplate()->getClampForceMax());


            m_currentProgress += m_force * _dt;


            // Compute input
            m_currentProgress = Clamp(m_currentProgress, 0.0f, getTemplate()->getDistanceMax());


            // When we arrived on Top or Down
            if (m_currentProgress != m_currentProgressOld)
            {
                if (f32_Abs(m_force) >= EPSILON_ELEVATOR)
                {
                    if ( m_currentProgress <= 0.0f)
                    {
                        m_force = -m_force * getTemplate()->getReboundPercentForce();

                        if (f32_Abs(m_force) >= EPSILON_NOT_ANIM_SHAKE)
                        {
                            m_isJustTopDown = btrue;
                        }
                    }
                    else if (m_currentProgress >= getTemplate()->getDistanceMax() && !m_stuckTop)
                    {
						m_stuckTop = btrue;
						m_timeStuckTop = getTemplate()->getTimeStuckTop();

                        //m_force = -m_force * getTemplate()->getReboundPercentForce();

                        if (f32_Abs(m_force) >= EPSILON_NOT_ANIM_SHAKE)
                        {
                           m_isJustTopDown = btrue;
                        }
                    }
                }
            }
            m_currentProgressOld = m_currentProgress;

            const StringID  inputMove = ITF_GET_STRINGID_CRC(ElevatorMoveCursor,3210582188);
            m_animatedComponent->setInput(inputMove, m_currentProgress);   

            // Sound
            if (m_soundComponent)
            {
                m_boneDetectCurPos = getBoneDetectPos();
                f32 speed = f32_Abs((m_boneDetectPrevPos - m_boneDetectCurPos).norm()) / _dt;
                m_soundComponent->setInput(inputSound, speed);
                m_boneDetectPrevPos = m_boneDetectCurPos;
#ifndef ITF_FINAL
                //DebugDraw::text("Speed : %f", speed);
#endif // ITF_FINAL
            }

        } // if m_state != STATE_FULL_BREAK

#ifndef ITF_FINAL
        // Debug
        //DebugDraw::text("Force : %f", m_force);
        //DebugDraw::text("Progress : %f", m_currentProgress);
#endif // ITF_FINAL
    }

    // ****************************************************************************

    void Ray_ElevatorAIComponent::addForce(u32 _level)
    {
        //m_countForce++;

        f32 forceAdd = 0.0;
        switch (_level)
        {
            case 0 :
             forceAdd = getTemplate()->getForceUpPerHit_Level_0();
             break;

            case 1 :
                forceAdd = getTemplate()->getForceUpPerHit_Level_1();
                break;

            case 2 :
                forceAdd = getTemplate()->getForceUpPerHit_Level_2();
                break;

        }

        m_force += forceAdd;
    }

    // ****************************************************************************

    void Ray_ElevatorAIComponent::hit( )
    {
        m_hitRecieved = btrue; 
    }

    // ****************************************************************************

    Vec2d Ray_ElevatorAIComponent::getBoneDetectPos()
    {
        ITF_WARNING(m_actor, m_boneDetect.isValid(), "My bone detect is invalid !");

        Vec2d posBoneDetect;
        m_animatedComponent->getBonePos(m_boneDetect.getIndex(), posBoneDetect);

        return posBoneDetect;
    }

    // ****************************************************************************

    bbool Ray_ElevatorAIComponent::getLaunchPursuit() const
    {
        f32 pourcentProgress = m_currentProgress / getTemplate()->getDistanceMax();

        return pourcentProgress >= getTemplate()->getPercentLaunchMonsterAttack();
    }

	//*****************************************************************************
	// Template
	//*****************************************************************************
	
	IMPLEMENT_OBJECT_RTTI(Ray_ElevatorAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_ElevatorAIComponent_Template)
		SERIALIZE_MEMBER("timeStuckTop",                m_timeStuckTop);
        SERIALIZE_MEMBER("clampForceMin",               m_clampForceMin);
        SERIALIZE_MEMBER("clampForceMax",               m_clampForceMax);
        SERIALIZE_MEMBER("reboundPercentForce",         m_reboundPercentForce);
        SERIALIZE_MEMBER("percentLaunchMonsterAttack",  m_percentLaunchMonsterAttack);
        SERIALIZE_MEMBER("forceGravity",                m_forceGravity);
        SERIALIZE_MEMBER("forceHitMonster",             m_forceHitMonster);
        SERIALIZE_MEMBER("distanceMax",                 m_DistanceMax);

        SERIALIZE_MEMBER("forceUpPerHit_Level_0",       m_forceUpPerHit_Level_0);
        SERIALIZE_MEMBER("forceUpPerHit_Level_1",       m_forceUpPerHit_Level_1);
        SERIALIZE_MEMBER("forceUpPerHit_Level_2",       m_forceUpPerHit_Level_2);
        
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_ElevatorAIComponent_Template::Ray_ElevatorAIComponent_Template()
    : 
    m_clampForceMin(-8.0f),
    m_clampForceMax(3.0f),
    m_reboundPercentForce(0.3f),
    m_percentLaunchMonsterAttack(0.05f),
    m_forceGravity(0.06f),
    m_forceHitMonster(5.0f),
    m_DistanceMax(43.0f),
    m_forceUpPerHit_Level_0(1.0f),
    m_forceUpPerHit_Level_1(2.0f),
    m_forceUpPerHit_Level_2(3.0f),
	m_timeStuckTop(3.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_ElevatorAIComponent_Template::~Ray_ElevatorAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************

	
};
