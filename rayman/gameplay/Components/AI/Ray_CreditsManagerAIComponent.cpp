#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CREDITSMANAGERAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_CreditsManagerAIComponent.h"
#endif //_ITF_RAY_CREDITSMANAGERAICOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#define CREDITS_OFFSET_LINE       -4
#define CREDITS_TIME_FADE_LOGO    0.25f
#define CREDITS_TIME_DYSPLAY_LOGO 5.0f
#define CREDITS_TIME_DYSPLAY_LUMS 25.0f


    namespace ITF
    {
    
    IMPLEMENT_OBJECT_RTTI(Ray_CreditsManagerAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_CreditsManagerAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_CreditsManagerAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "This component requires an LinkComponent.");
    END_VALIDATE_COMPONENT()

    #define fxFall ITF_GET_STRINGID_CRC(Tetris_IceFX_Fall,81788461)
    #define fxHit ITF_GET_STRINGID_CRC(Tetris_IceFX_Hit,3103459808)
    #define fxSteam ITF_GET_STRINGID_CRC(Tetris_IceFX_Steam,414946245)
    #define soundFall ITF_GET_STRINGID_CRC(Tetris_IceSnd_Fall,2564637324)
    #define soundHit ITF_GET_STRINGID_CRC(Tetris_IceSnd_Hit,3104294098)
    #define soundDestroy ITF_GET_STRINGID_CRC(Tetris_IceSnd_Destroy,2152943846)

    // Link tag
    #define nameGridTag ITF_GET_STRINGID_CRC(Grid,4267112909)
    #define nameChampiBumperTag ITF_GET_STRINGID_CRC(ChampiBumper,2965586883)
	#define nameLogoTag ITF_GET_STRINGID_CRC(Logo,1691847678)
	#define nameTheEndTag ITF_GET_STRINGID_CRC(TheEnd,1374271802)


    //*****************************************************************************

    Ray_CreditsManagerAIComponent::Ray_CreditsManagerAIComponent()
    : m_linkComponent(NULL)  
    , m_time(0.0f)
    , m_indexLine(0)
    , m_isInit(bfalse)
    , m_logoIsDisplay(bfalse)
	, m_endIsRequest(bfalse)
	, m_resetMapRequest(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_CreditsManagerAIComponent::~Ray_CreditsManagerAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get components
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT_MSG(m_linkComponent, "The door MUST have an LinkComponent.");

        // Event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

		// Load GMat		
		if ( getTemplate()->getGmatPath().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
		{
			World::addGameMaterialFromFile(getTemplate()->getGmatPath(), m_actor->getResourceGroup());
		}

    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::init()
    {

        StringID valueTag; 

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
            for (u32 i = 0; i < children.size(); i++)
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

                if ( pickable )
                {
                    Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if (childActor)
                    {
                        if (children[i].getTagValue(nameGridTag, valueTag) && !m_gridCreditRef.isValid())
                        {
                            m_gridCreditRef = childActor->getRef();                            
                        }    
                        else if (children[i].getTagValue(nameChampiBumperTag, valueTag) && !m_champiBumperRef.isValid())
                        {
                            m_champiBumperRef = childActor->getRef();                            
                        }
                        else if (children[i].getTagValue(nameLogoTag, valueTag) && !m_logoRef.isValid())
                        {
                            m_logoRef = childActor->getRef();                            
                        }    
						else if (children[i].getTagValue(nameTheEndTag, valueTag) && !m_theEndRef.isValid())
						{
							m_theEndRef = childActor->getRef();                            
						}   
                    }
                }
            }
        }

        ITF_ASSERT(m_gridCreditRef.isValid());
        ITF_ASSERT(m_champiBumperRef.isValid());
        ITF_ASSERT(m_logoRef.isValid());

        // Init Champi
        if (m_champiBumperRef.isValid())
        {
            if (Actor* champiAct = m_champiBumperRef.getActor())
            {
                if (RAY_GAMEMANAGER->isAllGameLevelsCompleted())
                {
					champiAct->enable();
                }
                else
                {
					champiAct->disable();
                }
            }
        }

        // Init Logo
        if (m_logoRef.isValid())
        {
            if (Actor* logoAct = m_logoRef.getActor())
            {
                logoAct->disable();
                EventShow alphaChange( 0.0f );
                logoAct->onEvent(&alphaChange);
            }
        }

		// Init Grid
		if (m_gridCreditRef.isValid())
		{
			if (Actor* gridAct = m_gridCreditRef.getActor())
			{
				if (Ray_BreakableStackManagerAIComponent* gridCompo = gridAct->GetComponent<Ray_BreakableStackManagerAIComponent>())
				{
					gridCompo->setCreditsMap(btrue);
				}
			}
		}

        m_isInit = btrue;
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if (EventGeneric* evtGeneric = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)))
        {
            if (evtGeneric->getId() == ITF_GET_STRINGID_CRC(breakableNewLine,1910942963))
            {
               newLine();
            }
        }
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (!m_isInit)
        {
            init();
        }

        // End of credits
		if (!m_endIsRequest)
		{
			if (!m_logoIsDisplay)
			{
				checkEndCredits();
			}
			else
			{
				// WII
				m_time -= _dt;
				if (m_time <= 0.0f)
				{
					onEndCredits();
				}
			}
		}

		if (m_endIsRequest && !m_resetMapRequest)
		{
			m_time -= _dt;
			if (m_time <= 0.0f)
			{
				m_resetMapRequest = btrue;

				RAY_GAMEMANAGER->resetToWorldMap();
			}
		}
      
    }


    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::checkEndCredits()
    {
        if (Actor* gridAct = m_gridCreditRef.getActor())
        {
            if (Ray_BreakableStackManagerAIComponent* breakManagerCompo = gridAct->GetComponent<Ray_BreakableStackManagerAIComponent>())
            {
                u32 gridHeight = breakManagerCompo->getHeight();
                const CreditsList& credits = getTemplate()->getCreditsList(); 
                if (m_indexLine > credits.size() + gridHeight + CREDITS_OFFSET_LINE)
                {
                    #ifdef ITF_WII
                        m_logoIsDisplay = btrue;
                        m_time = CREDITS_TIME_DYSPLAY_LOGO;
                        displayLogo(btrue);
                    #else
                        onEndCredits();
                    #endif
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::displayLogo( bbool _value )
    {
        if (m_logoRef.isValid())
        {
            if (Actor* logoAct = m_logoRef.getActor())
            {
                f32 alphaValue = 0.0f;

                if (_value)
                {
                    logoAct->enable();
                    alphaValue = 1.0f;
                }

                EventShow alphaChange( alphaValue, CREDITS_TIME_FADE_LOGO);
                logoAct->onEvent(&alphaChange);
            }
        }
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::onEndCredits()
    {
		if (!m_endIsRequest)
		{
			m_endIsRequest = btrue;
			m_time = CREDITS_TIME_DYSPLAY_LUMS;

			if (m_logoIsDisplay)
			{
				displayLogo( bfalse );
			}

			if (m_theEndRef.isValid())
			{
				if (Actor* theEndAct = m_theEndRef.getActor())
				{
					EventTrigger eventTrig;
					eventTrig.setSender( m_actor->getRef() );
					eventTrig.setActivated(btrue);
					eventTrig.setActivator(m_actor->getRef());
					theEndAct->onEvent(&eventTrig);
				}
			}
		}
    }

    //*****************************************************************************


    void Ray_CreditsManagerAIComponent::newLine( )
    {
        const CreditsList& credits = getTemplate()->getCreditsList(); 

        if (m_indexLine < credits.size())
        {
            CreditsLine credLine = credits[m_indexLine];
            u32 typeLine = credLine.m_type;

            if (Actor* gridAct = m_gridCreditRef.getActor())
            {
                if (Ray_BreakableStackManagerAIComponent* breakManagerCompo = gridAct->GetComponent<Ray_BreakableStackManagerAIComponent>())
                {
                    u32 gridWidth = breakManagerCompo->getWidth();

                    // Get Infos
                    if ( typeLine == 0 || typeLine == 1 )
                    {
                        String curText = credLine.m_text;

                        ITF_WARNING_CATEGORY(LD, m_actor, curText.getLen() <= gridWidth, "Credit line is bigger than grid width ! Check with Romain V.");

                        u32 col = (gridWidth - curText.getLen()) / 2;
                        u32 row = 1;

                        parseChar(curText, typeLine, row, col, breakManagerCompo);

                    }
                    else  if ( typeLine == 2 )
                    {
                        String firstName = credLine.m_firstName;
                        u32 colCenter = gridWidth / 2;
                        u32 col = colCenter - firstName.getLen();
                        u32 row = 1;

                        parseChar(firstName, typeLine, row, col, breakManagerCompo);

                        String lastName = credLine.m_lastName;
                        col = colCenter + 1;
                        parseChar(lastName, typeLine, row, col, breakManagerCompo);

                        ITF_WARNING_CATEGORY(LD, m_actor, firstName.getLen() +  lastName.getLen() + 1 <= gridWidth, "Credit line is bigger than grid width ! Check with Romain V.");

                    }
                    else
                    {
                        ITF_WARNING_CATEGORY(LD, m_actor, 0, "Bad line type ! Check with Romain V.");
                        return;
                    }
                }
            }
        }         
        m_indexLine++;
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::parseChar( String& _text, u32 _type, u32 _row, u32 _col, Ray_BreakableStackManagerAIComponent* _breakManagerCompo )
    {
        if (!_breakManagerCompo)
            return;

        u32 stringLen = _text.getLen();
        if(!stringLen)
            return;

        char* c = _text.getCharCopy();
        ITF_ASSERT(c);
        if (!c)
            return;

        for (u32 i = 0; i < stringLen; i++)
        {                           
            u32 asciiCode = *(c + i);
            u32 indexAtlas = 0;

            if (asciiCode == 32)
            {
                 // Space
                _col++;
                continue;
            }
            else if (asciiCode == 45)
            {
                // "-"
               indexAtlas = 30;
            }
            else if (asciiCode == 38)
            {
                // "&"
                indexAtlas = 31;
            }
            else if (asciiCode >= 64 && asciiCode <= 90)
            {
                indexAtlas =  asciiCode - 65;
            }
            else
            {
                // Erreur
                indexAtlas = 29;
            }

            if (_type == 0 || _type == 1)
            {
                indexAtlas += 32; // TODO expose;
            }

            // Rewards
            u32 reward = 0;
            if (
                asciiCode == 82/*R*/  || 
                asciiCode == 65/*A*/  || 
                asciiCode == 89/*Y*/  || 
                asciiCode == 77/*M*/  || 
                asciiCode == 78/*N*/ 
                )
            {
                reward = 1;
            }

            addBlock( indexAtlas, _type, _breakManagerCompo, _row, _col, reward );
            _col++;
        }

        SF_DEL_ARRAY(c);
    }

    //*****************************************************************************

    void Ray_CreditsManagerAIComponent::addBlock( u32 _index, u32 _type, Ray_BreakableStackManagerAIComponent* _breakManagerCompo, u32 _row, u32 _col, u32 _reward )
    {

        ITF_ASSERT(_breakManagerCompo);

        InfoHeadElement infohead;
        infohead.m_width = 1;
        infohead.m_height = 1;
        infohead.m_hitPoint = 1;
        infohead.m_blockStatic = btrue;
        infohead.m_gmat = getTemplate()->getGmatPath();

        FxData fxData;
        fxData.m_fxFall = fxFall;
        fxData.m_fxHit = fxHit;
        fxData.m_fxSteam = fxSteam;
        fxData.m_soundFall = soundFall;
        fxData.m_soundHit = soundHit;
        fxData.m_soundDestroy = soundDestroy;


        AnimationAtlas::Key sequenceKey;
        sequenceKey.m_atlas = _index;
        sequenceKey.m_count = 1;

        AnimationAtlas animAtlas;
        animAtlas.m_sequence.push_back(sequenceKey);

        AnimationAtlas animAtlasBreak;
        animAtlasBreak.m_sequence.push_back(sequenceKey);

        FragmentsList  fragments;
        if (_type == 0 || _type == 1)
        {
            fragments.m_fragments.push_back(58);
            fragments.m_fragments.push_back(59);
            fragments.m_fragments.push_back(60);
        }
        else
        {
            fragments.m_fragments.push_back(26);
            fragments.m_fragments.push_back(27);
            fragments.m_fragments.push_back(28);
        }

        InfoElement infoElement;
        infoElement.m_isOccupied = btrue;
        infoElement.m_reward = _reward;
        infoElement.m_anim = animAtlas;
        infoElement.m_animBreak = animAtlasBreak;
        infoElement.m_fragments = fragments;

        InfoElementList gridElementList;
        gridElementList.m_elements.push_back(infoElement);

        InfoElementGrid gridElement;
        gridElement.push_back(gridElementList);


        _breakManagerCompo->createCreditsBlocks(infohead, fxData, gridElement, _row, _col);

    }

    //*****************************************************************************


#ifdef ITF_SUPPORT_EDITOR

    void Ray_CreditsManagerAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

    }

#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_CreditsManagerAIComponent_Template)

    BEGIN_SERIALIZATION(CreditsLine)
        SERIALIZE_MEMBER("type",    m_type);
        SERIALIZE_MEMBER("flag",    m_flag);
        SERIALIZE_MEMBER("text",    m_text);
        SERIALIZE_MEMBER("firstName",    m_firstName);
        SERIALIZE_MEMBER("lastName",     m_lastName);
    END_SERIALIZATION()


    BEGIN_SERIALIZATION_CHILD(Ray_CreditsManagerAIComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("creditsList", m_creditsList);
        SERIALIZE_MEMBER("gmatPath", m_gmatPath);
    END_SERIALIZATION()


    //*****************************************************************************

    Ray_CreditsManagerAIComponent_Template::Ray_CreditsManagerAIComponent_Template()   
    {
        // none
    }

    //*****************************************************************************

    Ray_CreditsManagerAIComponent_Template::~Ray_CreditsManagerAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}
