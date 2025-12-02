#include "core/AdaptersInterfaces/FileManager.h"
#include <mkvparser/mkvparser.h>

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
            return m_file != nullptr;
        }

        void close()
        {
            if (m_file)
            {
                FILEMANAGER->closeFile(m_file);
                m_file = nullptr;
            }
        }

        int Read(long long pos, long len, unsigned char* buf) override
        {
            if (!m_file)
                return -1;

            m_file->seek((u64)pos, 0);

            u32 bytesRead = 0;
            m_file->read(buf, (u32)len, &bytesRead);

            return (bytesRead > 0) ? 0 : -1;
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
        File* m_file = nullptr;
    };
}
