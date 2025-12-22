#include "precompiled_engine.h"
#include "engine/video/webmDecoder.h"

#include <vpx/vp8dx.h>
#include <vpx/vpx_decoder.h>

#include <chrono>
#include <algorithm>

static inline unsigned long long ITF_TIME_US()
{
    using namespace std::chrono;
    return (unsigned long long)duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

namespace ITF
{
    webmDecoder::webmDecoder()
    {
        memset(&m_codec, 0, sizeof(m_codec));
    }

    webmDecoder::~webmDecoder()
    {
        close();
    }

    bool webmDecoder::open(const String& _path, f64 minBufferSeconds)
    {
        close();

        if (!m_reader.open(_path))
            return false;

        m_reader.setCacheSize(512 * 1024);

        if (!initSegment())
            return false;

        if (!initDecoder())
            return false;

        m_reader.setCacheSize(2 * 1024 * 1024);

        startIndexBuild();

        const double fps = m_fpsGuess;
        m_minBufferedFrames = (u32)std::max(1.0, minBufferSeconds * fps);

        m_currentFrame = 0;
        return true;
    }

    void webmDecoder::close()
    {
        if (m_codecInited)
        {
            vpx_codec_destroy(&m_codec);
            m_codecInited = false;
        }

        if (m_segment)
        {
            delete m_segment;
            m_segment = nullptr;
        }

        m_videoTrack = nullptr;
        m_videoTrackNumber = 0;
        m_width = m_height = 0;
        m_duration = 0.0;
        m_codecType = CodecType::Unknown;

        m_frameIndex.clear();
        m_currentFrame = 0;

        m_indexBuilding = false;
        m_indexDone = false;
        m_indexCluster = nullptr;
        m_indexBlockEntry = nullptr;

        m_minBufferedFrames = 0;

        m_reader.close();
    }

    bool webmDecoder::reset()
    {
        m_currentFrame = 0;
        return !m_frameIndex.empty() || m_indexBuilding || m_indexDone;
    }

    bool webmDecoder::isReady() const
    {
        return m_frameIndex.size() >= (size_t)m_minBufferedFrames;
    }

    size_t webmDecoder::estimateFrameCount() const
    {
        if (m_duration <= 0.0)
            return 2048;

        const double est = m_duration * m_fpsGuess + 256.0;
        return (size_t)std::max(1024.0, est);
    }

    bool webmDecoder::initSegment()
    {
        long long pos = 0;
        mkvparser::EBMLHeader ebmlHeader;
        long long ret = ebmlHeader.Parse(&m_reader, pos);
        if (ret < 0)
            return false;
        pos = ret;

        ret = mkvparser::Segment::CreateInstance(&m_reader, pos, m_segment);
        if (ret != 0 || !m_segment)
            return false;

        if (m_segment->Load() < 0)
            return false;

        const mkvparser::SegmentInfo* info = m_segment->GetInfo();
        m_timecodeScale = info->GetTimeCodeScale();
        m_duration = info->GetDuration() / 1000000000.0;

        const mkvparser::Tracks* tracks = m_segment->GetTracks();
        const unsigned long trackCount = tracks->GetTracksCount();

        m_videoTrack = nullptr;
        m_videoTrackNumber = 0;
        m_codecType = CodecType::Unknown;

        for (unsigned long i = 0; i < trackCount; ++i)
        {
            const mkvparser::Track* const t = tracks->GetTrackByIndex(i);
            if (!t)
                continue;
            if (t->GetType() != mkvparser::Track::kVideo)
                continue;

            const char* codecId = t->GetCodecId();

            CodecType detected = CodecType::Unknown;
            if (codecId)
            {
                if (strcmp(codecId, "V_VP8") == 0)
                    detected = CodecType::VP8;
                else if (strcmp(codecId, "V_VP9") == 0)
                    detected = CodecType::VP9;
                else
                    continue;
            }
            else
            {
                detected = CodecType::VP8;
            }

            m_codecType = detected;
            m_videoTrack = t;
            m_videoTrackNumber = t->GetNumber();

            const auto* const vt = static_cast<const mkvparser::VideoTrack*>(t);
            m_width = (u32)vt->GetWidth();
            m_height = (u32)vt->GetHeight();
            break;
        }

        return (m_videoTrack != nullptr && m_videoTrackNumber != 0 && m_codecType != CodecType::Unknown);
    }

    bool webmDecoder::initDecoder()
    {
        vpx_codec_iface_t* iface = nullptr;

        switch (m_codecType)
        {
        case CodecType::VP8:
            iface = vpx_codec_vp8_dx();
            break;
        case CodecType::VP9:
            iface = vpx_codec_vp9_dx();
            break;
        default:
            return false;
        }

        if (!iface)
            return false;

        vpx_codec_dec_cfg_t cfg{};
        cfg.w = m_width;
        cfg.h = m_height;
        cfg.threads = 2;

        unsigned int flags = 0;
        const vpx_codec_caps_t caps = vpx_codec_get_caps(iface);
        if (caps & VPX_CODEC_CAP_FRAME_THREADING)
            flags |= VPX_CODEC_USE_FRAME_THREADING;

        const vpx_codec_err_t err = vpx_codec_dec_init_ver(&m_codec, iface, &cfg, flags, VPX_DECODER_ABI_VERSION);

        if (err != VPX_CODEC_OK)
            ITF_ASSERT_MSG(0, "vpx_codec_dec_init_ver failed: %s\n", vpx_codec_err_to_string(err));

        m_codecInited = true;
        return true;
    }

    void webmDecoder::startIndexBuild()
    {
        m_frameIndex.clear();
        m_frameIndex.reserve(estimateFrameCount());

        m_indexCluster = m_segment ? m_segment->GetFirst() : nullptr;
        m_indexBlockEntry = nullptr;

        m_indexDone = false;
        m_indexBuilding = (m_indexCluster && !m_indexCluster->EOS());

        if (m_indexBuilding)
        {
            const mkvparser::BlockEntry* first = nullptr;
            long st = m_indexCluster->GetFirst(first);
            if (st >= 0)
                m_indexBlockEntry = first;
        }
    }

    bool webmDecoder::stepIndexOneBlock()
    {
        if (!m_indexBuilding || m_indexDone || !m_segment)
            return false;

        mkvparser::Segment* segment = m_segment;

        while (m_indexCluster && !m_indexCluster->EOS() && (!m_indexBlockEntry || m_indexBlockEntry->EOS()))
        {
            m_indexCluster = segment->GetNext(m_indexCluster);
            if (!m_indexCluster || m_indexCluster->EOS())
            {
                m_indexDone = true;
                m_indexBuilding = false;
                return false;
            }

            const mkvparser::BlockEntry* first = nullptr;
            long st = m_indexCluster->GetFirst(first);
            if (st < 0)
            {
                m_indexDone = true;
                m_indexBuilding = false;
                return false;
            }
            m_indexBlockEntry = first;
        }

        if (!m_indexCluster || m_indexCluster->EOS() || !m_indexBlockEntry || m_indexBlockEntry->EOS())
        {
            m_indexDone = true;
            m_indexBuilding = false;
            return false;
        }

        const mkvparser::Block* const block = m_indexBlockEntry->GetBlock();
        if (!block)
        {
            m_indexDone = true;
            m_indexBuilding = false;
            return false;
        }

        if ((unsigned int)block->GetTrackNumber() == (unsigned int)m_videoTrackNumber)
        {
            const int frameCount = block->GetFrameCount();
            if (frameCount > 0)
            {
                const mkvparser::Block::Frame& frame = block->GetFrame(0);

                FrameIndexEntry fi{};
                fi.time_ns = block->GetTime(m_indexCluster);
                fi.pos = frame.pos;
                fi.len = frame.len;

                m_frameIndex.push_back(fi);
            }
        }

        const mkvparser::BlockEntry* next = nullptr;
        long status = m_indexCluster->GetNext(m_indexBlockEntry, next);
        if (status < 0)
        {
            m_indexDone = true;
            m_indexBuilding = false;
            return false;
        }
        m_indexBlockEntry = next;

        return true;
    }

    void webmDecoder::pumpIndexBuild(u32 budgetUs)
    {
        if (!m_indexBuilding || m_indexDone)
            return;

        const unsigned long long startUs = ITF_TIME_US();

        while (m_indexBuilding && !m_indexDone)
        {
            stepIndexOneBlock();

            const unsigned long long nowUs = ITF_TIME_US();
            if ((nowUs - startUs) >= (unsigned long long)budgetUs)
                break;
        }
    }

    webmDecoder::DecodeResult webmDecoder::decodeNextFrame(const vpx_image_t*& outImage, f64& outPtsSeconds)
    {
        outImage = nullptr;
        outPtsSeconds = 0.0;

        if (!m_codecInited || !m_segment)
            return DecodeResult::Error;

        const bool buffering = !isReady();
        pumpIndexBuild(buffering ? m_budgetBufferingUs : m_budgetSteadyUs);

        if (!isReady())
            return DecodeResult::Buffering;

        if (m_currentFrame >= m_frameIndex.size())
        {
            if (m_indexDone)
                return DecodeResult::End;

            return DecodeResult::Buffering;
        }

        const FrameIndexEntry& fi = m_frameIndex[m_currentFrame];
        if (fi.len <= 0)
            return DecodeResult::Error;

        if (m_frameBuffer.size() < (size_t)fi.len)
            m_frameBuffer.resize((size_t)fi.len);

        if (m_reader.Read(fi.pos, fi.len, m_frameBuffer.data()) != 0)
            return DecodeResult::Error;

        const vpx_codec_err_t err = vpx_codec_decode(&m_codec, m_frameBuffer.data(), (unsigned int)fi.len, nullptr, 0);

        if (err != VPX_CODEC_OK)
            return DecodeResult::Error;

        vpx_codec_iter_t iter = nullptr;
        vpx_image_t* img = vpx_codec_get_frame(&m_codec, &iter);
        if (!img)
            return DecodeResult::Error;

        outImage = img;
        outPtsSeconds = (double)fi.time_ns / 1e9;

        ++m_currentFrame;
        return DecodeResult::Ok;
    }
}
