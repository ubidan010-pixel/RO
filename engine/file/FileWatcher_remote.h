#ifndef _ITF_FILEWATCHER_REMOTE_H_
#define _ITF_FILEWATCHER_REMOTE_H_



namespace ITF
{

    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    class WatchDirectoryListener;

    //Special class to handle file changed on the remote model
    //Supported only modified file
    class RemoteFileEventFilter
    {
        public:
            RemoteFileEventFilter(WatchDirectoryListener* _pListener,const String& _szFilter):m_pListener(_pListener),m_szFilter(_szFilter)
            {
    
            }

            void executeIfMatch(const String& _szFilter,const String& _szFile) const;

        protected:
            WatchDirectoryListener* m_pListener;
            String                  m_szFilter;

    };

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    class FileWatcher_Remote
    {
        public:
            FileWatcher_Remote();
            ~FileWatcher_Remote();

            void registerWatch(WatchDirectoryListener* _pListener,const String& _szFilter);

            void update();

            void addFileChange(const String& _file);

        protected:
            ITF_VECTOR<RemoteFileEventFilter*>  m_vRemoteFileEventFilter;

            ITF_VECTOR<String>                  m_FilesChanged;

    };

}

#endif //_ITF_FILEWATCHER_REMOTE_H_