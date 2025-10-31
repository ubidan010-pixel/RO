#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TRCHELPER_H_
#include "rayman/gameplay/Ray_TRCHelper.h"
#endif //_ITF_RAY_TRCHELPER_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

namespace ITF
{
  
    void Ray_TRCHelper::buildPlayerField( const String8& _menuName, u32 _playerIndex, bbool _asEmpty )
    {
        UI_MENUMANAGER->changeMenuItemFriendlyByID(_menuName, ITF_GET_STRINGID_CRC(template_player,2755027330), "");
        if(_playerIndex == U32_INVALID && !_asEmpty)
            return;

        UIComponent* pUIComponent = UI_MENUMANAGER->getUIComponentFromID(_menuName, ITF_GET_STRINGID_CRC(template_player,2755027330));
        if(pUIComponent)
        {
            TextureGraphicComponent2D* pTexGraphicComponent2D = pUIComponent->GetActor()->GetComponent<TextureGraphicComponent2D>();
            if(pTexGraphicComponent2D)
            {
                if(_asEmpty)
                {
                    pTexGraphicComponent2D->changeTexture(ResourceID::Invalid);                    
                }
                else
                {
                    Ray_PlayerHudScoreComponent* playerHud = Ray_PlayerHudScoreComponent::getInstance(_playerIndex);
                    if(playerHud)
                    {
                        pTexGraphicComponent2D->changeTexture(playerHud->getCharacterAtlas()->getResourceID());
                    } 
                    else
                    {
                        String playerStr ; 
                        playerStr.setTextFormat("%d", (_playerIndex+1));
                        UI_MENUMANAGER->changeMenuItemFriendlyByID(_menuName, ITF_GET_STRINGID_CRC(template_player,2755027330), playerStr);
                    }
                }                
            }
        }
    }

    void Ray_TRCHelper::buildMessageField( const String8& _menuName, const String& _message, bbool _asEmpty )
    {        
        UI_MENUMANAGER->changeMenuItemFriendlyByID(_menuName, ITF_GET_STRINGID_CRC(template_message,2306377060), _asEmpty ? "" : _message);
    }

    void Ray_TRCHelper::buildTitleField( const String8& _menuName, const String& _title, bbool _asEmpty )
    {
        UI_MENUMANAGER->changeMenuItemFriendlyByID(_menuName, ITF_GET_STRINGID_CRC(template_title,440395648), _asEmpty ? "" : _title);
    }

    Ray_TRCHelper::Ray_TRCHelper(): TRCHelper()
    {

    }

};
