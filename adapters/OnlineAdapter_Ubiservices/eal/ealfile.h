////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// \file ealfile.h  Engine Abstraction Layer File Reference API
//
// For any questions/feedback please email World-EALCommittee@ubisoft.com.
//
// See the ealdef.h file for details on how to use the various EAL APIs.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ealdef.h"
/// This module covers the file reference API.
/*! \addtogroup File
    @{
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface version.
/// The Format is an integer value equal to Major.Minor multiplied by 100 (Version 2.10 = 210)
/// See ealdef.h for more information.
#define EAL_FILE_VERSION             505

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void                    eal_file;           ///< File handle
typedef eal_u32                 eal_file_flags;     ///< File open flags

////////////////////////////////////////////////////////////////////////////////////////////////////
/// File open flags.
enum eal_file_mode
{
    EAL_FILE_READ               = 0x00000001,       ///< Open a file for reading.
    EAL_FILE_WRITE              = 0x00000002,       ///< Open a file for writing.
    EAL_FILE_CREATE             = 0x00000004,       ///< Create the file if it does not exist, truncate if it does.
    EAL_FILE_SHARED             = 0x00000008,       ///< Enable file sharing.
    EAL_FILE_RANDOM             EAL_DEPRECATED("Deprecated; there is no replacement")
                                = 0x00000010,       ///< Open file with random access. \deprecated DEPRECATED since version 2.00 (random access is always assumed). Will be removed in version 5.00.
    EAL_FILE_ASYNC              = 0x00000020,       ///< Open with asynchronous access.

    EAL_FILE_MODE_LAST          = EAL_FILE_ASYNC
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  The priority of a read/write operation.

    Priority is essentially linked with the caller's sensitivity to latency.
    High priority should be reserved for highly sensitive application like audio & video streaming
    Normal priority is intended for majority of data loading
    Low priority is only used for unessential data which can be loaded when anything else is needed.
*/
enum eal_file_priority
{
    EAL_FILE_PRIORITY_LOW       = 0,                ///< Low priority for background loading
    EAL_FILE_PRIORITY_NORMAL    = 1,                ///< Normal priority for usual data loading
    EAL_FILE_PRIORITY_HIGH      = 2                 ///< High priority for multimedia streams
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Status of an operation (error)
enum eal_file_status
{
    EAL_FERR_OK                 =   0,              ///< Success
    EAL_FERR_TIMEOUT            =   1,              ///< Operation timed out
    EAL_FERR_FILE_NOT_FOUND     =   2,              ///< File not found; can also mean that the path given to \ref EalFileOpen cannot be resolved (e.g. inexisting subdirectories)
    EAL_FERR_READ_ONLY          EAL_DEPRECATED("Deprecated; use EAL_FERR_ACCESS_VIOLATION instead")
                                =   3,              ///< File is read-only \deprecated DEPRECATED since version 3.00 (use EAL_FERR_ACCESS_VIOLATION instead), will be removed in version 6.00
    EAL_FERR_WRITE_ONLY
                                EAL_DEPRECATED("Deprecated; use EAL_FERR_ACCESS_VIOLATION instead")
                                =   4,              ///< File is write-only \deprecated DEPRECATED since version 3.00 (use EAL_FERR_ACCESS_VIOLATION instead), will be removed in version 6.00
    EAL_FERR_SHARING_VIOLATION  =   5,              ///< The file is already used by another process
    EAL_FERR_STREAM_ONLY        =   6,              ///< File cannot be opened in random access
    EAL_FERR_LOW_LEVEL_ERROR    =   7,              ///< Unspecified system or hardware error \warning CHANGED in version 3.00 (only use EAL_FERR_LOW_LEVEL_ERROR if none of the more precise status codes apply)
    EAL_FERR_EOF                =   8,              ///< File operation with a starting position at or beyond the end of file

    // New since version 3.0
    EAL_FERR_BAD_PATH           =  20,              ///< Given file or directory path is invalid (ill-formatted)
    EAL_FERR_PATH_NOT_FOUND     =  21,              ///< Given file or directory path was not found (but was correctly formatted)
    EAL_FERR_INVALID_PARAMETER  =  22,              ///< A parameter was incorrect (e.g. null pointer)
    EAL_FERR_INVALID_HANDLE     =  23,              ///< The given eal_file* pointer is null or invalid (e.g. closed or wrong type)
    EAL_FERR_BUFFER_TOO_SMALL   =  24,              ///< Buffer passed to call was too small
    EAL_FERR_ALREADY_EXISTS     =  50,              ///< File or directory already exists
    EAL_FERR_ACCESS_DENIED      =  51,              ///< No permission to access the file or directory
    EAL_FERR_ACCESS_VIOLATION   =  52,              ///< Writing to a read-only file, or reading from a write-only file
    EAL_FERR_DEVICE_FULL        =  53,              ///< Not enough space on storage device
    EAL_FERR_TOO_MANY_OPEN_FILES=  54,              ///< Too many files are open
    EAL_FERR_DEVICE_NOT_READY   =  55,              ///< Storage device cannot be used right now (ex: tray was open, external drive was disconnected)
    EAL_FERR_NOT_SUPPORTED      =  56,              ///< Operation is not supported on this device
    EAL_FERR_IMPL_ERROR         = 100,              ///< Unspecified error that is specific to the EAL implementation
    EAL_FERR_NOT_IMPLEMENTED    = 101,              ///< The EAL implementation is not provided
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Format of an asynchronous read or write callback.
    \param pData    A pointer to the data read. For a write operation, will be null.
    \param Size     Size of the data read or written in bytes.
                    Some conditions (e.g. EOF, device full) can cause this value to be smaller than requested.
                    Will be 0 if an error completely prevented the write operation.
                    Always verify 'Status' if this value is not equal to the requested size.
    \param pCookie  An opaque pointer provided by the caller.
                    Unfortunately this is the only way to pass the original
                    requested size for correct operation handling and meaningful
                    error reports.
    \param Status   The status of the operation. EAL_FERR_OK when read is successful,
                    or an error code otherwise.
    \sa EalFileAsyncRead, eal_file_status
*/
typedef void (*eal_async_cb)(void*           pData,
                             eal_u32         Size,
                             void*           pCookie,
                             eal_file_status Status);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Flags used to categorize file system directory entries.
    \sa EalFileVisitDirectoryEntries, eal_directory_entry_cb, eal_directory_entry
*/
enum eal_directory_entry_flags
{
    EAL_DIRENTRY_FLAGS_DIRECTORY = 0x00000001, // The entry is a directory (otherwise the entry is a regular file).
};

/*!  The description of a file system directory entry.
    \sa EalFileVisitDirectoryEntries, eal_directory_entry_cb, eal_directory_entry_flags
*/
struct eal_directory_entry
{
    const char* pName; // The name of the entry, relative to its parent directory (i.e. it is not an absolute path).
    eal_u32 Flags;     // A combination of flags from \ref eal_directory_entry_flags
};

/*!  Format of a directory visit callback.
    \param Entry    The description of the visited file system directory entry.
    \param pCookie  The user-defined value that was passed to \ref EalFileVisitDirectoryEntries.
    \return         If 'true' is returned, the visit continues; otherwise the visit stops immediately.
    \sa EalFileVisitDirectoryEntries, eal_directory_entry
*/
typedef bool (*eal_directory_entry_cb)(const eal_directory_entry& Entry, void* pCookie);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Initialize the library.

    \return true if initialization succeeded

    \param Version      Must be equal to current version = \ref EAL_FILE_VERSION.

    \PRECONDITIONS
    - Must be called before any other API call.
    - The client must keep track of the number of calls to \ref EalFileLibInit. so
      that \ref EalFileLibShutdown is called the exact number of times.
    - Implementation version must be no more than 3 versions higher.

    \POSTCONDITIONS
    - Multi-thread safe calls.
    - The library will be initialized and all API calls will be allowed.
    - \ref EalFileLibShutdown must be called even if 'false' is returned.

    \sa EalFileLibShutdown
*/
EAL_DLL_ENTRY bool EalFileLibInit(eal_u32 Version = EAL_FILE_VERSION);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Shut down the library after usage.
    \PRECONDITIONS
    - Must be called once for every call to \ref EalFileLibInit.

    \POSTCONDITIONS
    - Multi-thread safe calls.
    - You are not allowed to call any other library member after calling \ref EalFileLibShutdown.

    \sa EalFileLibInit
*/
EAL_DLL_ENTRY void EalFileLibShutdown();

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Open a file.
    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pFileName    A string containing the filename. The filename is platform dependent.
                        This can be a path with subdirectories but the subdirectories must exist prior to the call; they will not be created even if \ref EAL_FILE_CREATE is used.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \param OpenFlags    Can be any combination of flags from \ref eal_file_mode.
    \param [out] Status \ref EAL_FERR_OK on success, or an error code from \ref eal_file_status.
    \return             A non-null pointer to a file if the operation succeeds.

    \PRECONDITIONS
    - pFileName must point to a valid string.
    - All subdirectories specified in pFileName must exist; this call will not create them (use \ref EalFileExistsWithStatus and \ref EalFileCreateDirectory).

    \POSTCONDITIONS
    - Multi-thread safe calls.
    - If the function returns a non-null value, the file is opened.

    \sa EalFileGetSeparator, EalFileClose, EalFileRead, EalFileWrite, eal_file, eal_file_status, EalFileExistsWithStatus, EalFileCreateDirectory
*/
EAL_DLL_ENTRY eal_file* EalFileOpen(eal_id           Tag,
                                    const char*      pFileName,
                                    eal_file_flags   OpenFlags,
                                    eal_file_status& Status);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Close a previously opened file.
    \param pFile    Pointer to a valid file previously opened with \ref EalFileOpen.

    \PRECONDITIONS
    - pFile must be valid
    - If \ref EalFileAsyncRead or \ref EalFileAsyncWrite was called, you must not close the file until the callback is processed.

    \POSTCONDITIONS
    - This function DOES NOT need to be a multithread safe call for a singular pFile.
    - File will be closed. The file handle is now invalid and should not be used.

    \sa EalFileOpen, eal_file, eal_file_status
*/
EAL_DLL_ENTRY void EalFileClose(eal_file* pFile);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Verify if a file exists
    \deprecated DEPRECATED since version 3.00. Please use \ref EalFileExistsWithStatus instead.
    Client should avoid using this version ASAP. This function will be removed in version 6.00.
    Implementer must still support this function until version 6.00.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pPath        A string containing the file or directory path. The formatting of the path is platform dependent.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \return             True if the file was found, or false otherwise.

    \PRECONDITIONS
    - pPath must point to a valid string.

    \POSTCONDITIONS
    - Multithread safe call

    \sa EalFileExistsWithStatus, EalFileGetSeparator, EalFileOpen, eal_file
*/
EAL_DEPRECATED("Deprecated; use EalFileExistsWithStatus instead")
EAL_DLL_ENTRY bool EalFileExists(eal_id Tag, const char* pPath);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Verifies if a file or directory path exists.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pPath        A string containing the file or directory path. The formatting of the path is platform dependent.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \return             \ref EAL_FERR_OK if the file was found, \ref EAL_FERR_PATH_NOT_FOUND otherwise, or an error code from \ref eal_file_status.

    \PRECONDITIONS
    - pPath must point to a valid string.

    \POSTCONDITIONS
    - Multithread safe call

    \sa EalFileGetSeparator, eal_id, eal_file_status
*/
EAL_DLL_ENTRY eal_file_status EalFileExistsWithStatus(eal_id Tag,
                                                      const char* pPath);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Read data from a file, at the current position.
    \param pFile        Pointer to a valid file previously opened with \ref EalFileOpen.
    \param pBuffer      A pointer to a non-null buffer where the data will be written.
    \param Position     The position in the file.
    \param BufferSize   The size of the data to read.
    \param [out] ReadSize   Will return the number of bytes read. Will be 0 if an error occurs.
    \param Priority Priority of the read request. See \ref eal_file_priority.

    \return \ref EAL_FERR_OK if the operation succeeded, or an error code.

    \PRECONDITIONS
    - pFile must be valid, and open using the \ref EAL_FILE_READ flag
    - pBuffer must point to a valid buffer.
    - BufferSize > 0

    \POSTCONDITIONS
    - This function DOES NOT need to be a multi thread safe call for a singular pFile.
    - Will return the number of bytes read. ReadSize value will be the minimum between
      BufferSize and the size of the file minus the current position.

    \sa EalFileOpen, EalFileWrite, eal_file
*/
EAL_DLL_ENTRY eal_file_status EalFileRead(eal_file*           pFile,
                                          void*               pBuffer,
                                          const eal_u64&      Position,
                                          eal_u32             BufferSize,
                                          eal_u32&            ReadSize,
                                          eal_file_priority   Priority);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Queues an asynchronous read operation.
    When the operation is completed or an error occurs during the operation,
    the provided callback function is called.

    \param pFile        A pointer to a valid file previously opened with \ref EalFileOpen.
    \param pBuffer      A buffer to read the data in
    \param Size         The size of the buffer (size that will be read unless reaching the EOF).
    \param Pos          The position in the file whence to read.
    \param Callback     A valid callback function to call when reads completes.
    \param pCookie      A cookie; a pointer that will be given back to you in the callback.
    \param Priority     Priority of the Read request. See \ref eal_file_priority.

    \return \ref EAL_FERR_OK if the queuing of the operation succeeded, or an error
            code from \ref eal_file_status otherwise. Note this is not the
            status of the operation itself; that one is returned in the callback.

    \PRECONDITIONS
    - Multithread safe (many different threads can call \ref EalFileAsyncRead at the same time).
    - pFile must be valid, opened with the \ref EAL_FILE_READ | \ref EAL_FILE_ASYNC flags.
    - pBuffer must be aligned on a multiple of \ref EalFileGetBufferAlignment.
    - Size must be a multiple of \ref EalFileGetSectorSize.
    - Pos must be a multiple of \ref EalFileGetSectorSize.

    \POSTCONDITIONS
    - You must not free the pBuffer before the callback is called.

    \sa EalFileOpen, EalFileRead, eal_file, eal_async_cb, eal_file_status
*/
EAL_DLL_ENTRY eal_file_status EalFileAsyncRead(eal_file*         pFile,
                                               void*             pBuffer,
                                               eal_u32           Size,
                                               const eal_u64&    Pos,
                                               eal_async_cb      Callback,
                                               void*             pCookie,
                                               eal_file_priority Priority);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Write data to a file.
    \param pFile        Pointer to a valid file previously opened with \ref EalFileOpen.
    \param pBuffer      A pointer to a non-null buffer containing the data to be written to the file.
    \param Position     The position in the file.
    \param BufferSize   The size of the data to write.
    \param [out] WrittenSize Will return the number of bytes written. Will be 0 if an error occurs.
    \return             \ref EAL_FERR_OK if the operation succeeded, or an error code.

    \PRECONDITIONS
    - pFile must be valid and then open using the \ref EAL_FILE_WRITE flag.
    - pBuffer must point to a valid buffer.
    - BufferSize > 0

    \POSTCONDITIONS
    - This function DOES NOT need to be a multi thread safe call for a singular pFile.
    - WrittenSize will contain the number of bytes written.

    \sa EalFileOpen, EalFileRead, eal_file
*/
EAL_DLL_ENTRY eal_file_status EalFileWrite(eal_file*        pFile,
                                           const void*      pBuffer,
                                           const eal_u64&   Position,
                                           eal_u32          BufferSize,
                                           eal_u32&         WrittenSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Queues an asynchronous write operation.
    When the operation is completed or an error occurs during the operation,
    the provided callback function is called.

    \param pFile        A pointer to a valid file previously opened with \ref EalFileOpen.
    \param pBuffer      A pointer to a non-null buffer containing the data to be written to the file.
    \param Size         The size of the buffer (size that will be written).
    \param Pos          The position in the file where to write.
    \param Callback     A valid callback function to call when the operation completes.
    \param pCookie      A cookie; a pointer that will be given back to you in the callback.
    \param Priority     Priority of the Write request. See \ref eal_file_priority.

    \return \ref EAL_FERR_OK if the queuing of the operation succeeded, or an error
            code from \ref eal_file_status otherwise. Note this is not the
            status of the operation itself; that one is returned in the callback.

    \PRECONDITIONS
    - Multithread safe (many different threads can call \ref EalFileAsyncWrite at the same time).
    - pFile must be valid, opened with the \ref EAL_FILE_WRITE | \ref EAL_FILE_ASYNC flags.
    - pBuffer must be aligned on a multiple of \ref EalFileGetBufferAlignment.
    - Size must be a multiple of \ref EalFileGetSectorSize.
    - Pos must be a multiple of \ref EalFileGetSectorSize.

    \POSTCONDITIONS
    - You must not free the pBuffer before the callback is called.

    \sa EalFileOpen, EalFileWrite, eal_file, eal_async_cb, eal_file_priority, eal_file_status
*/
EAL_DLL_ENTRY eal_file_status EalFileAsyncWrite(eal_file*         pFile,
                                                const void*       pBuffer,
                                                eal_u32           Size,
                                                const eal_u64&    Pos,
                                                eal_async_cb      Callback,
                                                void*             pCookie,
                                                eal_file_priority Priority);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Return the file size.
    \deprecated DEPRECATED since version 3.00. Please use \ref EalFileGetSizeWithStatus instead.
    Client should avoid using this version ASAP. This function will be removed in version 6.00.
    Implementer must still support this function until version 6.00.

    \param pFile      Pointer to a valid file previously opened with \ref EalFileOpen.
    \return           The file size in bytes

    \PRECONDITIONS
    - pFile must be valid.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileGetSize)

    \sa EalFileGetSizeWithStatus, EalFileOpen, eal_file
*/
EAL_DEPRECATED("Deprecated; use EalFileGetSizeWithStatus instead")
EAL_DLL_ENTRY eal_u64 EalFileGetSize(eal_file* pFile);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*! Returns the size (in bytes) of the given file.

    \param pFile      Pointer to a valid file previously opened with \ref EalFileOpen.
    \param FileSize   The file size in bytes. Set to zero if an error occurred.
    \return           \ref EAL_FERR_OK if the call succeeded, or an error code from \ref eal_file_status otherwise.

    \PRECONDITIONS
    - pFile must be valid.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileGetSizeWithStatus)

    \sa EalFileOpen, eal_file
*/
EAL_DLL_ENTRY eal_file_status EalFileGetSizeWithStatus(eal_file* pFile,
                                                       eal_u64& FileSize);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Flush any cached data.

    \param pFile      Pointer to a valid file previously opened with \ref EalFileOpen.
    \return           \ref EAL_FERR_OK if the call succeeded, or an error code from \ref eal_file_status otherwise.

    \PRECONDITIONS
    - pFile must be valid.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileFlush).
    - All precached data read is discarded, for files opened with \ref EAL_FILE_READ.
    - All physically unwritten cached data is written on disk, for files opened with \ref EAL_FILE_WRITE.

    \sa EalFileOpen, EalFileRead, EalFileWrite, eal_file, eal_file_status
*/
EAL_DLL_ENTRY eal_file_status EalFileFlush(eal_file* pFile);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Deletes an existing file.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pPath        A string containing the file path. The formatting of the path is platform dependent. Directories are not accepted; use /ref EalFileRemoveDirectory instead.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \return             \ref EAL_FERR_OK if the file was deleted, or an error code from \ref eal_file_status otherwise.

    \PRECONDITIONS
    - pPath must point to a valid string.
    - The path must be formed in the same way as one would pass to \ref EalFileOpen.
    - The file must exist.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileDelete).
    - The implementation can refuse to delete the file for any reason, but if it does it must return an error status.

    \sa EalFileOpen, eal_file_status, EalFileGetSeparator
*/
EAL_DLL_ENTRY eal_file_status EalFileDelete(eal_id Tag,
                                            const char* pPath);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Creates a new directory.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pPath        A string containing the directory path. The formatting of the path is platform dependent.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \return             \ref EAL_FERR_OK if the directory was created, or an error code from \ref eal_file_status otherwise.

    \PRECONDITIONS
    - pPath must point to a valid string.
    - The path must be formed in the same way as one would pass to \ref EalFileOpen (except it does not end with a file name).
    - The directory must not exist.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileCreateDirectory).
    - The new directory must exist, if and only if \ref EAL_FERR_OK is returned.

    \sa EalFileRemoveDirectory, eal_file_status, EalFileGetSeparator
*/
EAL_DLL_ENTRY eal_file_status EalFileCreateDirectory(eal_id Tag,
                                                     const char* pPath);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Deletes an existing empty directory.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pPath        A string containing the directory path. The formatting of the path is platform dependent.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \return             \ref EAL_FERR_OK if the directory was deleted, or an error code from \ref eal_file_status otherwise.

    \PRECONDITIONS
    - pPath must point to a valid string.
    - The path must be formed in the same way as the one passed to \ref EalFileCreateDirectory.
    - The directory must exist.
    - The directory must be empty.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileRemoveDirectory).
    - The implementation can refuse to delete the directory for any reason, but if it does it must return an error status.
    - If the directory is not empty, it is not deleted and its children are preserved.

    \sa EalFileCreateDirectory, eal_file_status, EalFileGetSeparator
*/
EAL_DLL_ENTRY eal_file_status EalFileRemoveDirectory(eal_id Tag,
                                                     const char* pPath);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Visits all file system entries (files and directories) in the specified directory.
     The visit is not recursive. Recursive visit behavior must be handled by the caller.

    \param Tag          The tag identifying the origin of the call. See ealdef.h.
    \param pPath        A string containing the directory path. The formatting of the path is platform dependent.
                        Note that if the path has subdirectories, path separators must use the character returned by \ref EalFileGetSeparator.
    \param OptionFlags  Traversal options that must be supported by the implementation. \ref EAL_FERR_NOT_IMPLEMENTED is returned if any of the options are not supported.
    \param HintFlags    Traversal hints that can help the implementation, but can be safely ignored.
    \param Callback     A valid callback function that will be called for each visited entry. See \ref eal_directory_entry_cb.
                        If the callback function returns false, the visit will stop.
                        Unless specified otherwise, the callback must be called from the same thread used for calling \ref EalFileVisitDirectoryEntries.
    \param pCookie      An optional user-defined value that will be forwarded to the callback function. Can be null.
    \return             \ref EAL_FERR_OK if the visit was completed successfully, or an error code from \ref eal_file_status otherwise.

    \PRECONDITIONS
    - pPath must point to a valid string.
    - The path must be formed in the same way as one would pass to \ref EalFileOpen (except it does not end with a file name).
    - The directory must exist.
    - OptionFlags must be 0 or contain valid flags that are supported by the implementation.
    - HintFlags must be 0 or contain valid flags. The implementation may ignore hints.
    - Callback must be valid.

    \POSTCONDITIONS
    - Multithread safe (many different threads can call \ref EalFileVisitDirectoryEntries).
    - All internal system resources used for the visit have been freed.

    \sa EalFileCreateDirectory, EalFileRemoveDirectory, eal_directory_entry_cb, eal_file_status, EalFileGetSeparator
*/
EAL_DLL_ENTRY eal_file_status EalFileVisitDirectoryEntries(eal_id Tag,
                                                           const char* pPath,
                                                           eal_u32 OptionFlags,
                                                           eal_u32 HintFlags,
                                                           eal_directory_entry_cb Callback,
                                                           void* pCookie);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Returns the buffer alignment required by the hardware in order to use \ref EalFileAsyncRead and \ref EalFileAsyncWrite.
    \param pFile    Pointer to a valid file previously opened with \ref EalFileOpen.
    \return The required alignment, in bytes.

    \PRECONDITIONS
    - pFile must be valid and then open using the \ref EAL_FILE_ASYNC flag.

    \POSTCONDITIONS
    - Multithread safe
    - The return value is always a power of 2

    \sa EalFileOpen, EalFileAsyncRead, EalFileAsyncWrite
*/
EAL_DLL_ENTRY eal_u32 EalFileGetBufferAlignment(eal_file* pFile);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Returns the size of an hardware sector.
    An operation with \ref EalFileAsyncRead or \ref EalFileAsyncWrite requires that you also cover complete sectors.

    \param pFile    Pointer to a valid file previously opened with \ref EalFileOpen.
    \return         The sector size in bytes.

    \PRECONDITIONS
    - pFile must be valid and then open using the \ref EAL_FILE_ASYNC flag.

    \POSTCONDITIONS
    - Multithread safe
    - The return value is always a power of 2

    \sa EalFileOpen, EalFileAsyncRead, EalFileAsyncWrite
*/
EAL_DLL_ENTRY eal_u32 EalFileGetSectorSize(eal_file* pFile);

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Returns the character used for directory separator.
    \return     The character used for path separator (typically '/' or '\').

    \sa EalFileOpen
*/
EAL_DLL_ENTRY char EalFileGetSeparator();

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// The next section is only used if you are using Dynamic Linking (DLL).                          //
// If not, simply ignore this section. All the code declared in the next section is               //
// implemented in the corresponding CPP file (ealxxxdll.cpp).                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/// This structure contains a pointer to all EAL File functions.
struct eal_file_dll_interface
{
    bool                (*pEalFileLibInit)             (eal_u32             Version);
    void                (*pEalFileLibShutdown)         ();
    eal_file*           (*pEalFileOpen)                (eal_id              Tag,
                                                        const char*         pFileName,
                                                        eal_file_flags      OpenFlags,
                                                        eal_file_status&    Status);
    void                (*pEalFileClose)               (eal_file*           pFile);
    bool                (*pEalFileExists)              (eal_id              Tag,
                                                        const char*         pPath);
    eal_file_status     (*pEalFileRead)                (eal_file*           pFile,
                                                        void*               pBuffer,
                                                        const eal_u64&      Position,
                                                        eal_u32             BufferSize,
                                                        eal_u32&            SizeRead,
                                                        eal_file_priority   Priority);
    eal_file_status     (*pEalFileAsyncRead)           (eal_file*           pFile,
                                                        void*               pBuffer,
                                                        eal_u32             Size,
                                                        const eal_u64&      Pos,
                                                        eal_async_cb        Callback,
                                                        void*               pCookie ,
                                                        eal_file_priority   Priority);
    eal_file_status     (*pEalFileWrite)               (eal_file*           pFile,
                                                        const void*         pBuffer,
                                                        const eal_u64&      Position,
                                                        eal_u32             BufferSize,
                                                        eal_u32&            SizeWritten);
    eal_u64             (*pEalFileGetSize)             (eal_file*           pFile);
    eal_file_status     (*pEalFileFlush)               (eal_file*           pFile);
    eal_u32             (*pEalFileGetBufferAlignment)  (eal_file*           pFile);
    eal_u32             (*pEalFileGetSectorSize)       (eal_file*           pFile);
    char                (*pEalFileGetSeparator)        ();

    // New since version 3.0
    eal_file_status     (*pEalFileExistsWithStatus)    (eal_id              Tag,
                                                        const char*         pPath);
    eal_file_status     (*pEalFileAsyncWrite)          (eal_file*           pFile,
                                                        const void*         pBuffer,
                                                        eal_u32             Size,
                                                        const eal_u64&      Pos,
                                                        eal_async_cb        Callback,
                                                        void*               pCookie,
                                                        eal_file_priority   Priority);
    eal_file_status     (*pEalFileGetSizeWithStatus)   (eal_file*           pFile,
                                                        eal_u64&            FileSize);

    // New since version 4.0
    eal_file_status     (*pEalFileDelete)              (eal_id              Tag,
                                                        const char*         pPath);
    eal_file_status     (*pEalFileCreateDirectory)     (eal_id              Tag,
                                                        const char*         pPath);
    eal_file_status     (*pEalFileRemoveDirectory)     (eal_id              Tag,
                                                        const char*         pPath);

    // New since version 5.0
    eal_file_status     (*pEalFileVisitDirectoryEntries)(
                                                        eal_id              Tag,
                                                        const char*         pPath,
                                                        eal_u32             OptionFlags,
                                                        eal_u32             HintFlags,
                                                        eal_directory_entry_cb Callback,
                                                        void*               pCookie);
};

#if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the DLL side. It is used to resolve dynamic functions.
    This function should be called by the DLL initalization code and must be done before any
    call to any EAL function.
    \param Interface    Structure containing valid pointers to the engine EAL functions.
    \sa EalFileDllPopulateInterface
*/
void EalFileDllInitInterface(const eal_file_dll_interface& Interface);

#else // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!  Only defined on the engine side. It is used to fill up the Interface structure.
    This function is provided by the ealdll package. It fills up the interface structure
    with pointers to the current EAL functions implemented in the engine. You can then
    pass this structure to the DLL so it will dynamically connect it's internal Eal
    function calls to the one in your engine.
    \param Interface    A structure that will be filled up with pointers to local EAL functions.
    \sa EalFileDllInitInterface
*/
void EalFileDllPopulateInterface(eal_file_dll_interface& Interface);

#endif // #if defined(EAL_DLL) || defined(EAL_IMPORT_DLL)

/*! @} */
