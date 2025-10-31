#include "precompiled_gameplay.h"


#ifndef _ITF_DATAERRORCOMPONENT_H_
#include "gameplay/Components/Misc/DataErrorComponent.h"
#endif //_ITF_DATAERRORCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(DataErrorComponent)

DataErrorComponent::DataErrorComponent()
: Super()
/*, m_drawBoxWidth(200.f)
, m_drawBoxHeight(100.f)
, m_textBox(NULL)
, m_textAlpha(1.0f)
, m_textRed(0.0f)
, m_textGreen(0.0f)
, m_textBlue(0.0f)
, m_textSize(17.0f)
, m_backgroundAlpha(1.0f)
, m_backgroundRed(1.0f)
, m_backgroundGreen(1.0f)
, m_backgroundBlue(1.0f)*/
{
}


DataErrorComponent::~DataErrorComponent()
{
}

void DataErrorComponent::Draw()
{
 /*   switch(CAMERA->getMode())
    {
    case Camera::Camera_Editor:
        break;
    case Camera::Camera_InGame:                 
    case Camera::Camera_Slave:
        return;
    }
    f32 diam = 1.f + 0.5f*cos((f32)CURRENTFRAME/20.f);
    GFX_ADAPTER->drawDBGCircle(m_actor->getPos().m_x, m_actor->getPos().m_y, diam*0.5f, diam-0.6f, 0, 0, 0.3f, m_actor->getPos().m_z);*/
    /*
    if (!m_textBox)
    {
         m_textBox =  m_actor->GetComponent<UITextBox>();
         if (!m_textBox)
         {
             m_actor->AddComponent(newAlloc(mId_Gameplay, UITextBox()));
             m_textBox =  m_actor->GetComponent<UITextBox>();
             if (m_textBox)
             {
                m_textBox->setFontName("Misc/Fonts/font02.fnt");
                m_textBox->onActorLoaded();
             }

         }
    }
    else
    {
        TextInfo textInfo;
        textInfo.m_color = Color(m_textAlpha, m_textRed, m_textGreen, m_textBlue).getAsU32();
        textInfo.m_size = m_textSize;
        textInfo.m_text.setText(m_TextLabel.cStr());

        Vec3d pos;
        GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), pos);

        m_textBox->setData(pos,m_textSize,textInfo,m_drawBoxWidth,m_drawBoxHeight,UI_TEXTMANAGER->getTextsDepthRank(),Vec2d::Zero,Vec2d::One,Color(m_backgroundAlpha, m_backgroundRed, m_backgroundGreen, m_backgroundBlue).getAsU32(), btrue);      
        m_textBox->Draw();
    }*/
}

}

