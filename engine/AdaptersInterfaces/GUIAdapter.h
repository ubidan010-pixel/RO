#ifndef _ITF_GUIADAPTER_H_
#define _ITF_GUIADAPTER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_


namespace ITF
{
    class GUIAdapter: public ITF::Interface_InputListener
    {
    public:
        virtual ~GUIAdapter() {};
    };


} // namespace ITF



#endif //_ITF_TCPADAPTER_H_
