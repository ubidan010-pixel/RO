#ifndef _ITF_XMLFILE_H_
#define _ITF_XMLFILE_H_

#ifdef ITF_SUPPORT_XML

namespace ITF
{
    class PugiXMLWrap;
    class Resource;

	class XMLFile : public Resource
	{
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(XMLFile,Resource,3775533479);

	public:
		ResourceType getType()const {return ResourceType_XML;}
        PugiXMLWrap* getXMLWrap () { return m_XML; }
		bbool   tryLoadFile     (                        );

    private:
        friend class ResourceLoader; // Don't add more friends here. You MUST use RESOURCE_MANAGER to instanciate a resource

        PugiXMLWrap * m_XML;

		XMLFile(const Path &path) : Resource(path){m_XML = NULL;}
		void    flushPhysicalData        (  );
	};

} // namespace ITF

#endif //ITF_SUPPORT_XML

#endif // _ITF_XMLFILE_H_
