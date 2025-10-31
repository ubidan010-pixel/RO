#ifndef _ITF_INPUTUTILS_H_
#define _ITF_INPUTUTILS_H_

#ifndef _ITF_INPUTINFO_H_
#include "engine/zinput/ZInputInfo.h"
#endif //_ITF_INPUTINFO_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{

	class InputUtils
	{
	public:
		static void     ProcessAxis( struct SInputInfo& info, const struct SInputInfo& newInfo );
		static void     ProcessButton( struct SInputInfo& info, const struct SInputInfo& newInfo );
        static void     ProcessAccelerometer( struct SInputInfo& info, const struct SInputInfo& newInfo );
		static void     UpdateActionInput( ZInput& input, const struct SInputInfo& info, u32 deviceID );

        static SInputInfo::EInputQuery   TranslateQuery( const StringID & query );
	};
}; //namespace ITF

#endif // _ITF_INPUTUTILS_H_

