#ifndef _ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_
#define _ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERAICOMPONENT_H_
#include "gameplay/Components/AI/TimedSpawnerAIComponent.h"
#endif //_ITF_TIMEDSPAWNERAICOMPONENT_H_

namespace ITF
{
    class Ray_LightningGeneratorAIComponent : public TimedSpawnerAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LightningGeneratorAIComponent,TimedSpawnerAIComponent,2701334616)
        DECLARE_SERIALIZE();

    public:

        Ray_LightningGeneratorAIComponent();
        virtual ~Ray_LightningGeneratorAIComponent() {}

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBehaviorFinished();
        virtual void        updateAnimInput();
        virtual void        Update( f32 _dt );
        virtual void        onBecomeActive();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
        Vec2d               m_prevBindedActorPos;
#endif // ITF_SUPPORT_EDITOR


    protected:

        ITF_INLINE const class Ray_LightningGeneratorAIComponent_Template * getTemplate() const;

        f32    computeOrientationCursor( Actor * _actor, Vec2d & _pivotDir );

        f32     m_orientationCursor;
        u32     m_pivotBoneIndex;
        bbool   m_orientDone;
    
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************
    class Ray_LightningGeneratorAIComponent_Template : public TimedSpawnerAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LightningGeneratorAIComponent_Template,TimedSpawnerAIComponent_Template,1969464090);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LightningGeneratorAIComponent);

    public:

        Ray_LightningGeneratorAIComponent_Template();
        virtual ~Ray_LightningGeneratorAIComponent_Template()  {}

        ITF_INLINE const StringID &     getPivotBoneName() const        { return m_pivotBoneName; }
        ITF_INLINE const StringID &     getAddOrientationInput() const  { return m_addOrientationInput; }
        ITF_INLINE const Angle &        getMinAngle() const             { return m_minAngle; }
        ITF_INLINE const Angle &        getMaxAngle() const             { return m_maxAngle; }
        ITF_INLINE const bbool          useDynamicOrientation() const   { return m_dynamicOrientation; }
        
    private:
        StringID    m_pivotBoneName;
        StringID    m_addOrientationInput;
        Angle       m_minAngle;
        Angle       m_maxAngle;
        bbool       m_dynamicOrientation;
    };

    ITF_INLINE const class Ray_LightningGeneratorAIComponent_Template * Ray_LightningGeneratorAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_LightningGeneratorAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_

