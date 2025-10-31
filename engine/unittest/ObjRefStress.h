#ifndef _ITF_OBJREFSTRESS_H_
#define _ITF_OBJREFSTRESS_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif //SERIALIZEROBJECT_H   

namespace ITF
{
class ObjRefStress
{
public:
    static void runTest();

};
} // namespace ITF



#endif //_ITF_OBJREFSTRESS_H_

