#ifndef _ITF_FILETRACKER_H_
#define _ITF_FILETRACKER_H_

#if defined (ITF_CONSOLE) || defined (ITF_FINAL) || defined(ITF_IPAD) || defined(ITF_CTR)
#define RECORDFILE(file) {}
#define FILETRACKERSCOPE(name) {}
#else
//#define RECORDFILE(file) FileTracker::recordFile(file);
//#define FILETRACKERSCOPE(name) FileTrackerScope  __LINE__##__FUNCTION_(name);
#define RECORDFILE(file) {}
#define FILETRACKERSCOPE(name) {}
#endif //ITF_FINAL

//track file loading inside a ITF thread
#ifdef ITF_WINDOWS

namespace ITF
{

class FileTracker
{
public:
    FileTracker(const String& _szName);
    ~FileTracker();

    void addFile(const String& _szFile);

    static void init(const String& _szName);
    static void destroy();
    static void recordFile(const String& _szfilename);
            
    static ThreadLocalStorage m_TLSInstance;


protected:

    static FileTracker* getInstance() {return static_cast<FileTracker*>(m_TLSInstance.pGetPtrValue());}

    String   m_szName;
    ITF_VECTOR<String> *m_FileList;
    u32     m_Hierarchy;
    f64     m_startTime;

};


class FileTrackerScope
{
public:
    FileTrackerScope(const String& _szName);
    ~FileTrackerScope();
};

}

#endif //ITF_WINDOWS
#endif //_ITF_FILETRACKER_H_

