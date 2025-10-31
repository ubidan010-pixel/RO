#ifndef _ITF_BASEOBJECTFACTORY_H_
#define _ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_



namespace ITF
{
    class BaseObjectFactory
    {
    public:
        BaseObjectFactory();

    //private:

        ObjectFactory m_objectFactory;
    };

} // namespace ITF

#endif // _ITF_BASEOBJECTFACTORY_H_
