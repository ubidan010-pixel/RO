#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_BasicPlayerControllerComponent.h"
#endif //_ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_


namespace ITF
{
    //---------------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BasicPlayerControllerComponent)
    Ray_BasicPlayerControllerComponent::Ray_BasicPlayerControllerComponent()
    :Super()
    , m_deathLumDropTimer( 0.f )
    , m_deathLumDropTotalDuration( 0 )
    , m_stateDeadSoulTimer( 0.f )
    {
        m_deathLumDropList.clear();
    }

    Ray_BasicPlayerControllerComponent::~Ray_BasicPlayerControllerComponent()
    {
        destroyDeathDropLum();
    }

    //----------------------------------------------------------------------------------------------------------------------
    void Ray_BasicPlayerControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        initializeDeathDropLum();

        // fix for RO-10921, and probably others: disable the visible prediction on player characters
        if (m_animComponent)
        {
            m_animComponent->setDisableVisiblePrediction(btrue);
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicPlayerControllerComponent::initializeDeathDropLum()
    {
        if ( !getTemplate()->getDeathLumPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_deathLumSpawner, getTemplate()->getDeathLumPath() );
        }

        if ( m_deathLumDropList.size() > 0 )
        {
            return;
        }

        m_deathLumDropTotalDuration = 0.f;

        for ( u32 i = 0; i < getTemplate()->getDeathLumDropTemplateList().size(); ++i )
        {
            const LumDrop_Template * lumDrop_template = &(getTemplate()->getDeathLumDropTemplateList()[i]);
            
            m_deathLumDropTotalDuration += lumDrop_template->getDuration();
            
            m_deathLumDropList.push_back( newAlloc( mId_Gameplay, LumDrop( lumDrop_template, m_deathLumDropTotalDuration ) ) );
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicPlayerControllerComponent::destroyDeathDropLum()
    {
        for ( u32 i = 0; i < m_deathLumDropList.size(); ++i )
        {
            SF_DEL( m_deathLumDropList[i] );
        }
        m_deathLumDropList.clear();
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicPlayerControllerComponent::updateDeathDropLum( f32 _dt )
    {
        const u32 playerIndex = getPlayerIndex();
        Ray_PersistentGameData_Score& score = RAY_GAMEMANAGER->getCurrentScore();
        if (score.getLums(playerIndex) == 0)
        {
            return;
        }

        f32 totalDuration = m_deathLumDropTotalDuration;
        f32 prevTime = f32_Modulo(m_deathLumDropTimer, totalDuration);
        m_deathLumDropTimer += _dt;
        f32 curTime = f32_Modulo(m_deathLumDropTimer, totalDuration);
        if (curTime < prevTime)
        {
            curTime = totalDuration;  // handle loop
        }

        for (u32 i = 0; i < m_deathLumDropList.size(); i++)
        {
            const LumDrop * drop = m_deathLumDropList[i];
            if ( drop == NULL )
                continue;

            if (prevTime < drop->getTime() && drop->getTime() <= curTime)
            {
                deathSpawnLum(drop);
                score.setLums(playerIndex, score.getLums(playerIndex) - 1);
                break;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_BasicPlayerControllerComponent::deathSpawnLum( const LumDrop * _drop )
    {
        if ( _drop == NULL )
        {
            return;
        }
        Vec3d targetPos = m_actor->getPos() + (_drop->getDirection() * _drop->getTemplate()->getDistance() ).to3d();
#ifndef ITF_FINAL
        //DebugDraw::circle(targetPos, 0.1f, Color::yellow(), 1.f);
#endif // ITF_FINAL

        Actor* lum = NULL;
        if ( m_deathLumSpawner.isInitialized() )
        {
            lum = m_deathLumSpawner.getSpawnee(CURRENTWORLD->getRootScene(), targetPos);
        }

        if (!lum)
        {
            return;
        }
        Ray_EventChildLaunch launchBubble(m_actor->getPos());
        launchBubble.setSender(m_actor->getRef());
        lum->onEvent(&launchBubble);
    }


    //---------------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION( LumDrop_Template )
        SERIALIZE_MEMBER( "time",       m_duration );
        SERIALIZE_MEMBER( "angle",      m_angle );
        SERIALIZE_MEMBER( "distance",   m_distance );
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BasicPlayerControllerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BasicPlayerControllerComponent_Template)
        SERIALIZE_CONTAINER_OBJECT( "deathLumDropTemplateList", m_deathLumDropTemplateList );
        SERIALIZE_MEMBER(           "deathLumPath",             m_deathLumPath );
    END_SERIALIZATION()

    Ray_BasicPlayerControllerComponent_Template::Ray_BasicPlayerControllerComponent_Template()
    : Super()
    {

    }
    Ray_BasicPlayerControllerComponent_Template::~Ray_BasicPlayerControllerComponent_Template()
    {

    }


}