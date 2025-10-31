
#include "precompiled_engine.h"

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

namespace ITF
{

void TemplateClientHandler::addUsedTemplate(const TemplateClientHandler& _hdl)
{
    for ( u32 i = 0; i < _hdl.m_usedTemplates.size(); ++i )
        addUsedTemplate( _hdl.m_usedTemplates[i] );
}

void TemplateClientHandler::addUsedTemplate(const StringID& _id)
{        
    ITF_VECTOR<StringID>::iterator it = std::find(m_usedTemplates.begin(), m_usedTemplates.end(), _id);
    if (it != m_usedTemplates.end())
        return;
    m_usedTemplates.push_back(_id);
    if (TEMPLATEDATABASE)
        TEMPLATEDATABASE->addTemplateClient(_id, this);
}

void TemplateClientHandler::removeUsedTemplate(const StringID& _id)
{
    ITF_VECTOR<StringID>::iterator it = std::find(m_usedTemplates.begin(), m_usedTemplates.end(), _id);
    if (it == m_usedTemplates.end())
    {
        ITF_ASSERT_MSG(bfalse, "unknown template");
        return;
    }
    if (TEMPLATEDATABASE)
        TEMPLATEDATABASE->removeTemplateClient(_id, this);

    m_usedTemplates.erase(it);
}

void TemplateClientHandler::freeUsedTemplates()
{
    if (TEMPLATEDATABASE)
    {
        for (u32 i = 0; i < m_usedTemplates.size(); i++)
        {
            TEMPLATEDATABASE->removeTemplateClient(m_usedTemplates[i], this);
        }
    }
    m_usedTemplates.clear();
}

bbool TemplateClientHandler::isUsedTemplate(const StringID& _id)
{
    ITF_VECTOR<StringID>::iterator it = std::find(m_usedTemplates.begin(), m_usedTemplates.end(), _id);
    return (it != m_usedTemplates.end());
}

TemplateClientHandler::~TemplateClientHandler()
{
   // ITF_ASSERT_MSG(m_usedTemplates.size() == 0, "template leak");
    freeUsedTemplates();
}


}
