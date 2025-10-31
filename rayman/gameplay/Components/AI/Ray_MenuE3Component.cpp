#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYMENUE3COMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MenuE3Component.h"
#endif //_ITF_RAYMENUE3COMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MenuE3Component_Template)

    BEGIN_SERIALIZATION(Ray_MenuE3Component_Template)
        SERIALIZE_CONTAINER("maps", m_maps);
        SERIALIZE_MEMBER("music",m_music);
        SERIALIZE_MEMBER("validationSound",m_validationSound);
        SERIALIZE_MEMBER("selectionSound",m_selectionSound);
        SERIALIZE_MEMBER("inGameMenu", m_inGameMenu);
        SERIALIZE_MEMBER("offset2D", m_offset2D);
        SERIALIZE_MEMBER("zOffset", m_zOffset);
        SERIALIZE_MEMBER("pauseAnimSize", m_pauseAnimSize);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_MenuE3Component)

    BEGIN_SERIALIZATION_CHILD(Ray_MenuE3Component)
    END_SERIALIZATION()

    Ray_MenuE3Component::Ray_MenuE3Component()
        : m_currentAnimIndex(0)
        , m_subAnimCount(0)
        , m_animLightComponent(NULL)
        , m_fxControllerComponent(NULL)
        , m_isInGameMenu(false)
        , m_offset2D(Vec2d::Zero)
        , m_zOffset(0.0f)
        , m_repetitionTime(0.25f)
        , m_firstRepetitionTime(0.5f)
        , m_bJustPressed(bfalse)
        , m_bPressed(bfalse)
        , m_timer(0.0f)
        , m_listenerRegistered(bfalse)
    {
    }

    Ray_MenuE3Component::~Ray_MenuE3Component()
    {
        if(m_listenerRegistered)
            GAMEMANAGER->removeInputListener(this);
    }


    void Ray_MenuE3Component::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animLightComponent    = GetActor()->GetComponent<AnimLightComponent>();
        m_fxControllerComponent = GetActor()->GetComponent<FXControllerComponent>();

        if ( m_animLightComponent != NULL )
        {
            m_subAnimCount = m_animLightComponent->getSubAnimCount();
            if ( m_subAnimCount > 0 )
            {
                // Play first animation
                m_currentAnimIndex = 0;
                const SubAnim * subAnim = m_animLightComponent->getSubAnim(m_currentAnimIndex);
                m_animLightComponent->setAnim(subAnim->getFriendlyName());
            }
        }

        // set default values
        m_isInGameMenu  = getTemplate()->getInGameMenu();
        m_offset2D      = getTemplate()->getOffset2D();
        m_zOffset       = getTemplate()->getZOffset();

        // Apply fade effect
        Color endColor = Color::black();
        endColor.m_a = 0.f;
        //RAY_GAMEMANAGER->fxFade(Color::black(),endColor,2.0f,btrue);

    }

    void Ray_MenuE3Component::onBecomeActive()
    {
        Super::onBecomeActive();

        const StringID& musicId = getTemplate()->getMusicID();

        if ( musicId != StringID::Invalid )
        {
            MUSICMANAGER->play(METRONOME_TYPE_MENU,musicId);
        }

        if ( m_animLightComponent != NULL )
        {
            m_subAnimCount = m_animLightComponent->getSubAnimCount();
            if ( m_subAnimCount > 0 )
            {
                // Play first animation
                m_currentAnimIndex = 0;
                const SubAnim * subAnim = m_animLightComponent->getSubAnim(m_currentAnimIndex);
                m_animLightComponent->setAnim(subAnim->getFriendlyName());
            }
        }
    }

    void Ray_MenuE3Component::Update( f32 _dt )
    {
        Super::Update(_dt);
        if(!canReceive()) return ; 

        if( !m_listenerRegistered )
        {
            GAMEMANAGER->addMenuInputListener(this);
            m_listenerRegistered = btrue;
        }

        // Pause menu ?
        if ( m_isInGameMenu )
        {
            // Update position relatively to current camera
            //Vec3d newPos = Vec3d::Zero;
            Vec2d screenCenter = Vec2d(GFX_ADAPTER->getScreenWidth()*0.5f,GFX_ADAPTER->getScreenHeight() * 0.5f);
            Vec2d screenPos = screenCenter - Vec2d(screenCenter.m_x * m_offset2D.m_x, screenCenter.m_y * m_offset2D.m_y );
            //GFX_ADAPTER->compute2DTo3D(screenPos,CAMERACONTROLLERMANAGER->getCamPos().m_z - m_zOffset,newPos);
            
            // Temp code for E3
            Vec2d newScale = getTemplate()->getPauseAnimSize();
            Vec2d screenRatio ( GFX_ADAPTER->getScreenWidth() / 1280.0f, GFX_ADAPTER->getScreenHeight() / 720.0f);
            newScale *= screenRatio;

            m_actor->setScale(newScale);
            m_actor->set2DPos(screenPos);
        }

        // Make sure we have at least 1 player active to be able to test it. If not, then all pads will be checked
        bbool bTestActivePlayer = bfalse;
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount() && !bTestActivePlayer; i++)
        {
            if (GAMEMANAGER->getPlayer(i)->getActive())
                bTestActivePlayer = btrue;
        }

        if ( !m_bPressed )
            m_bJustPressed = bfalse;        
        m_timer += _dt;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
   
    void Ray_MenuE3Component::LaunchMap()
    {
        const ITF_VECTOR<Path>& paths = getTemplate()->getMaps();
        if ( m_currentAnimIndex < i32(paths.size()) )
        {
            // Play validation sound
            playSound(getTemplate()->getValidationsoundID());

            const Path * path = &paths[m_currentAnimIndex];
            GAMEMANAGER->loadGameplayMap((*path), btrue, bfalse);
        }
    }

    void Ray_MenuE3Component::ApplyMenuPauseButton()
    {
        switch(m_currentAnimIndex)
        {
            // Resume
        case 0:
            {
                playSound(getTemplate()->getValidationsoundID());
                if (RAY_GAMEMANAGER->isInPause())
                {
                    RAY_GAMEMANAGER->leavePause();
                }
            }
            break;
            // Retry
        case 1:
            {
                playSound(getTemplate()->getValidationsoundID());
                if (RAY_GAMEMANAGER->isInPause())
                {
                    RAY_GAMEMANAGER->leavePause();
                }

                REWARD_MANAGER->Session_Reset();

                GAMEMANAGER->reloadCurrentMap();
            }            
            break;
            // Back to main menu
        case 2:
            {
                playSound(getTemplate()->getValidationsoundID());
                if (RAY_GAMEMANAGER->isInPause())
                {
                    RAY_GAMEMANAGER->leavePause();
                }

                if(RAY_GAMEMANAGER->getDebugSaves() && !GAMEMANAGER->getUsePressConfMenu())
                    RAY_GAMEMANAGER->resetToWorldMap();
                else
                    RAY_GAMEMANAGER->goToTheMainMenu();   

                REWARD_MANAGER->Session_Stop();
            }            
            break;
        default:
            ITF_ASSERT(0);
        }
    }

    void Ray_MenuE3Component::playSound(const StringID& _sound)
    {
        if ( m_fxControllerComponent != NULL)
        {
            m_fxControllerComponent->playFX(_sound);
        }
    }

    void Ray_MenuE3Component::Receive(  u32 deviceID, f32 axis, const StringID& action )
    {
        if(!canReceive()) return ; 

        if(action == input_actionID_Up || action == input_actionID_Down
            || action == input_actionID_UpHold || action == input_actionID_DownHold
            || action == input_actionID_Valid )
        {

            bbool bDownPressed = (action == input_actionID_Down || action == input_actionID_DownHold);
            bbool bUpPressed = (action == input_actionID_Up || action == input_actionID_UpHold);

            StringID menuCommand;
            bbool bUpdateAnimation = bfalse;
            bbool bCheckInput = bfalse;
            if ( !m_bJustPressed && !m_bPressed && (bDownPressed || bUpPressed))
            {
                m_bJustPressed = true;
                bCheckInput = btrue;
            }
            else if ( m_bJustPressed && m_timer >= m_firstRepetitionTime )
            {
                m_bJustPressed = bfalse;
                bCheckInput = btrue;
            }
            else if ( !m_bJustPressed && m_bPressed && m_timer >= m_repetitionTime )
            {
                bCheckInput = btrue;
            }

            if ( bCheckInput )
            {
                if ( bUpPressed )
                {
                    m_currentAnimIndex--;
                    bUpdateAnimation = btrue;

                    m_timer = 0.0f;
                }
                else if ( bDownPressed )
                {
                    m_currentAnimIndex++;
                    bUpdateAnimation = btrue;

                    m_timer = 0.0f;
                }
            }

            if ( m_currentAnimIndex >= i32(m_subAnimCount) ) m_currentAnimIndex = 0;
            else if ( m_currentAnimIndex < 0 ) m_currentAnimIndex = i32(m_subAnimCount) - 1;

            if ( bUpdateAnimation )
            {
                const SubAnim * subAnim = m_animLightComponent->getSubAnim(m_currentAnimIndex);
                m_animLightComponent->setAnim(subAnim->getFriendlyName());
                playSound(getTemplate()->getSelectionsoundID());
            }

            if ( action == input_actionID_Valid )
            {
                if ( !m_isInGameMenu )
                    LaunchMap();
                else
                    ApplyMenuPauseButton();
            }

            m_bPressed = btrue;
        }
    }

    void Ray_MenuE3Component::onBecomeInactive()
    {
        if(m_listenerRegistered )
        {
            GAMEMANAGER->removeInputListener(this);
            m_listenerRegistered = bfalse;
        }        
    }

    bbool Ray_MenuE3Component::canReceive()
    {
        return bfalse;
    }

};
