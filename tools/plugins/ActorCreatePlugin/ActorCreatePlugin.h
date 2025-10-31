#ifndef _ITF_ACTORCREATEPLUGIN_H_
#define _ITF_ACTORCREATEPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{
    class ActorCreatePlugin;

    class ActorEditorParamsFiles
    {
    public:
        DECLARE_SERIALIZE()

        enum componentSaveType
        {
            noSave = 0,
            saveInActorBase,
            saveInComponent,
        };

        ActorEditorParamsFiles() : m_saveEnable(saveInActorBase) {}
        ~ActorEditorParamsFiles()   {}

        String8             m_component;
        Path                m_path;
        Path                m_subPath;
        componentSaveType   m_saveEnable;


        void blobToParams(class Blob& _blob)
        {
            String comp;
            _blob.extractString(comp);
            m_component = String8(comp.cStr());

            m_saveEnable = (componentSaveType)_blob.extractInt32();

            String path;
            _blob.extractString(path);
            // normalize path
            FilePath::normPath(path);
            FilePath::transformToRelative(path);
            path = FilePath::normalizePath(path);
            m_path = path;

            String subPath;
            _blob.extractString(subPath);
            // normalize path
            FilePath::normPath(subPath);
            FilePath::transformToRelative(subPath);
            subPath = FilePath::normalizePath(subPath);
            m_subPath = subPath;
        }

        void paramsToBlob(class Blob& _blob) const
        {
            PathString_t pathName, subPathName;

            m_path.getString(pathName);
            m_subPath.getString(subPathName);

            _blob.pushString(m_component.cStr());
            _blob.pushInt32((i32)m_saveEnable);
            _blob.pushString(pathName);
            _blob.pushString(subPathName);
        }
    };


    class ActorCreateSpProcess
    {
    public:
        static void    Register(ITF_MAP<String, ActorCreateSpProcess*> &_spMap, ActorCreateSpProcess * _spProcess, ActorCreatePlugin * _actorCreatePlugin)
        {
            _spMap[_spProcess->GetComponentName()] = _spProcess;
            _spProcess->m_actorCreatePlugin = _actorCreatePlugin;
        }
        virtual char *  GetSpViewMessage() { return "SpView"; }

        virtual char *          GetComponentName() = 0;
        virtual void            ReceiveMsg(class Blob& _blob) = 0;
        virtual void            ReceiveActorReload() = 0;
        virtual ITF_STDSTRING   saveComponent(Actor *actor, Path componentPath, ActorEditorParamsFiles::componentSaveType saveEnable, TemplateActorComponent *actorComponent) = 0;

    protected:
        ActorCreatePlugin * m_actorCreatePlugin;
    };

    typedef ITF_MAP<String, ActorCreateSpProcess *> SpProcessMap;


    class ActorEditorParams
    {
    public:
        DECLARE_SERIALIZE()

		enum actorSaveType
		{
			noSave = 0,
			saveAllInActor,
			save,
		};

        ActorEditorParams()
            : m_indentationNbChars(4)
            , m_actorFile ("")
            , m_saveActorEnable (saveAllInActor)
            , m_actorBaseFile ("")
            , m_saveActorBaseEnable (btrue)
			, m_useActorBaseEnable (btrue)
            {}

        ITF_VECTOR<ActorEditorParamsFiles>  m_componentFiles;
        i32                                 m_indentationNbChars;

        // actor file
        String                              m_actorFile;
        actorSaveType                       m_saveActorEnable;

        // actor_base file
        String                              m_actorBaseFile;
		bbool                               m_useActorBaseEnable;
        bbool                               m_saveActorBaseEnable;
    };


#define ACTOR_IN_CREATED     0x1
#define ACTOR_IN_NEW         0x2
#define ACTOR_IN_VISIBLE     0x4
#define ACTOR_IN_PAUSE       0x8


    class ActorIn {
    public:
        ObjectRef               m_actor;
        ActorEditorParams       m_params;
        bbool                   m_flags;
        bbool                   m_actorInMain;
        String                  m_actorCurComponent;
        ActorCreateSpProcess *  m_actorCurView;

        bool                    m_myActorTemplateIsValid;
        class Actor_Template    *m_myActorTemplate;
		class Actor_Template    *m_myActorTemplateOverride;

        ActorIn() : m_actor(ITF_INVALID_OBJREF)
            , m_flags(0)
            , m_actorInMain(bfalse)
            , m_actorCurView(NULL)
            , m_myActorTemplateIsValid(bfalse)
            , m_myActorTemplate(NULL)
            , m_myActorTemplateOverride(NULL)
        {}            
    };

    class ActorCreatePlugin : public Plugin
    {
    public:
        ActorCreatePlugin();
        virtual ~ActorCreatePlugin();

        void onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );

        /************************************************************************/
        /*                                                                      */
        /************************************************************************/
        static const char * actHeader() {return "includeReference(\"Actor/Includes/helpers.ilu\")\n\n";}
        static const char * actEnd() {return "\nparams.Actor_Template.COMPONENTS = {}\n";}

        static const char * actInclude() {return "includeReference(\"%S\")\n\n";}
        static const char * iluName() {return "\ncomponent.NAME = \"%s\"";}
        static const char * iluEnd() {return "\nappendTable(params.Actor_Template.COMPONENTS, {component})\n\n";}

        /************************************************************************/
        /* Messages                                                             */
        /************************************************************************/

        static const char*      getPluginName()                 {return "ActorCreate_Plugin";}
        static const char*      getSendCommandClearName()       {return "Clear";}
        static const char*      getRecvCommandDumpToFileName()  {return "DumpToFile";}

        static const char*      getSpViewName()                 {return "SpView";}

        static const char*      getSendSessionInfoCmdName()             {return "SessionInfo";}
        static const char*      getSendComponentListName()              {return "ComponentList";}
        static const char*      getSendActorListName()                  {return "ActorList"; }
        static const char*      getSendActorMainName()                  {return "ActorMain"; }
        static const char*      getSendActorMainDataName()              {return "ActorMainData"; }
        static const char*      getSendActorComponentListName()         {return "ActorComponentList"; }
        static const char*      getSendActorComponentName()             {return "ActorComponent"; }
        static const char*      getSendActorComponentDataBufferName()   {return "ActorComponentData";}
        static const char*      getSendActorEditorParamsName()          {return "ActorEditorParamsName";}
        static const char*      getSendErrorName()                      {return "Error";}

        static const char*      getRecvSetActorName()                   {return "setActor";}
		static const char*      getRecvNewActorName()                   {return "newActor";}
        static const char*      getRecvSetActorRefName()                {return "setActorRef";}
        static const char*      getRecvSetActorMainName()               {return "setActorMain"; }
        static const char*      getRecvSetActorMainDataName()           {return "setActorMainData";}
        static const char*      getRecvSetActorEditorParamsName()       {return "setActorEditorParams";}        
        static const char*      getRecvSetActorComponentName()          {return "setActorComponent";}
        static const char*      getRecvSetActorComponentDataName()      {return "setActorComponentData";}
        static const char*      getRecvSetActorComponentPosName()       {return "setActorComponentPos";}
        static const char*      getRecvAddActorComponentName()          {return "addActorComponent";}
        static const char*      getRecvDelActorComponentName()          {return "delActorComponent";}       

        static const char*      getRecvSaveActorName()                  {return "saveActor";}
        static const char*      getRecvCloseActorName()                 {return "closeActor";}
        static const char*      getRecvSpawnActorName()                 {return "spawnActor";}
        static const char*      getRecvChangeActorPauseName()           {return "changeActorPause";}
        static const char*      getRecvSetActorPauseName()              {return "setActorPause";}


        static const char*      getRecvCommandPropertiesName()  {return "getProperties";}
        static const char*      getRecvSessionInfoCmdName()     {return "getSessionInfo";}
        static const char*      getRecvComponentListName()      {return "getComponentList"; }
        static const char*      getRecvActorListName()          {return "getActorList"; }
        static const char*      getRecvActorComponentListName() {return "getActorComponentList"; }
        static const char*      getRecvActorComponentName()     {return "getActorComponent"; }
        static const char*      getRecvActorEditorParamsName()  {return "getActorEditorParams";}
        static const char*      getRecvGetActorMainDataName()   {return "getActorMainData";}


        static const float      getUpdateRate()             {return 0.5f;} // in seconds

        // Plugin inherited
        virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
        virtual void            onActorPicked(ObjectRef _actor); // inherited from Plugin
        void                    onBaseObjectPicked(ObjectRef _objref);

        virtual void            UnselectAll(bbool _force);
        virtual void            update();

        void                    cleanActorList();
        class Actor*            getActor(i32 _index  = -1, bbool forceSet = btrue);
        class Actor_Template *  getActorTemplate(i32 _index  = -1, bbool forceSet = btrue); 
        //Actor_Template *        getDummyActorTemplate(int _index);
        AnimTree_Template *     getAnimTreeTemplate(i32 _index  = -1, bbool forceSet = btrue);
        ActorEditorParams *     getActorEditorParams(i32 _index  = -1);
        u32                     getActorEditorFlags(i32 _index  = -1);
        void                    setActorEditorFlags(u32 flag, i32 _index  = -1);
        void                    addActorEditorFlags(u32 flag, i32 _index  = -1);
        void                    removeActorEditorFlags(u32 flag, i32 _index  = -1);

        class TemplateActorComponent *  getActorComponent(const String & _compoName, i32 _index  = -1);

    private:
        enum CreatorState
        {
            Creator_Idle,
            Creator_Copy,
            ENUM_FORCE_SIZE_32(0)
        };

        SpProcessMap            m_spViewMap;
        ITF_VECTOR<ActorIn>     m_actorList;
        i32                     m_actorIndex;
        i32                     m_actorNewCount;
        CreatorState            m_state;

        void                    reloadActor(i32 _index = -1);
        void                    pauseActor(i32 _index, bbool _pause);
        void                    setActorIndex(i32 _index);
        void                    setActorComponent(const String & _compoName);
        void                    setActorInMain(bbool _actorInMain);
        void                    clearActorIn(ActorIn *actorIn);

        void                    sendComponentList();
        void                    sendActorList();
        void                    sendActorCurrentData();
        void                    sendActorEditorParams(i32 _index);
        void                    sendActorMainData(i32 _index);
        void                    sendActorComponentList(i32 _index);
        void                    sendActorComponentData(const String & _compoName);
        void                    sendActorComponentData(TemplateActorComponent *_component);
        void                    sendActorComponentDataBuffer(const String & componentName, const String8& _data);
        void                    sendClearSelection();
        void                    sendError(const String &_errorMsg);

        void                    blobToActorEditorParams(class Blob& _blob, ActorEditorParams * _params);
        void                    actorEditorParamsToBlob(const ActorEditorParams * _params, class Blob& _blob);        

        void                    onSpView(class Blob& _blob);
        void                    onSetActor(class Blob& _blob );
		void                    onNewActor(class Blob& _blob );
        void                    onSetActorRef(class Blob& _blob );
        void                    onSetActorMain(class Blob& _blob );
        void                    onSetActorMainData(class Blob& _blob );
        void                    onSetActorEditorParams( class Blob& _blob );
        void                    onSetActorComponent(class Blob& _blob );
        void                    onSetActorComponentData(class Blob& _blob );
        void                    onSetActorComponentPos(class Blob& _blob );
        void                    onAddActorComponent(class Blob& _blob );
        void                    onDelActorComponent(class Blob& _blob );

        void                    onSaveActor(class Blob& _blob );
        void                    onCloseActor(class Blob& _blob );
        void                    onSpawnActor(class Blob& _blob );
        void                    onChangeActorPause(class Blob& _blob );
        void                    onSetActorPause(class Blob& _blob );

        void                    onPropertiesCmd( class Blob& _blob );
        void                    onGetSessionInfoCmd( class Blob& _blob );
        void                    onGetActorList(class Blob& _blob );
        void                    onGetComponentList( class Blob& _blob );
        void                    onGetActorComponentList( class Blob& _blob );
        void                    onGetActorEditorParams( class Blob& _blob );
        void                    onDumpToFileCmd( class Blob& _blob );
        void                    onGetActorMainData( class Blob& _blob );


        void                    fillComponentsPaths(i32 _actorIndex, bbool _resetPaths);
        void                    saveActor(class Blob& _blob);

        void                    manageForceReload(i32 _index, const String & _path, i32 _mainInfo);
        void                    manageSetActor(i32 _index, const String & _path, ObjectRef _ref, i32 _mainInfo, i32 _tags);
        void                    manageCopy();

        bbool                   m_toolIsAware;
        ObjectRef               m_currentSelection;
    };

} // namespace ITF


#endif // _ITF_ACTORCREATEPLUGIN_H_
