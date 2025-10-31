#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_NETADAPTER_H_
#include "engine/AdaptersInterfaces/NetAdapter.h"
#endif //_ITF_NETADAPTER_H_

#include "pluginWrapper.h"

bool pluginWrapper::dispatch(blobWrapper^ blob)
{
    ITF::Blob* gotBlob = m_plugin->getFrontBlob();
    blob->attachBlob(gotBlob);
    return (gotBlob != NULL);
}

void PluginReceiver::receive        (   ITF::Blob& _blob, const ITF::NETPeer* _peer, ITF::u32 _senderID)
{
    //Console::WriteLine("{0}, {1}",_blob.getSize(),_blob.getSeekPos());
    ITF::u32 position   = _blob.getSeekPos();
    ITF::u32 size       = _blob.getSize()-position;
    ITF::u8* data       = NULL;

    if ( _blob.getMode() == ITF::BLOB_EXTRACT_AND_WRITE_ENABLED )
        data = reinterpret_cast< ITF::u8* >( _blob.getData() ) + position;
    else
    {
        data = new ITF::u8[size];
        memcpy(data,((ITF::u8*) (_blob.getData())+position),size);
    }

    ITF::Blob* newBlob = new ITF::Blob(data,size,_blob.getMode());

    if ( newBlob->getMode() == ITF::BLOB_READONLY )
        newBlob->enableMemoryResponsability();
    m_vBlobs.push_back(newBlob);
    _blob.setSeekPos(_blob.getSize());
}

ITF::Blob* PluginReceiver::getFrontBlob()
{
    if (m_vBlobs.size())
    {
        ITF_VECTOR<ITF::Blob*>::iterator iter =m_vBlobs.begin();
        ITF::Blob* blob = (*iter);
        m_vBlobs.erase(iter);
        return blob;
    }

    return NULL;
}
