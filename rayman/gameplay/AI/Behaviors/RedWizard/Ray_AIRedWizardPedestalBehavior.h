#ifndef _ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_
#define _ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

namespace ITF
{
    class Ray_AIRedWizardPedestalBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardPedestalBehavior, AIBehavior,1111841414)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIRedWizardPedestalBehavior();
        virtual ~Ray_AIRedWizardPedestalBehavior();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual bbool       isActivatedByEvent( Event* _event );
        virtual void        setData( Event* _event );
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        onEvent( Event* _event );
        virtual void        update( f32 _dt );
        virtual void        updateAnimInputs();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_AIRedWizardPedestalBehavior_Template * getTemplate() const;

        AIAction*           m_idle;

        void                updatePolyline( f32 _dt );

        ProceduralPolyline  m_polyline;
        SafeArray<Vec2d>    m_polylinePoints;
        u32                 m_supportBoneIndex;

        ObjectRef           m_target;       // the object we're currently standing on
        f32                 m_playerDetectRadius;
    };

    class Ray_AIRedWizardPedestalBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRedWizardPedestalBehavior_Template, TemplateAIBehavior,2561050822);
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRedWizardPedestalBehavior);
        DECLARE_SERIALIZE();

    public:

        Ray_AIRedWizardPedestalBehavior_Template();
        ~Ray_AIRedWizardPedestalBehavior_Template();

        const AIAction_Template*    getIdle() const { return m_idle; }
        const StringID&             getSupportBone() const { return m_supportBone; }
        const Path&                 getGameMaterial() const { return m_gameMaterial; }

    private:

        AIAction_Template*          m_idle;
        StringID                    m_supportBone;
        Path                        m_gameMaterial;
    };

    ITF_INLINE const class Ray_AIRedWizardPedestalBehavior_Template * Ray_AIRedWizardPedestalBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRedWizardPedestalBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_