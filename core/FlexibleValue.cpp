#include "precompiled_core.h"

#ifndef _ITF_FLEXIBLEVALUE_H_
#include "core/FlexibleValue.h"
#endif //_ITF_FLEXIBLEVALUE_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_


namespace ITF
{

FlexibleValue::FlexibleValue() :
    m_int64(0),
    m_type(NOT_SET)
{
}

FlexibleValue::FlexibleValue(i32 value) :
    m_int32(value), m_type(INTEGER_32)
{
}
FlexibleValue::FlexibleValue(u32 value) :
    m_uint32(value), m_type(UNSIGNED_INTEGER_32)
{
}
FlexibleValue::FlexibleValue(i64 value) :
    m_int64(value), m_type(INTEGER_64)
{
}
FlexibleValue::FlexibleValue(u64 value) :
    m_uint64(value), m_type(UNSIGNED_INTEGER_64)
{
}
FlexibleValue::FlexibleValue(f32 value) :
    m_float32(value), m_type(FLOAT_32)
{
}
FlexibleValue::FlexibleValue(f64 value) :
    m_float64(value), m_type(FLOAT_64)
{
}
FlexibleValue::FlexibleValue(const String& value) :
    m_type(STRING)
{
    m_pString = new String(value);
}

FlexibleValue::FlexibleValue(const Blob& value) :
    m_type(BLOB)
{
    m_pBlob = new Blob(BLOB_EXTRACT_AND_WRITE_ENABLED);
    setBlobValue(value);
}

FlexibleValue::~FlexibleValue()
{
    clear();
}

FlexibleValue::FlexibleValue(const FlexibleValue& oValue)
    : m_int32(0)
    , m_type(NOT_SET)
{
    setType(oValue.getType());
    switch(oValue.getType())
    {
    case NOT_SET:
        break;

    case STRING:
        {
            setStringValue(oValue.getStringValue());
        }
        break;

    case INTEGER_32:
        {
            setInt32Value(oValue.getInt32Value());
        }
        break;

    case UNSIGNED_INTEGER_32:
        {
            setUnsignedInt32Value(oValue.getUnsignedInt32Value());
        }
        break;

    case INTEGER_64:
        {
            setInt64Value(oValue.getInt64Value());
        }
        break;

    case UNSIGNED_INTEGER_64:
        {
            setUnsignedInt64Value(oValue.getUnsignedInt64Value());
        }
        break;
        
    case FLOAT_32:
        {
            setFloat32Value(oValue.getFloat32Value());
        }
        break;

    case FLOAT_64:
        {
            setFloat64Value(oValue.getFloat64Value());
        }
        break;
    case BLOB:
        {
            setBlobValue(oValue.getBlobValue());
        }
        break;

    default:
        ITF_ASSERT(false);
        break;
    }
}

FlexibleValue& FlexibleValue::operator +=(const FlexibleValue& rhs)
{
    ITF_ASSERT(m_type == rhs.getType());
    switch(m_type)
    {
    case INTEGER_32:
        m_int32 += rhs.getInt32Value();
        break;
    case INTEGER_64:
        m_int64 += rhs.getInt64Value();
        break;
    case UNSIGNED_INTEGER_32:
        m_uint32 += rhs.getUnsignedInt32Value();
        break;
    case UNSIGNED_INTEGER_64:
        m_uint64 += rhs.getUnsignedInt64Value();
        break;
    case FLOAT_32:
        m_float32 += rhs.getFloat32Value();
        break;
    case FLOAT_64:
        m_float64 += rhs.getFloat64Value();
        break;
    case STRING:
        (*m_pString) += rhs.getStringValue();
        break;
    case BLOB:
        {
            (*m_pBlob).add( m_pBlob->getSize(), rhs.getBlobValue().getData(), rhs.getBlobValue().getSize());
            break;
        }
    default:
        break;
    }
    return (*this);
}

bool FlexibleValue::operator==(const FlexibleValue& rhs) const
{
    ITF_ASSERT(m_type == rhs.m_type);
    if (m_type != rhs.m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case INTEGER_32:
        return m_int32 == rhs.m_int32;

    case INTEGER_64:
        return m_int64 == rhs.m_int64;

    case UNSIGNED_INTEGER_32:
        return m_uint32 == rhs.m_uint32;           

    case UNSIGNED_INTEGER_64:
        return m_uint64 == rhs.m_uint64;  

    case FLOAT_32:
        return m_float32 == rhs.m_float32;

    case FLOAT_64:
        return m_float64 == rhs.m_float64;

    case STRING:
        return *m_pString == *rhs.m_pString;

    case BLOB:                
        {            
            u32 lenth1 = m_pBlob->getSize();
            u32 lenth2 = rhs.getBlobValue().getSize();
            
            if( lenth1 == lenth2 )
            {
                int cmpEqual = memcmp(m_pBlob->getData(), rhs.getBlobValue().getData(), lenth1);
                if(cmpEqual == 0)
                    return true;
            }
            
            return false;
        }

    default:
        ITF_ASSERT(false);
        return false;
    }
}

bool FlexibleValue::operator>(const FlexibleValue& rhs) const
{
    ITF_ASSERT(m_type == rhs.m_type);
    if (m_type != rhs.m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case INTEGER_32:
        return m_int32 > rhs.m_int32;           

    case INTEGER_64:
        return m_int64 > rhs.m_int64;           

    case UNSIGNED_INTEGER_32:
        return m_uint32 > rhs.m_uint32;           

    case UNSIGNED_INTEGER_64:
        return m_uint64 > rhs.m_uint64;

    case FLOAT_32:
        return m_float32 > rhs.m_float32;

    case FLOAT_64:
        return m_float64 > rhs.m_float64;

    case STRING:
        // TODO: can't compare string, it depends on locals.
        ITF_ASSERT(false);
        return false;

    case BLOB:
        // can't compare blob
        ITF_ASSERT(false);
        return false;

    default:
        ITF_ASSERT(false);
        return false;
    }
}

bool FlexibleValue::operator<(const FlexibleValue& rhs) const
{
    ITF_ASSERT(m_type == rhs.m_type);
    if (m_type != rhs.m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case INTEGER_32:
        return m_int32 < rhs.m_int32;           

    case INTEGER_64:
        return m_int64 < rhs.m_int64;           

    case UNSIGNED_INTEGER_32:
        return m_uint32 < rhs.m_uint32;           

    case UNSIGNED_INTEGER_64:
        return m_uint64 < rhs.m_uint64; 

    case FLOAT_32:
        return m_float32 < rhs.m_float32;

    case FLOAT_64:
        return m_float64 < rhs.m_float64;

    case STRING:
        // TODO: can't compare string, it depends on locals.
        ITF_ASSERT(false);
        return false;

    case BLOB:
        // can't compare blob
        ITF_ASSERT(false);
        return false;

    default:
        ITF_ASSERT(false);
        return false;
    }
}

bool FlexibleValue::operator>=(const FlexibleValue& rhs) const
{
    ITF_ASSERT(m_type == rhs.m_type);
    if (m_type != rhs.m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case INTEGER_32:
        return m_int32 >= rhs.m_int32;           

    case INTEGER_64:
        return m_int64 >= rhs.m_int64;           

    case UNSIGNED_INTEGER_32:
        return m_uint32 >= rhs.m_uint32;           

    case UNSIGNED_INTEGER_64:
        return m_uint64 >= rhs.m_uint64;  

    case FLOAT_32:
        return m_float32 >= rhs.m_float32;

    case FLOAT_64:
        return m_float64 >= rhs.m_float64;

    case STRING:
        // TODO: can't compare string, it depends on locals.
        ITF_ASSERT(false);
        return false;

    case BLOB:
        // can't compare blob
        ITF_ASSERT(false);
        return false;

    default:
        ITF_ASSERT(false);
        return false;
    }
}

bool FlexibleValue::operator<=(const FlexibleValue& rhs) const
{
    ITF_ASSERT(m_type == rhs.m_type);
    if (m_type != rhs.m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case INTEGER_32:
        return m_int32 <= rhs.m_int32;           

    case INTEGER_64:
        return m_int64 <= rhs.m_int64;           

    case UNSIGNED_INTEGER_32:
        return m_uint32 <= rhs.m_uint32;           

    case UNSIGNED_INTEGER_64:
        return m_uint64 <= rhs.m_uint64;   

    case FLOAT_32:
        return m_float32 <= rhs.m_float32;

    case FLOAT_64:
        return m_float64 <= rhs.m_float64;

    case STRING:
        // TODO: can't compare string, it depends on locals.
        ITF_ASSERT(false);
        return false;

    case BLOB:
        // can't compare blob
        ITF_ASSERT(false);
        return false;

    default:
        ITF_ASSERT(false);
        return false;
    }
}

bool FlexibleValue::operator!=(const FlexibleValue& rhs) const
{
    ITF_ASSERT(m_type == rhs.m_type);
    if (m_type != rhs.m_type)
    {
        return false;
    }

    switch (m_type)
    {
    case INTEGER_32:
        return m_int32 != rhs.m_int32;           

    case INTEGER_64:
        return m_int64 != rhs.m_int64;           

    case UNSIGNED_INTEGER_32:
        return m_uint32 != rhs.m_uint32;           

    case UNSIGNED_INTEGER_64:
        return m_uint64 != rhs.m_uint64;   

    case FLOAT_32:
        return m_float32 != rhs.m_float32;

    case FLOAT_64:
        return m_float64 != rhs.m_float64;

    case STRING:
        return !(*m_pString == *rhs.m_pString);

    case BLOB:
        {            
            u32 lenth1 = m_pBlob->getSize();
            u32 lenth2 = rhs.getBlobValue().getSize();
            
            if( lenth1 == lenth2 )
            {
                int cmpEqual = memcmp(m_pBlob->getData(), rhs.getBlobValue().getData(), lenth1);
                if(cmpEqual == 0)
                    return false;
            }
            
            return true;
        }

    default:
        ITF_ASSERT(false);
        return false;
    }
}

void FlexibleValue::clear(bool clearType)
{
    switch (m_type)
    {
    case NOT_SET :
        break;

    case STRING :
        {
            delete m_pString;
            m_pString = NULL;
        }
        break;

    case INTEGER_32:
        {
            m_int32 = 0;
        }
        break;

    case UNSIGNED_INTEGER_32:
        {
            m_uint32 = 0;
        }
        break;

    case INTEGER_64:
        {
            m_int64 = 0;
        }
        break;

    case UNSIGNED_INTEGER_64:
        {
            m_uint64 = 0;
        }
        break;

    case FLOAT_32:
        {
            m_float32 = 0;
        }
        break;

    case FLOAT_64:
        {
            m_float64 = 0;
        }
        break;
    
    case BLOB :
        {
            delete m_pBlob;
            m_pBlob = NULL;
        }
        break;

    default:
        ITF_ASSERT(!"memory overwrite?");
        break;
    }
    if(clearType)
    {
        m_type = NOT_SET;
    }
}

void FlexibleValue::setType(FlexibleType type)
{
    ITF_ASSERT(NOT_SET == m_type); // you can't change type if type already assigned
    ITF_ASSERT(0 == m_int32); // you can't change type if data already assigned

    switch (type)
    {
    case NOT_SET :
        break;

    case STRING :
        {
            m_type = type;
            m_pString = new String();
        }
        break;

    case BLOB :
        {
            m_type = type;
            m_pBlob = new Blob(BLOB_EXTRACT_AND_WRITE_ENABLED);
        }
        break;

    case INTEGER_32:
    case INTEGER_64:
    case UNSIGNED_INTEGER_32:
    case UNSIGNED_INTEGER_64:
    case FLOAT_32:
    case FLOAT_64:
        {
            m_type = type;
        }
        break;
    
    default:
        ITF_ASSERT(!"Bad boy");
        break;
    }
}
void FlexibleValue::setInt32Value(i32 value)
{
    ITF_ASSERT(m_type == INTEGER_32);
    m_int32 = value;
}

void FlexibleValue::setFloat32Value(f32 value)
{
    ITF_ASSERT(m_type == FLOAT_32);
    m_float32 = value;
}

void FlexibleValue::setFloat64Value(f64 value)
{
    ITF_ASSERT(m_type == FLOAT_64);
    m_float64 = value;
}

void FlexibleValue::setUnsignedInt32Value(u32 value)
{
    ITF_ASSERT(m_type == UNSIGNED_INTEGER_32);
    m_uint32 = value;
}

u32 FlexibleValue::getUnsignedInt32Value() const
{
    ITF_ASSERT(m_type == UNSIGNED_INTEGER_32);
    return m_uint32;
}

f32 FlexibleValue::getFloat32Value() const
{
    ITF_ASSERT(m_type == FLOAT_32);
    return m_float32;
}

f64 FlexibleValue::getFloat64Value() const
{
    ITF_ASSERT(m_type == FLOAT_64);
    return m_float64;
}

void FlexibleValue::setUnsignedInt64Value(u64 value)
{
    ITF_ASSERT(m_type == UNSIGNED_INTEGER_64);
    m_uint64 = value;
}

u64 FlexibleValue::getUnsignedInt64Value() const
{
    ITF_ASSERT(m_type == UNSIGNED_INTEGER_64);
    return m_uint64;
}

i64 FlexibleValue::getInt64Value() const
{
    ITF_ASSERT(m_type == INTEGER_64);
    return m_int64;
}

void FlexibleValue::setInt64Value(i64 value)
{
    ITF_ASSERT(m_type == INTEGER_64);
    m_int64 = value;
}

void FlexibleValue::setStringValue(const String& value)
{
    ITF_ASSERT(m_type == STRING);
    *m_pString = value;
}

i32 FlexibleValue::getInt32Value() const
{
    ITF_ASSERT(m_type == INTEGER_32);
    return m_int32;
}

const String& FlexibleValue::getStringValue() const
{
    ITF_ASSERT(m_type == STRING);
    return *m_pString;
}

void FlexibleValue::setBlobValue(const Blob& value)
{
    ITF_ASSERT(m_type == BLOB);    
    if( value.getSize() > 0 )
    {
        m_pBlob->copyBlob(0, value);
    }
    else
    {
        m_pBlob->clear();
    }
}

Blob& FlexibleValue::getBlobValue() const
{
    ITF_ASSERT(m_type == BLOB);
    return *m_pBlob;
}

void FlexibleValue::negate()
{
    switch (m_type)
    {
    case INTEGER_32:
        m_int32 = -m_int32;
        break;

    case INTEGER_64:
        m_int64 = -m_int64;
        break;

    case FLOAT_32:
        m_float32 = -m_float32;
        break;

    case FLOAT_64:
        m_float64 = -m_float64;
        break;

    default:
        ITF_ASSERT(false);
        break;
    }
}

const FlexibleValue& FlexibleValue::operator=(const FlexibleValue& oValue)
{
    if (this != &oValue)
    {
        clear();
        setType(oValue.getType());
        
        switch(oValue.getType())
        {
            case NOT_SET:
                break;

            case STRING:
            {
                setStringValue(oValue.getStringValue());
            }
            break;

            case INTEGER_32:
            {
                setInt32Value(oValue.getInt32Value());
            }
            break;

            case INTEGER_64:
            {
                setInt64Value(oValue.getInt64Value());
            }
            break;

            case UNSIGNED_INTEGER_64:
            {
                setUnsignedInt64Value(oValue.getUnsignedInt64Value());
            }
            break;

            case FLOAT_32:
            {
                setFloat32Value(oValue.getFloat32Value());
            }
            break;

            case FLOAT_64:
            {
                setFloat64Value(oValue.getFloat64Value());
            }
            break;

            case BLOB:
            {
                setBlobValue(oValue.getBlobValue());
            }
            break;

            default:
                ITF_ASSERT(false);
                break;
            }
    }
    return *this;       
}

const String FlexibleValue::asStringValue() const
{
    switch (m_type)
    {
    case STRING:
        return getStringValue();

    case INTEGER_32:
        {
            String res;
            res.i32toa(m_int32);
            return res;
        }

    case UNSIGNED_INTEGER_32:
        {
            String res;
            res.ui32toa(m_uint32);
            return res;
        }

    case INTEGER_64:
        {
            String res;
            res.i64toa(m_int64);
            return res;
        }

    case UNSIGNED_INTEGER_64:
        {
            String res;
            res.ui64toa(m_uint64);
            return res;
        }

    case FLOAT_32:
        {
            String res;
            res.setTextFormat("%f", m_float32);
            return res;
        }    
    case FLOAT_64:
        {
            String res;
            res.setTextFormat("%f", m_float64);
            return res;
        }
    default:
        ITF_ASSERT(false);
        return "";
    }
}

i32 FlexibleValue::asInt32Value() const
{
    switch (m_type)
    {
    case STRING:
        return m_pString->atoi32();

    case INTEGER_32:
        return m_int32;

    case UNSIGNED_INTEGER_32:
        return (i32)m_uint32;

    case INTEGER_64:
        return (i32)m_int64;

    case UNSIGNED_INTEGER_64:
        return (i32)m_uint64;

    case FLOAT_32:
        return (i32)m_float32;

    case FLOAT_64:
        return (i32)m_float64;

    default:
        ITF_ASSERT(false);
        return 0;
    }
}

u32 FlexibleValue::asUnsignedInt32Value() const
{
    switch (m_type)
    {
    case STRING:
        return m_pString->atoui32();

    case INTEGER_32:
        return (u32)m_uint32;

    case UNSIGNED_INTEGER_32:
        return m_uint32;

    case INTEGER_64:
        return (u32)m_int64;

    case UNSIGNED_INTEGER_64:
        return (u32)m_uint64;

    case FLOAT_32:
        return (u32)m_float32;

    case FLOAT_64:
        return (u32)m_float64;

    default:
        ITF_ASSERT(false);
        return 0;
    }
}

i64 FlexibleValue::asInt64Value() const
{
    switch (m_type)
    {
    case STRING:
        return m_pString->atoi64();

    case INTEGER_32:
        return (i64)m_int32;

    case UNSIGNED_INTEGER_32:
        return (i64)m_uint32;

    case INTEGER_64:
        return m_int64;

    case UNSIGNED_INTEGER_64:
        return (i64)m_uint64;

    case FLOAT_32:
        return (i64)m_float32;

    case FLOAT_64:
        return (i64)m_float64;

    default:
        ITF_ASSERT(false);
        return 0;
    }
}

u64 FlexibleValue::asUnsignedInt64Value() const
{
    switch (m_type)
    {
    case STRING:
        return m_pString->atoui64();

    case INTEGER_32:
        return (u64)m_uint32;

    case UNSIGNED_INTEGER_32:
        return (u64)m_uint32;

    case INTEGER_64:
        return (u64)m_int64;

    case UNSIGNED_INTEGER_64:
        return m_uint64;

    case FLOAT_32:
        return (u64)m_float32;

    case FLOAT_64:
        return (u64)m_float64;
    
    default:
        ITF_ASSERT(false);
        return 0;
    }
}

f32 FlexibleValue::asFloat32Value() const
{
    switch (m_type)
    {
    case STRING:
        ITF_ASSERT(false);
        return 0;

    case INTEGER_32:
        return (f32)m_int32;

    case UNSIGNED_INTEGER_32:
        return (f32)m_uint32;

    case INTEGER_64:
        return (f32)m_int64;

    case UNSIGNED_INTEGER_64:
        return (f32)m_uint64;

    case FLOAT_32:
        return m_float32;

    case FLOAT_64:
        return (f32)m_float64;
    
    default:
        ITF_ASSERT(false);
        return 0;
    }
}

f64 FlexibleValue::asFloat64Value() const
{
    switch (m_type)
    {
    case STRING:
        ITF_ASSERT(false);
        return 0;

    case INTEGER_32:
        return (f64)m_uint32;

    case UNSIGNED_INTEGER_32:
        return (f64)m_uint32;

    case INTEGER_64:
        return (f64)m_int64;

    case UNSIGNED_INTEGER_64:
        return (f64)m_uint64;

    case FLOAT_32:
        return (f64)m_float32;

    case FLOAT_64:
        return m_float64;
    
    default:
        ITF_ASSERT(false);
        return 0;
    }
}


const FlexibleValue FlexibleValue::asValue(const FlexibleType newType) const
{
    FlexibleValue result;
    result.setType(newType);
    switch (newType)
    {
    case INTEGER_32:
        result.setInt32Value(asInt32Value());
        break;

    case UNSIGNED_INTEGER_32:
        result.setUnsignedInt32Value(asUnsignedInt32Value());
        break;

    case INTEGER_64:
        result.setInt64Value(asInt64Value());
        break;

    case UNSIGNED_INTEGER_64:
        result.setUnsignedInt64Value(asUnsignedInt64Value());
        break;

    case STRING:
        result.setStringValue(asStringValue());
        break;

    case FLOAT_32:
        result.setFloat32Value(asFloat32Value());
        break;

    case FLOAT_64:
        result.setFloat64Value(asFloat64Value());
        break;

    default:
        ITF_ASSERT(false);
    }
    return result;
}

void FlexibleValue::serialize(u32 offset, Blob& blob) const
{
    switch (m_type)
    {
    case INTEGER_32:
        blob.addInt32(offset, m_int32);
        break;

    case UNSIGNED_INTEGER_32:
        blob.addUInt32(offset, m_uint32);
        break;

    case INTEGER_64:
        blob.addInt64(offset, m_int64);
        break;

    case UNSIGNED_INTEGER_64:
        blob.addUInt64(offset, m_uint64);
        break;

    case STRING:
        blob.addString(offset, getStringValue());
        break;

    case FLOAT_32:
        blob.addFloat32(offset, m_float32);
        break;

    case FLOAT_64:
        blob.addFloat64(offset, m_float64);
        break;

    case BLOB:
        {            
            blob.copyBlob(offset, getBlobValue() );
            blob.addUInt32(offset, getBlobValue().getSize());
            break;
        }

    default:
        ITF_ASSERT(!"You can't serialize that flexible value - unknown data type");
    }

    blob.addUInt32(offset, (u32)getType());

    blob.addBlobDataType(offset, DataType_FlexibleValue);
}

void FlexibleValue::unserialize(Blob& blob)
{
#if ITF_DEBUG_LEVEL > 0
    ITF_ASSERT(blob.extractBlobDataType(0) == DataType_FlexibleValue);
#else
    blob.extractBlobDataType(0);
#endif

    m_type = (FlexibleType)blob.extractUint32(0);

    switch(m_type)
    {
    case INTEGER_32:
        m_int32 = blob.extractInt32(0);
        break;
    case INTEGER_64:
        m_int64 = blob.extractInt64(0);
        break;
    case FLOAT_32:
        m_float32 = blob.extractFloat32(0);
        break;
    case FLOAT_64:
        m_float64 = blob.extractFloat64(0);
        break;
    case UNSIGNED_INTEGER_32:
        m_uint32 = blob.extractUint32(0);
        break;
    case UNSIGNED_INTEGER_64:
        m_uint64 = blob.extractUint64(0);
        break;
    case STRING:
        {
            m_pString = new String();
            String str;
            blob.extractString(str, 0);
            setStringValue(str);
        }
        break;

    case BLOB:
        {
            u32 blobSize = blob.extractUint32(0);            
            u8* ptrBlob = blob.extract(0,blobSize);
            
            m_pBlob->clear();
            m_pBlob->add(0, ptrBlob, blobSize);
            SF_DEL_ARRAY(ptrBlob);
        }
        break;

    default:
        ITF_ASSERT(false);
    }
}

} // namespace ITF
