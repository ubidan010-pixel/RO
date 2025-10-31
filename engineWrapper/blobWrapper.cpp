
// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "../PluginGateway/PluginGateway.h"
#endif // _ITF_PLUGINGATEWAY_H_

#include "engineWrapper.h"
#include "utils.h"



blobWrapper::blobWrapper()
{
    m_pBlob = new ITF::Blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
};

ITF::u8* blobWrapper::extractBinary(unsigned __int32% _size)
{
    ITF::u32 size = 0;
    ITF::u8* pBuf = m_pBlob->extractBinaryBlock(m_pBlob->getSeekPos(),size);
    _size = size;
    return pBuf;
}


void blobWrapper::pushBytes(System::Byte* _value,unsigned __int32 _size)
{
    m_pBlob->pushBinaryBlock(_value,_size);
}

void blobWrapper::extract(String^% _value)
{
    ITF::String extracted;
    m_pBlob->extractString(extracted);
    _value = gcnew String((wchar_t*)extracted.cStr());
}

void blobWrapper::extract(StringBuilder^% _value)
{
    ITF::String extracted;
    m_pBlob->extractString(extracted);
    _value->Append(Marshal::PtrToStringUni(IntPtr((wchar_t*)extracted.cStr())));
}

void blobWrapper::push(String^ _value)
{   
    m_pBlob->pushString(utils::getITFString(_value));
}

void blobWrapper::extractString8(String^% _value)
{
    ITF::String8 extracted;
    m_pBlob->extractString8(extracted);
    _value = gcnew String(extracted.cStr());
}

void blobWrapper::pushString8(String^ _value)
{   
    m_pBlob->pushString8(utils::getCharCopy(_value));
}

void blobWrapper::copyFrom(blobWrapper^ _blobSource)
{
    ITF_ASSERT(m_pBlob);
    m_pBlob->addBlob(0,*_blobSource->getBlob());
}

void blobWrapper::extract(unsigned __int32% _value)
{
    _value = m_pBlob->extractUint32();
}

void blobWrapper::push(unsigned __int32 _value)
{
    m_pBlob->pushUInt32(_value);
}

void blobWrapper::extract(__int32% _value)
{
    _value = m_pBlob->extractInt32();
}

void blobWrapper::push(__int32 _value)
{
    m_pBlob->pushInt32(_value);
}

void blobWrapper::extract(bool% _value)
{
    _value = (m_pBlob->extractBool() == ITF::btrue);
}

void blobWrapper::push(bool _value)
{
    m_pBlob->pushBool(_value);
}


void blobWrapper::push(__int64 _value)
{
    m_pBlob->pushInt64(_value);
}

void blobWrapper::extract(__int64% _value)
{
    _value = m_pBlob->extractInt64();
}


void blobWrapper::push(unsigned __int64 _value)
{
    m_pBlob->pushUInt64(_value);
}

void blobWrapper::extract(unsigned __int64% _value)
{
    _value = m_pBlob->extractUint64();
}


void blobWrapper::push(float _value)
{
    m_pBlob->pushFloat32(_value);
}

void blobWrapper::extract(float% _value)
{
    _value = m_pBlob->extractFloat32();
}


blobWrapper::~blobWrapper()
{
    SF_DEL(m_pBlob);
}

bool blobWrapper::sendToHost()
{
    if(!ITF::PluginGateWay::getSingleton())
        return false;
    return ITF::PluginGateWay::getSingleton()->sendToHost(*m_pBlob) != 0;
}


void blobWrapper::extract(List<String^>^ vString)
{
    ITF_VECTOR<ITF::String> vValues;
    extractVector(m_pBlob,vValues);

    for (ITF_VECTOR<ITF::String> ::iterator iter = vValues.begin();
        iter != vValues.end();++iter)
    {
        String ^ str = gcnew String((wchar_t *)(*iter).cStr());
        vString->Add(str);        
    }
}

void blobWrapper::extract(List<Int64>^ vI64)
{
    ITF_VECTOR<ITF::i64> vValues;
    extractVector(m_pBlob,vValues);

    for (ITF_VECTOR<ITF::i64> ::iterator iter = vValues.begin();
        iter != vValues.end();++iter)
    {
        vI64->Add((*iter));
    }
}

void blobWrapper::push(List<__int64>^ vI64)
{
    ITF_VECTOR<ITF::i64> vValues;
    vValues.reserve(vI64->Count);

    for (int index = 0;index<vI64->Count;++index)
    {
        vValues.push_back(vI64[index]);
    }

    pushVector(m_pBlob,vValues);
    
}

void blobWrapper::push(List<String^>^ vString)
{
    ITF_VECTOR<ITF::String> vValues;
    vValues.reserve(vString->Count);

    for (int index = 0;index<vString->Count;++index)
    {
        vValues.push_back(utils::getITFString(vString[index]));
    }

    pushVector(m_pBlob,vValues);

}

void blobWrapper::reset()
{
    if (m_pBlob)
        m_pBlob->setSeekPos(0);
}