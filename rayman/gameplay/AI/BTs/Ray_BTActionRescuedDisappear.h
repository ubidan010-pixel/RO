#ifndef _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_
#define _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{

    class Ray_BTActionRescuedDisappear : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionRescuedDisappear,BTAction,826466370);

    public:

        Ray_BTActionRescuedDisappear();
        ~Ray_BTActionRescuedDisappear() {}

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual void                            onActivate();
        virtual bbool                           update( f32 _dt );

    private:

        const class Ray_BTActionRescuedDisappear_Template*  getTemplate() const;

        class AnimLightComponent*               m_animComponent;
        SpawneeGenerator                        m_fxGenerator;
        bbool                                   m_finished;
        bbool                                   m_firstUpdate;
    };

    class Ray_BTActionRescuedDisappear_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionRescuedDisappear_Template,BTAction_Template,929322276);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionRescuedDisappear);

    public:

        Ray_BTActionRescuedDisappear_Template();
        ~Ray_BTActionRescuedDisappear_Template() {}

        const StringID&         getHappyAnim() const { return m_happyAnim; }
        const Path&             getSpawnFX() const { return m_spawnFX; }

    private:

        StringID                m_happyAnim;
        Path                    m_spawnFX;
    };

    ITF_INLINE const Ray_BTActionRescuedDisappear_Template*  Ray_BTActionRescuedDisappear::getTemplate() const {return static_cast<const Ray_BTActionRescuedDisappear_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_
