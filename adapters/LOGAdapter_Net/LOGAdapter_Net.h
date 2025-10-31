#ifndef _ITF_LOGADAPTER_NET_H_
#define _ITF_LOGADAPTER_NET_H_

#ifndef _ITF_LOGADAPTER_H_
    #include "engine/AdaptersInterfaces/LOGAdapter.h"
#endif //_ITF_LOGADAPTER_H_

namespace ITF
{


class LOGAdapter_Net : public LOGAdapter
{
public:

    void        init            (                    );
    void        close           (                    );
    void        msg             ( const String& _msg );
};

} // namespace ITF



#endif //_ITF_LOGADAPTER_NET_H_