#pragma once

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"  
#endif //_ITF_BLOB_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"  
#endif //_ITF_BLOBUTILS_H_

public ref class blobWrapper
{

public:
    blobWrapper();

    virtual ~blobWrapper();

    void copyFrom(blobWrapper^ _blobSource);

    void pushString8(String^ _value);
    void extractString8(String^% _value);

    void extract(String^% _value);
    void extract(StringBuilder^% _value);
    void push(String^ _value);

    void extract(__int32% _value);
    void push(__int32 _value);

    void extract(unsigned __int32% _value);
    void push(unsigned __int32 _value);

    void extract(bool% _value);
    void push(bool _value);

    void extract(__int64% _value);
    void push(__int64 _value);


    void extract(unsigned __int64% _value);
    void push(unsigned __int64 _value);

    void pushBytes(System::Byte*  _value,unsigned __int32 _size);

    void extract(float% _value);
    void push(float _value);

    ITF::u8* extractBinary(unsigned __int32% _size);
        
    ITF::Blob* getBlob() {return m_pBlob;}
    void attachBlob(ITF::Blob* _pBlob) {m_pBlob = _pBlob;}

    void extract(List<String^>^ vString);
    void push(List<String^>^ vString);
    
    void extract(List<__int64>^ vI64);
    void push(List<__int64>^ vI64);

    IntPtr getPtr() {return (IntPtr)(m_pBlob);}

    bool sendToHost();

    void reset();

protected:

    ITF::Blob* m_pBlob;
};

