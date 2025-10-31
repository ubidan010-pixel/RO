#ifndef _ITF_SOURCECONTROLADAPTER_H_
#define _ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_
#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_
namespace ITF
{

class SourceControlAdapter : public TemplateSingleton<SourceControlAdapter>
{
public:

	virtual             ~SourceControlAdapter     (                   ) {};

protected:

	SourceControlAdapter() {  };	

public:
    virtual bbool checkout(const String& _filenameRelative) = 0;
    virtual bbool add(const String& _filenameRelative) = 0;
};

#define SOURCECONTROL_ADAPTER SourceControlAdapter::getptr()


} // namespace ITF

#endif //_ITF_SOURCECONTROLADAPTER_H_