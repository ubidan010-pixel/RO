#include "precompiled_gameplay.h"

#ifndef _ITF_AIDESTROYACTION_H_
#include "gameplay/ai/Actions/AIDestroyAction.h"
#endif //_ITF_AIDESTROYACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIDestroyAction_Template)
    BEGIN_SERIALIZATION(AIDestroyAction_Template)
        SERIALIZE_MEMBER( "waitForFx", m_waitForFx );
    END_SERIALIZATION()

    AIDestroyAction_Template::AIDestroyAction_Template()
    : Super()
    , m_waitForFx( bfalse )
    {
    }

    AIDestroyAction_Template::~AIDestroyAction_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(AIDestroyAction)

    void AIDestroyAction::onActivate()
    {
        // Wait for FX's end => pause all components except FX/AI
        //
        if ( getTemplate()->getWaitForFx() && ( m_fxController != NULL ) && !m_fxController->IsAllFXStopped() )
        {
            const Actor::ComponentsVector& components = m_actor->GetAllComponents();

            for ( u32 i = 0; i < components.size(); ++i )
            {
                ActorComponent* comp = components[i];
                if (    ( comp->DynamicCast<FXControllerComponent>(ITF_GET_STRINGID_CRC(FXControllerComponent,2370830262)) == NULL ) &&
                        ( comp->DynamicCast<FxBankComponent>(ITF_GET_STRINGID_CRC(FxBankComponent,2523615645)) == NULL ) &&
                        ( comp->DynamicCast<SoundComponent>(ITF_GET_STRINGID_CRC(SoundComponent,2111333436)) == NULL ) && 
                        ( comp->DynamicCast<AIComponent>(ITF_GET_STRINGID_CRC(AIComponent,3090898017)) == NULL ) )
                {
                    comp->pause();
                }
            }
            m_actor->clearBinds(btrue);
        }

        m_destructionRequested = bfalse;
        m_safeTimer = 5.0f;
    }


    void AIDestroyAction::update( f32 _dt )
    {
        m_safeTimer = f32_Max( 0.0f, m_safeTimer - _dt );

        if ( getTemplate()->getWaitForFx() && ( m_fxController != NULL ) )
        {
            m_destructionRequested = ( m_fxController->IsAllFXStopped() || ( m_safeTimer == 0.0f ) );
        }
        else
        {
            m_destructionRequested = btrue;
        }

        if ( m_destructionRequested )
        {
            m_actor->getBinding()->clear();
            m_aiBehavior->onActionFinished();
            //TEMPORAY REPLACE WITH DISABLE WHEN ITS READY
            if (m_actor->isSerializable())
                m_actor->disable();
            else
            {
                m_actor->requestDestruction();
            }
        }
    }
}