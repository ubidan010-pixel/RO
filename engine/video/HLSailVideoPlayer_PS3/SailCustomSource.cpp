#include "precompiled_engine.h"

#include "SailCustomSource.h"
#include <cell/sail/source_check.h>
#include "core/system/PS3/ModuleManager_ps3.h"

namespace cell {
namespace Sail {

    static int sourceMakeup(CustomSource * pSelf, const char * pProtocol)
    {
        return pSelf->makeup(pProtocol);
    }

    static int sourceCleanup(CustomSource * pSelf)
    {
        return pSelf->cleanup();
    }

    static void sourceOpen(  CustomSource * pSelf,
        int streamType,
        void * pMediaInfo,
        const char * pUri,
        const CellSailSourceStreamingProfile * pProfile)
    {
        pSelf->open(streamType, pMediaInfo, pUri, pProfile);
    }

    static void sourceClose(CustomSource * pSelf)
    {
        pSelf->close();
    }

    static void sourceStart(CustomSource * pSelf,
        const CellSailSourceStartCommand * pCommand,
        uint32_t sessionId)
    {
        pSelf->start(pCommand, sessionId);
    }

    static void sourceStop(CustomSource * pSelf)
    {
        pSelf->stop();
    }

    static void sourceCancel(CustomSource * pSelf)
    {
        pSelf->cancel();
    }

    static int sourceCheckout(CustomSource * pSelf,
        const CellSailSourceBufferItem ** ppItem)
    {
        return pSelf->checkout(ppItem);
    }

    static int sourceCheckin(CustomSource *pSelf,
        const CellSailSourceBufferItem * pItem)
    {
        return pSelf->checkin(pItem);
    }

    static int sourceClear(CustomSource * pSelf)
    {
        return pSelf->clear();
    }

    static void sourceRead(CustomSource * pSelf,
        int streamType,
        void * pMediaInfo,
        const char * pUri,
        uint64_t offset,
        uint8_t * pBuf,
        size_t size,
        uint64_t * pTotalSize)
    {
        pSelf->read(streamType, pMediaInfo, pUri, offset, pBuf, size, pTotalSize);
    }

    static int sourceReadSync(CustomSource * pSelf,
        int streamType,
        void * pMediaInfo,
        const char * pUri,
        uint64_t offset,
        uint8_t * pBuf,
        size_t size,
        uint64_t * pTotalSize)
    {
        return pSelf->readSync(streamType, pMediaInfo, pUri, offset, pBuf, size, pTotalSize);
    }

    static int sourceGetCapabilities(CustomSource *pSelf,
        int streamType,
        void * pMediaInfo,
        const char * pUri,
        uint64_t * pCapability)
    {
        return pSelf->getCapabilities(streamType, pMediaInfo, pUri, pCapability);
    }

    static int sourceInquireCapability( CustomSource * pSelf,
        int streamType, 
        void * pMediaInfo,
        const char * pUri,
        const CellSailSourceStartCommand * pCommand)
    {
        return pSelf->inquireCapability(streamType, pMediaInfo, pUri, pCommand);
    }

    static const CellSailSourceFuncs SourceMemFuncs =
    {
        (CellSailSourceFuncMakeup)   sourceMakeup,
        (CellSailSourceFuncCleanup)  sourceCleanup,
        (CellSailSourceFuncOpen)     sourceOpen,
        (CellSailSourceFuncClose)    sourceClose,
        (CellSailSourceFuncStart)    sourceStart,
        (CellSailSourceFuncStop)     sourceStop,
        (CellSailSourceFuncCancel)   sourceCancel,
        (CellSailSourceFuncCheckout) sourceCheckout,
        (CellSailSourceFuncCheckin)  sourceCheckin,
        (CellSailSourceFuncClear)    sourceClear,
        (CellSailSourceFuncRead)     sourceRead,
        (CellSailSourceFuncReadSync) sourceReadSync,
        (CellSailSourceFuncGetCapabilities) sourceGetCapabilities,
        (CellSailSourceFuncInquireCapability) sourceInquireCapability,
    };

    CustomSource::CustomSource()
    {
        ITF::MODULE_MANAGER->loadModule(CELL_SYSMODULE_SAIL);
        ITF_VERIFY(CELL_OK == cellSailSourceInitialize(&getSource(), &SourceMemFuncs, this));
    }

    CustomSource::~CustomSource()
    {
       ITF_VERIFY(CELL_OK == cellSailSourceFinalize(&getSource()));
       ITF::MODULE_MANAGER->unloadModule(CELL_SYSMODULE_SAIL);
    }

    void CustomSource::registerProtocolToPlayer(CellSailPlayer & _player)
    {
        const char * protocolName = getProtocolName();
        ITF_ASSERT(protocolName!=NULL);
        ITF_ASSERT(strlen(protocolName) < CELL_SAIL_MAX_LENGTH_PROTOCOL_NAME_WITH_NULL_TERMINATE);
        ITF_VERIFY(CELL_OK == cellSailPlayerRegisterSource(&_player, getProtocolName(), &getSource()));
    }

    bool CustomSource::check(const char * pUri)
    {
        CellSailSourceCheckResource res;
        memset(&res, 0, sizeof(res));

        res.ok.streamType = CELL_SAIL_STREAM_PAMF;
        res.ok.pMediaInfo = NULL;
        res.ok.pUri        = pUri;
        int ret;
        ret = cellSailSourceCheck(  &getSource(),
                                    &SourceMemFuncs,
                                    this,
                                    getProtocolName(),
                                    &res,
                                    0,
                                    0);
        return (ret >= 0);
    }

} // namespace Sail
} // namespace cell
