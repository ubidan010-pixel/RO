#include "precompiled_gameplay_rayman.h"

#include "engine/actors/managers/ContextIconsManager.h"

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_RAY_PLAYERSCORECOMPONENT_H_
#include "rayman/gameplay/Components/score/Ray_PlayerScoreComponent.h"
#endif //_ITF_RAY_PLAYERSCORECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerScoreComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlayerScoreComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlayerScoreComponent)
    END_VALIDATE_COMPONENT()

    // ****************************************************************************

    Ray_PlayerScoreComponent::Ray_PlayerScoreComponent()
        : m_textComponent(NULL)
          , m_releasing(bfalse)
          , m_bezierComponent(NULL)
          , m_linkComponent(NULL)
          , m_animComponent(NULL)
          , m_arrivedIndex(0)
          , m_playerIndex(U32_INVALID)
          , m_timer(0.0f)
          , m_releasedIndex(0)
          , m_numLums(0)
          , m_releasingFinish(bfalse)
          , m_countPlayerActive(0)
          , m_factorPlayer(0.0f)
          , m_speedUpButtonPressed(bfalse)
          , m_showSpeedUpContextIcon(bfalse)
          , m_speedFactor(1.0f)
          ,m_lastReleaseIndex(0)
    {
        // none
    }

    // ****************************************************************************

    Ray_PlayerScoreComponent::~Ray_PlayerScoreComponent()
    {
        if (m_actor && m_resource.isValidResourceId())
        {
            m_actor->removeResource(m_resource);
            m_resource.invalidateResourceId();
        }

        m_resource.invalidateResourceId();
    }

    // ****************************************************************************

    void Ray_PlayerScoreComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Components
        m_textComponent = m_actor->GetComponent<UITextBoxesComponent>();
        m_bezierComponent = m_actor->GetComponent<BezierCurveComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_animComponent = m_actor->GetComponent<AnimatedComponent>();

        // Event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

        // Particles generator
        m_resource = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());
        m_particleGenerator.init(&getTemplate()->getParticleGenerator());
        m_particleGenerator.setMaxParticles(3000);
        m_particleGenerator.getParameters()->setNumToEmit(U32_INVALID);
        m_particleGenerator.getParameters()->setTexture(m_resource);
        m_particleGenerator.getParameters()->setGeneratorMode(PARGEN_MODE_MANUAL);
        m_particleGenerator.getParameters()->setPosition(m_actor->getPos());
        //m_particleGenerator.spawnNParticle(getTemplate()->getNumParticles(), 0.f);
    }

    // ****************************************************************************
    void Ray_PlayerScoreComponent::ShowSpeedUpContextIcon(bbool _show)
    {
        if (_show)
        {
            if (!UIMenuManager::IsBaseMenuHelper())
                CONTEXTICONSMANAGER->show(ContextIcon_SpeedUp, ContextIcon_Invalid);
        }
        else
        {
            if (!UIMenuManager::IsBaseMenuHelper())
                CONTEXTICONSMANAGER->hide();
        }
    }

    void Ray_PlayerScoreComponent::Update(f32 _dt)
    {
        Super::Update(_dt);
        if (isPlayerActive())
        {
            String8 str;

            m_numLums = std::min(m_numLums, (u32)999); //we clamp to 999 lums
            u32 count = m_numLums - m_arrivedIndex;
            str.setTextFormat("%d", count);

            m_textComponent->setText(ITF_GET_STRINGID_CRC(B_Pla_Text01, 3457442411), str);

            if (m_releasing)
            {
                if (!m_showSpeedUpContextIcon)
                {
                    ShowSpeedUpContextIcon(btrue);
                    m_showSpeedUpContextIcon = btrue;
                }
                u32 arrivedThisFrame = updateParticles(_dt);

                //add lums to guage
                Ray_EventScoreGuageAddLum addLum;
                addLum.setSender(m_actor->getRef());
                addLum.setNumLums(arrivedThisFrame);
                m_linkComponent->sendEventToChildren(&addLum);
                m_timer += _dt *  m_speedFactor;
                if (m_arrivedIndex == m_numLums)
                {
                    if (m_showSpeedUpContextIcon)
                    {
                        ShowSpeedUpContextIcon(bfalse);
                        m_showSpeedUpContextIcon = bfalse;
                    }
                    m_releasing = bfalse;
                    m_releasingFinish = btrue;

                    // Play anim disappear
                    if (getTemplate()->getIndex() == 0)
                    {
                        m_animComponent->setAnim(ITF_GET_STRINGID_CRC(DisappearWin, 135381880));
                    }
                    else
                    {
                        m_animComponent->setAnim(ITF_GET_STRINGID_CRC(DisappearClassic, 4090380858));
                    }

                    // tell guage we have finished sending lums
                    EventTrigger trigger;
                    trigger.setSender(m_actor->getRef());
                    trigger.setActivated(btrue);
                    m_linkComponent->sendEventToChildren(&trigger);
                }
                else
                {
                    HandleSpeedUpQoL();
                }
            }
        }
    }

    // ****************************************************************************

    void Ray_PlayerScoreComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if (EventGeneric* onEventGeneric = _event->DynamicCast<EventGeneric>(
            ITF_GET_STRINGID_CRC(EventGeneric, 4063838687)))
        {
            if (onEventGeneric->getId() == ITF_GET_STRINGID_CRC(DisplayScore, 955041297))
            {
                processDisplayScore();
            }
            else if (onEventGeneric->getId() == ITF_GET_STRINGID_CRC(PauseReleasing, 3938703612) && !m_releasingFinish)
            {
                m_releasing = bfalse;
            }
            else if (onEventGeneric->getId() == ITF_GET_STRINGID_CRC(ResumeReleasing, 26409933) && !m_releasingFinish)
            {
                m_releasing = btrue;
            }
        }
        else if (EventTrigger* onTrigger = _event->DynamicCast<EventTrigger>(
            ITF_GET_STRINGID_CRC(EventTrigger, 1343042510)))
        {
            // Release lums

            if (isPlayerActive())
            {
                setupParticles();

                m_releasing = btrue;
                m_timer = 0.0f;

                // Play anim shake
                if (getTemplate()->getIndex() == 0)
                {
                    m_animComponent->setAnim(ITF_GET_STRINGID_CRC(ShakeWin, 2358713560));
                }
                else
                {
                    m_animComponent->setAnim(ITF_GET_STRINGID_CRC(ShakeClassic, 3069698806));
                }
            }
            else
            {
                // player is inactive tell guage this guy is finished
                EventTrigger trigger;
                trigger.setSender(m_actor->getRef());
                trigger.setActivated(btrue);
                m_linkComponent->sendEventToChildren(&trigger);

                m_releasingFinish = btrue;
            }
        }
    }

    // ****************************************************************************
    void Ray_PlayerScoreComponent::initLumSpeedFactor(u32 _playerNum)
    {
        switch (_playerNum)
        {
        case 0:
        case 1:
            m_factorPlayer = getTemplate()->getFactorSpeedRelease_1P();
            break;
        case 2:
            m_factorPlayer = getTemplate()->getFactorSpeedRelease_2P();
            break;
        case 3:
            m_factorPlayer = getTemplate()->getFactorSpeedRelease_3P();
            break;
        case 4:
            m_factorPlayer = getTemplate()->getFactorSpeedRelease_4P();
            break;
        }
    }

    void Ray_PlayerScoreComponent::processDisplayScore()
    {
        ITF_VECTOR<std::pair<u32, u32>> sortedPlayers;
        for (u32 i = 0; i < RewardManager::PlayersMax; ++i)
        {
            if (RAY_GAMEMANAGER->getPlayer(i) && RAY_GAMEMANAGER->getPlayer(i)->getActiveAndPersistent())
                sortedPlayers.push_back(std::pair<u32, u32>(i,RAY_GAMEMANAGER->getCurrentScore().getLums(i)));
        }

        // Compute factor for speed release
        m_countPlayerActive = sortedPlayers.size();
        initLumSpeedFactor(m_countPlayerActive);
        // Sort players by lums score
        std::sort(sortedPlayers.begin(), sortedPlayers.end(), AIUtils::sortPlayersByScore);

        if (sortedPlayers.size() > getTemplate()->getIndex())
        {
            setPlayer(sortedPlayers[getTemplate()->getIndex()].first);
            m_numLums = sortedPlayers[getTemplate()->getIndex()].second;
        }

        // Start Anim
        if (isPlayerActive())
        {
            if (m_animComponent)
            {
                if (getTemplate()->getIndex() == 0)
                {
                    m_animComponent->setAnim(ITF_GET_STRINGID_CRC(ArrivalStandWin, 2667337017));
                }
                else
                {
                    m_animComponent->setAnim(ITF_GET_STRINGID_CRC(ArrivalClassic, 583158772));
                }
            }
        }
    }

    // ****************************************************************************

    void Ray_PlayerScoreComponent::Draw()
    {
        Super::Draw();
        if (m_releasing)
            m_particleGenerator.render();
    }

    // ****************************************************************************

    void Ray_PlayerScoreComponent::setPlayer(u32 _playerIndex)
    {
        m_playerIndex = _playerIndex;
    }

    // ****************************************************************************
    void Ray_PlayerScoreComponent::HandleSpeedUpQoL()
    {
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* p = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
            if (p && p->getActiveAndPersistent())
            {
                InputAdapter::PressStatus buts[JOY_MAX_BUT];
                INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua, i, buts, JOY_MAX_BUT);
                if (buts[m_joyButton_A] == InputAdapter::Pressed && !m_speedUpButtonPressed)
                {
                    m_speedUpButtonPressed = btrue;
                    m_speedFactor =  getTemplate()->getFactorSpeedUpQoL();;
                    CONTEXTICONSMANAGER->highlightText(ContextIcon_SpeedUp, btrue);
                }
                else if (buts[m_joyButton_A] == InputAdapter::Released && m_speedUpButtonPressed)
                {
                    CONTEXTICONSMANAGER->highlightText(ContextIcon_SpeedUp, bfalse);

                    m_speedFactor = 1.0f;
                    m_speedUpButtonPressed = false;
                }
            }
        }
    }

    u32 Ray_PlayerScoreComponent::updateParticles(f32 _dt)
    {
        m_particleGenerator.startManualUpdate(m_actor->getPos());

        f32 factor = m_timer * (getTemplate()->getLumReleaseSpeed() * m_factorPlayer);
        u32 numLums = m_particles.size();

        f32 v = f32_Sin(m_timer * MTH_2PI * getTemplate()->getParticleGenerationFrequency());
        u32 offset = u32(getTemplate()->getParticleGenerationAmplitude() * f32_Abs(v));
        m_releasedIndex = Min(u32(factor) + offset , numLums);
        m_releasedIndex = Max(m_releasedIndex, m_lastReleaseIndex);
        m_lastReleaseIndex = m_releasedIndex;
        BezierCurve& curve = m_bezierComponent->getBezierCurve();

        u32 arrivedThisFrame = 0;

        f32 size = getTemplate()->getParticleSize();


        for (u32 i = m_arrivedIndex; i < m_releasedIndex; ++i)
        {
            ParticleData& data = m_particles[i];
            f32 prevDist = data.m_curDist;
            data.m_curDist += _dt * getTemplate()->getLumTrajectorySpeed() * m_speedFactor;
            data.m_curDist = Min(data.m_curDist, curve.getTotalLength());

            m_particleGenerator.changeManualParticleSize(i, size);

            if (prevDist < curve.getTotalLength() && data.m_curDist >= curve.getTotalLength())
            {
                arrivedThisFrame++;
            }

            if (data.m_curDist >= curve.getTotalLength())
            {
                m_particleGenerator.changeManualParticleState(i, bfalse);
            }
            else
            {
                m_particleGenerator.changeManualParticleState(i, btrue);
                Vec3d newPos;
                Vec3d tan;
                curve.getPosTanAtDistance(data.m_curDist, newPos, tan);
                Vec3d normal = tan.getPerpendicular();
                normal.m_z = 0.0f;

                Vec3d targetPos = m_actor->getPos() + newPos + normal * data.m_initialOffset;

                Vec3d pos = data.m_pos + (targetPos - data.m_pos) * getTemplate()->getSmoothFactor();
                Vec3d speed = pos - data.m_pos;
                data.m_pos = pos;

                Vec2d twoDSpeed = speed.truncateTo2D();
                twoDSpeed.normalize();
                if (twoDSpeed.sqrnorm() <= 1.f - MTH_EPSILON_2)
                {
                    twoDSpeed = Vec2d::Right;
                }

                f32 angle = twoDSpeed.getAngle();

                m_particleGenerator.updateManualParticle(i, pos, speed, Angle(bfalse, angle));
            }
        }

        m_particleGenerator.update(_dt, bfalse);

        m_arrivedIndex += arrivedThisFrame;
        return arrivedThisFrame;
    }

    void Ray_PlayerScoreComponent::setupParticles()
    {
        m_particleGenerator.spawnNParticle(m_numLums, 0.f);
        m_particles.resize(m_numLums);

        for (u32 i = 0; i < m_particles.size(); i++)
        {
            ParticleData& data = m_particles[i];
            data.m_pos = m_actor->getPos();
            data.m_curDist = 0.0f;
            data.m_speed = 0.0f;
            data.m_initialOffset = Seeder::getSharedSeeder().GetFloat(-getTemplate()->getParticleMaxOffset(),
                                                                      getTemplate()->getParticleMaxOffset());
        }
        m_releasedIndex = 0;
        m_arrivedIndex = 0;
        m_speedFactor = 1.0f;
    }

    //*****************************************************************************

    bbool Ray_PlayerScoreComponent::isPlayerActive()
    {
        return m_playerIndex != U32_INVALID;
    }


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_PlayerScoreComponent_Template)

    BEGIN_SERIALIZATION(Ray_PlayerScoreComponent_Template)
        SERIALIZE_MEMBER("lumTrajectorySpeed", m_lumTrajectorySpeed);
        SERIALIZE_MEMBER("lumReleaseSpeed", m_lumReleaseSpeed);
        SERIALIZE_OBJECT("particleGenerator", m_particleGenerator);
        SERIALIZE_MEMBER("texture", m_textureFile);
        SERIALIZE_MEMBER("index", m_index);
        SERIALIZE_MEMBER("particleSize", m_particleSize);
        SERIALIZE_MEMBER("particleMaxOffset", m_particleMaxOffset);
        SERIALIZE_MEMBER("particleGenerationAmplitude", m_particleGenerationAmplitude);
        SERIALIZE_MEMBER("particleGenerationFrequency", m_particleGenerationFrequency);
        SERIALIZE_MEMBER("smoothFactor", m_smoothFactor);

        SERIALIZE_MEMBER("factorSpeedRelease_1P", m_factorSpeedRelease_1P);
        SERIALIZE_MEMBER("factorSpeedRelease_2P", m_factorSpeedRelease_2P);
        SERIALIZE_MEMBER("factorSpeedRelease_3P", m_factorSpeedRelease_3P);
        SERIALIZE_MEMBER("factorSpeedRelease_4P", m_factorSpeedRelease_4P);
        SERIALIZE_MEMBER("factorSpeedUpQoL", m_factorSpeedUpQoL);

    END_SERIALIZATION()

    //*****************************************************************************

    Ray_PlayerScoreComponent_Template::Ray_PlayerScoreComponent_Template()
        : m_lumReleaseSpeed(20.0f)
          , m_lumTrajectorySpeed(10.0f)
          , m_index(0)
          , m_particleSize(1.0f)
          , m_particleMaxOffset(0.2f)
          , m_particleGenerationAmplitude(3)
          , m_particleGenerationFrequency(1.0f)
          , m_smoothFactor(0.9f)
          , m_factorSpeedRelease_1P(1.0f)
          , m_factorSpeedRelease_2P(1.0f)
          , m_factorSpeedRelease_3P(1.0f)
          , m_factorSpeedRelease_4P(1.0f)
          , m_factorSpeedUpQoL(1.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_PlayerScoreComponent_Template::~Ray_PlayerScoreComponent_Template()
    {
        // none
    }

    //*****************************************************************************
};
