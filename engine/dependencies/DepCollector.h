#ifndef _ITF_DEP_COLLECTOR_H_
#define _ITF_DEP_COLLECTOR_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{

class File;

struct DepStats
{
    DepStats();

    u32 getAverage() const;

    u32 total;
    u32 count;
    u32 worst;
    u32 best;
};

class DepCollection
{
    friend class DepCollector;

private:
    struct RawPath
    {
        RawPath();
        RawPath( const String& path );

        u16 path[_MAX_PATH];

        bool operator <( const RawPath& rhs ) const;
    };

public:
    typedef ITF_SET< RawPath >          Collection;
    typedef Collection::iterator        iterator;
    typedef Collection::const_iterator  const_iterator;
    typedef Collection::size_type       size_type;

    class NETIterator
    {
    public:
        NETIterator( const Collection& container );

        bool haveNext() const;
        void get( String& filename ) const;
        void next();

    private:
        NETIterator& operator =( const NETIterator& rhs );

        const Collection&           _container;
        Collection::const_iterator  _it;
    };

    bool load( const String& cacheKey, const String& filename, const String& platform );
    void save( const String& cacheKey, const String& filename, const String& platform );

    bool add( const String& filename );
    bool remove( const String& filename );

    size_type size() const;

    bool contains( const String& filename ) const;

    void clear();

    NETIterator getNetIterator() const;

    iterator        begin();
    const_iterator  begin() const;
    iterator        end();
    const_iterator  end() const;

    void initFilters();
    void initFilters( const char** filters );
    void destroyFilters();

    bool filterFile( const String& filename ) const;
    void filterExt();

    static String getDependencyFilenameFrom( const String& filename, const String& platform );

private:
    Collection                  m_col;
    mutable String              m_swap;
    ITF_SET< String >           m_extFilters;
};

class DepCollector
{
public:
    typedef ITF_MAP< String, DepStats > StatMap;

    DepCollector();
    ~DepCollector();

    bool contains( const String& filename ) const;

    void clear();

    bool add( const String& filename );
    void add( const DepCollection& col );

    bool getNextProcessing( String& filename );

    void addStat( const String& filename, u32 timer );

    StatMap&        getStats();
    const StatMap&  getStats() const;

    const char**    getFilters() const;
    void            setDefaultFilters();
    void            setFilters( const char** filters );
    void            resetFilters();

    DepCollection&          getProcessed();
    const DepCollection&    getProcessed() const;

    DepCollection&          getPendings();
    const DepCollection&    getPendings() const;

private:
    mutable ITF_THREAD_CRITICAL_SECTION     m_cs;
    DepCollection                           m_processed;
    DepCollection                           m_pendings;
    ITF_VECTOR< DepCollection::RawPath >    m_diff;
    StatMap                                 m_stats;
    const char**                            m_filters;
};

}

#endif  // _ITF_DEP_COLLECTOR_H_
