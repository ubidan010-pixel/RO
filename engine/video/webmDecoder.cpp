#include "precompiled_engine.h"
#include "engine/video/webmDecoder.h"

#include <core/memory/UniquePtr.h>
#include <vpx/vp8dx.h>
#include <vpx/vpx_decoder.h>

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

    bool webmDecoder::open(const String& _path)
    {
        close();

        if (!m_reader.open(_path))
            return false;

        if (!initSegment())
            return false;

        if (!buildFrameIndex())
            return false;

        if (!initDecoder())
            return false;

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

        m_cluster = nullptr;
        m_blockEntry = nullptr;
        m_videoTrack = nullptr;
        m_videoTrackNumber = 0;
        m_width = m_height = 0;
        m_duration = 0.0;
        m_codecType = CodecType::Unknown;

        m_frameIndex.clear();
        m_frameIndex.shrink_to_fit();
        m_currentFrame = 0;

        m_reader.close();
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
                {
                    continue;
                }
            }
            else
            {
                detected = CodecType::VP8;
            }

            m_codecType = detected;
            m_videoTrack = t;
            m_videoTrackNumber = t->GetNumber();

            const auto* const vt = static_cast<const mkvparser::VideoTrack*>(t);
            m_width  = (u32)vt->GetWidth();
            m_height = (u32)vt->GetHeight();
            break;
        }

        if (!m_videoTrack || m_codecType == CodecType::Unknown)
            return false;

        m_cluster = m_segment->GetFirst();
        if (!m_cluster || m_cluster->EOS())
            return false;

        long status = m_cluster->GetFirst(m_blockEntry);
        return (status >= 0 && m_blockEntry && !m_blockEntry->EOS());
    }

    bool webmDecoder::buildFrameIndex()
    {
        if (!m_segment || !m_videoTrackNumber)
            return false;

        m_frameIndex.clear();
        m_frameIndex.shrink_to_fit();
        m_frameIndex.reserve(1024);

        mkvparser::Segment* segment = m_segment;
        const mkvparser::Cluster* cluster = segment->GetFirst();

        while (cluster && !cluster->EOS())
        {
            const mkvparser::BlockEntry* blockEntry = nullptr;
            long status = cluster->GetFirst(blockEntry);
            if (status < 0)
                return false;
            if (!blockEntry || blockEntry->EOS())
            {
                cluster = segment->GetNext(cluster);
                continue;
            }

            while (blockEntry && !blockEntry->EOS())
            {
                const mkvparser::Block* const block = blockEntry->GetBlock();
                if (!block)
                    return false;

                const unsigned int track_number =
                    static_cast<unsigned int>(block->GetTrackNumber());

                if (track_number == m_videoTrackNumber)
                {
                    const int frameCount = block->GetFrameCount();
                    if (frameCount > 0)
                    {
                        const mkvparser::Block::Frame& frame = block->GetFrame(0);

                        FrameIndexEntry fi{};

                        fi.time_ns = block->GetTime(cluster);
                        fi.pos     = frame.pos;
                        fi.len     = frame.len;

                        m_frameIndex.push_back(fi);
                    }
                }

                status = cluster->GetNext(blockEntry, blockEntry);
                if (status < 0)
                    return false;
            }

            cluster = segment->GetNext(cluster);
        }

        return !m_frameIndex.empty();
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

        //vp8_postproc_cfg_t pp = {};
        //pp.post_proc_flag = 0;
        //vpx_codec_control(&m_codec, VP8_SET_POSTPROC, &pp);

        m_codecInited = true;
        return true;
    }

    bool webmDecoder::decodeNextFrame(const vpx_image_t*& outImage, f64& outPtsSeconds)
    {
        outImage = nullptr;
        outPtsSeconds = 0.0;

        if (!m_codecInited || !m_segment)
            return false;

        if (m_currentFrame >= m_frameIndex.size())
            return false;

        const FrameIndexEntry& fi = m_frameIndex[m_currentFrame];

        if (m_frameBuffer.size() < static_cast<size_t>(fi.len))
            m_frameBuffer.resize(static_cast<size_t>(fi.len));

        uint8_t* frameData = m_frameBuffer.data();

        if (m_reader.Read(fi.pos, fi.len, frameData) != 0)
            return false;

        const vpx_codec_err_t err = vpx_codec_decode(&m_codec, frameData, static_cast<unsigned int>(fi.len), nullptr, 0);

        if (err != VPX_CODEC_OK)
            return false;

        vpx_codec_iter_t iter = nullptr;
        vpx_image_t* img = vpx_codec_get_frame(&m_codec, &iter);
        if (!img)
            return false;

        outImage = img;
        outPtsSeconds = static_cast<double>(fi.time_ns) / 1000000000.0;

        ++m_currentFrame;
        return true;
    }

    bool webmDecoder::reset()
    {
        if (m_frameIndex.empty())
            return false;

        m_currentFrame = 0;

        return true;
    }

}
