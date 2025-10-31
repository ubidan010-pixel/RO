#ifndef _ITF_SOUND_H_
#define _ITF_SOUND_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/resource.h"
#endif //_ITF_RESOURCE_H_

namespace ITF
{
	class Sound : public Resource
	{
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Sound,Resource,2608772127);
	public:

		ResourceType getType()const {return ResourceType_Sound;}
		void* m_adapterImplementationData;
        bbool m_loop;

	private:
        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instanciate a resource

        Sound(const Path &path) : Resource(path),m_adapterImplementationData(0),m_loop(bfalse) {};
		void    flushPhysicalData        (                        );
		bbool   tryLoadFile     (                        );

#ifdef ITF_SUPPORT_COOKING
        virtual bbool mustBeCooked()      const   {return btrue;}
#endif //ITF_SUPPORT_COOKING


	};

} // namespace ITF

#endif // _ITF_SOUND_H_
