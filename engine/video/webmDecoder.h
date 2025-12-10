#include "core/types.h"
#include "engine/video/webmMkvReader.h"

#include <vpx/vpx_decoder.h>
#include <mkvparser/mkvparser.h>
#include <thread>

namespace ITF
{
    class webmDecoder
    {
    public:
        webmDecoder();
        ~webmDecoder();

        bool open(const String& _path);
        void close();

        bool isValid() const { return m_codecInited && m_videoTrack != nullptr; }

        u32 getWidth()  const { return m_width; }
        u32 getHeight() const { return m_height; }
        f64 getDurationSeconds() const { return m_duration; }

        bool decodeNextFrame(const vpx_image_t*& outImage, f64& outPtsSeconds);
        bool reset();

    private:
        bool initSegment();
        bool buildFrameIndex();
        bool initDecoder();

        enum class CodecType
        {
            Unknown,
            VP8,
            VP9
        };

        struct FrameIndexEntry
        {
            long long time_ns = 0;
            long long pos = 0;
            long len = 0;
        };

    private:
        webmMkvReader m_reader;
        mkvparser::Segment* m_segment = nullptr;
        const mkvparser::Cluster* m_cluster = nullptr;
        const mkvparser::BlockEntry* m_blockEntry = nullptr;

        const mkvparser::Track* m_videoTrack = nullptr;
        unsigned long m_videoTrackNumber = 0;

        long long m_timecodeScale = 1000000;
        u32 m_width = 0;
        u32 m_height = 0;
        f64 m_duration = 0.0;

        vpx_codec_ctx_t m_codec{};
        bool m_codecInited = false;
        CodecType m_codecType = CodecType::Unknown;

        std::vector<uint8_t> m_frameBuffer;

        ITF_VECTOR<FrameIndexEntry> m_frameIndex;
        size_t m_currentFrame = 0;
    };
}
