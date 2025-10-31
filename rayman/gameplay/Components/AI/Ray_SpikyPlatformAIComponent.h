#ifndef _ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_
#define _ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_

namespace ITF
{
    class TweenComponent;
    class PolylineComponent;

    /*
        Play a "down" tween when a user steps on our polyline,
        then go back up when everybody's left.
    */
    class Ray_SpikyPlatformAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyPlatformAIComponent, ActorComponent, 3383273419)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_SpikyPlatformAIComponent();
        virtual ~Ray_SpikyPlatformAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );

    private:
        ITF_INLINE const class Ray_SpikyPlatformAIComponent_Template* getTemplate() const;

        TweenComponent*     m_tweenComponent;
        PolylineComponent*  m_polylineComponent;

        bbool m_isUp;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_SpikyPlatformAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SpikyPlatformAIComponent_Template, TemplateActorComponent, 2629081214)
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SpikyPlatformAIComponent);

    public:

        Ray_SpikyPlatformAIComponent_Template()
            : m_setDown(ITF_GET_STRINGID_CRC(Down,652655112))
            , m_setUp(ITF_GET_STRINGID_CRC(Up,3605073068))
        {
        }
        virtual ~Ray_SpikyPlatformAIComponent_Template() {}

        ITF_INLINE const StringID& getSetDown() const { return m_setDown; }
        ITF_INLINE const StringID& getSetUp() const { return m_setUp; }

    private:

        StringID    m_setDown;
        StringID    m_setUp;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_SpikyPlatformAIComponent_Template* Ray_SpikyPlatformAIComponent::getTemplate() const
    {
        return static_cast<const Ray_SpikyPlatformAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_