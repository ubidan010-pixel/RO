#ifndef _ITF_RAY_AIUTILS_H_
#define _ITF_RAY_AIUTILS_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

namespace ITF
{

class AnimLightComponent;
class Ray_AIScoreLumBehavior;
class Ray_ScoreLumAIComponent;
class Ray_PersistentGameData_Level;

namespace Ray_AIUtils
{
    const PolyLineEdge * getSquashingPolyline(const Actor * _actor, ObjectRef _currentStickedPolylineRef, const PolyLineEdge* _currentEdge, f32 _radius, f32 _penetrationRadius);
    Vec2d getSoftCollForce( Actor* _to, const Vec2d& _fromPos, const f32 _fromRadius, const f32 _exitSpeed, const f32 _exitForce);
    
    Ray_AIScoreLumBehavior* fetchScoreLumBehavior();
    Ray_ScoreLumAIComponent* fetchScoreLumComponent();
    
    Vec2d getRadialConstrainedDir(const Vec2d& _from, const Vec2d& _to);
    void sendRadialConstrainedPunchStim(const Actor* _from, Actor* _to);

    void showBossMedal( ActorRef _cineRef, ActorRef _sender );
    
    bbool setupNoseShape(
        const Actor* _actor,
        const AnimLightComponent* _animComponent,
        const StringID& _nosePolyline,
        PhysShapePolygon& _noseShape,
        PhysShapePolygon::PointsList& _noseShapePoints,
        PhysShapePolygon::PointsList& _prevNosePoints
        );

    bbool isBubblePrize(const Actor* _actor);

    u32 computeElectoonMask(const StringID &_missionType, 
        const Ray_PersistentGameData_Level *_level,
        Actor *_ignoredCage,
        u32 _ignoreChallenges/*see EChallengeType*/,
        bbool & _isComplete);
};

};


#endif // _ITF_RAY_AIUTILS_H_
