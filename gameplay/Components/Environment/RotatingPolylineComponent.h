
#ifndef _ITF_ROTATINGPOLYLINECOMPONENT_H_
#define _ITF_ROTATINGPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{
    class RotatingPolylineComponent : public PolylineComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RotatingPolylineComponent,PolylineComponent,4217284053);

    public:

        DECLARE_SERIALIZE()

        RotatingPolylineComponent();
        virtual ~RotatingPolylineComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );
        virtual void        validate( bbool &_isComponentValidated );

    protected:
        
        const class RotatingPolylineComponent_Template* getTemplate() const;

        typedef SafeArray <u32> BoneIndexList;

        struct RotatingPoly
        {
            RotatingPoly();

            f32                     m_force;
            f32                     m_currentTorque;
            f32                     m_currentAngle;
            u32                     m_rotatingBoneIndex;
            Vec2d                   m_rotatingModelPos;
            f32                     m_rotatingModelAngle;
            BoneIndexList           m_orderedBones;
        };

        typedef ITF_VECTOR <RotatingPoly> RotatingPolyList;

        virtual void            readModel();
        virtual void            processNewUser( class EventStickOnPolyline* _event );
        virtual void            processNewHanging( class EventHanging* _event );

        void                    clear();

        void                    transformWorldToLocal( const Vec2d& _pos, const Vec2d& _forceDir, RotatingPoly& _rotPoly,
                                                       Vec2d& _localPos, Vec2d& _localGravityDir ) const;
        void                    calculateRotatingBones( AnimMeshScene* _animMeshScene, RotatingPoly& _rotPoly );
        void                    updateRotatingPoly( f32 _dt, const SafeArray <StringID>& _polylines, f32 _forceMultiplier,
                                                    f32 _brake, const Vec2d& _gravity, RotatingPoly& _rotatingPoly );
        void                    updateTransform( RotatingPoly& _rotatingPoly, AnimMeshScene* _animMeshScene );
        void                    applyForceFromPoint( const Vec2d& _pos, const Vec2d& _force, f32 _forceMultiplier,
                                                     RotatingPoly& _rotatingPoly, ProceduralPolyline* _procPoly );
        void                    processPhysBodyForceEvent( EventForceTransfer* _physBodyForce );

        RotatingPolyList        m_rotatingPolys;
    };

    //---------------------------------------------------------------------------------------------------

    class RotatingPolylineComponent_Template : public PolylineComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(RotatingPolylineComponent_Template,PolylineComponent_Template,2496154729);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(RotatingPolylineComponent);

    public:

        class RotatingPoly
        {
            DECLARE_SERIALIZE()

        public:

            RotatingPoly();

            f32                         getBrake() const { return m_brake; }
            f32                         getForceMultiplier() const { return m_forceMultiplier; }
            const StringID&             getRotatingBoneName() const { return m_rotatingBoneName; }
            const SafeArray <StringID>& getPolylines() const { return m_polylines; }

        private:
            f32                         m_brake;
            f32                         m_forceMultiplier;
            StringID                    m_rotatingBoneName;
            SafeArray <StringID>        m_polylines;
        };

        typedef ITF_VECTOR <RotatingPoly> RotatingPolyList;

        RotatingPolylineComponent_Template() {}
        ~RotatingPolylineComponent_Template() {}

        const RotatingPolyList&         getRotatingPolys() const { return m_rotatingPolys; }
        const RotatingPoly*             findRotatingPolyFromAnimRef( const StringID& _ref, u32& _index ) const;

    private:

        RotatingPolyList                m_rotatingPolys;
    };

    ITF_INLINE const RotatingPolylineComponent_Template* RotatingPolylineComponent::getTemplate() const
    {
        return static_cast<const RotatingPolylineComponent_Template*>(m_template);
    }
}

#endif // _ITF_ROTATINGPOLYLINECOMPONENT_H_
