#ifndef _ITF_FILEPATH_H_
#define _ITF_FILEPATH_H_

namespace ITF
{
    // TODO : make it multi platform
    class FilePath
    {
    public:
        static const char   Separator = '/';

        /// Replace all separator with FilePath::Separator
        /// C:\\TEST\\bla.txt   =>  C:/test/bla.txt
        static String       normalizePath(const String& path);
        static void         normPath(String& path);
        static void         normPath(String8& path);
        static void         normPath(const u16 *_path, u16 _destBuffer[256]);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// @split a normalized version of a path into _destDir and _destBasename. You must check _path length beforehand
        /// @param _path Input path
        /// @param _destDir Destination buffer containing the directory (will include trailing /, or be empty).  Gets rid of double slashes
        /// @param _destBasename Destination buffer containing the basename
        /// @param _destBasenameBufferSize Size, ***IN ITEMS*** of the destination buffer
        static void         splitPathAndNormalize(const u16 *_path, u16 (&_destDir)[256], u16 _destBasename[], u32 _destBasenameBufferSize);
        static void         splitPathAndNormalize(const char *_path, char (&_destDir)[256], char _destBasename[], u32 _destBasenameBufferSize);

        /// @return btrue if the path is normalized
        static bbool        isNormalized(const String& path);

        /// @return btrue if the path is valid
        static bbool        isValid(const String& path);

        /// @return btrue if the given path is a file path (bfalse if it's a directory)
        static bbool        isFilePath(const String& path);

        /// Find the parent directory of the given path
        /// test/sub/               =>  test/
        /// C:/test/sub/            =>  C:/test/
        /// C:/test/sub/bla.txt     =>  C:/test/
        /// @param path the given path
        /// @param parent the parent found (if found)
        /// @return btrue if the parent exists
        static bbool        getParentDirectory(const String& path, String &parent);

        /// Get all parent directories from a path to the root
        /// @param path the base path
        /// @param _emptyList the vector to be filled
        static void         getParentDirList(const String& path, ITF_VECTOR<String> & _emptyList);

        /// return the directory of a path
        /// C:/test/bla.txt     =>  C:/test/
        static String       getDirectory(const String& path);

        /// return the root of a path
        /// C:/test/bla.txt     =>  C:/
        static String       getDrive(const String& path);

        /// return the filename of a path
        /// C:/test/bla.txt     =>  bla.txt
        static String       getFilename(const String& path);


        /// return the filename of a path without extension
        /// C:/test/bla.txt     =>  bla
        static String       getFilenameWithoutExtension(const String& path);

        /// return the extension of a filepath
        /// C:/test/bla.txt     =>  txt
        static String       getExtension(const String& path);

        /// Transform an absolute path into relative.
        /// we support lowercase/uppercase, but it must be normalized with slash instead of backslash
        /// Return true if relative path is set
        static bbool        transformToRelative( String& _path );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if path is absolute
        /// @param _path The path to test
        static bbool        isAbsolute(const String& _path);

        /// return new string with extension changed
        static String       getChangedExtension(const String & path, const String & _newExtension);

        /// Change path extension
        static void         changeExtension(String & path, const String & _newExtension);
        /// Change path extension
        static void         changeExtension(String8 & path, const char * _newExtension);
    };
}

#endif // _ITF_FILEPATH_H_
