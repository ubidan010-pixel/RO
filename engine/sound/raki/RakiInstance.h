#ifndef _ITF_RAKIINSTANCE_H_
#define _ITF_RAKIINSTANCE_H_



#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_







namespace ITF
{

    class RakiInstance
    {
    public:

        virtual bool start() = 0;

        virtual void stop() = 0;

        virtual bool isPlaying() = 0;
    };


}
#endif // _ITF_RAKIINSTANCE_H_

