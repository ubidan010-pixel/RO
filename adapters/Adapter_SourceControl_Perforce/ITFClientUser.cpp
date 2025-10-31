#include "precompiled_SourceControlAdapter_Perforce.h"

#ifndef ITF_FINAL

#ifndef _ITFCLIENTUSER_PERFORCE_H_
#	include "adapters/Adapter_SourceControl_Perforce/ITFClientUser.h"
#endif //_ADAPTERSOURCECONTROL_PERFORCE_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/BaseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

namespace  ITF
{
	void ITFClientUser::OutputBinary( const char *data, int length )
	{
	}

	void ITFClientUser::OutputError( const char *errBuf )
	{        
		ITF_ERROR("[PERFORCE] %s",errBuf);
	}
	void ITFClientUser::OutputInfo( char level, const char *data )
	{	
        String displayMsg;
        displayMsg.setTextFormat("[PERFORCE] %s",data);
        DEBUGINFO->addDebugInfoDisplay(displayMsg,5.0f);
	}

	void ITFClientUser::OutputStat( StrDict *varList )
	{
		
	}

	void 	ITFClientUser::OutputText( const char *data, int length )
	{

	}
} // namespace ITF

#endif // ITF_FINAL