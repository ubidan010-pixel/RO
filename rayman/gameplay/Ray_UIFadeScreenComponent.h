#ifndef _ITF_RAY_UIFADESCREENCOMPONENT_H_
#define _ITF_RAY_UIFADESCREENCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{
    class UIFadeEntry
    {
        DECLARE_SERIALIZE()

    public:

        const StringID&     getId() const { return m_id; }
        const f32           getDuration() const { return m_duration; }
        const Color&        getColor() const { return m_color; }
        const StringID&     getType() const { return m_animType; }

    private:

        StringID    m_id;
        f32         m_duration;
        Color       m_color;
        StringID    m_animType;
    };

    typedef SafeArray <UIFadeEntry> FadeEntryList;

    class Ray_UIFadeScreenComponent : public UIComponent, IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_UIFadeScreenComponent, UIComponent,1402917130)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_UIFadeScreenComponent();
        ~Ray_UIFadeScreenComponent();

        virtual     void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     bbool           needsUpdate         (       ) const             { return btrue; }
        virtual     bbool           needsDraw           (       ) const             { return bfalse; }
        virtual     void            Update              ( f32 _deltaTime );
        virtual     void            onResourceReady();
        virtual     void            updateAnimInput();

        void                        startFade( const StringID& _id, bbool _bforce = bfalse );
        void                        stopFade( bbool _force = bfalse );
        bbool                       isFadeOutFinished() const;
        bbool                       isFadeInFinished() const;

    private:

        ITF_INLINE const class Ray_UIFadeScreenComponent_Template*  getTemplate() const;

        u32                         getFadeEntryIndex( const StringID& _id ) const;
        void                        clearFade();

        class AnimatedComponent*    m_animComponent;
        f32                         m_currentFadeTime;
        u32                         m_currentFadeIndex;
        bbool                       m_fadeOut;

        u32                         m_animPosInputIndex;
        u32                         m_fadeOutInputIndex;
    };

    class Ray_UIFadeScreenComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_UIFadeScreenComponent_Template, UIComponent_Template,3100936897)
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_UIFadeScreenComponent);

    public:

        const FadeEntryList&        getTypesList() const { return m_types; }

    private:

        FadeEntryList               m_types;
    };

    const Ray_UIFadeScreenComponent_Template*  Ray_UIFadeScreenComponent::getTemplate() const {return static_cast<const Ray_UIFadeScreenComponent_Template*>(m_template);}
}
#endif // _ITF_UIMENUITEMTEXT_H_