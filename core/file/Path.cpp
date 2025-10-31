#include "precompiled_core.h"

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{

///////////////////////////////////////////////////////////////////////////////////////////
///A string in the dictionary
struct MapEntry
{
    MapEntry() 
    {
        m_refCount = 0;
    }

    MapEntry(const MapEntry &_other) 
    {
        m_content = _other.m_content;
        m_refCount = _other.m_refCount;
    }

#ifdef _DEBUG
    ~MapEntry()
    {
    }

#endif
    PathStringStorage_t m_content;

    i32 m_refCount;
};

///////////////////////////////////////////////////////////////////////////////////////////
///The dictionary
class PathDictionary
{
public:
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// fetch String Entry, create it if needed
    /// @param _path
    /// @return the MapEntry, or NULL if _path is ""
    MapEntry *fetchStringEntry(const PathChar_t *_path)
    {
        if (_path[0]==0)
            return NULL;

        StringID::StringIdValueType val = StringID::StrToCRC(_path);

        m_lock.lock();
        MapEntry *e = getMapEntry_Unsafe(_path, val);
        e->m_refCount++;
        m_lock.unlock();

        return e;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// increment reference count of an entry
    /// @param e
    void referenceEntry(MapEntry *e)
    {
        m_lock.lock();
        ITF_ASSERT_CRASH(e->m_refCount>0, "string is not allocated");
        e->m_refCount++;
        m_lock.unlock();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// decrement reference count of an entry
    /// @param e
    void releaseEntry(MapEntry *e)
    {
        m_lock.lock();
        e->m_refCount--;
        ITF_ASSERT_CRASH(e->m_refCount>=0, "string is not allocated");
        m_lock.unlock();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// DEBUG : compute size of the dictionary
    u32 computeDictionarySize() const
    {
        u32 count=0;
        for (u32 i=0; i<256; i++)
        {
            count += u32(m_entries[i].size());
        }
        return count;
    }

private:

    u8 getHash(StringID::StringIdValueType _id)
    {
        return (u8)(_id + (_id>>8));
    }

    MapEntry *getMapEntry_Unsafe(const PathChar_t *_path, StringID::StringIdValueType _val)
    {
        u8 hash = getHash(_val);

        HashEntry &entry = m_entries[hash];
        HashEntry::iterator iter = entry.find(_val);
        if (iter!=entry.end())
        {
            return &iter->second;
        }
        MapEntry &newMapEntry = entry[_val];
        newMapEntry.m_content = _path;
        newMapEntry.m_refCount = 0;
        return &newMapEntry;
    }

    typedef ITF_MAP<StringID::StringIdValueType,MapEntry> HashEntry;
    HashEntry m_entries[256];
    ITF_THREAD_FASTMUTEX m_lock;
};


static PathDictionary  m_dictionary;


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

Path Path::EmptyPath;

Path::Path()
{
    m_baseName[0]=0;
    m_dirEntry = NULL;
}

Path::Path( const char * _szPath )
{
    m_dirEntry = NULL;
    set(PathString_t(_szPath));
}

Path::Path( const PathString_t& _path )
{
    m_dirEntry = NULL;
    set(_path);
}

Path::Path( const Path& _path )
{
    ITF_MEMCOPY(m_baseName, _path.m_baseName, sizeof(_path.m_baseName));
    m_dirEntry = _path.m_dirEntry;
    if (m_dirEntry)
    {
        m_dictionary.referenceEntry(m_dirEntry);
    }

    m_stringID = _path.m_stringID;
}

Path::~Path()
{
    releaseEntry();
}

void Path::releaseEntry()
{
    if (m_dirEntry)
    {
        m_dictionary.releaseEntry(m_dirEntry);
    }
    m_stringID.invalidate();
    m_dirEntry = NULL;
}

const Path& Path::operator = ( const PathString_t& _path )
{
    releaseEntry();

    set(_path);
    return *this;
}

const Path& Path::operator = ( const char * _szPath )
{
    releaseEntry();

    set(PathString_t(_szPath));
    return *this;
}

const Path& Path::operator = ( const Path& _path )
{
    if (this != &_path)
    {
        releaseEntry();

        ITF_MEMCOPY(m_baseName, _path.m_baseName, sizeof(_path.m_baseName));
        m_dirEntry = _path.m_dirEntry;
        if (m_dirEntry)
        {
            m_dictionary.referenceEntry(m_dirEntry);
        }
        m_stringID = _path.m_stringID;
    }
    return *this;
}

void Path::set( const String& _path )
{
    ITF_ASSERT(m_dirEntry==NULL);
    PathChar_t directory[256];
    m_stringID.invalidate();

    if (FilePath::isAbsolute(_path))
    {
        String path(_path);
        FilePath::normPath(path);
        if(!FilePath::transformToRelative(path))
        {
            ITF_ASSERT_MSG(0, "Path error, not in data root : '%s'", StringToUTF8(_path).get());
        }
        char pathBuffer[512];
        path.charCopyToBuffer(pathBuffer, sizeof(pathBuffer));
        FilePath::splitPathAndNormalize(pathBuffer, directory, m_baseName, maxBasenameLength);
        m_dirEntry = m_dictionary.fetchStringEntry(directory);    
    }
    else
    {
        char pathBuffer[512];
        _path.charCopyToBuffer(pathBuffer, sizeof(pathBuffer));
        FilePath::splitPathAndNormalize(pathBuffer, directory, m_baseName, maxBasenameLength);
        m_dirEntry = m_dictionary.fetchStringEntry(directory);    
    }
}

const StringID &Path::getStringID() const
{
    if (!m_stringID.isValid())
    {
        PathChar_t result[PATH_C_BUFFERSIZE];
        getString(result);
        m_stringID = StringID::StrToCRC(result);
    }
    return m_stringID;
}

const PathChar_t *Path::getDirectory() const
{
    if (m_dirEntry)
        return m_dirEntry->m_content.cStr();
    else
        return (PathChar_t*)PATHSTRING_T("");
}

void Path::getString(PathString_t &_result ) const
{
    PathChar_t result[256];
    getString(result);
    _result = result;
}

void Path::getString(PathString8_t &_result) const
{
    PathChar_t result[256];
    getString(result);
    _result = result;
}


void Path::concatenateStrings(PathChar_t _result[PATH_C_BUFFERSIZE], const PathChar_t _src1[], const PathChar_t _src2[])
{
    PathChar_t *dest = _result;
    PathChar_t *resultEnd = _result+PATH_C_BUFFERSIZE;
    ITF_UNUSED(resultEnd);

    for(;;)
    {
        PathChar_t v=*_src1;
        if (!v)
            break;
        *dest = v;
        _src1++;
        dest++;
    }

    const PathChar_t *src=_src2;
    for(;;) 
    {
        PathChar_t v=*src;
        *dest = v;
        if (!v)
            break;
        src++;
        dest++;
    }
    ITF_ASSERT_CRASH(dest<resultEnd, "path is too long");
}

void Path::getBasenameWithoutExtension(PathString_t &_result ) const
{
    PathCharStdlib_t *found = PATHSTRINGRCHR((PathCharStdlib_t*)m_baseName, L'.');
    if (found)
    {
        _result.setText(m_baseName);
        _result.truncate(u32(found-(PathCharStdlib_t*)m_baseName));
    }
    else
    {
        _result.setText(m_baseName);
    }
}

const PathChar_t* Path::getExtension() const
{
    PathCharStdlib_t *found = PATHSTRINGRCHR((PathCharStdlib_t*)m_baseName, L'.');
    if (found)
        return (const PathChar_t*)found+1;
    return (const PathChar_t*)PATHSTRING_T("");
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Change extension.
void Path::changeExtension(const PathChar_t * extension)
{
    bool hasExtDot = (extension!=0 && extension[0]==PATHSTRING_T('.'));
    if (hasExtDot)
        extension++;
    PathCharStdlib_t * found = PATHSTRINGRCHR(m_baseName, PATHSTRING_T('.'));
    if (found)
    {
        *found = 0; // cut to dot
    }
    if (extension!=0 && extension[0]!=PATHSTRING_T('\0')
        && PATHSTRINGLEN(m_baseName) + 1u + PATHSTRINGLEN(extension) < maxBasenameLength)
    {
        PATHSTRINGCAT(m_baseName, PATHSTRING_T("."));
        PATHSTRINGCAT(m_baseName, extension);
    }

    m_stringID.invalidate();
}


void Path::getString(PathChar_t _result[PATH_C_BUFFERSIZE]) const
{
    const PathChar_t *dir;
    if (m_dirEntry)
        dir = m_dirEntry->m_content.cStr();
    else
        dir = (PathChar_t*)PATHSTRING_T("");

    concatenateStrings(_result, dir, m_baseName);
    
}
void Path::serialize(ArchiveMemory& _archive)
{
    if (_archive.isReading())
    {
        releaseEntry();
        if (m_dirEntry)
        {
            m_dictionary.releaseEntry(m_dirEntry);
        }

        m_dirEntry = NULL;
        
        u32 lenDirectory = 0;
        u32 lenBasename  = 0;

        char directory[PATH_C_BUFFERSIZE];
        _archive.serialize(lenDirectory);
        ITF_ASSERT_CRASH(lenBasename < PATH_C_BUFFERSIZE, "Serialize lenDirectory too big");
        _archive.serializeBlock8((u8*)directory,lenDirectory);
         directory[lenDirectory] = 0;
        m_dirEntry = m_dictionary.fetchStringEntry(directory);   

        _archive.serialize(lenBasename);
        ITF_ASSERT_CRASH(lenBasename < maxBasenameLength, "Serialize base name size too big");
        _archive.serializeBlock8((u8*)m_baseName,lenBasename);
        m_baseName[lenBasename] = 0; 

    #ifdef ITF_DEBUG_STRINGID
        // Assign the string ID with the loaded path.
        // This is only for debug purpose, to give a chance to see the string ID in the debugger.
        // m_stringID.serialize will overwrite it anyway.
        PathChar_t pathStringLoaded[PATH_C_BUFFERSIZE]{};
        getString(pathStringLoaded);
        StringID strIDFromPathStr{pathStringLoaded};
        m_stringID = strIDFromPathStr;
    #endif

        m_stringID.serialize(_archive);

    #ifdef ITF_DEBUG_STRINGID
        // Check coherency between string of path (if existing) and string id.
        ITF_ASSERT(pathStringLoaded[0] == 0 || m_stringID == strIDFromPathStr);
    #endif
    }
    else
    {
        //save the folder
        const PathChar_t *pDirectory  = Path::getDirectory();
        const PathChar_t *pBasename   = Path::getBasename();

        u32 lenDirectory = u32(strlen(pDirectory));
        u32 lenBasename  = u32(strlen(pBasename));

        _archive.serialize(lenDirectory);
        _archive.serializeBlock8((u8*)pDirectory,lenDirectory);

        _archive.serialize(lenBasename);
        _archive.serializeBlock8((u8*)pBasename,lenBasename);

        getStringID();
        m_stringID.serialize(_archive);
    }
}

} // namespace ITF
