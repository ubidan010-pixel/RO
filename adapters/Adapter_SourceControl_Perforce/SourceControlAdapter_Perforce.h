#ifndef _ADAPTERSOURCECONTROL_PERFORCE_H_
#define _ADAPTERSOURCECONTROL_PERFORCE_H_

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif //_ITF_SOURCECONTROLADAPTER_H_

namespace  ITF
{
class SourceControlAdapter_Perforce : public SourceControlAdapter
{
public:
    SourceControlAdapter_Perforce();
    ~SourceControlAdapter_Perforce();
    

    bbool checkout(const String& _filenameRelative);
    bbool add(const String& _filenameRelative);

protected:
	String8 ProjectDir;
private:

};    


} // namespace ITF

#endif //_ADAPTERSOURCECONTROL_PERFORCE_H_