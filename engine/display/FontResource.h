#ifndef _ITF_FONTRESOURCE_H_
#define _ITF_FONTRESOURCE_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

namespace ITF
{
    class Font;

	class FontResource : public Resource
	{
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(FontResource,Resource,4199737023);
	public:

		ResourceType getType()const {return ResourceType_Font;}
		
        Font*       getFont()   {return m_pFont;}
        

	private:
        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instanciate a resource

        FontResource(const Path &path);
		void    flushPhysicalData        (                        );
		bbool   tryLoadFile     (                        );

        Font*   m_pFont;
	};

} // namespace ITF

#endif // _ITF_FONTRESOURCE_H_
