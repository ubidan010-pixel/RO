#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_CEGUI_ADAPTER_H_
#include "CEGUIITFAdapter.h"
#endif //_ITF_CEGUI_ADAPTER_H_


namespace ITF
{
    CEGUI::MouseButton ITFButtonToCEGUI(ITF::InputAdapter::MouseButton  _but)
    {
        CEGUI::MouseButton result;
        switch (_but)
        {
            case ITF::InputAdapter::MB_Left:
                result = CEGUI::LeftButton;
                break;
            case ITF::InputAdapter::MB_Right:
                result = CEGUI::RightButton;
                break;
            case ITF::InputAdapter::MB_Middle:
                result = CEGUI::MiddleButton;
                break;
            default:
                ITF_ASSERT(0);
                break;
        }
    
        return result;
    }


    bbool  CEGUIAdapter::onMouseButton   (   ITF::InputAdapter::MouseButton _but, ITF::InputAdapter::PressStatus _status )
    {
        if (_status == ITF::InputAdapter::Pressed)
            CEGUI::System::getSingleton().injectMouseButtonDown(ITFButtonToCEGUI(_but));
        else
            CEGUI::System::getSingleton().injectMouseButtonUp(ITFButtonToCEGUI(_but));
        return ITF::btrue;
    }

    bbool  CEGUIAdapter::onMousePos        (   ITF::i32 _x, ITF::i32 _y )
    {
        CEGUI::System::getSingleton().injectMousePosition((float) _x, (float)_y);
        return ITF::btrue;
    }

    bbool  CEGUIAdapter::onMouseWheel    (   ITF::i32 _wheelValue, ITF::i32 _wheelDelta )
    {
        return ITF::btrue;
    }

    bbool    CEGUIAdapter::onKey           (   i32 _key, ITF::InputAdapter::PressStatus _status )
    {

        if (_status == ITF::InputAdapter::Released)
            return btrue;

        // extract some keys may be handled via key code and generate those too
        switch (_key)
        {
        case 0x127:  // backspace
            CEGUI::System::getSingleton().injectKeyDown(CEGUI::Key::Backspace);
            break;
        case 0x129:  // delete
            CEGUI::System::getSingleton().injectKeyDown(CEGUI::Key::Delete);
            break;

        case 0x126:  // CR (Return)
            CEGUI::System::getSingleton().injectKeyDown(CEGUI::Key::Return);
            break;
        default:
            // inject Character code
            if (_key<0x120)
                CEGUI::System::getSingleton().injectChar(static_cast<CEGUI::utf32>(_key));
            break;
        }
        return btrue;
    }


} //namespace ITF