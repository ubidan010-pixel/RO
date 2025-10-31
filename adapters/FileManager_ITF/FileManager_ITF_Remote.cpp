#include "precompiled_FileManager_ITF.h"

#ifndef _ITF_FILEMANAGER_ITF_REMOTE_H_
#include "adapters/FileManager_ITF/FileManager_ITF_Remote.h"
#endif //_ITF_FILEMANAGER_ITF_REMOTE_H_

#if ITF_SUPPORT_FILEMANAGER_REMOTE

#ifndef _ITF_PATH_H_
#include "core/file/path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_NETPACKETHANDLER_H_
#include "engine/network/NETPacketHandler.h"
#endif //_ITF_NETPACKETHANDLER_H_

#ifndef _ITF_TCPADAPTER_H_
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif //_ITF_TCPADAPTER_H_

#ifndef _ITF_TCPADAPTER_ITF_H_
#include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"
#endif //_ITF_TCPADAPTER_ITF_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_FILEREMOTE_H_
#include "core/file/FileRemote.h"
#endif //_ITF_FILEREMOTE_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_FILEWATCHER_REMOTE_H_
#include "engine/file/FileWatcher_remote.h"
#endif  //_ITF_FILEWATCHER_REMOTE_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{

#ifndef ITF_FINAL
#ifndef ITF_WII
#define REMOTE_SHOW_LOG 1
#endif //ITF_WII
#endif //ITF_FINAL

    u32 requestCount = 0;

    ITF_INLINE void waitRequest()
    {
        sleep(1);
    }

    FileManager_ITF_Remote::FileManager_ITF_Remote():m_tcpAdapter(NULL),m_clientRemote(NULL),m_pNetPacketHandler(NULL),m_exitThread(bfalse)
    {
        Synchronize::createCriticalSection(&m_csSyncing);
        Synchronize::createCriticalSection(&m_csPendingMap);
        Synchronize::createEvent(&m_exitEvent,"FileManager_ITF_Remote_ExitEvent");
        Synchronize::resetEvent(&m_exitEvent);
    }

    void FileManager_ITF_Remote::addFileDescription(const String& _szFilename,u32 _filesize,u64 _flushtime)
    {
        m_DescriptionFiles.insert(std::pair<String,FileDescription>(_szFilename,FileDescription(_filesize,_flushtime)));       
    }

    void  FileManager_ITF_Remote::mountFileDevice(const String& _szDeviceName,const String& _szDrivePath,u32 _priority,u32 _mode)
    {
        FileManager_ITF::mountFileDevice(_szDeviceName, _szDrivePath, _priority, _mode);

        
    }

    u32 WINAPI runNetwork(void* user)
    {
        FileManager_ITF_Remote* pThis = (FileManager_ITF_Remote*) user;

        pThis->updateNetwork();

        return 0;
    }


    void FileManager_ITF_Remote::updateNetwork()
    {
        while (!m_exitThread)
        {
            m_pNetPacketHandler->update(m_tcpAdapter);
            sleep(1);
        }

        if(m_exitThread)
            Synchronize::setEvent(&m_exitEvent);
    }

    void FileManager_ITF_Remote::initConnection()
    {
        m_pNetPacketHandler = new NETPacketHandler;

        m_tcpAdapter = new TCPAdapter_ITF();
        m_tcpAdapter->initialize();

        
        bbool res = m_tcpAdapter->setHost("", 2002);
        ITF_ASSERT(res);

        sleep(15);

        ITF_MemoryBarrier();

        THREADMANAGER->createThread(runNetwork,(void*) this, ThreadSettings::m_settings[eThreadId_SceneManager]);

        m_clientRemote = new ClientRemote(this);
        m_clientRemote->init();
        m_clientRemote->setCBCommand(&FileManager_ITF_Remote::cbRequest);
        m_clientRemote->startReceive(m_pNetPacketHandler);

        m_fileWatcher = new FileWatcher_Remote();
    }

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    FileManager_ITF_Remote::~FileManager_ITF_Remote()
    {
        if(m_clientRemote)
        {
            m_exitThread = btrue;
            Synchronize::waitEvent(&m_exitEvent);
            Synchronize::destroyEvent(&m_exitEvent);

            m_clientRemote->stopReceive(m_pNetPacketHandler);
            SF_DEL(m_clientRemote);

            m_tcpAdapter->destroy();
            SF_DEL(m_tcpAdapter);
        }

        SF_DEL(m_fileWatcher);
        Synchronize::destroyCriticalSection(&m_csSyncing);
        Synchronize::destroyCriticalSection(&m_csPendingMap);

        SF_DEL(m_pNetPacketHandler);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::addFileEvent(WatchDirectoryListener* _pListener,const String& _szFilter)
    {
        m_fileWatcher->registerWatch(_pListener,_szFilter);
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    RequestStatus* FileManager_ITF_Remote::getRequestStatus(u32 _id)
    {
        ITF_MAP<u32 , RequestStatus*>::iterator iter = m_PendingFiles.find(_id);
        if (iter!=m_PendingFiles.end())
        {
            RequestStatus* pStatus = (*iter).second;
            return pStatus;
        }
    
        return NULL;
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::cbRequest(u32 requestID,bbool resultat)
    {
        ITF_MAP<u32 , RequestStatus*>::iterator iter = m_PendingFiles.find(requestID);
        if (iter!=m_PendingFiles.end())
        {
            RequestStatus* pStatus = (*iter).second;
            pStatus->status = 1;
            pStatus->resultat = resultat;

//            LOG("[remote] cbRequest : %d %ls",pStatus->id, pStatus->filename.cStr());

        }
        else
        {

            //LOG("[remote] cbRequest NOT FOUND : %d",requestID);
        }
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  
    bbool FileManager_ITF_Remote::fileExists(const String& _szFileName)
    {

        static f64 timeSum = 0.0f;
        static u32 countFileExists   = 0;

        f64 time = SYSTEM_ADAPTER->getTime();
        String lowerName(_szFileName);
        lowerName.toLower();

        //special optimization for atlas file
        bbool fileExist = bfalse;
        if (lowerName.strstr((u16*)L"_atl.atl.ckd"))
        {
            ITF_MAP<String,bbool>::iterator iter = m_mFastFileExist.find(lowerName);
            if (iter != m_mFastFileExist.end())
            {
                fileExist = fileExists_internal(_szFileName);
#ifdef REMOTE_SHOW_LOG
                LOG("file exists fast %d %ls",fileExist,_szFileName.cStr());
#endif
            }
        }
        else
        {
            fileExist = fileExists_internal(_szFileName);
        }

        f64 time2 = SYSTEM_ADAPTER->getTime()-time;

        timeSum+=time2;
#ifdef REMOTE_SHOW_LOG
        LOG("file exists %d %d /  %f ms // %f s  ---  %ls",fileExist ,countFileExists++,(time2)*1000.0f,timeSum,_szFileName.cStr());
#endif
        return fileExist;
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    u32 FileManager_ITF_Remote::sendRequest(const String& _szFilename,const String& _szCommand)
    {
        while (!m_clientRemote->isConnected())
        {
           waitRequest();
        }

        u32 countId= requestCount;

        {
            requestCount++;
            countId=requestCount;
        }
   
        //f64 time = SYSTEM_ADAPTER->getTime();

        Blob blob(BLOB_EXTRACT_AND_WRITE_ENABLED, 512);
        blob.pushString("RemoteInstallation_Plugin");
        blob.pushString(_szCommand);
        blob.pushString(_szFilename);
        blob.pushUInt32(countId);

        RequestStatus* pRequestStatus = new RequestStatus();
        pRequestStatus->status = 0;
        pRequestStatus->id = countId;
        pRequestStatus->filename = _szFilename;

        {
            m_PendingFiles[countId] = pRequestStatus;
        }

        //f64 time2 = SYSTEM_ADAPTER->getTime()-time;
        //LOG(">> %f ms ",(time2)*1000.0f);

        bbool res = m_clientRemote->send(m_pNetPacketHandler,blob,m_clientRemote->getNetPeer());
        ITF_ASSERT(res);
    
        return countId;
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    u64   FileManager_ITF_Remote::getFileSize_internal(const String& _szFilename)
    {
        csAutoLock cs(m_csSyncing);

        u32 reqId = sendRequest(_szFilename,"getfilesize");

        while (!isRequestDone(reqId))
        {
           waitRequest();
        }


        return (getRequestStatus(reqId)->fileSize);

    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    bbool FileManager_ITF_Remote::fileExists_internal(const String& _szFilename)
    {   
        csAutoLock cs(m_csSyncing);

        u32 reqId = sendRequest(_szFilename,"fileexists");


        while (!isRequestDone(reqId))
        {
            waitRequest();
        }


        return (getRequestStatus(reqId)->resultat);
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    File* FileManager_ITF_Remote::openFile_internal(const String& _szFilename)
    {
        csAutoLock cs(m_csSyncing);
        
        u32 reqId = sendRequest(_szFilename,"openfile");
        
        while (!isRequestDone(reqId))
        {
            waitRequest();
        }

        return (getRequestStatus(reqId)->fileRemote);
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool FileManager_ITF_Remote::isRequestDone(u32 request)
    {
        ITF_MAP<u32 , RequestStatus*>::iterator iter = m_PendingFiles.find(request);

        if (iter!=m_PendingFiles.end())
        {
            RequestStatus* pStatus = (*iter).second;
            return pStatus->status;
        }
        else
        {
            ITF_ASSERT(0);
        }

        return bfalse;
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    File*  FileManager_ITF_Remote::openFile(const String& _szFileName,u32 mode)
    {
        static f64 timeFileOpen = 0.0f;
        static u32 countOpenFile   = 0;

        f64 time = SYSTEM_ADAPTER->getTime();

        if ((mode & ITF_FILE_ATTR_WRITE)  == ITF_FILE_ATTR_WRITE)
        {
            ITF_ASSERT(0);
        }

        File* pFile =  openFile_internal(_szFileName);

        f64 time2 = SYSTEM_ADAPTER->getTime()-time;

#ifndef ITF_FINAL
        if ( pFile != NULL )
            m_logger.LogOpenedFile( _szFileName );
#endif //ITF_FINAL

        timeFileOpen+=time2;
        
        if (pFile)
        {
#ifdef REMOTE_SHOW_LOG
            LOG("open file threadId:0x%08x CountFile:%d / %f ms // %f s  ---  size :%d  %0.02f kb/s %ls",(u32)Synchronize::getCurrentThreadId(),countOpenFile++,(f32)(time2*1000.0),(f32)timeFileOpen,(u32)pFile->getLength(),(f32)((f64)pFile->getLength()/(1024.0*time2)),_szFileName.cStr());
#endif
        }
        
        return pFile;
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void  FileManager_ITF_Remote::deleteFile(const String& _szFileName)
    {
        csAutoLock cs(m_csSyncing);

        ITF_MAP<String , bbool> ::iterator iter = m_AvailableFiles.find(_szFileName);
        if (iter!=m_AvailableFiles.end())
        {
            m_AvailableFiles.erase(iter);
        }
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    u64   FileManager_ITF_Remote::getFileSize(const String& _szFileName)
    {
        u64 fileSize = getFileSize_internal(_szFileName);
        return fileSize;
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::getFileSizeCommand(Blob& _blob)
    {
        u32 requestId = _blob.extractUint32();
        u64 fileSize  = _blob.extractUint64();

        RequestStatus* pRequestStatus = getRequestStatus(requestId);
        pRequestStatus->fileSize = fileSize;
        cbRequest(requestId,btrue);
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::pingCommand(Blob& _blob)
    {
        u32 pingID = _blob.extractUint32();
        //send directly to the host
        Blob blob(BLOB_EXTRACT_AND_WRITE_ENABLED, 256);
        blob.pushString("RemoteInstallation_Plugin");
        blob.pushString("answerping");
        blob.pushUInt32(pingID);

        bbool testBandWidth =  _blob.extractBool();
        
        if (testBandWidth)
        {
            u32 dataSize = _blob.extractUint32();
            //consume the blob
            _blob.extractBinaryBlock(0,dataSize);
        }
        m_clientRemote->send(m_pNetPacketHandler,blob,m_clientRemote->getNetPeer());
    }


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::fileExistsCommand(Blob& _blob)
    {
        u32 requestId = _blob.extractUint32();
        bbool result = _blob.extractBool();

        cbRequest(requestId,result);
    }


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::openFileCommand(Blob& _blob)
    {
        u32 requestId       = _blob.extractUint32();


        //LOG("[remote] openFileCommand : %d ",requestId);
        bbool exist         = _blob.extractBool();

        RequestStatus* pRequestStatus = getRequestStatus(requestId);
        ITF_ASSERT(pRequestStatus);

        if (exist)
        {
            u32 fileSize        = _blob.extractUint32();
            u32 offset          = _blob.extractUint32();
            u32 block_index     = _blob.extractUint32();
            u32 block_count     = _blob.extractUint32();
            u32 block_Length    = _blob.extractUint32();

            u32 dataSize = 0;
            u8* ptr = _blob.extractBinaryBlock(0,dataSize);

            float timeSpent        = _blob.extractFloat32();
            (void)timeSpent;//unused for the moment

            if (block_index == 0) //Start the opening
            {
                pRequestStatus->fileRemote = new FileRemote(fileSize);
            }


            pRequestStatus->fileRemote->writeStream(offset,ptr,block_Length);

            if (block_index == block_count-1) //is it finish ?
            {
                pRequestStatus->resultat = true;
                pRequestStatus->status = 1;

            }


        }
        else
        {
            pRequestStatus->resultat = false;
            pRequestStatus->status = 1;

        }

    }
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    void FileManager_ITF_Remote::connectedCommand(Blob& _blob)
    {
        m_clientRemote->setConnected(btrue);

        int atlasCount = _blob.extractInt32();
        String filename;
        for (int index =0;index<atlasCount;index++)
        {   
            _blob.extractString(filename);
            m_mFastFileExist[filename]= true;
        }

    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::openmapCommand(Blob& _blob)
    {
        String filenameRelative;
        _blob.extractString(filenameRelative);

        {
            csAutoLock cs(m_csPendingMap);
            m_mapPendings.push_back(filenameRelative);
        }

    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::filesChangeCommand(Blob& _blob)
    {
        u32 filesCount = _blob.extractUint32();

        String fileToExtract;
        for (u32 index= 0;index<filesCount;++index)
        {
            _blob.extractString(fileToExtract);
            m_fileWatcher->addFileChange(fileToExtract);

        }


    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    void FileManager_ITF_Remote::informationCommand(Blob& _blob)
    {
        Blob sendtoblob(BLOB_EXTRACT_AND_WRITE_ENABLED);
        sendtoblob.pushString("RemoteInstallation_Plugin");
        sendtoblob.pushString("information");
        const String& depot = FILESERVER->getWorkingDepot();
        String directory;
        directory.setStringFormat("%ls%ls",L"game:\\",depot.cStr()); //CHANGE
        sendtoblob.pushString(directory);
        
        m_clientRemote->send(m_pNetPacketHandler,sendtoblob,m_clientRemote->getNetPeer());
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void FileManager_ITF_Remote::update()
    {
        m_fileWatcher->update();

        {
            csAutoLock cs(m_csPendingMap);
            if(!SCENE_MANAGER->isLoadingPending())
            {
                if(m_mapPendings.size() != 0)
                {
                    const String& fileName = m_mapPendings[0];

                    ITF_VERIFY(GAMEMANAGER->loadGameplayMap(fileName, btrue, bfalse));
                    m_mapPendings.removeAt(0);
                }            
            }
        }
        
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void ClientRemote::init()
    {
        m_CommandBlob["fileexists"]     = &FileManager_ITF_Remote::fileExistsCommand;
        m_CommandBlob["information"]    = &FileManager_ITF_Remote::informationCommand;
        m_CommandBlob["openfile"]       = &FileManager_ITF_Remote::openFileCommand;
        m_CommandBlob["getfilesize"]    = &FileManager_ITF_Remote::getFileSizeCommand;
        m_CommandBlob["askping"]        = &FileManager_ITF_Remote::pingCommand;
        m_CommandBlob["connected"]      = &FileManager_ITF_Remote::connectedCommand;
        m_CommandBlob["fileschanged"]   = &FileManager_ITF_Remote::filesChangeCommand;
        m_CommandBlob["openmap"]        = &FileManager_ITF_Remote::openmapCommand;
        
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    ClientRemote::ClientRemote(FileManager_ITF_Remote* pOwner):m_owner(pOwner),m_peer(NULL),m_connected(bfalse),m_funcCommand(NULL) 
    {
        Synchronize::createCriticalSection(&m_csReceive);
    }
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bbool ClientRemote::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID, bbool _unknowListener)
    {
        csAutoLock cs(m_csReceive);

        String pluginName;
        String command;
        _blob.extractString(pluginName);    

        ITF_ASSERT_MSG(pluginName == "RemoteInstallation_Plugin" ,"we receive a msg not allowed for this client");
        _blob.extractString(command);

        m_peer = _peer;

        ITF_MAP<String,func_CommandBlob> ::iterator iter = m_CommandBlob.find(command);
        if (iter!=m_CommandBlob.end())
        {
            func_CommandBlob func = (*iter).second;
            CALL_FONCTION(*m_owner,func)(_blob);
            return btrue;
        }

        return btrue;
    }

}

#endif // ITF_SUPPORT_FILEMANAGER_REMOTE
