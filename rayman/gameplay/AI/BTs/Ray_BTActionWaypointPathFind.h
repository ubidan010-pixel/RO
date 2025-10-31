#ifndef _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_
#define _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

#ifndef _ITF_RAY_AINETWORKUTILS_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkUtils.h"
#endif //_ITF_RAY_AINETWORKUTILS_H_

namespace ITF
{

    class Ray_BTActionWaypointPathFind : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionWaypointPathFind,BTAction,3546756539);

    public:

        Ray_BTActionWaypointPathFind();
        ~Ray_BTActionWaypointPathFind() {}

        virtual void                            onActivate();
        virtual bbool                           decide( f32 _dt );
        virtual bbool                           update( f32 _dt );

    private:

        const class Ray_BTActionWaypointPathFind_Template*  getTemplate() const;
        bbool                                   isAtWaypoint( Actor* _waypoint ) const;
        void                                    setObjective( Pickable* _objective, Ray_AINetworkUtils::WaypointLinkType _linkType );
    };

    class Ray_BTActionWaypointPathFind_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionWaypointPathFind_Template,BTAction_Template,1736178641);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionWaypointPathFind);

    public:

        Ray_BTActionWaypointPathFind_Template();
        ~Ray_BTActionWaypointPathFind_Template() {}

    private:

    };

    ITF_INLINE const Ray_BTActionWaypointPathFind_Template*  Ray_BTActionWaypointPathFind::getTemplate() const {return static_cast<const Ray_BTActionWaypointPathFind_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_
