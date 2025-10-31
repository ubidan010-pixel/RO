#ifndef _ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_
#define _ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_

#ifndef _ITF_AIPLAYACTIONSBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIPlayActionsBehavior.h"
#endif //_ITF_AIPLAYACTIONSBEHAVIOR_H_

namespace ITF
{
    class Ray_AIRestoreAngleBehavior : public AIPlayActionsBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRestoreAngleBehavior, AIPlayActionsBehavior,3361548879)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIRestoreAngleBehavior();
        virtual ~Ray_AIRestoreAngleBehavior();

        virtual void            update(f32 _dt);
#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    private:

        ITF_INLINE const class Ray_AIRestoreAngleBehavior_Template * getTemplate() const;

        f32 m_initialAngle;
 
    };


    //*****************************************************************************
    // Template
    //*****************************************************************************

 
    class Ray_AIRestoreAngleBehavior_Template : public AIPlayActionsBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRestoreAngleBehavior_Template, AIPlayActionsBehavior_Template,3042698962);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRestoreAngleBehavior);

    public:

        Ray_AIRestoreAngleBehavior_Template();
        ~Ray_AIRestoreAngleBehavior_Template();

        ITF_INLINE f32 getAngleSpeed() const { return m_angleSpeed; }

    private:

        f32 m_angleSpeed;

    };

    ITF_INLINE const class Ray_AIRestoreAngleBehavior_Template * Ray_AIRestoreAngleBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRestoreAngleBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_
