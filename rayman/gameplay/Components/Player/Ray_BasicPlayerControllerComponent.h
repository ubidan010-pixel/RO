#ifndef _ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_
#define _ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif




namespace ITF
{
    class LumDrop;

    class Ray_BasicPlayerControllerComponent : public PlayerControllerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BasicPlayerControllerComponent, PlayerControllerComponent,4097809989)

    public:

        Ray_BasicPlayerControllerComponent();
        virtual ~Ray_BasicPlayerControllerComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType /*_hotReload */);

    protected:

        void                        initializeDeathDropLum();
        void                        destroyDeathDropLum();
        void                        startDeathDropLum()                 { m_deathLumDropTimer = 0.0f; }
        void                        updateDeathDropLum( f32 _dt );
        void                        deathSpawnLum( const LumDrop * _drop );
        
        SpawneeGenerator            m_deathLumSpawner;
        f32                         m_deathLumDropTimer;
        f32                         m_deathLumDropTotalDuration;
        SafeArray<LumDrop *>        m_deathLumDropList;

		f32							m_stateDeadSoulTimer;

    private :
        const class Ray_BasicPlayerControllerComponent_Template* getTemplate() const;

    };


    //**************************************************************************************************************************
    //---------------------------------------------------------------------------------------------------------------------------
    //                                              TAMPLATE DATA
    //---------------------------------------------------------------------------------------------------------------------------
    //***************************************************************************************************************************
    //--------------------------
    //      COMMON CLASSES
    //--------------------------
    class LumDrop_Template
    {
        DECLARE_SERIALIZE();
    public:
        LumDrop_Template() : m_duration(1.f), m_angle(bfalse, 0.f), m_distance(1.f) {}

        ITF_INLINE f32              getDuration()   const { return m_duration; }
        ITF_INLINE f32              getDistance()   const { return m_distance; }
        ITF_INLINE const Angle &    getAngle()      const { return m_angle; }
    private:
        f32     m_duration;
        Angle   m_angle;
        f32     m_distance;        
    };
    
    //------------------------------------
    class LumDrop
    {
        DECLARE_SERIALIZE();
    public:
        LumDrop() : m_time(0.f) , m_template(NULL){}
        LumDrop( const LumDrop_Template * _template, f32 _time )
            : m_time(_time)
            , m_template(_template) 
        {
            if ( _template != NULL )
            {
                m_direction = Vec2d::Right.Rotate( _template->getAngle().ToRadians() );
            }
        }
        ITF_INLINE const Vec2d&                 getDirection()                                      const   { return m_direction; }
        ITF_INLINE f32                          getTime()                                           const   { return m_time; }
        ITF_INLINE const LumDrop_Template *     getTemplate( )                                      const   { return m_template; }

    private:
        const LumDrop_Template *    m_template;
        f32                         m_time;
        Vec2d                       m_direction;
    };
    typedef SafeArray<LumDrop_Template>    LumDropTemplateList;


    //---------------------------------------------------------------------------------------------------------------------------
    //                                              TEMPLATE
    //---------------------------------------------------------------------------------------------------------------------------
    class Ray_BasicPlayerControllerComponent_Template : public PlayerControllerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BasicPlayerControllerComponent_Template, PlayerControllerComponent_Template,3679706346);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BasicPlayerControllerComponent);

    public:

        Ray_BasicPlayerControllerComponent_Template();
        virtual ~Ray_BasicPlayerControllerComponent_Template();

        ITF_INLINE const LumDropTemplateList &      getDeathLumDropTemplateList()   const { return m_deathLumDropTemplateList; }
        ITF_INLINE const Path&                      getDeathLumPath()               const { return m_deathLumPath; }

    private :

        LumDropTemplateList     m_deathLumDropTemplateList;
        Path                    m_deathLumPath;
    };

    ITF_INLINE const Ray_BasicPlayerControllerComponent_Template* Ray_BasicPlayerControllerComponent::getTemplate() const
    {
        return static_cast<const Ray_BasicPlayerControllerComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_