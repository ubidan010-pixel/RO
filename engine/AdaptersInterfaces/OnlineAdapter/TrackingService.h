#ifndef ITF_TRACKING_SERVICE_H
#define ITF_TRACKING_SERVICE_H

#include "core/templateSingleton.h"
#include "core/types.h"

namespace ITF
{

class TrackingService
{
public:

	/**
	* Attribute Datatype Enum
	*/
	enum ATTRIBUTE_DATATYPE
	{
		E_ATTRIB_DATATYPE_STRING	=	0,
		E_ATTRIB_DATATYPE_INT32	,
		E_ATTRIB_DATATYPE_UINT32 ,
        E_ATTRIB_DATATYPE_F64   ,
        E_ATTRIB_DATATYPE_U64 ,
        E_ATTRIB_DATATYPE_BOOL
	};

    virtual ~TrackingService() {}

    virtual bool init() = 0;
    virtual void term() = 0;
    virtual void update() = 0;

    // create a new tag, attributes need to be added to that tag
    virtual void startTag(const char *_name) = 0;
    virtual void updatePlayTime(u32 _sessionTime) = 0;
    // add as many attributes as needed for a tag
    virtual void appendAttribute(const char *_attributeName, const char *_value, const ATTRIBUTE_DATATYPE dataType) = 0;

    // fire-and-forget
    virtual void sendTag() = 0;
};

} // namespace ITF


#endif // ITF_TRACKING_SERVICE_H
