#ifndef __SOUNDFACTORIESREGISTRATION_H__
#define __SOUNDFACTORIESREGISTRATION_H__

namespace	ITF
{
	class		ObjectFactory;

	namespace	AudioSerializedData
    {

	    void	registerEvents(ObjectFactory &_objectFactory);
	    void	registerComponents(ObjectFactory &_objectFactory);
	    void	registerTemplateComponents(ObjectFactory &_objectFactory);
    }	
}		
#endif // __SOUNDFACTORIESREGISTRATION_H__
