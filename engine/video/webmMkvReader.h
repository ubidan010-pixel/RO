#include "core/AdaptersInterfaces/FileManager.h"
#include <mkvparser/mkvparser.h>

#include <vector>
#include <algorithm>
#include <cstring>

namespace ITF
{
    class webmMkvReader : public mkvparser::IMkvReader
    {
    public:
        webmMkvReader() = default;
        ~webmMkvReader() override { close(); }

        bool open(const String& _path)
        {
            close();
            m_file = FILEMANAGER->openFile(_path, ITF_FILE_ATTR_READ);
            if (!m_file)
                return false;

            if (m_cacheCapacity == 0)
                m_cacheCapacity = 1024 * 1024; // default 1MB

            m_cache.resize(m_cacheCapacity);
            m_cacheStart = 0;
            m_cacheValid = 0;
            return true;
        }

        void close()
        {
            if (m_file)
            {
                FILEMANAGER->closeFile(m_file);
                m_file = nullptr;
            }

            m_cache.clear();
            m_cache.shrink_to_fit();

            m_cacheStart = 0;
            m_cacheValid = 0;
        }

        void setCacheSize(size_t bytes)
        {
            m_cacheCapacity = std::max<size_t>(bytes, 64 * 1024);

            if (m_file)
            {
                m_cache.resize(m_cacheCapacity);
                m_cacheStart = 0;
                m_cacheValid = 0;
            }
        }

        int Read(long long pos, long len, unsigned char* buf) override
        {
            if (!m_file || !buf || len <= 0 || pos < 0)
                return -1;

            const u64 upos = (u64)pos;
            const u32 ulen = (u32)len;

            if (isInCache(upos, ulen))
            {
                const size_t offset = (size_t)(upos - m_cacheStart);
                std::memcpy(buf, m_cache.data() + offset, ulen);
                return 0;
            }

            if (!refillCache(upos))
                return -1;

            if (isInCache(upos, ulen))
            {
                const size_t offset = (size_t)(upos - m_cacheStart);
                std::memcpy(buf, m_cache.data() + offset, ulen);
                return 0;
            }

            m_file->seek(upos, 0);

            u32 bytesRead = 0;
            m_file->read(buf, ulen, &bytesRead);

            return (bytesRead == ulen) ? 0 : -1;
        }

        int Length(long long* total, long long* available) override
        {
            if (!m_file)
                return -1;

            const u64 size = m_file->getLength();
            if (total)     *total = (long long)size;
            if (available) *available = (long long)size;
            return 0;
        }

    private:
        bool isInCache(u64 pos, u32 len) const
        {
            if (m_cacheValid == 0)
                return false;

            if (pos < m_cacheStart)
                return false;

            const u64 end = pos + (u64)len;
            const u64 cacheEnd = m_cacheStart + (u64)m_cacheValid;
            return end <= cacheEnd;
        }

        bool refillCache(u64 startPos)
        {
            if (!m_file || m_cache.empty())
                return false;

            const u64 aligned = startPos & ~(u64)(64 * 1024 - 1);

            m_file->seek(aligned, 0);

            u32 bytesRead = 0;
            const u32 toRead = (u32)m_cache.size();
            m_file->read(m_cache.data(), toRead, &bytesRead);

            m_cacheStart = aligned;
            m_cacheValid = bytesRead;
            return bytesRead > 0;
        }

    private:
        File* m_file = nullptr;

        size_t m_cacheCapacity = 0;
        std::vector<unsigned char> m_cache;
        u64 m_cacheStart = 0;
        u32 m_cacheValid = 0;
    };
}
