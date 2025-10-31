#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#define _ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

namespace ITF
{
    class PlayerDetectorComponent : public ShapeDetectorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayerDetectorComponent, ShapeDetectorComponent,683675795);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:



        PlayerDetectorComponent();
        virtual ~PlayerDetectorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        Update( f32 _dt );

        ITF_INLINE i32      getPlayerId() const;

    private:


        ITF_INLINE const class PlayerDetectorComponent_Template*  getTemplate() const;
    };

    //-------------------------------------------------------------------------------------

    class PlayerDetectorComponent_Template : public ShapeDetectorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(PlayerDetectorComponent_Template,ShapeDetectorComponent_Template,982701186);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PlayerDetectorComponent);

    public:

        PlayerDetectorComponent_Template();
        ~PlayerDetectorComponent_Template() {}

        ITF_INLINE i32      getPlayerID()           const { return m_playerId; }
        ITF_INLINE bbool    getAllowDeadActors()    const { return m_allowDeadActors; }
        ITF_INLINE bbool    getFirstPlayerOnly()    const { return m_firstPlayerOnly; }
        ITF_INLINE GameMode getAllPlayerInMode()    const { return m_allPlayerInMode; }
        ITF_INLINE f32      getMaxDetectionRadius() const { return m_maxDetectionRadius; }

    private:

        i32                 m_playerId;     // triggers only if this specific player id is in the trigger
        bbool               m_allowDeadActors;
        bbool               m_firstPlayerOnly;
        GameMode            m_allPlayerInMode;
        f32                 m_maxDetectionRadius;
    };


    const PlayerDetectorComponent_Template*  PlayerDetectorComponent::getTemplate() const {return static_cast<const PlayerDetectorComponent_Template*>(m_template);}
    ITF_INLINE i32      PlayerDetectorComponent::getPlayerId() const { return getTemplate()->getPlayerID(); }
};

#endif //_ITF_PLAYERDETECTORCOMPONENT_H_