#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _CEGUI_h_
#include "CEGUI.H"
#endif //_CEGUI_h_

#ifndef _CEGUIITFUTILS_H_
#include "CEGUIITFUtils.h"
#endif // _CEGUIITFUTILS_H_


//----------------------------------------------------------------------------//
namespace ITF
{
  

ITF::String strCEGUI_ITF(const CEGUI::String& str_cegui)
{   
    int len = str_cegui.length();

    char* buff = new char[len+1];
    strncpy(buff,str_cegui.c_str(),len);
    buff[len ] = 0;

    ITF::String str_itf(buff);
    SF_DEL_ARRAY(buff);
    return str_itf;
};


//----------------------------------------------------------------------------//

CEGUI::String strITF_CEGUI(const ITF::String& str_itf)
{   
    ITF::StringConverter str(str_itf);
    int len = str.getUTF8Length();

    char* buff = new char[len+1];
    strncpy(buff,str.getUTF8(),len);
    buff[len ] = 0;

    CEGUI::String str_cegui(buff);
    SF_DEL_ARRAY(buff);
    return str_cegui;
};

}//namespace ITF