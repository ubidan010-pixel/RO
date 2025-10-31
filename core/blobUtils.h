#ifndef _ITF_BLOBUTILS_H_
#define _ITF_BLOBUTILS_H_

namespace ITF
{
//#define USE_BLOBTESTTEMPLATE 1

ITF_INLINE void pushValue(Blob* _blob,const String& _value)
{
    _blob->pushString(_value);
}

ITF_INLINE void extractValue(Blob* _blob,String& _value)
{
    _blob->extractString(_value);
}

ITF_INLINE void pushValue(Blob* _blob,const i8& _value)
{
    _blob->pushInt32(_value);
}

ITF_INLINE void extractValue(Blob* _blob,i8& _value)
{
    _value = _blob->extractInt8();
}

ITF_INLINE void pushValue(Blob* _blob,const u8& _value)
{
    _blob->pushUInt8(_value);
}


ITF_INLINE void extractValue(Blob* _blob,u8& _value)
{
    _value = _blob->extractUint8();
}

ITF_INLINE void pushValue(Blob* _blob,const i16& _value)
{
    _blob->pushInt16(_value);
}

ITF_INLINE void extractValue(Blob* _blob,i16& _value)
{
    _value = _blob->extractInt16();
}


ITF_INLINE void pushValue(Blob* _blob,const u16& _value)
{
    _blob->pushUInt16(_value);
}

ITF_INLINE void extractValue(Blob* _blob,u16& _value)
{
    _value = _blob->extractUint16();
}

ITF_INLINE void pushValue(Blob* _blob,const i32& _value)
{
    _blob->pushUInt32(_value);
}

ITF_INLINE void extractValue(Blob* _blob,i32& _value)
{
    _value = _blob->extractUint32();
}

ITF_INLINE void pushValue(Blob* _blob,const u32& _value)
{
    _blob->pushUInt32(_value);
}

ITF_INLINE void extractValue(Blob* _blob,u32& _value)
{
    _value = _blob->extractUint32();
}

ITF_INLINE void pushValue(Blob* _blob,const f32& _value)
{
    _blob->pushFloat32(_value);
}

ITF_INLINE void extractValue(Blob* _blob,f32& _value)
{
    _value = _blob->extractFloat32();
}

ITF_INLINE void pushValue(Blob* _blob,const f64& _value)
{
    _blob->pushFloat64(_value);
}

ITF_INLINE void extractValue(Blob* _blob,f64& _value)
{
    _value = _blob->extractFloat64();
}

ITF_INLINE void pushValue(Blob* _blob,const i64& _value)
{
    _blob->pushInt64(_value);
}

ITF_INLINE void extractValue(Blob* _blob,i64& _value)
{
    _value = _blob->extractInt64();
}

ITF_INLINE void pushValue(Blob* _blob,const u64& _value)
{
    _blob->pushUInt64(_value);
}

ITF_INLINE void extractValue(Blob* _blob,u64& _value)
{
    _value = _blob->extractUint64();
}

template <class T>
void pushVector(Blob* _blob, const ITF_VECTOR<T>& _vector)
{
    _blob->pushUInt32(_vector.size());

    for (const T & value : _vector)
    {
        pushValue(_blob, value);
    }
}

template <class T> void extractVector(Blob* _blob,ITF_VECTOR<T>& _vector)
{
    ITF_ASSERT(_vector.size()==0);
    u32 size = _blob->extractUint32();
    _vector.reserve(size);
    T mytype;

    for (u32 index=0;index<size;++index)
    {
        extractValue(_blob,mytype);
        _vector.push_back(mytype);
    }
}

extern void testTemplateBlob();
}//namespace ITF

#endif 
