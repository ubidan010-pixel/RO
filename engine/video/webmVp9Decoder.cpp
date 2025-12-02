#include "precompiled_engine.h"
#include "engine/video/webmVp9Decoder.h"
#include <core/memory/UniquePtr.h>
#include <vpx/vp8dx.h>

namespace ITF
{
    webmVp9Decoder::webmVp9Decoder()
    {
        memset(&m_codec, 0, sizeof(m_codec));
    }

    webmVp9Decoder::~webmVp9Decoder()
    {
        close();
    }

    bool webmVp9Decoder::open(const String& _path)
    {
        close();

        if (!m_reader.open(_path))
            return false;

        if (!initSegment())
            return false;

        if (!initDecoder())
            return false;

        return true;
    }

    void webmVp9Decoder::close()
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
        m_reader.close();
    }

    bool webmVp9Decoder::initSegment()
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

        for (unsigned long i = 0; i < trackCount; ++i)
        {
            const mkvparser::Track* const t = tracks->GetTrackByIndex(i);
            if (!t)
                continue;

            if (t->GetType() == mkvparser::Track::kVideo)
            {
                m_videoTrack = t;
                m_videoTrackNumber = t->GetNumber();
                const auto* const vt = static_cast<const mkvparser::VideoTrack*>(t);
                m_width  = (u32)vt->GetWidth();
                m_height = (u32)vt->GetHeight();
                break;
            }
        }

        if (!m_videoTrack)
            return false;

        m_cluster = m_segment->GetFirst();
        if (!m_cluster || m_cluster->EOS())
            return false;

        long status = m_cluster->GetFirst(m_blockEntry);
        return (status >= 0 && m_blockEntry && !m_blockEntry->EOS());
    }

    bool webmVp9Decoder::initDecoder()
    {
        vpx_codec_iface_t* iface = vpx_codec_vp9_dx();
        vpx_codec_dec_cfg_t cfg{};
        cfg.w = m_width;
        cfg.h = m_height;
        cfg.threads = 0;

        const vpx_codec_err_t err = vpx_codec_dec_init(&m_codec, iface, &cfg, 0);
        if (err != VPX_CODEC_OK)
            return false;

        m_codecInited = true;
        return true;
    }

    bool webmVp9Decoder::advanceBlock()
    {
        while (true)
        {
            if (!m_blockEntry || m_blockEntry->EOS())
            {
                if (!m_cluster || m_cluster->EOS())
                    return false;

                m_cluster = m_segment->GetNext(m_cluster);
                if (!m_cluster || m_cluster->EOS())
                    return false;

                long status = m_cluster->GetFirst(m_blockEntry);
                if (status < 0 || !m_blockEntry || m_blockEntry->EOS())
                    return false;
            }

            const mkvparser::Block* const block = m_blockEntry->GetBlock();
            if (block && block->GetTrackNumber() == m_videoTrackNumber)
                return true;

            long status = m_cluster->GetNext(m_blockEntry, m_blockEntry);
            if (status < 0)
                return false;
        }
    }

    bool webmVp9Decoder::decodeNextFrame(const vpx_image_t*& outImage, f64& outPtsSeconds)
    {
        outImage = nullptr;
        outPtsSeconds = 0.0;

        if (!m_codecInited || !m_segment)
            return false;

        if (!advanceBlock())
            return false;

        const mkvparser::Block* const block = m_blockEntry->GetBlock();
        const int frameCount = block->GetFrameCount();
        if (frameCount <= 0)
            return false;

        const mkvparser::Block::Frame& frame = block->GetFrame(0);
        const size_t frameSize = (size_t)frame.len;

        UniquePtr<uint8_t[]> buffer(new uint8_t[frameSize]);
        if (frame.Read(&m_reader, buffer.get()))
            return false;

        const vpx_codec_err_t err = vpx_codec_decode(&m_codec, buffer.get(), (unsigned int)frameSize, nullptr, 0);
        if (err != VPX_CODEC_OK)
            return false;

        vpx_codec_iter_t iter = nullptr;
        vpx_image_t* img = vpx_codec_get_frame(&m_codec, &iter);
        if (!img)
            return false;

        const long long timecode = block->GetTime(m_cluster);
        const double t_ns = (double)timecode * (double)m_timecodeScale;
        outPtsSeconds = t_ns / 1000000000.0;

        outImage = img;

        long status = m_cluster->GetNext(m_blockEntry, m_blockEntry);
        if (status < 0)
            m_blockEntry = nullptr;

        return true;
    }

    bool webmVp9Decoder::reset()
    {
        if (!m_segment)
            return false;

        m_cluster = m_segment->GetFirst();
        if (!m_cluster || m_cluster->EOS())
            return false;

        long status = m_cluster->GetFirst(m_blockEntry);
        return (status >= 0 && m_blockEntry && !m_blockEntry->EOS());
    }
}
