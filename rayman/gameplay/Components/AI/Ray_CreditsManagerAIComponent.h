#ifndef _ITF_RAY_CREDITSMANAGERAICOMPONENT_H_
#define _ITF_RAY_CREDITSMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackManagerAIComponent.h"
#endif //_ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_

namespace ITF
{

    class LinkComponent;

    //*****************************************************************************
    // CreditsLine
    //*****************************************************************************

    class CreditsLine
    {
        DECLARE_SERIALIZE();

    public:

        CreditsLine()
        : m_type(0)
        , m_flag(0)
        {}
        ~CreditsLine(){}

        u32     m_type;
        u32     m_flag; 
        String  m_text; 
        String  m_firstName; 
        String  m_lastName; 

    };

    typedef ITF_VECTOR<CreditsLine> CreditsList;

    //*****************************************************************************

    class Ray_CreditsManagerAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CreditsManagerAIComponent, Ray_AIComponent,2075960340)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_CreditsManagerAIComponent();
        virtual ~Ray_CreditsManagerAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return bfalse; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    onBecomeActive();

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);


#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_CreditsManagerAIComponent_Template*  getTemplate() const;

        void init();
        void addBlock( u32 _index, u32 _type, Ray_BreakableStackManagerAIComponent* _breakManagerCompo, u32 _row, u32 _col, u32 _reward );
        void newLine();
        void checkEndCredits();
        void onEndCredits();
      
        void parseChar( String& _text, u32 _type, u32 _row, u32 _col, Ray_BreakableStackManagerAIComponent* _breakManagerCompo );
        void displayLogo( bbool _value );

    private:
     
        LinkComponent*     m_linkComponent;

        f32    m_time;
        u32    m_indexLine;

        ActorRef m_gridCreditRef;
        ActorRef m_champiBumperRef;
		ActorRef m_logoRef;
		ActorRef m_theEndRef;

        bbool    m_isInit;
        bbool    m_logoIsDisplay;
		bbool	 m_endIsRequest;
		bbool	 m_resetMapRequest;
      

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_CreditsManagerAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CreditsManagerAIComponent_Template, Ray_AIComponent_Template,2491834364);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CreditsManagerAIComponent);

        ITF_INLINE const CreditsList&   getCreditsList() const    { return m_creditsList; }
        ITF_INLINE const Path&          getGmatPath() const       { return m_gmatPath; }

    public:

        Ray_CreditsManagerAIComponent_Template();
        ~Ray_CreditsManagerAIComponent_Template();

    private:
    
        CreditsList m_creditsList;
        Path        m_gmatPath;

    };

    const Ray_CreditsManagerAIComponent_Template*  Ray_CreditsManagerAIComponent::getTemplate() const {return static_cast<const Ray_CreditsManagerAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_CREDITSMANAGERAICOMPONENT_H_