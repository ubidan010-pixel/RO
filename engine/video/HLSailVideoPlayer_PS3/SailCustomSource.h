#ifndef SAIL_CUSTOM_SOURCE_H_
#define SAIL_CUSTOM_SOURCE_H_

#include <cell/sail.h>

namespace cell {
namespace Sail {

class CustomSource
{
public:
    CustomSource();
    ~CustomSource();

    void registerProtocolToPlayer(CellSailPlayer & _player);
    bool check(const char * pUri); // perform validity check -> ensure overloading of method coherency

    // methods to overload
    virtual const char * getProtocolName() const = 0;
    virtual int makeup(const char * pProtocol) = 0;
    virtual int cleanup() = 0;
    virtual void open(int streamType, void * pMediaInfo, const char * pUri, const CellSailSourceStreamingProfile * pProfile) = 0;
    virtual void close() = 0;
    virtual void start(const CellSailSourceStartCommand * pCommand, uint32_t sessionId) = 0;
    virtual void stop() = 0;
    virtual void cancel() = 0;
    virtual int checkout(const CellSailSourceBufferItem ** ppItem) = 0;
    virtual int checkin(const CellSailSourceBufferItem * pItem) = 0;
    virtual int clear() = 0;
    virtual void read(int streamType, void * pMediaInfo, const char * pUri, uint64_t offset, uint8_t * pBuf, size_t size, uint64_t * pTotalSize) = 0;
    virtual int readSync(int streamType, void * pMediaInfo, const char * pUri, uint64_t offset, uint8_t * pBuf, size_t size, uint64_t * pTotalSize) = 0;
    virtual int getCapabilities(int streamType, void * pMediaInfo, const char * pUri, uint64_t * pCapability) = 0;
    virtual int inquireCapability(int streamType, void * pMediaInfo, const char * pUri, const CellSailSourceStartCommand * pCommand) = 0;

    // access to the private member
    CellSailSource & getSource() { return m_sailSource; }
    const CellSailSource & getSource() const { return m_sailSource; }

private:
    CellSailSource m_sailSource;
};

} // namespace Sail
} // namespace cell

#endif // SAIL_CUSTOM_SOURCE_H_
