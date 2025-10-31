#include "precompiled_core.h"


#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_FLEXIBLEVALUE_H_
#include "core/FlexibleValue.h"
#endif //_ITF_FLEXIBLEVALUE_H_

#include "core/ByteEndian.h"

namespace ITF
{
    Blob::Blob(BlobMode mode, u32 _bufferSize)
    {
        m_mode = mode;
        m_pData = NULL;
        m_buffer = NULL;
        m_bufferSize = _bufferSize;
        m_dataSize = 0;
        resetSeekPos();
        m_responsibleForFreeingMemory = (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        m_bErrorOccured = false;
        m_bIsSafeMode = false;
        m_bErrorRaised = false;
    }

    void Blob::setData(const void* pData, u32 dataSize)
    {
        ITF_ASSERT(NULL == m_pData);
        ITF_ASSERT(0 == m_dataSize);
        ITF_ASSERT(dataSize < m_bufferSize);

        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            // the blob can grow and will handle the memory deallocation
            m_buffer = newAlloc(mId_Blob, i8[m_bufferSize]);
            m_pData = m_buffer;
            ITF_Memcpy(m_pData, pData, dataSize);
        }
        else
        {
            m_pData = (i8*)pData;
            // the blob can't grow and won't handle the memory deallocation
            m_buffer = m_pData;
        }
        m_dataSize = dataSize;
        resetSeekPos();
    }

    Blob::Blob(void* pData, u32 dataSize, BlobMode mode, u32 _bufferSize)
    {
        resetSeekPos();
        m_mode = mode;
        m_pData = NULL;
        m_buffer = NULL;
        m_bufferSize = _bufferSize;
        m_dataSize = 0;
        setData(pData, dataSize);
        m_responsibleForFreeingMemory = (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        m_bErrorOccured = false;
        m_bIsSafeMode = false;
        m_bErrorRaised = false;
    }

    Blob::~Blob()
    {
        ITF_ASSERT(!m_bErrorOccured);

        if (m_responsibleForFreeingMemory)
        {
            SF_DEL_ARRAY(m_buffer);
        }
    }

    void Blob::enableMemoryResponsability()
    {
        ITF_ASSERT( false == m_responsibleForFreeingMemory );
        m_responsibleForFreeingMemory = true;
    }

    void Blob::makeReadOnly()
    {
        ITF_ASSERT(BLOB_EXTRACT_AND_WRITE_ENABLED == m_mode);
        m_mode = BLOB_READONLY;
        // don't touch the m_responsibleForFreeingMemory, since we are still
        // responsible to Memory::free the memory
    }

    void Blob::clear()
    {
        if ((m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED) && (m_pData))
        {
            SF_DEL_ARRAY(m_buffer);
            m_pData=NULL;
            resetSeekPos();
            m_dataSize=0;
        } else 
        {
            setSeekPos(m_dataSize);
        }
    }
    void Blob::addInt8(u32 offset, i8 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_i8);        
    }

    void Blob::addUInt8(u32 offset, u8 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_u8);       
    }

    void Blob::addInt16(u32 offset, i16 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_i16);    
    }

    void Blob::addUInt16(u32 offset, u16 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_u16);    
    }

    void Blob::addInt32(u32 offset, i32 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_i32);    
    }

    void Blob::addUInt32(u32 offset, u32 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_u32);   
    }

    void Blob::addInt64(u32 offset, i64 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_i64);  
    }

    void Blob::addUInt64(u32 offset, u64 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_u64);  
    }

    void Blob::addFloat32(u32 offset, f32 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_f32);  
    }

    void Blob::addFloat64(u32 offset, f64 data)
    {
        addNumerical(offset,data);
        addBlobDataType(offset, DataType_f64);  
    }

    void Blob::addBlob(u32 offset, Blob &blob)
    {
        ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        ITF_ASSERT(offset <= getSize());
        
        u32 bytesRemaining;
        if (blob.m_mode == BLOB_READONLY)
        {
            bytesRemaining = blob.m_dataSize - blob.getSeekPos();
        }
        else
        {
            bytesRemaining = blob.m_dataSize;
        }

        u8 *pData = blob.extract(0, bytesRemaining);
        add(offset,pData,bytesRemaining);
        // if blob is not readOnly we have to Memory::free the extracted blob
        if(!blob.isReadOnly())
        {
            SF_DEL_ARRAY(pData);
        }
    }

    void Blob::copyBlob(u32 offset, const Blob &blob)
    {
        ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        ITF_ASSERT(offset <= getSize());


        u32 dataSize;
        if (blob.m_mode == BLOB_READONLY)
        {
            dataSize =  (blob.getSize() - blob.getSeekPos());
        }
        else
        {
            dataSize = blob.m_dataSize ;
        }

        u8 *pData = blob.getBlobData(0, dataSize);
        add(offset,pData,dataSize);
        
        // if blob is not readOnly we have to Memory::free the extracted blob
        if(!blob.isReadOnly())
        {
            SF_DEL_ARRAY(pData);
        }
    }

    void Blob::addString(u32 offset, const String& data)
    {
        ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        ITF_ASSERT(offset <= getSize());
        u32 length;
        // it is possible that an empty string is exactly what we want to
        // serialize, in that case serialize only the length
        if (0 == data.getLen())
        {
            addUInt32(offset,(u32) 0);
        }
        else
        {
            u8 *UTF8Data = (u8*)ITF_Alloca(data.getLen()+1);
            data.convertStringClasstoUtf8Str(&length,UTF8Data);
            length++; // to write ending 0 too
                  
            add(offset, (void*) UTF8Data, length);
            addUInt32(offset, length);  
        }
        addBlobDataType(offset, DataType_String); 
    }

    void Blob::addString8(u32 offset, const String8& data)
    {
        ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        ITF_ASSERT(offset <= getSize());
        u32 length;
        // it is possible that an empty string is exactly what we want to
        // serialize, in that case serialize only the length
        if (0 == data.getLen())
        {
            addUInt32(offset,(u32) 0);
        }
        else
        {
            length = data.getLen() + 1; // +1 to write ending 0 too                  
            add(offset, (void*) data.cStr(), length);
            addUInt32(offset, length);  
        }
        addBlobDataType(offset, DataType_String8); 
    }

    void Blob::addBool(u32 offset, const bbool bData)
    {
        ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        ITF_ASSERT(offset <= getSize());
        u8 endian[4];
        u32 data;
        if (bData)
        {
            data = 1;
        }
        else
        {
            data = 0;
        }
        Endian::hideEndian<u32>(data, endian);
        add(offset, endian, sizeof(endian));
        addBlobDataType(offset, DataType_Bool);
    }
    
    void Blob::addBinaryBlock(u32 offset, const void* pData, u32 dataSize)
    {
        add(offset,pData,dataSize);
        addInt32(offset, dataSize);
        addBlobDataType(offset, DataType_BinaryBlock);
    }
    void Blob::add(u32 offset, const void* pData, u32 dataSize)
    {
        ITF_ASSERT(m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED);
        ITF_ASSERT(offset <= getSize());
        
        if (m_pData == NULL)
        {
            m_buffer =  newAlloc(mId_Blob, i8[m_bufferSize]);
            m_pData = m_buffer;
        }
        
        u32 newSize = m_dataSize + dataSize;
        i8* newData = m_pData + offset;
        
        // check for buffer overflow
        ITF_ASSERT(m_pData - m_buffer + newSize < m_bufferSize);
        
        if (offset != m_dataSize)
        {
            std::memmove(newData + dataSize, newData, m_dataSize - offset);
        }
        
        ITF_Memcpy(newData, pData, dataSize);
        m_dataSize = newSize;
    }

#define BLOB_SECURITY_CHECK(dataType, dataID) {\
if (setErrorIf(offset + sizeof(dataType) + sizeof(u8) > getSize()))\
{\
return 0;\
}\
onSuccessOccured();\
DataType type = extractBlobDataType(offset);\
if (setErrorIf(type!=dataID))\
{\
return 0;\
}\
}

#define BLOB_SECURITY_CHECK_BINARY(dataSize, dataID) {\
if (setErrorIf(offset + dataSize + sizeof(u8) > getSize()))\
{\
return 0;\
}\
onSuccessOccured();\
DataType type = extractBlobDataType(offset);\
if (setErrorIf(type!=dataID))\
{\
return NULL;\
}\
}

#define BLOB_SECURITY_CHECK_STRING(dataType, dataID) {\
if (setErrorIf(offset + sizeof(dataType) + sizeof(u8) > getSize()))\
{\
return;\
}\
onSuccessOccured();\
DataType type = extractBlobDataType(offset);\
if (setErrorIf(type!=dataID))\
{\
return;\
}\
}

    i8 Blob::extractInt8(u32 offset)
    {
        BLOB_SECURITY_CHECK(i8, DataType_i8)

        u8* pRet;
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            pRet = extract(offset, sizeof(i8));
        } else
        {
            pRet = extract(0, sizeof(i8));
        }
        i8 ret;
        ITF_Memcpy(&ret, pRet, sizeof(i8));
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    u8 Blob::extractUint8(u32 offset)
    {
        BLOB_SECURITY_CHECK(u8, DataType_u8);

        u8* pRet;
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            pRet = extract(offset, sizeof(u8));
        } else
        {
            pRet = extract(0, sizeof(u8));
        }
        u8 ret;
        ITF_Memcpy(&ret, pRet, sizeof(u8));
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    i16 Blob::extractInt16(u32 offset)
    {
        BLOB_SECURITY_CHECK(i16, DataType_i16);

        u8* pRet;
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            pRet = extract(offset, sizeof(i16));
        } else
        {
            pRet = extract(0, sizeof(i16));
        }
        i16 ret;
        Endian::restoreEndian<i16>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    u16 Blob::extractUint16(u32 offset)
    {
        BLOB_SECURITY_CHECK(u16, DataType_u16);

        u8* pRet;
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            pRet = extract(offset, sizeof(u16));
        } else
        {
            pRet = extract(0, sizeof(u16));
        }
        u16 ret;
        Endian::restoreEndian<u16>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    i32 Blob::extractInt32(u32 offset)
    {
        BLOB_SECURITY_CHECK(i32, DataType_i32);

        u8* pRet;
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            pRet = extract(offset, sizeof(i32));
        } else
        {
            pRet = extract(0, sizeof(i32));
        }
        i32 ret;
        Endian::restoreEndian<i32>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    u32 Blob::extractUint32(u32 offset)
    {
        BLOB_SECURITY_CHECK(u32, DataType_u32);

        u8* pRet;
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            pRet = extract(offset, sizeof(u32));
        } else
        {
            pRet = extract(0, sizeof(u32));
        }
        u32 ret;
        Endian::restoreEndian<u32>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    // no extraction taking place
    u32 Blob::readUint32(u32 offset)
    {
        ITF_ASSERT(m_mode == BLOB_READONLY);

        u32 _savePos = getSeekPos();
        u32 _result = extractUint32(offset);

        setSeekPos(_savePos);
        return _result;
    }


    i64 Blob::extractInt64(u32 offset)
    {
        BLOB_SECURITY_CHECK(i64, DataType_i64);

        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        u8* pRet = extract(offset, sizeof(i64));
        i64 ret;
        Endian::restoreEndian<i64>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    f32 Blob::extractFloat32(u32 offset)
    {
        BLOB_SECURITY_CHECK(f32, DataType_f32);

        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        u8* pRet = extract(offset, sizeof(f32));
        f32 ret;
        Endian::restoreEndian<f32>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    f64 Blob::extractFloat64(u32 offset)
    {
        BLOB_SECURITY_CHECK(f64, DataType_f64);

        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        u8* pRet = extract(offset, sizeof(f64));
        f64 ret;
        Endian::restoreEndian<f64>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    u64 Blob::extractUint64(u32 offset)
    {
        BLOB_SECURITY_CHECK(u64, DataType_u64);

        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        u8* pRet = extract(offset, sizeof(u64));
        u64 ret;
        Endian::restoreEndian<u64>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return ret;
    }

    bool Blob::tryExtractUint64(u32 offset, u64& result)
    {
        if (offset + sizeof(u64) + sizeof(u8) > getSize())
        {
            return false;
        }

        DataType type = extractBlobDataType(offset);
        if (type != DataType_u64)
        {
            return false;
        }

        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        u8* pRet = extract(offset, sizeof(u64));
        Endian::restoreEndian<u64>(pRet, result);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return true;
    }

    bbool Blob::extractBool(u32 offset)
    {
        BLOB_SECURITY_CHECK(u32, DataType_Bool);

        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        u8* pRet = extract(offset, sizeof(u32));
        u32 ret;
        Endian::restoreEndian<u32>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        if (ret == 0)
        {
            return bfalse;
        }
        return btrue;
    }

    u32 Blob::getNumberOfRemainingBytes() const
    {
        if (m_mode == BLOB_READONLY)
        {
            return (getSize() - getSeekPos());
        }
        else
        {
            return m_dataSize ;
        }
    }

    u8* Blob::extractBinaryBlock(u32 offset, u32& dataSize)
    {
        ITF_ASSERT(m_dataSize > 0);
        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }

        BLOB_SECURITY_CHECK_BINARY(dataSize, DataType_BinaryBlock);

        onSuccessOccured();
        dataSize = extractInt32(offset);
        if (errorOccured())
        {
            setErrorIf(true);
            return NULL;
        }
        if (setErrorIf(offset + dataSize > getSize()))
        {
            return NULL;
        }
        if (m_mode != BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            offset=0;
        }
        return extract(offset,dataSize);
    }

    u8* Blob::extract(u32 offset, u32 dataSize)
    {
        ITF_ASSERT(dataSize > 0);
        ITF_ASSERT(offset + dataSize <= getSize());

        if (m_mode == BLOB_READONLY)
        {
            offset += getSeekPos();
            ITF_ASSERT(offset + dataSize <= getSize());
            setSeekPos(offset + dataSize);
            return (u8*)&m_pData[offset];
        }
        
        i8* extractedData = m_pData + offset;
        u32 endOffset = offset + dataSize;
        
        u8* pRet = newAlloc(mId_Blob, u8[dataSize]);
        ITF_Memcpy(pRet, extractedData, dataSize);
        
        if (offset == 0)
        {
            m_pData += dataSize;
        }
        else if (endOffset != m_dataSize)
        {
            std::memmove(extractedData, extractedData + dataSize, m_dataSize - endOffset);
        }
        m_dataSize -= dataSize;

        return pRet;
    }


    // same as above but does not affect the seek position
    u8* Blob::getBlobData(u32 offset, u32 dataSize) const
    {
        ITF_ASSERT(dataSize > 0);
        ITF_ASSERT(offset + dataSize <= getSize());

        if (m_mode == BLOB_READONLY)
        {
            ITF_ASSERT(offset + getSeekPos() + dataSize <= getSize());
            return (u8*)&m_pData[offset + getSeekPos()];
        }

        // retrieve data
        u8* pRet = newAlloc(mId_Blob, u8[dataSize]);
        ITF_Memcpy(pRet, &m_pData[offset], dataSize);
        return pRet;
    }


    void Blob::extractString(String& out, u32 offset)
    {
        BLOB_SECURITY_CHECK_STRING(u32, DataType_String);  // check we have enough data to extract string size

        ITF_ASSERT(offset <= getSize());
        u32 len;
        onSuccessOccured();
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            len = extractUint32(offset);
        }
        else
        {
            len = extractUint32(0);
        }
        if (errorOccured())
        {
            setErrorIf(true);
            return;
        }
        if (setErrorIf(offset + len > getSize()))
        {
            return;
        }
        if (len > 0)
        {
            u8* UTF8String;
            if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
            {
                UTF8String = extract(offset, len);
            } else
            {
                UTF8String = extract(0, len);
            }
            out.setUTF8Text(UTF8String);
            if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
            {
                SF_DEL_ARRAY(UTF8String);
            }
        }
        else
        {
            out.clear();
        }
    }

    void Blob::extractString8(String8& out, u32 offset)
    {
        BLOB_SECURITY_CHECK_STRING(u32, DataType_String8);  // check we have enough data to extract string size

        ITF_ASSERT(offset <= getSize());
        u32 len;
        onSuccessOccured();
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            len = extractUint32(offset);
        }
        else
        {
            len = extractUint32(0);
        }
        if (errorOccured())
        {
            setErrorIf(true);
            return;
        }
        if (setErrorIf(offset + len > getSize()))
        {
            return;
        }
        if (len > 0)
        {
            char* localData;
            if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
            {
                localData = (char*)extract(offset, len);
            } else
            {
                localData = (char*)extract(0, len);
            }
            out.setText(localData);
            if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
            {
                SF_DEL_ARRAY(localData);
            }
        }
        else
        {
            out.clear();
        }
    }

    u32 Blob::getSize() const
    {
        return m_dataSize;
    }

    void* Blob::getData()const
    {
        return m_pData;
    }

    bool Blob::isReadOnly() const
    {
        return (m_mode==BLOB_READONLY);
    }

    void Blob::addBlobDataType(u32 offset, DataType type)
    {
        addNumerical(offset,(u8)type);
    }

    DataType Blob::extractBlobDataType(u32 offset)
    {
        if (offset + sizeof(u8) >= getSize())
        {
            return DataType_Unknown;
        }
        u8* pRet = extract(offset, sizeof(u8));
        u8 ret;
        Endian::restoreEndian<u8>(pRet, ret);
        if (m_mode == BLOB_EXTRACT_AND_WRITE_ENABLED)
        {
            SF_DEL_ARRAY(pRet);
        }
        return (DataType) ret;
    } 


    bool Blob::executeExtractCommands(ITF_VECTOR<BlobExtractCommand>& commands)
    {
        m_bIsSafeMode = true;

        ITF_VECTOR<BlobExtractCommand>::iterator it;

        for (it = commands.begin(); it != commands.end(); ++it)
        {
            BlobExtractCommand& curCmd = *it;
            switch (curCmd.m_type)
            {
                case DataType_i8 :
                {
                    i8 localData = extractInt8(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_u8 :
                {
                    u8 localData = extractUint8(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_i16 :
                {
                    i16 localData = extractInt16(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_u16 :
                {
                    u16 localData = extractUint16(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_i32 :
                {
                    i32 localData = extractInt32(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_u32 :
                {
                    u32 localData = extractUint32(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_i64 :
                {
                    i64 localData = extractInt64(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_u64 :
                {
                    u64 localData = extractUint64(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }
                break;

                case DataType_String :
                    extractString(*((String*)curCmd.m_data), curCmd.m_offset);
                break;

                case DataType_Bool :
                {
                    bbool localData = extractBool(curCmd.m_offset);
                    ITF_Memcpy(curCmd.m_data, &localData, sizeof(localData));
                }                    
                break;

                case DataType_BinaryBlock :
                    curCmd.m_data = extractBinaryBlock(curCmd.m_offset, curCmd.m_size);
                break;

                case DataType_FlexibleValue :
                    ITF_ASSERT(!"Flexible are serialized and can't be in here");
                break;

                default:
                    setErrorIf(true);
                break;
            }
            
            if (errorOccured())
            {
                m_bIsSafeMode = false;
                return false;
            }
        }

        m_bIsSafeMode = false;
        return true;
    }
} // namespace ITF

