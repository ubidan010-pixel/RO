#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#define _ITF_WATCHDIRECTORYLISTENER_H

namespace ITF
{


///Any classes that want a directory must herit or use a component form WatchDirectoryListener
class WatchDirectoryListener
{
public:

    /// @fn WatchDirectoryListener()
    /// @brief  Default constructor. 
    WatchDirectoryListener() {};

    /// @fn virtual ~WatchDirectoryListener()
    /// @brief  Destructor. 
    virtual ~WatchDirectoryListener() {};

    /// @fn virtual void onNewFile(const String& _absoluteFilename)
    /// @brief  Executes the new file action. 
    /// @param  _absoluteFilename    Filename of the size file. 
    virtual void onNewFile(const String& _absoluteFilename)       {};

    /// @fn virtual void onDeleteFile(const String& _absoluteFilename)
    /// @brief  Executes the delete file action. 
    /// @param  _absoluteFilename    Filename of the size file. 
    virtual void onDeleteFile(const String& _absoluteFilename);

    /// @fn virtual void onModifiedFile(const String& _absoluteFilename)
    /// @brief  Executes the modified file action. 
    /// @param  _absoluteFilename    Filename of the size file. 
    virtual void onModifiedFile(const String& _absoluteFilename)  {};

    const String& getFilter(u32 _index)                     {return m_szFilters[_index];}
    u32  getFilterCount()                                   {return m_szFilters.size();}
    void addFilter(const String& _szFilter)                 {m_szFilters.push_back(_szFilter);}

    void addTracing(const String& _filename);

private:
    ITF_VECTOR<String>       m_szFilters;
};

#endif //_ITF_WATCHDIRECTORYLISTENER_H

}