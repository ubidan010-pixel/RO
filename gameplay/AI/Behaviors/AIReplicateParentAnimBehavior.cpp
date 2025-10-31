#include "precompiled_gameplay.h"

#ifndef _ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIReplicateParentAnimBehavior.h"
#endif //_ITF_AIREPLICATEPARENTANIMBEHAVIOR_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIReplicateParentAnimBehavior)
    
    //----------------------------------------------------------------------------------------------------------------------------------
    AIReplicateParentAnimBehavior::AIReplicateParentAnimBehavior()
        : Super()
    {
    }

    AIReplicateParentAnimBehavior::~AIReplicateParentAnimBehavior()
    {
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AIReplicateParentAnimBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded( _hotReload );
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AIReplicateParentAnimBehavior::onActivate()
    {
        if ( !getTemplate()->useParentBind() )
        {
            LinkComponent * linkComp = m_actor->GetComponent<LinkComponent>();
            if ( linkComp != NULL )
            {
                ObjectRefList* parentList = LINKMANAGER->getParents( m_actor->getRef() );
                if ( ( parentList != NULL ) && ( parentList->size() > 0 ) )
                {
                    m_parentLink = (*parentList)[0];
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    StringID  AIReplicateParentAnimBehavior::getParentAnimation( Actor * & _res_parent ) const
    {
        Actor * parent = NULL;
        _res_parent = NULL;

        // Get parent
        //
        if ( getTemplate()->useParentBind() )
        {
            if ( ( m_actor->getParentBind() != NULL ) && !m_actor->getParentBind()->isRuntimeDisabled() )
            {
                if ( BaseObject * baseObj = m_actor->getParentBind()->m_runtimeParent.getObject() )
                {
                    parent = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                }
            }
        }
        else
        {
            if ( BaseObject * baseObj = m_parentLink.getObject() )            
            {
                parent = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            }
        }

        // Get parent action animation
        //
        if ( parent != NULL )
        {
            _res_parent = parent;

            AIComponent * AIComp = parent->GetComponent<AIComponent>();
            if ( ( AIComp != NULL ) && ( AIComp->getCurrentBehavior() != NULL ) && ( AIComp->getCurrentBehavior()->getCurrentAction() != NULL ) )
            {
                return AIComp->getCurrentBehavior()->getCurrentAction()->getAnimAction();
            }
        }

        return StringID::Invalid;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AIReplicateParentAnimBehavior::replicateParentAnimation()
    {
        Actor * parent = NULL;
        const StringID &  parentAnim = getParentAnimation( parent );

        if ( ( parent == NULL ) || ( m_currentAnim == parentAnim ) )
        {
            return;
        }

        if ( m_animComponent != NULL )
        {
            SubAnim* subanim = NULL;

            if ( getTemplate()->isAnimReplicable( parentAnim ) )
            {
                m_currentAnim = parentAnim;

                subanim = m_animComponent->getSubAnim( parentAnim );
            }
            
            // has this animation, play it 
            if ( subanim != NULL )
            {
                m_animComponent->setAnim( m_currentAnim );
            }
            // play default animation 
            else if ( getTemplate()->getDefaultAnim().isValid() && ( m_currentAnim != getTemplate()->getDefaultAnim() ) )
            {
                m_currentAnim = getTemplate()->getDefaultAnim();
                m_animComponent->setAnim( m_currentAnim );
            }        
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AIReplicateParentAnimBehavior::update( f32 _dt  )
    {
        replicateParentAnimation();
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(AIReplicateParentAnimBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(AIReplicateParentAnimBehavior_Template)
        SERIALIZE_MEMBER( "useParentBind",      m_useParentBind );
        SERIALIZE_MEMBER( "defaultAnim",        m_defaultAnim );
        SERIALIZE_CONTAINER( "animsToAvoid",    m_animsToAvoid );
    END_SERIALIZATION()

    AIReplicateParentAnimBehavior_Template::AIReplicateParentAnimBehavior_Template()
    : Super()
    , m_useParentBind( bfalse )
    {
    }

    AIReplicateParentAnimBehavior_Template::~AIReplicateParentAnimBehavior_Template()
    {   

    }

    bbool AIReplicateParentAnimBehavior_Template::isAnimReplicable( const StringID & _anim ) const
    {
        for ( u32 i = 0; i < m_animsToAvoid.size(); ++i )
        {
            if ( m_animsToAvoid[i] == _anim )
                return bfalse;
        }
        return btrue;
    }
}

