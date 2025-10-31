#ifndef _ITF_RAY_DYNAMICFOGCOMPONENT_H_
#define _ITF_RAY_DYNAMICFOGCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class GraphicComponent;

    class Ray_DynamicFogComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DynamicFogComponent, ActorComponent ,822399796)

    public:
        DECLARE_SERIALIZE()

        Ray_DynamicFogComponent();
        ~Ray_DynamicFogComponent() {}

        virtual bbool               needsUpdate() const  { return btrue; }
        virtual bbool               needsDraw()   const  { return bfalse; }
        virtual bbool               needsDraw2D() const  { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _dt );
        virtual void                onEvent( Event * _event );

        u32                         getFogColor() const;
        f32                         getFogMaxDepth() const;

    private:

        const class Ray_DynamicFogComponent_Template* getTemplate() const;

        void                        preChangeProperties();
        void                        computeFog( const f32 _z );
        void                        processTriggered();

        Color                       m_curFogColor;
        SafeArray<GraphicComponent*> m_graphicComponents;

        // instance params
        bbool                       m_useTempateFogParams;
        Color                       m_instanceDynamicFogColor;
        f32                         m_instanceDynamicFogMaxDepth;
      
    };



    //---------------------------------------------------------------------------------------------------------------------------------------
    class Ray_DynamicFogComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DynamicFogComponent_Template, TemplateActorComponent,2089509264);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DynamicFogComponent);

    public:

        Ray_DynamicFogComponent_Template();
        ~Ray_DynamicFogComponent_Template();

        ITF_INLINE bbool                useDynamicFog()             const   { return m_useDynamicFog; }
        ITF_INLINE const Color &        getDynamicFogDefaultColor() const   { return m_dynamicFogDefaultColor; }
        ITF_INLINE f32                  getDynamicFogMaxDepth()     const   { return m_dynamicFogMaxDepth; }
        ITF_INLINE bbool                isDataOnly()                const   { return m_isDataOnly; }
        ITF_INLINE bbool                isModifier()                const   { return m_isModifier; }

    private:

        bbool       m_useDynamicFog;
        Color       m_dynamicFogDefaultColor;
        f32         m_dynamicFogMaxDepth;
        bbool       m_isDataOnly;                   // disable the actor
        bbool       m_isModifier;                   // send fog param to children on triggered
    };

    ITF_INLINE const Ray_DynamicFogComponent_Template* Ray_DynamicFogComponent::getTemplate() const
    {
        return static_cast<const Ray_DynamicFogComponent_Template*>(m_template);
    }
    
    ITF_INLINE u32  Ray_DynamicFogComponent::getFogColor() const
    {
        return m_useTempateFogParams ? getTemplate()->getDynamicFogDefaultColor().getAsU32() : m_instanceDynamicFogColor.getAsU32();
    }

    ITF_INLINE f32  Ray_DynamicFogComponent::getFogMaxDepth() const
    {
        return m_useTempateFogParams ? getTemplate()->getDynamicFogMaxDepth() : m_instanceDynamicFogMaxDepth;
    }
}

#endif //_ITF_RAY_DYNAMICFOGCOMPONENT_H_


