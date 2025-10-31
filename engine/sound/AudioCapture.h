#ifndef _ITF_AUDIOCAPTURE_H_
#define _ITF_AUDIOCAPTURE_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_


namespace ITF
{
    class AudioCapture: public TemplateSingleton<AudioCapture>
    {
    public:
        virtual void start(String _Filename) {};
        virtual void stop()  {};
        virtual bbool isEnable() {return bfalse;}

    };
};

#endif //_ITF_AUDIOCAPTURE_H_