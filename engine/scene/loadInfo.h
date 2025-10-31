#ifndef _ITF_LOAD_INFO_H_
#define _ITF_LOAD_INFO_H_

namespace ITF
{

    struct LoadInfo
    {
        Path      m_mapPath;                    // The relative path of the scene or worldfile
        String      m_szUniqueName;             // The unique name of the scene (OUT)
        bbool       m_forceLoadResources;       // Force all scene's resources to be loaded
        bbool       m_disconnectFromCells;      // Remove all scene's content from cells, so objects are always actives
        bbool       m_bAsyncLoad;               // Request for asynchronouos loading, used with Interface_SceneEventsListener
        bbool       m_bNodata;                  // If nodata then it's a new empty scene
        bbool       m_bBinary;                  // Load file in binary mode or plain xml (old format)
        ObjectRef   m_worldRef;                 // When loading a scene, attach it to this world ref

            LoadInfo()
                : m_forceLoadResources(bfalse)
                , m_disconnectFromCells(bfalse)
                , m_bAsyncLoad(bfalse)
                , m_worldRef(ITF_INVALID_OBJREF)
                , m_bNodata(bfalse)
                , m_bBinary(bfalse)
            {}
    };

} // namespace ITF

#endif // _ITF_LOAD_INFO_H_