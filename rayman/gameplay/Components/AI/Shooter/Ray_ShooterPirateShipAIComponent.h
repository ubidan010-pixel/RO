#ifndef _ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_
#define _ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_

#ifndef _ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MultiPiecesActorAIComponent.h"
#endif //_ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_

namespace ITF
{
    class Ray_ShooterPirateShipAIComponent : public Ray_MultiPiecesActorAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterPirateShipAIComponent, Ray_MultiPiecesActorAIComponent ,1339744900)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterPirateShipAIComponent();
        ~Ray_ShooterPirateShipAIComponent() {}

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );
        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);
        virtual void                updateAnimInput();
        virtual void                onDepthChanged( f32 _oldZ, f32 _newZ );
        virtual void                onBecomeActive();

    private:

        const class Ray_ShooterPirateShipAIComponent_Template* getTemplate() const;
        //void                        onCheckpointLoaded();

        virtual void                processHit( HitStim* _hit );
        virtual void                onPieceHit( Actor * _piece );
        
        void                        preChangeProperties();

        f32         m_bounceBallonAnimCursor;
        Color       m_curFogColor;
        f32         m_lastCamZ;
        f32         m_lastCamDepth;
        Color       m_destFogColor;
        f32         m_fogInterpolTimer;

        // instance params
        bbool       m_useTempateFogParams;
        Color       m_instanceDynamicFogColor;
        f32         m_instanceDynamicFogMaxDepth;

        bbool       m_useModifierFogParams;
        Color       m_modifierDynamicFogColor;
        f32         m_modifierDynamicFogMaxDepth;
      
    };



    //---------------------------------------------------------------------------------------------------------------------------------------
    class Ray_ShooterPirateShipAIComponent_Template : public Ray_MultiPiecesActorAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterPirateShipAIComponent_Template, Ray_MultiPiecesActorAIComponent_Template,4045220477);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterPirateShipAIComponent);

    public:

        Ray_ShooterPirateShipAIComponent_Template();
        ~Ray_ShooterPirateShipAIComponent_Template();

        ITF_INLINE const StringID &     getBounceBallonInput()      const   { return m_bounceBallonInput; }
        ITF_INLINE bbool                useDynamicFog()             const   { return m_useDynamicFog; }
        ITF_INLINE const Vec3d &        getDynamicFogDefaultColor() const   { return m_dynamicFogDefaultColor; }
        ITF_INLINE f32                  getDynamicFogMaxDepth()     const   { return m_dynamicFogMaxDepth; }

    private:

        StringID    m_bounceBallonInput;
        bbool       m_useDynamicFog;
        Vec3d       m_dynamicFogDefaultColor;
        f32         m_dynamicFogMaxDepth;
    };

    ITF_INLINE const Ray_ShooterPirateShipAIComponent_Template* Ray_ShooterPirateShipAIComponent::getTemplate() const
    {
        return static_cast<const Ray_ShooterPirateShipAIComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_


