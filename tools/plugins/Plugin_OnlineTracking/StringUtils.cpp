#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_


#ifndef __ITF__STRINGUTILS_H_
#include "StringUtils.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_


#include <string>
#include <vector>



namespace ITF {

	StringSplitUtils::StringSplitUtils() {}
	StringSplitUtils::~StringSplitUtils() {}

	std::map<String,String> StringSplitUtils::SplitTrackingAttributes(const String & input){

		std::map <String,String> data;
		std::vector <String> elements;
		StringSplitUtils::SplitString( input ,"&", elements);

	//	LOG("elements[0] : %s",elements[0].getCharCopy());

		for (u32 i=0;i<elements.size();i++)
		{
		
			std::vector <String> elements1;
			StringSplitUtils::SplitString( elements[i] , "=", elements1);
			//StringConverter c(elements[0]);
			//std::string key = c.getChar();

			if (elements1.size()>1)
			{
				data[ elements1[0] ] = elements1[1];
			//	LOG("setting data [\"%s\"] = %s ",elements1[0].getCharCopy(), elements1[1].getCharCopy() );
			}

		}

		return data;


	}

	int StringSplitUtils::SplitString(const String & input_, const String & delimiter_, std::vector<String>& results)
	{

		StringConverter c(input_);
		std::string input = c.getChar();
		StringConverter c2(delimiter_);
		std::string delimiter = c2.getChar();

		int iPos = 0;
		int newPos = -1;
		int sizeS2 = int(delimiter.size());
		int isize = int(input.size());

		std::vector<int> positions;

		newPos = int(input.find(delimiter, 0));

		if( newPos < 0 ) { return 0; }

		int numFound = 0;

		while( newPos > iPos )
		{
			numFound++;
			positions.push_back(newPos);
			iPos = newPos;
			newPos = int(input.find(delimiter, iPos+sizeS2+1));
		}

		for( unsigned int i=0; i <= positions.size(); i++ )
		{
			std::string s;
			
			if( i == 0 ) { s = input.substr( i, positions[i] ); }
			int offset = positions[i-1] + sizeS2;
			if( offset < isize )
			{
				if( i == positions.size() )
				{
					s = input.substr(offset);
				}
				else if( i > 0 )
				{
					s = input.substr( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
				}
			}
			if( s.size() > 0 )
			{
				String fs = s.c_str();

				results.push_back(fs);
			}
		}
		return numFound;
	}

}

