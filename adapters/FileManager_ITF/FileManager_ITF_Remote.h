#ifndef _ITF_FILEMANAGER_ITF_REMOTE_H_
#define _ITF_FILEMANAGER_ITF_REMOTE_H_

#include "core/Macros.h"

#if !defined(ITF_FINAL) && defined(ITF_SUPPORT_NET)
#define ITF_SUPPORT_FILEMANAGER_REMOTE 1
#endif

#ifdef ITF_SUPPORT_FILEMANAGER_REMOTE

#ifndef _ITF_FILEMANAGER_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_NETPACKETCLIENT_H_
#include "engine/network/NETPacketClient.h"
#endif //_ITF_NETPACKETCLIENT_H_


namespace ITF
{
    class TCPAdapter_ITF;
    class FileRemote;
    class FileWatcher_Remote;
    class ClientRemote;
    class WatchDirectoryListener;
    class NETPacketHandler;

    struct RequestStatus
    {
        RequestStatus():fileRemote(NULL),fileSize(0),id(U32_INVALID),status(U32_INVALID),resultat(bfalse) {};
        u32     id;
        u32     status;
        String  filename;
        bbool   resultat;
        u64     fileSize;
        FileRemote* fileRemote;

    };

    struct FileDescription
    {
        FileDescription(u32 _size,u64 _flushtime):size(_size),flushTime(_flushtime) {};
        u32 size;
        u64 flushTime;

    };

    class FileManager_ITF_Remote : public FileManager_ITF
    {
    public:

        FileManager_ITF_Remote();
        virtual ~FileManager_ITF_Remote();

        virtual bbool   loadSecureFat() { return bfalse; }
        virtual bbool   registerBundle(const String& _szFilename, bbool useCache = bfalse, BundlePriority prio = Lowest, const char* tag = NULL) { return bfalse; }
        virtual bbool   unregisterBundle(const String& _szFilename) { return bfalse; }
        virtual bbool   unregisterBundleTag(const char* tag) { return bfalse; }
        virtual bbool   isBundleRegistered(const String& _szFilename) const { return bfalse; }
        virtual void    loadBundles() {}
        virtual void    unloadBundles() {}
        virtual void    reloadBundles() {}
        virtual void    closePendingBundles() {}

        File*  openFile(const String& _szFileName,u32 mode);
        virtual void    mountFileDevice(const String& _szDeviceName,const String& _szDrivePath,u32 _priority,u32 _mode = ITF_FILE_ATTR_READ);
  
        void initConnection();
        void updateNetwork();

        void ping();

        void  deleteFile(const String& _szFileName);

        bbool fileExists(const String& _szFileName);

        u64   getFileSize(const String& _szFileName);
      
        void cbRequest(u32 _requestID,bbool _resultat);

        void fileExistsCommand(Blob& _blob);
        void informationCommand(Blob& _blob);
        void openFileCommand(Blob& _blob);
        void getFileSizeCommand(Blob& _blob);
        void pingCommand(Blob& _blob);
        void connectedCommand(Blob& _blob);
        void filesChangeCommand(Blob& _blob);
        void openmapCommand(Blob& _blob);

        bbool isRequestDone(u32 request);

        void addFileDescription(const String& _szFilename,u32 _filesize,u64 _flushtime);
        bbool   isRemote()                            {return btrue;}

        RequestStatus* getRequestStatus(u32 _id);

        void update();
        
        
        void addFileEvent(WatchDirectoryListener* _pListener,const String& _szFilter);



    protected:


        ITF_THREAD_CRITICAL_SECTION m_csSyncing;
        ITF_THREAD_CRITICAL_SECTION m_csPendingMap;

        TCPAdapter_ITF*         m_tcpAdapter;
        ClientRemote*           m_clientRemote;
        FileWatcher_Remote*     m_fileWatcher;
        NETPacketHandler*       m_pNetPacketHandler;

        bbool fileExists_internal(const String& _szFilename);
        File* openFile_internal(const String& _szFilename);
        u64   getFileSize_internal(const String& _szFilename);

        u32 sendRequest(const String& _szFilename,const String& _szCommand);

        ITF_VECTOR<String>                      m_mapPendings;

        ITF_MAP<String,bbool>                   m_mFastFileExist;
        ITF_MAP<u32 , RequestStatus*>           m_PendingFiles;
        ITF_MAP<String , bbool>                 m_AvailableFiles;
        ITF_MAP<String , FileDescription>       m_DescriptionFiles;

        ITF_THREAD_EVENT    m_exitEvent;
        bbool               m_exitThread;
    };


    typedef void (FileManager_ITF_Remote::*func_Command)(u32 requestID,bbool resultat);
    typedef void (FileManager_ITF_Remote::*func_CommandBlob)(Blob& _blob);

    #define CALL_FONCTION(object,ptrToMember)  ((object).*(ptrToMember)) 


#if !defined( ITF_CTR )
    class ClientRemote: public NETPacketClient
    {
        public:
            ClientRemote(FileManager_ITF_Remote* pOwner);

            void init();

            bbool receive(Blob& _blob, const NETPeer* _peer, u32 _senderID, bbool _unknowListener);
            const NETPeer*      getNetPeer()                {return m_peer;}

            bbool isConnected()                         {return m_connected;}
            void setConnected(bbool _bConnected)        {m_connected = _bConnected;}

            void setCBCommand(func_Command _func) {m_funcCommand = _func;}



        protected:
            const NETPeer* m_peer;
            volatile bbool m_connected;
            FileManager_ITF_Remote* m_owner;

            func_Command m_funcCommand;

            ITF_THREAD_CRITICAL_SECTION m_csReceive;

            ITF_MAP<String,func_CommandBlob> m_CommandBlob;

    };
#endif // ITF_CTR    

} //namespace ITF

#endif // ITF_SUPPORT_FILEMANAGER_REMOTE

#endif  //_ITF_FILEMANAGER_ITF_REMOTE_H_ 
