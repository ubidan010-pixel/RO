#include "precompiled_gameplay.h"
#include "core/Macros.h"

#ifdef ITF_SUPPORT_BOT_AUTO

#include "PerceptionModule.h"
#include "Components/common/StickToPolylinePhysComponent.h"
#include "managers/GameManager.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"

namespace ITF
{

    PerceptionModule::PerceptionModule(GameManager* gameManager)
    : m_gameManager(gameManager)
    , m_getStanceCallback(nullptr)
    , m_scanTargetsCallback(nullptr)
    {

    }

    PerceptionModule::~PerceptionModule()
    {
    }

    void PerceptionModule::update(f32 deltaTime)
    {
        extractPlayerState();
        scanNearbyTargets();
        debugDraw();
    }

    void PerceptionModule::extractPlayerState()
    {
        if (!m_gameManager)
        {
            return;
        }
        Player* player = m_gameManager->getPlayer(0);
        if (!player || !player->getActive())
        {
            m_currentState.playerPosition = Vec3d(0.0f, 0.0f, 0.0f);
            m_currentState.playerVelocity = Vec2d(0.0f, 0.0f);
            m_currentState.isGrounded = bfalse;
            m_currentState.stance = STANCE_STAND;
            m_currentState.gameMode = 0;
        }
        else
        {
            Actor* playerActor = player->getActor();
            if (!playerActor)
            {
                return;
            }

            m_currentState.playerPosition = playerActor->getPos();
            m_currentState.gameMode = player->getCurrentMode();
            
            if (m_getStanceCallback)
            {
                m_currentState.stance = static_cast<EStance>(m_getStanceCallback(m_gameManager, 0));
            }
            else
            {
                m_currentState.stance = STANCE_STAND;
            }

            StickToPolylinePhysComponent* physComp = playerActor->GetComponent<StickToPolylinePhysComponent>();
            if (physComp)
            {
                const Vec2d& worldSpeed = physComp->getWorldSpeed();
                m_currentState.playerVelocity = worldSpeed;
                const PhysContactsContainer& contacts = physComp->getContacts();
                m_currentState.isGrounded = (contacts.size() > 0);
            }
            else
            {
                m_currentState.playerVelocity.m_x = 0.0f;
                m_currentState.playerVelocity.m_y = 0.0f;
                m_currentState.isGrounded = bfalse;
            }
        }
    }

    void PerceptionModule::scanNearbyTargets()
    {
        m_currentState.nextTarget = TargetInfo();

        if (m_scanTargetsCallback)
        {
            m_scanTargetsCallback(m_gameManager, &m_currentState);
        }
    }

    void PerceptionModule::debugDraw() const
    {
        if (!GFX_ADAPTER)
            return;

        char buffer[128];
        const f32 startX = 20.0f;
        const f32 lineHeight = 20.0f;
        f32 y = 50.0f;

        GFX_ADAPTER->drawDBGText("=== AI BOT STATE ===", startX, y, 0.0f, 1.0f, 1.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Position: (%.2f, %.2f, %.2f)",
            m_currentState.playerPosition.m_x,
            m_currentState.playerPosition.m_y,
            m_currentState.playerPosition.m_z);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 1.0f, 1.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Velocity: (%.2f, %.2f)",
            m_currentState.playerVelocity.m_x,
            m_currentState.playerVelocity.m_y);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 1.0f, 0.0f, bfalse);
        y += lineHeight;

        sprintf_s(buffer, sizeof(buffer), "Grounded: %s", 
            m_currentState.isGrounded ? "YES" : "NO");
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.0f, 1.0f, 0.0f, bfalse);
        y += lineHeight;

        const char* stanceName = "UNKNOWN";
        switch (m_currentState.stance)
        {
            case STANCE_STAND: stanceName = "STAND"; break;
            case STANCE_HANG: stanceName = "HANG"; break;
            case STANCE_CLIMB: stanceName = "CLIMB"; break;
            case STANCE_HELICO: stanceName = "HELICOPTER"; break;
            case STANCE_WALLSLIDE: stanceName = "WALLSLIDE"; break;
            case STANCE_SUPPORT: stanceName = "SUPPORT"; break;
            case STANCE_SWIM: stanceName = "SWIM"; break;
            case STANCE_SWING: stanceName = "SWING"; break;
        }
        sprintf_s(buffer, sizeof(buffer), "Stance: %s", stanceName);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 1.0f, 0.5f, 0.0f, bfalse);
        y += lineHeight;

        const char* modeName = "UNKNOWN";
        switch (m_currentState.gameMode)
        {
            case 0: modeName = "PLATFORMER"; break;
            case 1: modeName = "SHOOTER"; break;
            case 2: modeName = "WORLDMAP"; break;
        }
        sprintf_s(buffer, sizeof(buffer), "Mode: %s", modeName);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.5f, 0.5f, 1.0f, bfalse);
        y += lineHeight;

        const char* targetType = m_currentState.nextTarget.isCheckpoint ? "CHECKPOINT" : "CHANGEPAGE";
        sprintf_s(buffer, sizeof(buffer), "NextTarget: %s (%.1fm)", 
            targetType,
            m_currentState.nextTarget.distance);
        GFX_ADAPTER->drawDBGText(buffer, startX, y, 0.0f, 1.0f, 1.0f, bfalse);
    }

}
#endif
