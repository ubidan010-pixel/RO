#ifndef _ITF_WORLDFILE_H_
#define _ITF_WORLDFILE_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

namespace ITF
{
    class Interface_WorldFile_ProjectData : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Interface_WorldFile_ProjectData, IRTTIObject,584629053);
        DECLARE_SERIALIZE_INTERFACE()

    public:
        Interface_WorldFile_ProjectData() {};
        virtual ~Interface_WorldFile_ProjectData() {};

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called when the scene is loaded and physically ready, to apply some settings specific to the scene
        virtual void    applySettings() = 0;
    };

    /*
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Sample xml :

    <?xml version="1.0" ?>
    <root>
        <WorldFile scenePath="debug/simpleground2.isc" description="testouille">
            <projectSpecific NAME="Ray_WorldFile">
                <Ray_WorldFile>
                </Ray_WorldFile>
            </projectSpecific>
        </WorldFile>
    </root>

    //////////////////////////////////////////////////////////////////////////////////////////////////
    */

    class WorldFile
    {
        DECLARE_SERIALIZE()
        
    public:
        WorldFile();
        virtual ~WorldFile();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Load from file
        /// @param _relativePath (const String &) the relative path to the file to load
        /// @return (ITF::bbool)
        bbool                       loadFromFile(const Path& _relativePath);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set project data - WARNING : PARAMETER WILL BE DELETED AT DESTRUCTION!
        void                        setProjectData(Interface_WorldFile_ProjectData *_projectData) {SF_DEL(m_projectData); m_projectData = _projectData;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Save to file
        /// @param _absolutePath (const String &) the absolute path to output file
        /// @return (ITF::bbool)
#if defined(ITF_SUPPORT_XML)
        bbool                       saveToFile(const String& _absolutePath);
#endif // ITF_SUPPORT_XML

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called when the world becomes active, to apply some settings specific to the scene
        void                        applySettings();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Scene path accessor
        /// @return (const Path&) the path to the root scene
        const Path&                 getScenePath()const     { return m_scenePath; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Scene description
        /// @return (const String&) the description for the root scene in this context
        const String&               getDescription()const   { return m_description; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// WorldFile_ProjectData accessor
        /// @return (WorldFile_ProjectData*) the project data
        Interface_WorldFile_ProjectData*      getProjectData()const   { return m_projectData; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Register the project data type for serialization
        template <class C>
        static void                 registerProjectData()   { m_projectDataFactory.RegisterObject<C>(C::GetClassCRCStatic()); }

    private:
        Path                        m_scenePath;
        String                      m_description;
        Interface_WorldFile_ProjectData*      m_projectData;

        static ObjectFactory        m_projectDataFactory;
    };

}

#endif // _ITF_WORLDFILE_H_
