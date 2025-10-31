#ifndef _ITF_VOLUME_
#define _ITF_VOLUME_
    

namespace ITF
{
    class Volume
    {
    public:

        //************************************
        // @name	ITF::DecibelsToAmplitudeRatio
        // @public 
        // @param	f32 _decibels
        // @return	ITF_INLINE f32
        //************************************
        static f32 DecibelsToAmplitudeRatio(f32 _decibels)
        {
            return powf(10.0f, _decibels * 0.05f);
        }

        //************************************
        // @name	ITF::AmplitudeRatioToDecibels
        // @public 
        // @param	f32 _volume
        // @return	ITF_INLINE f32
        //************************************
        static f32 AmplitudeRatioToDecibels(f32 _volume)
        {
            if (_volume == 0)
            {
                //return -3.402823466e+38f; // Smallest float value (-FLT_MAX)
                return -96.0f;
            }
            return 20.0f * log10f(_volume);
        }


        ITF_INLINE Volume() : m_ratio(1.0f) {}
        ITF_INLINE Volume(f32 _volume, bbool _isdB) { if (_isdB) setdB(_volume); else setratio(_volume); }
        ITF_INLINE Volume(const Volume & _volume) { m_ratio = _volume.m_ratio;}

        //************************************
        // @name	ITF::Volume::dB
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32 dB() const { return AmplitudeRatioToDecibels(m_ratio); }
        //************************************
        // @name	ITF::Volume::ratio
        // @public 
        // @return	ITF_INLINE f32
        //************************************
        ITF_INLINE f32 ratio() const { return m_ratio; }
        //************************************
        // @name	ITF::Volume::setdB
        // @public 
        // @param	f32 _dB
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setdB(f32 _dB) { m_ratio = DecibelsToAmplitudeRatio(_dB); }
        //************************************
        // @name	ITF::Volume::setratio
        // @public 
        // @param	f32 _ratio
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void setratio(f32 _ratio) { m_ratio = _ratio; }
        //************************************
        // @name	ITF::Volume::adddB
        // @public 
        // @param	f32 _dB
        // @return	ITF_INLINE void
        //************************************
        ITF_INLINE void adddB(f32 _dB) { m_ratio *= DecibelsToAmplitudeRatio(_dB); }
        //************************************
        // @name	ITF::Volume::serialize
        // @public 
        // @param	Archive & _archive
        // @return	void
        //************************************
        void serialize(ArchiveMemory& _archive) { _archive.serialize(m_ratio); }
        //************************************
        // @name	ITF::Volume::operator==
        // @public 
        // @param	const Volume & _vol
        // @return	bool
        //************************************
        bool operator == (const Volume& _vol) const { return (ratio() == _vol.ratio()); }

    private:
        f32 m_ratio; //volume stored as ratio
    };
}
#endif //_ITF_VOLUME_