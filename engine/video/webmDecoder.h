#include "core/types.h"
#include "engine/video/webmMkvReader.h"

#include <vpx/vpx_decoder.h>
#include <mkvparser/mkvparser.h>

namespace ITF
{
    class webmDecoder
    {
    public:
        enum class DecodeResult
        {
            Ok,         // got frame
            Buffering,  // not an error: waiting for index to have enough frames
            End,        // end of stream
            Error       // fatal decode/read/parser error
        };

        webmDecoder();
        ~webmDecoder();

        bool open(const String& _path, f64 minBufferSeconds = 1.5);
        void close();

        bool isValid() const { return m_codecInited && (m_videoTrackNumber != 0) && (m_segment != nullptr); }

        u32 getWidth() const { return m_width; }
        u32 getHeight() const { return m_height; }
        f64 getDurationSeconds() const { return m_duration; }

        bool isReady() const;

        DecodeResult decodeNextFrame(const vpx_image_t*& outImage, f64& outPtsSeconds);

        bool reset();

    private:
        bool initSegment();
        void startIndexBuild();
        void pumpIndexBuild(u32 budgetUs);
        bool stepIndexOneBlock();
        bool initDecoder();

        size_t estimateFrameCount() const;

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

        const mkvparser::Track* m_videoTrack = nullptr;
        unsigned long m_videoTrackNumber = 0;

        long long m_timecodeScale = 1000000;
        u32 m_width = 0;
        u32 m_height = 0;
        f64 m_duration = 0.0;

        CodecType m_codecType = CodecType::Unknown;

        vpx_codec_ctx_t m_codec{};
        bool m_codecInited = false;

        std::vector<uint8_t> m_frameBuffer;

        ITF_VECTOR<FrameIndexEntry> m_frameIndex;
        size_t m_currentFrame = 0;

        bool m_indexBuilding = false;
        bool m_indexDone = false;
        const mkvparser::Cluster* m_indexCluster = nullptr;
        const mkvparser::BlockEntry* m_indexBlockEntry = nullptr;

        u32 m_minBufferedFrames = 0;
        double m_fpsGuess = 30.0;

        u32 m_budgetBufferingUs = 12000; // aggressive while buffering first frames
        u32 m_budgetSteadyUs = 1500;  // light maintenance once playback started
    };
}
