#ifndef _ITF_RAY_ELECTOONTEETHCOMPONENT_H_
#define _ITF_RAY_ELECTOONTEETHCOMPONENT_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{
    class Ray_Player;
    class UITextBox;
    class SoundComponent;

    class Ray_ElectoonTeethComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElectoonTeethComponent, ActorComponent,498150873)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ElectoonTeethComponent();
        virtual ~Ray_ElectoonTeethComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual bbool               needsUpdate()  const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return btrue; }

        virtual void                Update( f32 _dt );
        virtual void                Draw2D();
        virtual void                onEvent( Event* _event );

        static Ray_ElectoonTeethComponent* getInstance() {return s_instance;}
    private:
        static const u8 icon_Electoon = 0;
        static const u8 icon_Tooth = 1;
        struct MovingIcon
        {
            u8 m_icon;
            Vec3d m_startPosProj, m_endPosProj, m_centerPosProj;
            f32 m_time;
            f32 m_sizeMultiplier;
            Vec3d m_projPos;
            f32 m_currentSize;
            f32 m_flightDuration;
            bbool m_isInVisiblePhase;
            bbool m_justAppeared;

            bbool update(f32 _dt);
            void draw(AtlasDynamicObject &_atlas);
            static Vec2d computeExageratedCenterPoint(const Vec3d &_projectedStart, const Vec3d &_projectedEnd, f32 _exageration);
            static ITF_INLINE f32 computeInterpolator(f32 _x)
            {
                f32 x2 = (_x-0.5f)*2;
                return f32_Clamp((x2*x2*x2+1)*0.5f,0,1);
            }

            MovingIcon() {}
            MovingIcon(u8 _icon, const Vec3d &_startPos, const Vec2d &_destination2D, f32 _normalSize, f32 _nearZ, f32 _centerExageration, f32 _timeOffset, f32 _flightDuration);
        };

        void updatePulse(f32 _t, f32 &_scale);
        void updateCount(f32 _dt);
        void displayScore(ITF_VECTOR<TextInfo> &_textTab, const Vec2d & _scorePos, f32 _height, u32 _count);
        bbool reachScoreValue(f32 _dt, f32 &_value, f32 _goal);
        void computeScoreItemAABB(AABB &_electoonAABB, AABB &_toothAABB);
        void addMissingIcons(const Vec3d &_startPos);
        void addNewIcon(u8 _icon, const Vec3d &_startPos3d, f32 _timeOffset);

        f32 m_alpha;
        f32 m_lastElectoonCount;
        f32 m_lastToothCount;
        u32 m_nextToothCount, m_nextElectoonCount;
        u32 m_scoreFontIndex;
        f32 m_stateChangeTimer_Tooth;
        f32 m_stateChangeTimer_Electoon;
        f32 m_electoonScale, m_toothScale;
        SoundComponent *m_soundComponent;
        
        SafeArray<MovingIcon> m_movingIcons;
        bbool m_isActive;

        ITF_VECTOR<TextInfo> m_electoonTextTab;
        ITF_VECTOR<TextInfo> m_toothTextTab;

        AtlasDynamicObject m_atlas;

        const class Ray_ElectoonTeethComponent_Template* getTemplate() const;
        static Ray_ElectoonTeethComponent *s_instance;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ElectoonTeethComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElectoonTeethComponent_Template,TemplateActorComponent,1750584553);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ElectoonTeethComponent);

    public:

        Ray_ElectoonTeethComponent_Template();
        ~Ray_ElectoonTeethComponent_Template() {}

        f32 getXFromRight() const {return m_xFromRight;}
        f32 getYFromTop() const {return m_yFromTop;}
        f32 getItemWidth() const {return m_itemWidth;}
        f32 getItemHeight() const {return m_itemHeight;}
        f32 getSpacing() const {return m_spacing;}
        const Path& getFontName() const {return m_fontName;}
        const Path& getAtlasPath() const {return m_atlasPath;}
        const Vec2d& getElectoonScorePos() const {return m_electoonScorePos;}
        const Vec2d& getToothScorePos() const {return m_toothScorePos;}
        f32 getElectoonScoreHeight() const {return m_electoonScoreHeight;}
        f32 getToothScoreHeight() const {return m_toothScoreHeight;}
        const Color &getScoreColor() const {return m_scoreColor;}
        f32 getPulseDuration() const {return m_pulseDuration;}
        f32 getPulseAmplitude() const {return m_pulseAmplitude;}
        f32 getFadeSpeed() const {return m_fadeSpeed;}
        f32 getIncrementSpeed() const {return m_incrementSpeed;}
        f32 getMovementCurveDistance() const {return m_movementCurveDistance;}
        f32 getRelativeNormalSize() const {return m_relativeNormalSize;}
        f32 getNearestRelativeZ() const {return m_nearestRelativeZ;}
        f32 getInterIconDelay() const {return m_interIconDelay;}
        f32 getFlightDuration() const {return m_flightDuration;}

    private:
        f32 m_xFromRight;
        f32 m_yFromTop;
        f32 m_itemWidth;
        f32 m_itemHeight;
        f32 m_spacing;
        Path m_fontName;
        Path m_atlasPath;
        Vec2d m_electoonScorePos;
        f32 m_electoonScoreHeight;
        Vec2d m_toothScorePos;
        f32 m_toothScoreHeight;
        Color m_scoreColor;
        f32 m_pulseDuration;
        f32 m_pulseAmplitude;
        f32 m_fadeSpeed;
        f32 m_incrementSpeed;
        f32 m_movementCurveDistance;
        f32 m_relativeNormalSize;
        f32 m_nearestRelativeZ;
        f32 m_interIconDelay;
        f32 m_flightDuration;

    };

    ITF_INLINE const Ray_ElectoonTeethComponent_Template* Ray_ElectoonTeethComponent::getTemplate() const
    {
        return static_cast<const Ray_ElectoonTeethComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_ELECTOONTEETHCOMPONENT_H_