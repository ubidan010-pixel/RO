#ifndef _ITF_SCADAPTER_H_
#define _ITF_SCADAPTER_H_


//Source Control Adapter

#ifndef ITF_IPAD	//TODO_iPad do we need this?
namespace ITF
{
    class ProcessSpawner;
    class Thread;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Source control request info class
    ///
    class SCRequestInfo
    {
    public:
        SCRequestInfo():m_result(0) {};

        void            setOutputDetails(const String& str) {m_ouputDetails = str;}
        const String&   getOutputDetails()const             {return m_ouputDetails;}
        const u32       getResult() const   {return m_result;}
        void            setResult(u32 v)    {m_result=v;}        

    protected:
        String  m_ouputDetails;
        u32     m_result;
    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Source control request id class
    ///
    class SCRequestID
    {
        private:
            u64        m_value;

        public:
            SCRequestID():m_value(0) {};
            SCRequestID(const u64 v):m_value(v) {};

            ITF_INLINE const u64    getValue() const {return m_value;}
            
            void        operator ++ ();
            bool        operator == ( const SCRequestID& o    ) const;
            bool        operator == ( const SCRequestID* o    ) const;
            bool        operator <  ( const SCRequestID& o    ) const;

    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Source control request class
    ///

    class SCRequest
    {
        public:
            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Constructor Source Control Request
            ///
            SCRequest(const String& appName,const String& commandLine,const String& logFile):
              m_processSpawner(NULL),m_appName(appName),m_logFile(logFile),m_commandLine(commandLine),
                  m_result(0),m_bDone(bfalse) {};

              //////////////////////////////////////////////////////////////////////////
              ///
              ///  Destructor Source Control Request
              ///
              virtual ~SCRequest();

              //////////////////////////////////////////////////////////////////////////
              ///
              /// Execute the request
              ///
              void   execute();

              //////////////////////////////////////////////////////////////////////////
              ///
              ///  Set a SCRequestID 
              ///  @param id request ID
              ///
              void   setId(SCRequestID id)                  {m_Id = id;}


              //////////////////////////////////////////////////////////////////////////
              ///
              /// Get the request ID
              /// @return a SCRequestID
              ///
              const SCRequestID      getId()                    const  {return m_Id;}


              //////////////////////////////////////////////////////////////////////////
              ///
              /// Get the result of process execution
              /// @return a u32 ,0 for success 
              ///
              const u32              getResult()                const  {return m_result;}

              //////////////////////////////////////////////////////////////////////////
              ///
              /// Get the result of process execution
              /// @return a ProcessSpawner
              ///
              const ProcessSpawner*     getProcessSpawner()     const  {return m_processSpawner;}

        protected:
            
            bbool       m_bDone;
            u32         m_result;
            SCRequestID m_Id;

            ProcessSpawner*     m_processSpawner;
            String              m_appName;
            String              m_logFile;
            String              m_commandLine;
            
    };

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Source control manipulation class
    ///


    class SCAdapter
    {
        public:
            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Constructor Source Control Adapter
            ///
            SCAdapter():m_RunningThread(NULL) {};

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Destructor Source Control Adapter
            ///
            virtual ~SCAdapter();

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Initialize the source control adapter and run the update thread
            /// @param applicationName the exe appname to control the source control commands
            /// @param url Source control server
            /// @param depotFolder Local depot directory
            /// @param logFile local filename for commands result output
            ///
            void init(const String& applicationName,const String& url, u32 port, const String& username, const String& userpass, const String& depotFolder, const String& logFile);

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Get the server base URL
            ///
            const String& getUrl() {return m_Url;}

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Get the depot folder
            ///
            const String& getDepotFolder() {return m_DepotFolder;}

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Process the pending source control request done in a thread
            ///
            void process_request();

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Create a local working copy from a (remote) repository 
            ///
            virtual SCRequestID checkout() = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Mark specified file to be added to repository at next commit
            /// @param path path you want to add to the source control
            ///
            virtual SCRequestID add(const String& path)  = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Mark specified file to be removed from repository at next commit (note: keeps revision history)
            /// @param path path you want to remove from the source control
            ///
            virtual SCRequestID remove(const String& path) = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Mark specified files to be added to repository at next commit 
            /// @param paths list of path you want to add to the source control
            ///
            virtual SCRequestID add(const ITF_VECTOR<String> &paths) = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Mark specified files to be removed from repository at next commit (note: keeps revision history)
            /// @param path list of paths you want to remove from the source control
            ///
            virtual SCRequestID remove(const ITF_VECTOR<String> &paths) = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Update the files in a working copy with the latest version from a repository 
            ///
            virtual SCRequestID update() = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Send changes from your working copy to the (local or remote) repository
            ///  @param _paths the list of path to be committed
            ///  @param name of the user that commit
            ///
            virtual SCRequestID commit(const ITF_VECTOR<String>& _paths, const String& user) = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Get the last revision (non blocking)
            ///  @return the request ID
            ///
            virtual SCRequestID getLastRevision_Prepare() = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            ///  Grab the last revision (when non blocking request is over)
            ///  @param _requestInfo the request info
            ///  @return the last revision, U32_INVALID if unavailable
            ///
            virtual u32 getLastRevision_Finish(const SCRequestInfo& _requestInfo) = 0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Ask for a list of needed actions to reflect file status between two revision number
            /// If _revisionStart not provided then start from first revision
            /// If _revisionEnd not provided then end to HEAD revision
            /// @param _revisionStart the revision number start
            /// @param _revisionEnd the revision number end
            /// @return the latest revision
            ///
            virtual SCRequestID logChanges_Prepare(u32 _revisionStart = 0, u32 _revisionEnd = 0)=0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Ask for a list of needed actions to reflect file status between two revision number
            /// @param _changes the result map
            /// @param _requestInfo the request result
            /// @return the latest revision
            ///
            virtual u32 logChanges_Finish(ITF_MAP<String, char> &_changes, const SCRequestInfo& _requestInfo)=0;

            //////////////////////////////////////////////////////////////////////////
            ///
            /// poll the source control request
            /// @param id request id
            /// @param rqInfo return the request information for the specific request
            /// @return a bool result ;btrue if the request has been proceeded ,bfalse in other case
            ///
            bbool   pollRequest(const SCRequestID& id,SCRequestInfo& rqInfo);

         
    protected:

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Exit the SCAdapter thread
            ///
            void    exit()  {m_bExit = btrue;}

            //////////////////////////////////////////////////////////////////////////
            ///
            /// Add source control request to the pending request list
            /// @param request request you want to add
            ///
            SCRequestID   add_request(SCRequest* request);


            //////////////////////////////////////////////////////////////////////////
            ///
            /// Build a command list with unknown char* parameters
            ///
            SCRequestID prepare(char* first,... );

            SCRequestID     m_currentRequestID;
            volatile bbool   m_bExit;

            Thread* m_RunningThread;

            String  m_ApplicationName;
            String  m_Url;
            u32     m_Port;
            String  m_DepotFolder;
            String  m_logFile;
            String  m_Username;
            String  m_Userpass;

            ITF_MAP<SCRequestID,SCRequest*> m_mResquestDone;
            ITF_VECTOR<SCRequest*> m_vRequests;

    };


} // namespace ITF

#endif //ITF_IPAD

#endif //_ITF_SCADAPTER_H_