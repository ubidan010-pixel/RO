
#ifndef _ITF_AXISPOLYLINECOMPONENT_H_
#define _ITF_AXISPOLYLINECOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{
    class AxisPolylineComponent : public PolylineComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AxisPolylineComponent,PolylineComponent,1003966487);

    public:

        DECLARE_SERIALIZE()

        AxisPolylineComponent();
        virtual ~AxisPolylineComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );
        virtual void        validate( bbool &_isComponentValidated );

    protected:

        const class AxisPolylineComponent_Template* getTemplate() const;

        typedef SafeArray <u32> BoneIndexList;

        struct AxisPoly
        {
            AxisPoly();

            f32                     m_forces;
            f32                     m_totalWeight;
            f32                     m_currentTorque;
            f32                     m_currentAngle;
            u32                     m_rotatingBoneIndex;
            Vec2d                   m_rotatingModelPos;
            f32                     m_rotatingModelAngle;
            BoneIndexList           m_orderedBones;
        };

        typedef ITF_VECTOR <AxisPoly> AxisPolyList;

        void                postLoadPropertyEdit();

        virtual void        readModel();
        virtual void        onNewUser( class EventStickOnPolyline* _event, ProceduralPolyline* _procPoly );
        virtual void        onNewHangActor( class EventHanging* _event, ProceduralPolyline* _procPoly );
        virtual void        processPhysBodyForceEvent( class EventForceTransfer* _physBodyForce );

        void                clear();
        void                transformWorldToLocal( const Vec2d& _pos, const Vec2d& _gravityDir, AxisPoly& _rotPoly,
                                                   Vec2d& localPos, Vec2d& localGravityDir ) const;
        void                calculateRotatingBones( AnimMeshScene* _animMeshScene, AxisPoly& _rotPoly );
        void                applyForceFromPoint( const Vec2d& _pos, f32 _force, AxisPoly& _axisInfo );
        void                applyWeightFromPoint( const Vec2d& _pos, f32 _weight, AxisPoly& _axisInfo );
        void                updateAxis( f32 _dt, class AnimMeshScene* _animMeshScene, const SafeArray <StringID>& _polylines,
                                        f32 _weight2Angle, f32 _stiff, f32 _damp, const Angle& _maxAngle, AxisPoly& _axisPoly );
        void                updateTransform( AxisPoly& _axisPoly, AnimMeshScene* animMeshScene );
        void                applyLand( ObjectRef _poly, const Vec2d& _pos, const Vec2d& _speed, f32 _weight );
        
        AxisPolyList        m_axisPolylines;
    };

    //---------------------------------------------------------------------------------------------------

    class AxisPolylineComponent_Template : public PolylineComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AxisPolylineComponent_Template,PolylineComponent_Template,2334182358);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AxisPolylineComponent);

    public:

        class AxisPoly
        {
            DECLARE_SERIALIZE()

        public:

            AxisPoly();

            f32                         getStiff() const { return m_stiff; }
            f32                         getDamp() const { return m_damp; }
            f32                         getWeightToAngle() const { return m_weightToAngle; }
            const Angle&                getMaxAngle() const { return m_maxAngle; }
            const StringID&             getRotatingBoneName() const { return m_rotatingBoneName; }
            f32                         getWeightMultiplier() const { return m_weightMultiplier; }
            f32                         getForceMultiplier() const { return m_forceMultiplier; }
            const SafeArray<StringID>&  getPolylines() const { return m_polylines; }

        private:

            f32                     m_stiff;
            f32                     m_damp;
            f32                     m_weightToAngle;
            Angle                   m_maxAngle;
            StringID                m_rotatingBoneName;
            f32                     m_weightMultiplier;
            f32                     m_forceMultiplier;
            SafeArray <StringID>    m_polylines;
        };

        typedef ITF_VECTOR <AxisPoly> AxisPolyList;

        AxisPolylineComponent_Template() {}
        ~AxisPolylineComponent_Template() {}

        const AxisPolyList&         getAxisPolylines() const { return m_axisPolylines; }
        const AxisPoly*             findAxisPolyFromAnimRef( const StringID& _ref, u32& _index ) const;

    private:

        AxisPolyList                m_axisPolylines;
    };

    ITF_INLINE const AxisPolylineComponent_Template* AxisPolylineComponent::getTemplate() const
    {
        return static_cast<const AxisPolylineComponent_Template*>(m_template);
    }
}

#endif // _ITF_AXISPOLYLINECOMPONENT_H_
