#ifndef __ITF_RAY_FLUIDSHAPERS_H__
#define __ITF_RAY_FLUIDSHAPERS_H__

#ifndef _ITF_FLUIDSHAPER_H_
#include "engine/display/Frieze/FluidShaper.h"
#endif //_ITF_FLUIDSHAPER_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_FluidShaper_Bump : public FluidShaper
    {
    public:
        Ray_FluidShaper_Bump() : FluidShaper() {m_linearRadius=1.f;m_yScale = 0.4f;}
        f32 getHeight(f32 _linearX, bbool &_found, f32 &_waveWeight, bbool &_hasColor, bbool &_hasGameMaterial) const
        {
            f32 x = _linearX - m_linearOffset;
            _hasColor = getIsColored();
            _hasGameMaterial = _hasColor && hasGameMaterial();
            if (x<-m_linearRadius || x>m_linearRadius)
            {
                _found = bfalse;
                _waveWeight = 0;
                return 0;
            }
            _found = btrue;
            float nx = x/m_linearRadius;

            f32 normalizedValue = expf(-1.5f*nx*nx);
            normalizedValue = normalizedValue*normalizedValue;
            _waveWeight = normalizedValue;
            return m_yScale*normalizedValue;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_FluidShaper_Spikes : public FluidShaper
    {
    public:
        static const u32 maxScalerPartCount = 256;
        static const u32 scalerPartCount_Reference = 20;

        struct ScalerPartDesc
        {
            f32 m_phaseMultiplier;
            f32 m_amplitude;
            f32 m_weight;
        };

        Ray_FluidShaper_Spikes() : FluidShaper() 
        {
            m_offsetX = 0;
            m_scalerPhase = 0;
            m_transitionLen = 1.0f;
            m_rampLength = 1.5f * (0.9f/1.0f);

            computeLinearPartCount();
        }

        ~Ray_FluidShaper_Spikes()
        {
        }

        void setRampLength(f32 _rampLength) {m_rampLength = _rampLength;  computeLinearPartCount();}

        void setRadius(f32 _radius) 
        {
            FluidShaper::setRadius( std::max(f32_Abs(_radius),0.2f) );
            computeLinearPartCount();
        }


        f32 getHeight(f32 _linearX, bbool &_found, f32 &_waveWeight, bbool &_hasColor, bbool &_hasGameMaterial) const;

        virtual void addTimeSlice(f32 /*_dt*/);

    private:
        void computeLinearPartCount();

        ITF_INLINE f32 sample(f32 _x, bbool _high, f32 _relativeX, i32 _pointIndex, f32 &_outputWeight) const;

        f32 m_offsetX;
        f32 m_scalerPhase;
        u32 m_linearPartCount;
        f32 m_rampLength;
        ScalerPartDesc m_scalerParts[maxScalerPartCount];
    };

}

#endif
