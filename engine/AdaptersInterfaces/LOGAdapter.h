#ifndef _ITF_LOGADAPTER_H_
#define _ITF_LOGADAPTER_H_

namespace ITF
{


class LOGAdapter
{
public:

    virtual             ~LOGAdapter     (                    )   {};  // always declare virtual destructor for adapters
    virtual void        init            (                    )   =   0;
    virtual void        close           (                    )   =   0;
    virtual void        msg             ( const String& _msg )   =   0;
};

} // namespace ITF



#endif //_ITF_LOGADAPTER_H_