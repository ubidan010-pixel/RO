#ifndef _ITF_FILEWATCHER_WIN_H_
#define _ITF_FILEWATCHER_WIN_H_

#include "core/system/Synchronize.h"

namespace ITF
{

///----------------------------------------------------------------------------------------------------------------

    class WatchDirectoryListener;

   struct WatchFileEvent
   {
        #define ITF_WatchFileEvent_defaultDelay 1000.f      // milliseconds
       WatchFileEvent(const String& _szFilename,u32 _uAction) {m_timer = 0;m_szFilename = _szFilename;m_dwAction = _uAction;}
       u32     m_dwAction;
       String  m_szFilename;
       unsigned long m_timer;
   };

///----------------------------------------------------------------------------------------------------------------

   /// Directory watcher for a specific folder
   class WatchDirectory
   {
   public:

       /// @fn  WatchDirectory(const String& _szPath,WatchDirectoryListener* _pListener,u32 _index)
       /// @brief   Constructor. 
       /// @param   _szDirectoryPath    Directory path name 
       /// @param   _szFilter  
       /// @param [in]  _pListener      If non-null, the listener. 
       /// @param   _index              Unique Id counter. 
       WatchDirectory(const String& _szDirectoryPath,const String& _szFilter, WatchDirectoryListener* _pListener,u32 _index);

       /// @fn  ~WatchDirectory()
       /// @brief   Destructor. 
       ~WatchDirectory();

       /// @fn  void process()
       /// @brief   Get the files updated for this given watcher. 
       void process();

       /// @fn  void update()
       /// @brief   poll all the listener. 
       void update();

       /// @fn  const String& getPath() const
       /// @brief   Gets the path. 
       /// @return  The directory path. 
       const String& getPath() const {return m_szDirName;}

       /// @fn  const ITF_THREAD_EVENT getEvent() const
       /// @brief   Gets the event. 
       /// @return  The event associate to this directory watcher. 
       const ITF_THREAD_EVENT getEvent() const {return m_hEvent;}

       /// @fn  void addListener(WatchDirectoryListener* _pListener)
       /// @brief   Adds a listener to this directory watcher,avoid a duplication of the watchDirectory for the same directory. 
       /// @param [in]  _pListener  If non-null, the listener. 
       void addListener(WatchDirectoryListener* _pListener);

       void addEvent(WatchFileEvent& _evt);
   private:

       HANDLE       m_hDirectory;
       u32          m_uBufferSize;
       DWORD        m_dwflags_notification;
       String       m_szDirName;

       PFILE_NOTIFY_INFORMATION pNotifyBuffer;

       OVERLAPPED   m_OverLap;

       ITF_THREAD_CRITICAL_SECTION  m_cs;
       ITF_THREAD_EVENT             m_hEvent;

       ITF_VECTOR<WatchFileEvent>   m_vWatchEventList;

       //listeners;
       ITF_VECTOR<WatchDirectoryListener*> m_pListeners;
   };

///----------------------------------------------------------------------------------------------------------------

   class Thread;

   ///Manager to handle directory notification
   class FileWatcher
   {
   public:

       /// @fn  FileWatcher()
       /// @brief   Default constructor. 
       FileWatcher():m_uEventCount(0),m_Events(0),m_bExitReceiveThread(bfalse),m_bInit(bfalse),m_bDisable(bfalse),m_uEventIncrement(0), m_pThread(NULL) {};

       /// @fn  virtual ~FileWatcher()
       /// @brief   Destructor. 
       virtual ~FileWatcher();

       /// @fn  void registerWatch(const String& _path,WatchDirectoryListener* _pListener)
       /// @brief   Register a new directory watcher. 
       /// @param   _path               Full pathname of the directory. 
       /// @param [in]  _pListener  If non-null, the listener. 
       void registerWatch(const String& _szPath,WatchDirectoryListener* _pListener,const String& _szFilter);

       /// @fn  void update()
       /// @brief   Updates all directory watcher registred. 
       void update();

       /// @fn  void init()
       /// @brief   Initialize the file watcher manager. 
       void init();

       /// @fn  void process()
       /// @brief   Process all notification. 
       void process();

       void setDisable(bbool _bDisable) {m_bDisable = _bDisable;}

   protected:

       /// @fn  WatchDirectory* getWatchDirectories(const ITF_THREAD_EVENT _theevent)
       /// @brief   Gets a watch directories from an event. 
       /// @param   _theevent event you want to retrieve watch directories
       /// @return  null if it fails, else the watch directories. 
       WatchDirectory* getWatchDirectories(const ITF_THREAD_EVENT _theEvent);

       /// @fn  void addEvents(WatchDirectory* _pWatchDir)
       /// @brief   Adds an event from a watch directory and build a new list. 
       /// @param [in]  _pWatchDir  If non-null, the watch dir. 
       void addEvents(WatchDirectory* _pWatchDir);

       /// @fn  void wakeup()
       /// @brief   Wakeups the thread to notify a new event insertion. 
       void wakeup();

       /// @fn  void buildEvents()
       /// @brief   Builds the events list. 
       void buildEvents();

       /// @fn  void destroy()
       /// @brief   Destroys this object. 
       void destroy();


       volatile bbool                  m_bExitReceiveThread;
       bbool                           m_bInit;
       bbool                           m_bDisable;

       ITF_THREAD_CRITICAL_SECTION     m_csEvent;

       ITF_VECTOR<ITF_THREAD_EVENT>    m_vEventUsed;
       ITF_VECTOR<WatchDirectory*>     m_vWatchDirectory;
       ITF_THREAD_EVENT                m_SpecialEvent;
       ITF_THREAD_EVENT*               m_Events;//dynamic array of event
       u32                             m_uEventCount;
       u32                             m_uEventIncrement;
       Thread*                         m_pThread;
   };
///----------------------------------------------------------------------------------------------------------------
}

#endif //_ITF_FILEWATCHER_WIN_H_
