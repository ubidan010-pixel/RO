#ifndef _ITF_SOFTPLATFORMCOMPONENT_H_
#define _ITF_SOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SOFTPLATFORM_H_
#include "gameplay/components/Environment/SoftPlatform.h"
#endif //_ITF_SOFTPLATFORM_H_

namespace ITF
{
    class SoftPlatformComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(SoftPlatformComponent,ActorComponent,3322875823);
        DECLARE_SERIALIZE()

        SoftPlatformComponent();
        virtual ~SoftPlatformComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        virtual void                onResourceReady();
        virtual void                Update( f32 _deltaTime );
        virtual void                validate( bbool &_isComponentValidated );

        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onEvent( Event * _event );

    protected:

        ITF_INLINE const class SoftPlatformComponent_Template* getTemplate() const;

        struct BodyData
        {
            BodyData();

            StringID            m_boneId;
            u32                 m_boneIndex;
            bbool               m_boneEnd;
        };

        struct SoftBoneInfo
        {
            u32                     m_bodyIndex;
            u32                     m_boneIndex;
            u32                     m_parentIndex;
            Vec2d                   m_pos;
            f32                     m_angle;
        };

        typedef ITF_VECTOR <BodyData> BodiesArray;
        typedef SafeArray <SoftBoneInfo> SoftBoneInfoArray;

        virtual void                readModel();
        virtual void                createSoftPlatform();

        virtual void                processAnimUpdated( class EventAnimUpdated* _animUpdated );

        void                        syncPhysWithAnim();

        void                        updateConstraints();
        void                        updateTransforms();
        void                        updatePhysBodyRotations();
        u32                         findBodyIndex( const ITF_STDSTRING& _name ) const;
        void                        orderBonesRecursive( const VectorAnim<class AnimBone>& _boneList,
                                                         u32 _boneIndex, ITF_VECTOR <bbool>& _processed );

        void                        updateBodyBone( u32 _softBoneIndex, AnimMeshScene* _animScene, const struct transform2d& _xf );
        void                        getParentTransform( u32 _parentSoftBoneIndex, Vec2d& _pos, f32& _angle ) const;
        void                        transformWorldToModel( const Vec2d& _world, const struct transform2d& _xf, Vec2d& _model ) const;
        void                        getBodySkelPos( const BodyData& _bodyData,
                                                    u32 _bodyIndex,
                                                    bbool _isStatic,
                                                    AnimBoneDyn *_boneDyn,
                                                    u32 _parentIndex,
                                                    const struct transform2d& _xf,
                                                    Vec2d& _model,
                                                    Vec2d& _local ) const;
        void                        getBodySkelRot( const Vec2d _posLocalA,
                                                    const Vec2d _posLocalB,
                                                    const Vec2d _posModelA,
                                                    const Vec2d _posModelB,
                                                    f32& _angleLocal,
                                                    f32& _angleModel ) const;

        void                        clear();

        class AnimLightComponent*   m_animComponent;
        SoftPlatform*               m_softPlatform;
        BodiesArray                 m_bodies;
        SoftBoneInfoArray           m_orderedBones;
        bbool                       m_ready;
        bbool                       m_teleported;
    };

    //---------------------------------------------------------------------------------------------------

    class SoftPlatformComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SoftPlatformComponent_Template,TemplateActorComponent,1141144583);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SoftPlatformComponent);

    public:

        class BodyData
        {
            DECLARE_SERIALIZE()

            BodyData();

            const ITF_STDSTRING&    getBoneName() const { return m_boneName; }
            f32                     getGravityMultiplier() const { return m_gravityMultiplier; }
            f32                     getWindMultiplier() const { return m_windMultiplier; }
            const Angle&            getSetAngle() const { return m_setAngle; }
            bbool                   getIsStatic() const { return m_static; }
            bbool                   getUseStaticAngle() const { return m_useStaticAngle; }

        private:

            ITF_STDSTRING       m_boneName;
            f32                 m_gravityMultiplier;
            f32                 m_windMultiplier;
            Angle               m_setAngle;
            bbool               m_static;
            bbool               m_useStaticAngle;
        };

        class ConstraintData
        {
            DECLARE_SERIALIZE()

            ConstraintData();

            const ITF_STDSTRING&    getBodyAName() const { return m_bodyAName; }
            const ITF_STDSTRING&    getBodyBName() const { return m_bodyBName; }
            const Angle&            getMinAngle() const { return m_minAngle; }
            const Angle&            getMaxAngle() const { return m_maxAngle; }
            f32                     getMinLength() const { return m_minLength; }
            f32                     getMaxLength() const { return m_maxLength; }
            f32                     getStiff() const { return m_stiff; }
            f32                     getDamp() const { return m_damp; }
            bbool                   getIsLimitAngle() const { return m_limitAngle; }
            bbool                   getIsRelaxLength() const { return m_relaxLength; }

        private:

            ITF_STDSTRING           m_bodyAName;
            ITF_STDSTRING           m_bodyBName;
            Angle                   m_minAngle;
            Angle                   m_maxAngle;
            f32                     m_minLength;
            f32                     m_maxLength;
            f32                     m_stiff;
            f32                     m_damp;
            bbool                   m_limitAngle;
            bbool                   m_relaxLength;
        };

        typedef ITF_VECTOR <BodyData> BodiesArray;
        typedef ITF_VECTOR <ConstraintData> ConstraintArray;

        SoftPlatformComponent_Template();
        ~SoftPlatformComponent_Template() {}

        const BodiesArray&          getBodies() const { return m_bodies; }
        const ConstraintArray&      getConstraints() const { return m_constraints; }
        f32                         getWeightMultiplier() const { return m_weightMultiplier; }
        f32                         getLandSpeedMultiplier() const { return m_landSpeedMultiplier; }
        f32                         getHitForceMultiplier() const { return m_hitForceMultiplier; }
        f32                         getImpulseMultiplier() const { return m_impulseMultiplier; }
        f32                         getMovingPolylineForce() const { return m_movingPolylineForce; }
        const Path&                 getGameMaterial() const { return m_gameMaterial; }
        bbool                       getUsePhantom() const { return m_usePhantom; }

    private:

        BodiesArray                 m_bodies;
        ConstraintArray             m_constraints;
        f32                         m_weightMultiplier;
        f32                         m_landSpeedMultiplier;
        f32                         m_hitForceMultiplier;
        f32                         m_impulseMultiplier;
        f32                         m_movingPolylineForce;
        Path                        m_gameMaterial;
        bbool                       m_usePhantom;
    };

    ITF_INLINE const SoftPlatformComponent_Template* SoftPlatformComponent::getTemplate() const
    {
        return static_cast<const SoftPlatformComponent_Template*>(m_template);
    }
}

#endif // _ITF_SOFTPLATFORMCOMPONENT_H_
