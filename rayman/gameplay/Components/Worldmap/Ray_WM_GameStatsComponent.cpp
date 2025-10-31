#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMGAMESTATSCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_GameStatsComponent.h"
#endif //_ITF_RAYWMGAMESTATSCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_GameStatsComponent_Template)

    BEGIN_SERIALIZATION(Ray_WM_GameStatsComponent_Template)
        SERIALIZE_MEMBER("scoreTextActor",m_scoreTextActor);
        SERIALIZE_MEMBER("fontHeight",m_initialFontHeight);
        SERIALIZE_MEMBER("textColor",m_textColor);
        SERIALIZE_CONTAINER("scoreOffsets2D",m_scoreOffsets2D);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_GameStatsComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_GameStatsComponent)
    END_SERIALIZATION()

    Ray_WM_GameStatsComponent::Ray_WM_GameStatsComponent()
    {
    }

    Ray_WM_GameStatsComponent::~Ray_WM_GameStatsComponent()
    {
        for ( u32 i = 0 ; i < m_textFields.size() ; ++i )
        {
            if ( m_textFields[i].m_textActor.isValid() )
            {
                Actor * textBoxAct = static_cast<Actor *>(m_textFields[i].m_textActor.getActor());
                if ( textBoxAct )
                    textBoxAct->requestDestruction();
            }
        }
        m_textFields.clear();
    }


    void Ray_WM_GameStatsComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);

        // Get actor position to set text fields where there are supposed to be relatively to owner
        const Vec3d position3d = m_actor->getPos();

        const Path& scoreTextPath = getTemplate()->getScoreTextActorPath();
        ITF_WARNING_CATEGORY(GPP,NULL,!scoreTextPath.isEmpty(),"A default text actor must be specified to work properly");
        if ( !scoreTextPath.isEmpty() )
        {
            SpawneeGenerator scoreTextSpawnee;
            SPAWNER->declareNeedsSpawnee(m_actor, &(scoreTextSpawnee), scoreTextPath);

            const ITF_VECTOR<Vec2d>& offsets2D = getTemplate()->getOffsets2D();

            for ( u32 i = 0 ; i < SCORE_TYPE_COUNT ; i++ )
            {
                Actor * scoreTextAct = scoreTextSpawnee.getSpawnee( m_actor->getScene(),position3d + (i < offsets2D.size() ? offsets2D[i].to3d() : Vec3d::Zero));
                ITF_WARNING_CATEGORY(GPP,NULL, scoreTextAct != NULL, "Unable to load default text actor for game stats !" );

                if ( scoreTextAct != NULL )
                {

                    UITextBox* uiTextBox = scoreTextAct->GetComponent<UITextBox>();
                    ITF_WARNING_CATEGORY(GPP,NULL, uiTextBox != NULL, "Unable get UITextBox component on game stats actor !" );

                    if ( uiTextBox != NULL )
                    {
                        TextField newTextField;
                        newTextField.m_textActor    = scoreTextAct->getRef();
                        newTextField.m_offset2D     = i < offsets2D.size() ? offsets2D[i] : Vec2d::Zero;

                        TextInfo textInfo;
                        textInfo.m_color = getTemplate()->getTextColor().getAsU32();
                        textInfo.m_size = getTemplate()->getInitialFontHeight();

                        // Score
                        String8 valueString;
                        switch (i)
                        {
                        case SCORE_TYPE_CAGE:
                            valueString.setTextFormat("%d",RAY_GAMEMANAGER->getTotalBrokenCageCount());
                        	break;
                        case SCORE_TYPE_LUMS:
                            //valueString.setTextFormat("%d",RAY_GAMEMANAGER->getTotalSkullCoinCount());
                            break;
                        case SCORE_TYPE_STONE:
                            valueString.setTextFormat("%d",RAY_GAMEMANAGER->getTotalTakenToothCount());
                            break;
                        default:
                            ITF_ASSERT(0);
                            break;
                        }
                        textInfo.m_text.setText(valueString);
                        newTextField.m_textInfo = textInfo;

                        m_textFields.push_back(newTextField);
                    }
                }
            }
        }
    }

    void Ray_WM_GameStatsComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);
    }

    void Ray_WM_GameStatsComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        //Vec2d startUV = Vec2d::Zero;
        //Vec2d endUV = Vec2d::One;

        // Update 2D position
        //const Vec3d position3d = m_actor->getPos();
        //Vec3d position2D;
        //GFX_ADAPTER->compute3DTo2D(position3d, position2D);

        //u32 size = m_textActors.size();
        //for ( u32 i = 0 ; i < size ; ++i )
        //{
        //    if ( m_textActors[i].isValid() )
        //    {
        //        Actor * textBoxAct = static_cast<Actor *>(m_textActors[i].getActor());
        //        UITextBox* uiTextBox = textBoxAct->GetComponent<UITextBox>();
        //        ITF_WARNING_CATEGORY(GPP,NULL, uiTextBox != NULL, "Unable get UITextBox component on level name actor !" );
        //        if ( uiTextBox != NULL )
        //        {
        //            uiTextBox->setData(position2D, getTemplate()->getInitialFontHeight(), m_textsTabs[i], 
        //                0.0f, 0.0f, 0, startUV, endUV);
        //        }

        //    }
        //}
    }

    void Ray_WM_GameStatsComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
    }

    void Ray_WM_GameStatsComponent::Draw2D()
    {
        Super::Draw2D();

        Vec2d startUV = Vec2d::Zero;
        Vec2d endUV = Vec2d::One;

        const ITF_VECTOR<Vec2d>& offsets2D = getTemplate()->getOffsets2D();

        u32 size = m_textFields.size();
        for ( u32 i = 0 ; i < size ; ++i )
        {
            if ( m_textFields[i].m_textActor.isValid() )
            {
                Actor * textBoxAct = m_textFields[i].m_textActor.getActor();
                if ( textBoxAct != NULL )
                {
                    // Set pos
                    textBoxAct->setPos(m_actor->getPos() + (i < offsets2D.size() ? offsets2D[i].to3d() : Vec3d::Zero));

                    UITextBox* uiTextBox = textBoxAct->GetComponent<UITextBox>();
                    ITF_WARNING_CATEGORY(GPP,NULL, uiTextBox != NULL, "Unable get UITextBox component !" );
                    if ( uiTextBox != NULL )
                    {
                        //uiTextBox->setData(position2D, getTemplate()->getInitialFontHeight(), m_textFields[i].m_textTab, 
                        //    0.0f, 0.0f, 1, startUV, endUV);
                        uiTextBox->setTextAt(m_textFields[i].m_textInfo,0);
                    }
                }
            }
        }
    }
};
