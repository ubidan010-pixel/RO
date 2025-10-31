#ifndef __ITF__STRINGUTILS_H_
#define __ITF__STRINGUTILS_H_

#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_  

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_


#include <vector>


namespace ITF{

	class String;

	class StringSplitUtils
	{

	public:

		StringSplitUtils();
		~StringSplitUtils();

		static int SplitString(const String & input, const String & delimiter, std::vector<String>& results);

		static std::map<String,String> SplitTrackingAttributes(const String & input);
	};

}

#endif

