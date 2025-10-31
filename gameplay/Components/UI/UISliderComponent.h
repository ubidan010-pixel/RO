#ifndef _ITF_UISLIDERCOMPONENT_H_
#define _ITF_UISLIDERCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class UISliderComponent : public UIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(UISliderComponent,UIComponent,236227314);
    public:
        DECLARE_SERIALIZE()

        UISliderComponent();
        ~UISliderComponent();

        virtual     bbool           needsUpdate         (       ) const             { return btrue; }
        virtual     bbool           needsDraw           (       ) const             { return bfalse; }
        virtual     bbool           needsDraw2D         (       ) const             { return bfalse; }
        virtual     void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual     void            Update              ( f32 _deltaTime );


        virtual     void            onActorClearComponents();
        // Returns a value between 0.0f and 1.0f. If -1.0f is returned, the Slider is not correct.
        f32             getValue() const { return m_value; }
        void            setValue(f32 val, bbool updatePosition = btrue);
        bbool           isValueUpdated() const { return m_valueUpdated; }
		bbool			isSliding() const { return m_isSliding; }

        virtual     void            onPressed();
        virtual     void            onReleased();
        virtual     void            onRollover();
        virtual     void            onRollout();
        virtual     void            onAction(const StringID & action);

    private:
        ITF_INLINE const class UISliderComponent_Template * getTemplate() const;
        void                        clear();
        
    protected:
        class LinkComponent * m_linkComponent;
        UISliderComponent*    m_child1;
        UISliderComponent*    m_child2;
        class TextureGraphicComponent2D* m_graphComponent;
        bbool           m_isSliding;
        Vec2d           m_originalScale;
        f32             m_value;
        bbool           m_valueUpdated;
    };

    class UISliderComponent_Template : public UIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(UISliderComponent_Template,UIComponent_Template,3650720017);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(UISliderComponent);
    public:
        UISliderComponent_Template();
        ITF_INLINE bbool    getIsCursor() const { return m_isCursor; }
        ITF_INLINE f32      getScaleOnSelected() const { return m_scaleOnSelected; }
        ITF_INLINE Color	getColorOnSelected() const { return m_colorOnSelected; }
        ITF_INLINE f32      getSpeedCursor() const {return m_speedCursor; }
    private:
        
        bbool       m_isCursor;
        f32         m_scaleOnSelected;
        Color		m_colorOnSelected;
        f32         m_speedCursor;
    };

    ITF_INLINE const UISliderComponent_Template* UISliderComponent::getTemplate() const
    {
        return static_cast<const UISliderComponent_Template*>(m_template);
    }

}
#endif // _ITF_UISLIDERCOMPONENT_H_