#ifndef _ITF_SYNCHRONIZE_H_
#define _ITF_SYNCHRONIZE_H_

#pragma once

namespace ITF
{

}

#if defined (ITF_MICROSOFT)
#include "Microsoft/Synchronize_microsoft.h"
#elif defined (ITF_PS3)
#include "PS3/Synchronize_ps3.h"
#elif defined (ITF_X360)
#include "X360/Synchronize_x360.h"
#elif defined (ITF_IPAD)
#include "iPad/Synchronize_iPad.h"
#elif defined (ITF_CTR)
#include "CTR/Synchronize_CTR.h"
#elif defined (ITF_WII)
#include "WII/Synchronize_WII.h"
#elif defined (ITF_VITA)
#include "VITA/Synchronize_VITA.h"
#elif defined (ITF_PS5)
#include "PS5/Synchronize_PS5.h"
#elif defined (ITF_NINTENDO)
#include "Nintendo/Synchronize_Nintendo.h"
#else
#error Platform not supported
#endif //ITF_WINDOWS


#ifndef _ITF_AUTOLOCK_H_
#include "core/system/autolock.h"
#endif //_ITF_AUTOLOCK_H_



#endif // _ITF_SYNCHRONIZE_H_
