#ifndef _ITF_BLOB_H_
#define _ITF_BLOB_H_


#ifndef _ITF_CORETYPES_H_   
#include "core/types.h"
#endif //_ITF_CORETYPES_H_   

#include "core/ByteEndian.h"

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_STRING8_H_
#include "core/String8.h"
#endif //_ITF_STRING8_H_

namespace ITF
{

    class FlexibleValue;
    class BlobExtractCommand;
    class String;

    ///
    /// A List of all known datatypes that can reside in a Blob. 
    /// Used to check if stored and extracted value are of same Type
    /// Be careful if adding a new type, add it to the end, because 
    ///  the data you will read might have been written with an older version
    ///  of this enum.s
    ///


    //////////////////////////////////////////////////////////////////////////
    ///
    /// BLOB_EXTRACT_AND_WRITE_ENABLED:
    ///     - Can call "add" methods of Blob class
    ///     - When calling "extract..." methods, the extracted data is removed from the Blob's data
    ///     - When calling "extract..." methods, the offset argument is from the beginning of the Blob's data
    ///     - When calling the Blob's destructor, the data is freed
    ///
    /// BLOB_READONLY:
    ///     - Can NOT call "add" methods of Blob class
    ///     - When calling "extract..." methods, the extracted data is NOT removed from the Blob's data
    ///     - When calling "extract..." methods, the offset argument is from the current seek pos
    ///     - When calling the Blob's destructor, the data is NOT freed
    ///
    /// the advantage of read only mode is that it makes no allocation operations
    enum BlobMode
    {
        BLOB_EXTRACT_AND_WRITE_ENABLED,
        BLOB_READONLY,
        ENUM_FORCE_SIZE_32(D)
    };

    ///
    ///
    class Blob
    {
        static const u32 BLOB_DEFAULT_BUFFER_SIZE  = 2 * 1024 * 1024; // 2 Mo
        friend class FlexibleValue;

    public:

        inline void pushInt8(i8 data) {addInt8(getSize(), data);}
        inline void pushUInt8(u8 data) {addUInt8(getSize(), data);}
        inline void pushInt16(i16 data) {addInt16(getSize(), data);}
        inline void pushUInt16(u16 data) {addUInt16(getSize(), data);}
        inline void pushInt32(i32 data) {addInt32(getSize(), data);}
        inline void pushUInt32(u32 data) {addUInt32(getSize(), data);}
        inline void pushInt64(i64 data) {addInt64(getSize(), data);}
        inline void pushUInt64(u64 data) {addUInt64(getSize(), data);}
        inline void pushFloat32(f32 data) {addFloat32(getSize(), data);}
        inline void pushFloat64(f64 data) {addFloat64(getSize(), data);}
        inline void pushString(const String& data) {addString(getSize(), data);}
        inline void pushString8(const String8& data) {addString8(getSize(), data);}
        inline void pushBool(const bbool data) {addBool(getSize(), data);}
        inline void pushBinaryBlock(const void* pData, u32 dataSize) {addBinaryBlock(getSize(), pData, dataSize);}


        //////////////////////////////////////////////////////////////////////////
        ///
        /// Constructor. 
        /// @param mode see above BlobMode enum doc
        ///
        Blob(BlobMode mode, u32 _bufferSize = BLOB_DEFAULT_BUFFER_SIZE);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Constructor. 
        /// @param pData pointer to the data to build the Blob (can be NULL)
        /// @param dataSize size of the data to build the Blob (can be NULL)
        /// @param mode see above BlobMode enum doc
        ///
        Blob(void* pData, u32 dataSize, BlobMode mode, u32 _bufferSize = BLOB_DEFAULT_BUFFER_SIZE);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Destructor. 
        ///
        ~Blob();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Set data. 
        /// @param pData pointer to the data to build the Blob (can be NULL)
        /// @param dataSize size of the data to build the Blob (can be NULL)
        /// @throws assert if data has already been assigned to the Blob
        ///
        void setData(const void* pData, u32 dataSize);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Clear's the blob if it is a write Enabled Blob
        ///
        void clear();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Enable the memory management by the blob (this is automatically done
        /// when the blob is BLOB_EXTRACT_AND_WRITE_ENABLED )
        ///    
        void enableMemoryResponsability();
     
        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a i8 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addInt8(u32 offset, i8 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a u8 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addUInt8(u32 offset, u8 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a i16 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addInt16(u32 offset, i16 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a u16 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addUInt16(u32 offset, u16 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a i32 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addInt32(u32 offset, i32 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a u32 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addUInt32(u32 offset, u32 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a i64 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addInt64(u32 offset, i64 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a u64 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addUInt64(u32 offset, u64 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a f32 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addFloat32(u32 offset, f32 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a f64 to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addFloat64(u32 offset, f64 data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a Blob to a Blob (from current read position to end of Blob
        /// extracts the copied data from the source Blob if this 
        /// is  BLOB_EXTRACT_AND_WRITE_ENABLED)
        /// @param offset the place where the data should be added
        /// @param blob the Blob to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addBlob(u32 offset, Blob &blob);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Copies a blob to a blob (from current read position to end of Blob
        /// this does not extract any data from the source blob
        /// @param offset the place where the data should be added
        /// @param blob the Blob to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void copyBlob(u32 offset, const Blob &blob);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Converts a String to UTF8 and adds it to the blob
        /// @param offset the place where the String should be added
        /// @param data the String to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addString(u32 offset, const String& data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a String8 to the blob
        /// @param offset the place where the String should be added
        /// @param data the String to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addString8(u32 offset, const String8& data);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Converts a bool to big endian and adds it to the blob
        /// @param offset the place where the bool should be added
        /// @param data the bool to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addBool(u32 offset, const bbool data);

        

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 8 bits signed value from the Blob.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        i8 extractInt8(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 8 bits unsigned value from the Blob.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u8 extractUint8(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 16 bits signed value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        i16 extractInt16(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 16 bits unsigned value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u16 extractUint16(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 32 bits signed value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        i32 extractInt32(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 32 bits unsigned value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u32 extractUint32(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 32 bits float value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        f32 extractFloat32(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 64 bits float value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        f64 extractFloat64(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Reads (without extracting) a 32 bits unsigned value from the Blob and restores the platform's native endian.
        /// The blob is in BLOB_READONLY mode
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u32 readUint32(u32 offset = 0);
        
        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 64 bits signed value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        i64 extractInt64(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a 64 bits unsigned value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u64 extractUint64(u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Try to extracts a 64 bits unsigned value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @param result the extracted data
        /// @return true if the data was extracted successfully
        ///
        bool tryExtractUint64(u32 offset, u64& result);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a UTF8 block from the Blob and build a String out of it.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @param out the extracted String (yours to delete it)
        /// @throws assert of offset is greater than the Blob's current size
        ///
        void extractString(String& out, u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a char[] from the Blob and build a String out of it.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @param out the extracted String (yours to delete it)
        /// @throws assert of offset is greater than the Blob's current size
        ///
        void extractString8(String8& out, u32 offset = 0);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a bool value from the Blob and restores the platform's native endian.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @return the extracted data
        /// @throws assert of offset is greater than the Blob's current size
        ///
        bbool extractBool(u32 offset = 0);

        

        //////////////////////////////////////////////////////////////////////////
        ///
        /// @return the size in bytes of the Blob's data
        ///
        u32 getSize() const;

        //////////////////////////////////////////////////////////////////////////
        ///
        /// @return a pointer to the Blob's data
        ///
        void* getData()const;

        //////////////////////////////////////////////////////////////////////////
        /// 
        /// useful function for checking how many bytes are left unread (not equal to remaining data size)
        /// @return u32 number of bytes left to be read
        ///
        u32 getNumberOfRemainingBytes() const;


        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a binary buffer to the blob (Warning: care about endians !)
        /// @param offset the place where the data should be added
        /// @param data the data to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void addBinaryBlock(u32 offset, const void* pData, u32 dataSize);
        
        //////////////////////////////////////////////////////////////////////////
        /// 
        /// @return True if read Only Blob
        /// 
        bool isReadOnly() const;
        
        //////////////////////////////////////////////////////////////////////////
        ///
        /// Extracts a binary block from the Blob.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @param dataSize return the amount bytes extracted
        /// @return the extracted data (yours to free the allocated memory).
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u8* extractBinaryBlock(u32 offset, u32& dataSize);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// converts a blob from BLOB_EXTRACT_AND_WRITE_ENABLED to BLOB_READONLY
        /// the memory will be freed at the blob's destruction
        /// 
        void makeReadOnly();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Helper : Retrieves a binary block from the Blob without extracting it
        /// The extracted data is not suppressed from the Blob
        /// @param offset the place where the data should be copied from
        /// @param dataSize the amount of data to copy
        /// @return the copied data (yours to free the allocated memory).
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u8* getBlobData(u32 offset, u32 dataSize) const;


        bool executeExtractCommands(ITF_VECTOR<BlobExtractCommand>& commands);


        //////////////////////////////////////////////////////////////////////////
        ///
        /// Helper : saves the current seek pos to be restored by popSeekPos.
        /// @throws assert blob is not in read only or if more than on push was done.
        ///
        void pushSeekPos();

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Helper : restore the current seek pos saved by pushSeekPos
        /// @throws assert blob is not in read only or if more than on push was done.
        ///
        void popSeekPos();

		//////////////////////////////////////////////////////////////////////////
		/// 
		/// Helper Method to extract the DataType of the following Element to the blob
		/// @param offset the place where the data should be extracted from
		///
		DataType extractBlobDataType(u32 offset);

        BlobMode getMode() const {return m_mode;}

        void        resetSeekPos()              { setSeekPos(0); }
        void        setSeekPos(u32 _seekPos)    { ITF_ASSERT(_seekPos <= m_dataSize); m_seekPos = _seekPos; }
        const u32   getSeekPos()const           { return m_seekPos; }

    private:

        //////////////////////////////////////////////////////////////////////////
        ///
        /// checks a condition and raises an error if the condition occurs
        /// @param conditionForError condition for the error to occur
        /// @return true if the error occurred
        /// 
        bool setErrorIf(bool conditionForError)
        {
            if (!m_bIsSafeMode)
            {
                ITF_ASSERT(!conditionForError);               
                return false;
            }
            ITF_ASSERT(!m_bErrorRaised);
            m_bErrorOccured = conditionForError;
            m_bErrorRaised = true;
            return conditionForError;
        }

        //////////////////////////////////////////////////////////////////////////
        ///
        /// @return true if last call to setErrorIf() raised an error
        /// 
        bool errorOccured()
        {
            if (!m_bIsSafeMode)
            {
                return false;
            }
            ITF_ASSERT(m_bErrorRaised);
            m_bErrorRaised = false;
            return m_bErrorOccured;
        }

        //////////////////////////////////////////////////////////////////////////
        ///
        /// resets error flags when we know last error code was a success
        /// 
        void onSuccessOccured()
        {
            if (m_bIsSafeMode)
            {
                ITF_ASSERT(m_bIsSafeMode);
                ITF_ASSERT(m_bErrorRaised);
                ITF_ASSERT(!m_bErrorOccured);
                m_bErrorRaised = false;
            }
        }
        
        //////////////////////////////////////////////////////////////////////////
        /// 
        /// Helper Method to add the DataType of the following Element to the blob
        /// @param offset the place where the data should be added
        /// @param type the DataType
        ///
        void addBlobDataType(u32 offset, DataType type);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a numerical value to the Blob. 
        /// @param offset the place where the value should be added
        /// @param data the value to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        template<typename T>
            void addNumerical(u32 offset, T data)
        {
            ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
            ITF_ASSERT(offset <= getSize());
            u8 endian[sizeof(T)];
            Endian::hideEndian<T>(data, endian);
            add(offset, endian, sizeof(endian));
        }

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Adds a binary buffer to the blob (Warning: care about endians !)
        /// @param offset the place where the data should be added
        /// @param data the data to add
        /// @throws assert of offset is greater than the Blob's current size
        /// @throws assert if the Blob was created in BLOB_READONLY mode
        ///
        void add(u32 offset, const void* pData, u32 dataSize);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Helper : Extracts a binary block from the Blob.
        /// The extracted data is suppressed from the Blob if the Blob was created in mode BLOB_EXTRACT_AND_WRITE_ENABLED
        /// @param offset the place where the data should be extracted from
        /// @param dataSize the amount of data to extract
        /// @return the extracted data (yours to free the allocated memory).
        /// @throws assert of offset is greater than the Blob's current size
        ///
        u8* extract(u32 offset, u32 dataSize);

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Copy Constructor. Private so nobody can use it.  
        /// @param blob The blob to copy
        ///
        Blob(const Blob& blob);
        Blob& operator=(const Blob& blob);
        i8*    m_buffer;        // allocated buffer starting position
        i8*    m_pData;         // data starting position (this means there could be junk before the data which must still be deallocated but isn't in the blob, from a user point of view)
        u32   m_dataSize;
        u32   m_bufferSize;
        u32   m_seekPos;
        BlobMode    m_mode;
        bool        m_responsibleForFreeingMemory;
        bool        m_bErrorOccured;    /// last extract operation failed
        bool        m_bErrorRaised;     /// last extract operation raised an error (success or failure)
        bool        m_bIsSafeMode;      /// indicates that errors will be raised during extract commands
    };

    class SerializableClass
    {
    public:
        virtual ~SerializableClass(){};
        //////////////////////////////////////////////////////////////////////////
        /// 
        /// serialize the message class into the end of the blob
        /// @param _blob reference to the blob that will contain the serialized data
        /// 
        virtual void serialize(Blob& _blob) = 0;

        //////////////////////////////////////////////////////////////////////////
        /// 
        /// unserialize the message class from the start of the blob
        /// @param _blob reference to the blob that contains the serialized class data
        /// 
        virtual void unserialize(Blob& _blob) = 0;
        
        u32 m_classId;

    //    virtual u32 getSerializableClassId() = 0;
    };

    /*
    Description of BlobCommand by parameter type:

    - BOOL, i8, u8, i16, u16, i32, u32, i64, u64 :
        m_data [in]     must point to a destination variable that has the right type (data is copied via memcpy)
        m_offset [in]   read offset

    -  STRING:
        m_data [in]     must point to a destination string
        m_offset [in]   read offset

    - BINARYBLOCK :
        m_size [in]     size of the block to read
        m_data [out]    is assigned with the address of the binary block that has been created and read
        m_offset [in]   read offset

    - FLEXIBLEVALUE :
        m_data [in]     must point to an already created FlexibleValue
        m_offset [in]   read offset
    */
    class BlobExtractCommand
    {
    public:
        BlobExtractCommand()
        {
            ITF_MemSet(this, 0, sizeof(BlobExtractCommand));
        }
        ~BlobExtractCommand(){}
        DataType m_type;
        void*           m_data;
        u32       m_size;
        u32       m_offset;
    #if ITF_UNITTEST > 0
        static void testPlan();
    #endif
    };

} // namespace ITF

#endif // _ITF_BLOB_H_
