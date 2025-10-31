#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LightningGeneratorAIComponent.h"
#endif //_ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_LightningGeneratorAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_LightningGeneratorAIComponent)
    END_SERIALIZATION()

    Ray_LightningGeneratorAIComponent::Ray_LightningGeneratorAIComponent()
    : Super()
    , m_orientationCursor( 0.0f ) 
    , m_pivotBoneIndex( U32_INVALID )
    , m_orientDone( bfalse )
    {

    }
    //------------------------------------------------------------------------------------------------------------------

    void Ray_LightningGeneratorAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_LightningGeneratorAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();
        /*if ( m_currentBehavior == m_receiveHitBehavior )
        {
            if ( m_behavior != NULL )
            {
                setBehavior( m_behavior );
            }
        }*/
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_LightningGeneratorAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        m_orientDone = bfalse;
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_LightningGeneratorAIComponent::Update( f32 _dt )
    {
        Super::Update( _dt );

        if ( ( m_animComponent == NULL ) || ( m_actor->getChildrenBindHandler().getChildren().size() != 1 ) )
            return;

        Actor * bindedGenerator = AIUtils::getActor( m_actor->getChildrenBindHandler().getChildren()[0] );
        
        if ( bindedGenerator == NULL )
            return;

#ifdef ITF_SUPPORT_EDITOR
        
        if ( !( bindedGenerator->get2DPos() - m_prevBindedActorPos ).IsEqual( Vec2d::Zero ) )
        {
            m_orientDone = bfalse;
        }
        m_prevBindedActorPos = bindedGenerator->get2DPos();
#endif //ITF_SUPPORT_EDITOR

        if ( m_orientDone && !getTemplate()->useDynamicOrientation() )
        {
            return;
        }

        AnimatedComponent * bindedGeneratorAnimComp = bindedGenerator->GetComponent<AnimatedComponent>();

        if ( bindedGeneratorAnimComp == NULL )
            return;

        // concider that the binded actor has the same bones has me !!

        if ( m_pivotBoneIndex == U32_INVALID )
        {
            m_pivotBoneIndex = m_animComponent->getBoneIndex( getTemplate()->getPivotBoneName() );
        }
        if ( m_pivotBoneIndex != U32_INVALID )
        {
            Vec3d pivotPos, bindedPivotPos;
            m_animComponent->getBonePos( m_pivotBoneIndex, pivotPos, bfalse );

            bindedGeneratorAnimComp->getBonePos( m_pivotBoneIndex, bindedPivotPos, bfalse );
            
            Vec2d generatorsDir = ( bindedPivotPos - pivotPos ).truncateTo2D();
            generatorsDir = generatorsDir.normalize();
            
            m_orientationCursor = computeOrientationCursor( m_actor, generatorsDir );
            
            generatorsDir = -generatorsDir;
            bindedGeneratorAnimComp->setInput( getTemplate()->getAddOrientationInput(), computeOrientationCursor( bindedGenerator, generatorsDir ) );
        }
        m_orientDone = btrue;
    }

    //------------------------------------------------------------------------------------------------------------------
    f32 Ray_LightningGeneratorAIComponent::computeOrientationCursor( Actor * _actor, Vec2d & _pivotDir )
    {
        // binded generator
        Angle minAngle = getTemplate()->getMinAngle();
        Angle maxAngle = getTemplate()->getMaxAngle();
        Vec2d actorDir = Vec2d::Right.Rotate( _actor->getAngle() );
        Vec2d actorDirPerp = -actorDir.getPerpendicular();

        if ( _actor->isFlipped() )
        {
            minAngle.SetRadians( canonizeAngle( minAngle.ToRadians() - MTH_PI ) );
            maxAngle.SetRadians( canonizeAngle( maxAngle.ToRadians() - MTH_PI ) );
            actorDirPerp = -actorDirPerp;
        }
        
        f32 generatorsAngle = actorDir.getOrientedAngle( _pivotDir ) * getSign( _pivotDir.dot( actorDirPerp ) );

        Angle desAngle = ClampAngle( Angle( bfalse, generatorsAngle ), minAngle, maxAngle );

        f32 total = ( maxAngle.ToRadians() - minAngle.ToRadians() );
        if ( total != 0.0 )
        {
            return ( 1.0f - ( desAngle.ToRadians() - minAngle.ToRadians() ) / total );
        }
        return 0.0f;
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_LightningGeneratorAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        if ( ( m_animatedComponent != NULL ) && getTemplate()->getAddOrientationInput().isValid() )
        {
            m_animatedComponent->setInput( getTemplate()->getAddOrientationInput(), m_orientationCursor );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
    void Ray_LightningGeneratorAIComponent::onEditorMove(bbool _modifyInitialPos /*= btrue*/)
    {
        m_orientDone = bfalse;
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_LightningGeneratorAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_LightningGeneratorAIComponent_Template)
        SERIALIZE_MEMBER( "pivotBoneName",      m_pivotBoneName );
        SERIALIZE_MEMBER( "minAngle",           m_minAngle );
        SERIALIZE_MEMBER( "maxAngle",           m_maxAngle );
        SERIALIZE_MEMBER( "addOrientationInput",m_addOrientationInput );
        SERIALIZE_MEMBER( "dynamicOrientation", m_dynamicOrientation );
    END_SERIALIZATION()

    Ray_LightningGeneratorAIComponent_Template::Ray_LightningGeneratorAIComponent_Template() 
    : Super()
    , m_dynamicOrientation( bfalse )
    {
        m_minAngle.SetRadians( 0.0f );
        m_maxAngle.SetRadians( 0.0f );
    }



}
