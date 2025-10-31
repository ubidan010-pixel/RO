#ifndef _ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_
#define _ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackManagerAIComponent.h"
#endif //_ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

    class FXControllerComponent;

    class Ray_BreakableStackElementAIComponent : public Ray_AIComponent
    {

        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableStackElementAIComponent, Ray_AIComponent,1985991899)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BreakableStackElementAIComponent();
        virtual ~Ray_BreakableStackElementAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        Update( f32 _dt );
        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onCheckpointLoaded();
        virtual void        onEvent( Event* _event );
        virtual void        Draw();

#ifdef ITF_SUPPORT_EDITOR
                void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
                
        // For manager
        const FxData&          getFxData() const;
        const InfoHeadElement  getInfoElement() const;
        const InfoElementGrid& getGridElement() const;
              void             flagDestroy();
              void             saveCheckpointPos(u32 _row, u32 _col);
    private:
                void        setBlockState(u32 _state);
                u32         getBlockState() const { return m_block.m_state; }
                void        sendEventToPolyline(Event* _event, ObjectRef _polylineRef);
                void        growAABB();
                void        initAtlas();
                void        initBlock();
                void        updatePolyline( f32 _dt );
                void        updatePosBallistics( f32 _dt );
                void        drawAtlas() const;
                void        printBlock();
                void        hitBlock(Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender);
                void        destroyBlock(Vec2d _dirHit, u32 _hitLevel, ObjectRef _sender);

                void        playSound(const StringID& _sound);
                u32         playFx(StringID _name, Vec3d _pos);
                void        setFxPos(u32 _handle, Vec3d _pos);
                void        setFxAngle(u32 _handle, f32 _angle);
                void        stopFX(u32 _handle);

                Vec2d       getShake(f32 _intensX, f32 _freqX, f32 _intensY, f32 _freqY, bbool _dephase = bfalse) const;

                void        activateAllBox(bbool _active);

                void        drawParticles(const Ray_BreakableStackManagerAIComponent::Box* _box) const;
                void        updateParticles( f32 _dt );

    private:
        ITF_INLINE const class Ray_BreakableStackElementAIComponent_Template * getTemplate() const;

        FXControllerComponent* m_fxControllerComponent;

        f32     m_timer;

        // Size
        u32     m_width;
        u32     m_height;
        f32     m_edgeSize;

        // Spawn / Print
        u32     m_countSpawnMax;
        u32     m_countSpawn;

        // Manager
        ObjectPath m_managerPath;
        ActorRef   m_managerRef;

        // Atlas
        mutable AtlasDynamicObject  m_atlasObject;
        mutable AtlasDynamicObject  m_atlasParticles;
        f32     m_atlasOffset;
        Vec2d   m_offsetCenter;

        bbool   m_instantSpawned;

        PhysShapePolygon    m_physShapeBox;

        Ray_BreakableStackManagerAIComponent::Block  m_block;

        u32 m_blockState;
        bbool m_blockIsDestroy;
        u32 m_checkPointRow;
        u32 m_checkPointCol;

#ifdef ITF_SUPPORT_EDITOR 
		u32 m_countLums;
#endif

    };

    //-------------------------------------------------------------------------------------
    class Ray_BreakableStackElementAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BreakableStackElementAIComponent_Template,Ray_AIComponent_Template,3047820808);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BreakableStackElementAIComponent);

    public:

 
        Ray_BreakableStackElementAIComponent_Template();
        ~Ray_BreakableStackElementAIComponent_Template();

        const InfoElementGrid&   getGrid() const        { return m_grid; }
        const FxData&    getFxData() const              { return m_fxData; }
        ITF_INLINE u32   getWidth() const               { return m_width; }
        ITF_INLINE u32   getHeight() const              { return m_height; }
        ITF_INLINE Path  getGmatPath() const            { return m_gmatPath; }
        ITF_INLINE u32   getHitPoint() const            { return m_hitPoint; }
        ITF_INLINE Path  getAtlasPath() const           { return m_atlasPath; }
        ITF_INLINE Path  getAtlasParticlesPath() const  { return m_atlasParticlesPath; }


        ITF_INLINE f32   getCountDownHit() const        { return m_countDownHit; }
        ITF_INLINE f32   getGravityBallistics() const   { return m_gravityBallistics; }
        ITF_INLINE f32   getTimeExpulse() const         { return m_timeExpulse; }
        ITF_INLINE f32   getEdgeSize() const            { return m_edgeSize; }
        ITF_INLINE bbool getBlockStatic() const         { return m_blockStatic; }
        ITF_INLINE bbool getExplosive() const           { return m_explosive; }
        ITF_INLINE f32   getRadiusExplosive() const     { return m_radiusExplosive; }
        ITF_INLINE f32   getTimeBeforeExplode() const   { return m_timeBeforeExplode; }
        ITF_INLINE bbool getInstantSpawn() const        { return m_instantSpawn; }



    private:

        InfoElementGrid m_grid;

        u32 m_width;
        u32 m_height;
        Path m_gmatPath;
        u32  m_hitPoint;
        f32  m_edgeSize;
        f32 m_countDownHit;

        bbool m_blockStatic;
        bbool m_instantSpawn;

        // Explosive
        bbool m_explosive;
        f32   m_radiusExplosive;
        f32   m_timeBeforeExplode;

        // Ballistics
        f32 m_gravityBallistics;
        f32 m_timeExpulse;

        // Atlas
        Path m_atlasPath;
        Path m_atlasParticlesPath;

        // Fx
        FxData m_fxData;
    };

    ITF_INLINE const Ray_BreakableStackElementAIComponent_Template * Ray_BreakableStackElementAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BreakableStackElementAIComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_