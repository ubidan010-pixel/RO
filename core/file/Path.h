#ifndef _ITF_PATH_H_
#define _ITF_PATH_H_

namespace ITF
{
    #define PATHSTRINGCOMPARISON strcmp
    #define PATHSTRINGCHR strchr
    #define PATHSTRINGRCHR strrchr
    #define PATHSTRINGLEN strlen
    #define PATHSTRINGCPY strcpy
    #define PATHSTRINGCAT strcat
    #define PATHSTRING_T(t) t
    typedef char PathChar_t;
    typedef char PathCharStdlib_t;
    typedef String8 PathStringStorage_t;

    typedef String PathString_t; //from API user point of view
    typedef String8 PathString8_t; //from API user point of view

    #define PATH_C_BUFFERSIZE 256

    struct MapEntry;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// The path class. Paths are split into 2 parts : directory and basename.
    /// Directories are stored in a string dictionary. They are normalized automatically, and double slashed are removed.
    class Path
    {
    public:
        static Path EmptyPath;
        static const int maxBasenameLength = 48; //Size, in character, of the basename. Includes the 0 character.

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Empty constructor
        Path();
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Build path from string
        /// @param _path
        Path( const PathString_t& _path );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Build path from const char*
        /// @param _path
        Path( const char * _szPath );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Copy constructor
        /// @param _path
        Path( const Path& _path );
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Destructor
        ~Path();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// operator=
        /// @param _path
        const Path& operator = ( const String& _path );
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// operator=
        /// @param _path
        const Path& operator = ( const Path& _path );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// operator=
        /// @param _path
        const Path& operator = ( const char * _szPath );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// serialization
        /// @param _archive
        void serialize(ArchiveMemory& _archive);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// operator==
        /// @param p
        ITF_INLINE bool operator==(const Path& p) const
        {
            return (m_dirEntry == p.m_dirEntry) && (PATHSTRINGCOMPARISON((PathCharStdlib_t*)m_baseName, (PathCharStdlib_t*)p.m_baseName)==0);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// operator!=
        /// @param p
        ITF_INLINE bool operator!=(const Path& p) const
        {
            return !(operator==(p));
        }

        ITF_INLINE bool operator <( const Path& rhs ) const
        {
            if ( m_dirEntry == rhs.m_dirEntry )
                return PATHSTRINGCOMPARISON( ( PathCharStdlib_t* )m_baseName, ( PathCharStdlib_t* )rhs.m_baseName ) < 0;
            return m_dirEntry < rhs.m_dirEntry;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get string representation. CAUTION : this is not a simple accessor, it may be relatively slow.
        /// @param _result The string which will receive the string
        void getString(PathString_t &_result ) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get string8 representation. CAUTION : this is not a simple accessor, it may be relatively slow.
        /// @param _result The string which will receive the string
        void getString(PathString8_t &_result ) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// same as other getString, but uses a C buffer as output, which is faster
        /// @param result
        void getString(PathChar_t result[PATH_C_BUFFERSIZE]) const;

// #ifndef ITF_FINAL
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the string representation. CAUTION : this is not a simple accessor, it may be relatively slow.
        /// Forbidden in ITF_FINAL!!!
        PathString_t getString() const {PathString_t r; getString(r); return r;}

        PathString8_t getString8() const { PathString8_t r; getString(r); return r; }
// #endif

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// getDirectory part
        const PathChar_t *getDirectory() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get basename
        const PathChar_t *getBasename() const {return m_baseName;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get extension. At least returns empty string. Does not return the starting dot.
        const PathChar_t *getExtension() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Change extension.
        void changeExtension(const PathChar_t * extension);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get basename, without the extension part.
        /// @param _result
        void getBasenameWithoutExtension(PathString_t &_result ) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// empty the path
        void clear() {releaseEntry(); m_baseName[0]=0;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get StringID of the string representation. May compute the string ID if needed.
        const StringID& getStringID() const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if string is empty
        bbool isEmpty() const {return m_dirEntry==NULL && m_baseName[0]==0;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Utility : will with concatenated string (simple concatenation, as strings)
        static void concatenateStrings(PathChar_t _dest[PATH_C_BUFFERSIZE], const PathChar_t _src1[], const PathChar_t _src2[]);

    private:
        void            set( const String& _path );
        void            releaseEntry();

        PathChar_t m_baseName[maxBasenameLength];
        MapEntry *m_dirEntry; //NB : may be NULL if no directory
        mutable StringID m_stringID;

    };
}

#endif // _ITF_PATH_H_
