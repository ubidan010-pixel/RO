#ifndef _RAY_AI_UMBRELLABEHAVIOR_H_
#define _RAY_AI_UMBRELLABEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    //forward
    class Ray_AIUmbrellaBehavior_Template;
    class PolylineComponent;

    /**********************************************************************/
    //  Ray_AIUmbrellaBehavior :
    //
    //      Make umbrella platform toggle between closed and opened state using events
    /**********************************************************************/

    class Ray_AIUmbrellaBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIUmbrellaBehavior, AIBehavior,2834551575);
        DECLARE_VALIDATE_BEHAVIOR();

    public:
        Ray_AIUmbrellaBehavior();
        virtual ~Ray_AIUmbrellaBehavior();

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActivate();
        virtual void update(f32 _dt);
        virtual void onEvent(Event * _event);

        ITF_INLINE const Ray_AIUmbrellaBehavior_Template * getTemplate() const;

    private:
        //start behavior with the right action according to metronome current state
        void initAction();

        void startAction();
        void startOpenAction();
        void startClosedAction();
        void startOpeningAction();
        void startClosingAction();
        void startWarningAction();

        void updateOpenAction();
        void updateClosedAction();
        void updateOpeningAction();
        void updateClosingAction();
        void updateWarningAction();

        void onTriggerEvent(const EventTrigger * const _event);
        void onGenericEvent(const EventGeneric * const _event);

    private:
        AIAction * m_openAction;
        AIAction * m_closedAction;
        AIAction * m_openingAction;
        AIAction * m_closingAction;
        AIAction * m_warningAction;

        PolylineComponent * m_polylineComponent;

        f32 m_currentActionTimer;
    };




    /**********************************************************************/
    //  template class
    /**********************************************************************/

    class Ray_AIUmbrellaBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIUmbrellaBehavior_Template,TemplateAIBehavior,1503834881);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIUmbrellaBehavior);

    public:
        Ray_AIUmbrellaBehavior_Template();
        ~Ray_AIUmbrellaBehavior_Template();

        ITF_INLINE const AIAction_Template * getOpenAction() const;
        ITF_INLINE const AIAction_Template * getClosedAction() const;
        ITF_INLINE const AIAction_Template * getOpeningAction() const;
        ITF_INLINE const AIAction_Template * getClosingAction() const;
        ITF_INLINE const AIAction_Template * getWarningAction() const;

        ITF_INLINE f32 getClosedStateTime() const;
        ITF_INLINE f32 getWarningStateTime() const;
        ITF_INLINE f32 getWeightThreshold() const;

        ITF_INLINE bbool getCloseWithWeight() const;

    private:
        AIAction_Template * m_openAction;
        AIAction_Template * m_closedAction;
        AIAction_Template * m_openingAction;
        AIAction_Template * m_closingAction;
        AIAction_Template * m_warningAction;

        f32 m_closedStateTime;
        f32 m_warningStateTime;
        f32 m_weightThreshold;

        bbool m_closeWithWeight;
    };



    /**********************************************************************/
    //  inlined functions definition
    /**********************************************************************/

    ITF_INLINE const Ray_AIUmbrellaBehavior_Template * Ray_AIUmbrellaBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIUmbrellaBehavior_Template *>( m_template );
    }


    ITF_INLINE const AIAction_Template * Ray_AIUmbrellaBehavior_Template::getOpenAction() const
    {
        return m_openAction;
    }

    ITF_INLINE const AIAction_Template * Ray_AIUmbrellaBehavior_Template::getClosedAction() const
    {
        return m_closedAction;
    }

    ITF_INLINE const AIAction_Template * Ray_AIUmbrellaBehavior_Template::getOpeningAction() const
    {
        return m_openingAction;
    }

    ITF_INLINE const AIAction_Template * Ray_AIUmbrellaBehavior_Template::getClosingAction() const
    {
        return m_closingAction;
    }

    ITF_INLINE const AIAction_Template * Ray_AIUmbrellaBehavior_Template::getWarningAction() const
    {
        return m_warningAction;
    }        

    ITF_INLINE f32 Ray_AIUmbrellaBehavior_Template::getClosedStateTime() const
    {
        return m_closedStateTime;
    }

    ITF_INLINE f32 Ray_AIUmbrellaBehavior_Template::getWarningStateTime() const
    {
        return m_warningStateTime;
    }

    ITF_INLINE f32 Ray_AIUmbrellaBehavior_Template::getWeightThreshold() const
    {
        return m_weightThreshold;
    }

    ITF_INLINE bbool Ray_AIUmbrellaBehavior_Template::getCloseWithWeight() const
    {
        return m_closeWithWeight;
    }
}

#endif //_RAY_AI_UMBRELLABEHAVIOR_H_