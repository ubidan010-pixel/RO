#ifndef _ITF_CEGUI_ADAPTER_H_
#define _ITF_CEGUI_ADAPTER_H_


#ifndef _IFF_CEGUI_HEADERS_H_
#include "CEGUIITFHeaders.h"
#endif //_IFF_CEGUI_HEADERS_H_

#include "engine/AdaptersInterfaces/GUIAdapter.h"


namespace ITF
{
    class CEGUIAdapter :public GUIAdapter
{
public:
    bbool                onMouseButton   (   ITF::InputAdapter::MouseButton _but, ITF::InputAdapter::PressStatus _status );
    bbool                onMousePos        (   ITF::i32 _x, ITF::i32 _y );
    bbool               onMouseWheel    (   ITF::i32 _wheelValue, ITF::i32 _wheelDelta );
    bbool                onKey           (   ITF::i32 _key, ITF::InputAdapter::PressStatus _status );
};

}//namespace ITF

#endif //_ITF_CEGUI_ADAPTER_H_