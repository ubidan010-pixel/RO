#ifndef _ITF_RAYWMLEVELSPOTCOMPONENT_H_
#define _ITF_RAYWMLEVELSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

namespace ITF
{
    class Ray_WM_LevelSpotComponent : public Ray_WM_SpotComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LevelSpotComponent, Ray_WM_SpotComponent,4045004918)
        DECLARE_SERIALIZE()

    public:

        Ray_WM_LevelSpotComponent()
            : m_displayTimeAttack(bfalse)
            , m_displayLumAttack(bfalse)
			, m_displayRelicCount(bfalse)
        {};
        virtual ~Ray_WM_LevelSpotComponent() {};

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onSceneActive();
        virtual void        Update( f32 _dt );
        virtual void        Draw();

        virtual void        onAction();
        virtual void        onConnection();

        virtual void        onEnter(bbool _changeMusic = bfalse);
        virtual void        onExit();

    protected:

        bbool               canEnter();

        void                writeInfo();

        void                updateState();

    private:
        ITF_INLINE const class Ray_WM_LevelSpotComponent_Template* getTemplate() const;

        void                loadMap();

        bbool               m_displayTimeAttack;
        bbool               m_displayLumAttack;
		bbool               m_displayRelicCount;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_LevelSpotComponent_Template : public Ray_WM_SpotComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LevelSpotComponent_Template, Ray_WM_SpotComponent_Template,657886431);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_LevelSpotComponent);

    public:

        Ray_WM_LevelSpotComponent_Template()
            : m_textCageID(StringID::Invalid)
            , m_textCoinID(StringID::Invalid)
            , m_textLumID(StringID::Invalid)
            , m_textTimeID(StringID::Invalid)
			, m_textRelicID(StringID::Invalid)
        {}
        virtual ~Ray_WM_LevelSpotComponent_Template() {}

        ITF_INLINE const StringID&      getTextCageID() const { return m_textCageID; }
        ITF_INLINE const StringID&      getTextCoinID() const { return m_textCoinID; }
        ITF_INLINE const StringID&      getTextLumID() const { return m_textLumID; }
        ITF_INLINE const StringID&      getTextTimeID() const { return m_textTimeID; }
		ITF_INLINE const StringID&      getTextRelicID() const { return m_textRelicID; }


    private:

        StringID        m_textCageID;
        StringID        m_textCoinID;
        StringID        m_textLumID;
        StringID        m_textTimeID;
		StringID        m_textRelicID;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_LevelSpotComponent_Template* Ray_WM_LevelSpotComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_LevelSpotComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMLEVELSPOTCOMPONENT_H_