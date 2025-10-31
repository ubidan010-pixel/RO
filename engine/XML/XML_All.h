#ifndef _ITF_XMLALL_H_
#define _ITF_XMLALL_H_

class  TiXmlElement;

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_LOAD_INFO_H_
#include "engine/scene/loadInfo.h"
#endif //_ITF_LOAD_INFO_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

namespace ITF
{

class PlayField;
class Scene;
struct XMLConfig;
class String;
class Editor;
class Skeleton;
struct CharacterConfig;
class ResourceGroup;

class AnimSkeleton;
class AnimPatchBank;
class AnimTrack;
class AnimPolylineBank;
class ArchiveMemory;
class FriseTextureConfig;
class AnimationDependenciesMap;

class XMLAllHotReloadHandler : public WatchDirectoryListener
{
public:
    void onNewFile(const String& _absoluteFilename);
    void onModifiedFile(const String& _absoluteFilename);
};

class XMLAll
{
public:
    
    // global / config
    static u32  readCoreConfig              (   XMLConfig* _config, const String& _szXMLFile   );
    static u32  readStaticData              (   const String& _szXMLFile                       );
    static u32  readProjectConfig           (   XMLConfig* _config, const String& _szXMLFile   );
    
    // scenes
    static bbool readMapFolder              (   Scene* _pScene,const LoadInfo& _info, bbool _bcookMode = bfalse);

#ifdef ITF_SUPPORT_XML
    static bbool readScene                  (   Scene* _pScene,const Path& _szXMLFile, class CSerializerObjectParser* serializer = NULL );
#endif //ITF_SUPPORT_XML

#ifndef ITF_CONSOLE
    static bbool saveScene                  (   const String& _absoluteFilePath ,Scene* _pScene,bbool _binary = bfalse  );
#endif ITF_CONSOLE

#ifdef ITF_SUPPORT_XML
    // various
    static u32    saveFields                (   const ITF_MAP<String, String>& _FieldMap, const String& _szXMLFile  );
    static u32    readFields                (   ITF_MAP<String, String>& _FieldMap, const String& _szXMLFile  );
    static String readField                 (   const String &_Field, const String& _szXMLFile  );

    static void     readTextDB              (   const String& _szXMLFile    );

    static void readFluidFriseLayers        (PugiXMLWrap &_XML, class FriseConfig* _pConfig);
#endif //ITF_SUPPORT_XML

    static class FriseConfig* loadFriseConfig(const String& _file, bbool _reload, bbool _cooking);
    static XMLAllHotReloadHandler m_hotReloader;

private:
    static void   loadSceneAfterFxBank      (Scene* _pScene, const String& _afterFxFile);

#ifdef ITF_SUPPORT_XML
    static void   readTriggers              (PugiXMLWrap & _xml);
    static void   saveTriggers              (pugi::xml_node * _root);
    static void   loadTextureConfig         (FriseConfig* pConfig, FriseTextureConfig& _texConfig, PugiXMLWrap& _XML);
#endif //ITF_SUPPORT_XML
};

} // namespace ITF

#endif // _ITF_XMLALL_H_
