#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_XML

#ifndef _ITF_XMLFILE_H_
#include "engine/XML/XMLFile.h"
#endif //_ITF_XMLFILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(XMLFile)

	void    XMLFile::flushPhysicalData()
	{
        ITF_ASSERT(isPhysicallyLoaded());
        setPhysicalLoaded(bfalse);
		SF_DEL(m_XML);
	}

	bbool    XMLFile::tryLoadFile ()
	{
        ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");

        if (m_XML)
            return btrue;

        PathString8_t pathName;

        getPath().getString(pathName);

        m_XML = newAlloc(mId_Resource,PugiXMLWrap());
        setPhysicalLoaded(btrue);
        return m_XML->openFile(pathName.cStr(), bfalse);
	}

} // namespace ITF

#endif //ITF_SUPPORT_XML
