
#ifndef _ITF_STANDEECOMPONENT_H_
#define _ITF_STANDEECOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_
#include "texturegraphiccomponent.h"

namespace ITF
{
    struct StandeeMapping
    {
        DECLARE_SERIALIZE()
        StandeeMapping(){};
        bbool               operator==(const StringID& _mapName) const               { return StringID(m_MapName) == _mapName; }

        String              m_MapName;
        String              m_StandeePath;
    };

class StandeeComponent : public TextureGraphicComponent
{
    DECLARE_OBJECT_CHILD_RTTI(StandeeComponent,TextureGraphicComponent,4081679346);

public:
    DECLARE_SERIALIZE()

    StandeeComponent();
    ~StandeeComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return btrue; }
    virtual void  Draw();
    virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void  Update( f32 _deltaTime );
    void ChangeStandee(StringID& _levelName);
#ifdef ITF_SUPPORT_EDITOR
    virtual void  onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
    ITF_INLINE const class StandeeComponent_Template* getTemplate() const;
private :
        void ReloadStandee();

};

//-------------------------------------------------------------------------------------
class StandeeComponent_Template : public TextureGraphicComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(StandeeComponent_Template,TextureGraphicComponent_Template,170543165);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(StandeeComponent);
public:

    StandeeComponent_Template();
    ~StandeeComponent_Template() {}

    ITF_VECTOR<StandeeMapping>    m_standeeMapping;
};


const StandeeComponent_Template*  StandeeComponent::getTemplate() const {return static_cast<const StandeeComponent_Template*>(m_template);}

}

#endif // _ITF_STANDEECOMPONENT_H_

