#include "precompiled_gameplay_rayman.h"

#ifndef __ITF_RAY_FLUIDSHAPERS_H__
#include "rayman/gameplay/Components/Misc/Ray_FluidShapers.h"
#endif

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    static const f32 maxHighY = 1.0f;
    static const f32 minHighY = 0.8f;
    static const f32 maxLowY = 0.3f;
    static const f32 minLowY = 0.0f;

    static const f32 offsetY = 0.0f;

    static const f32 minScaler = 0.f;

    static const f32 scalerPhaseStep = 0.5f;
    static const f32 offsetAngleStep = 0.5f;

    //static const u32 scalerPartCount = 20;

    static const f32 scalerWeightAtStart = 0.3f;
    static const f32 scalerWeightBeforeMax_Reference = 0.7f;
    static const f32 scalerAmplitudeBeforeMax_Reference = 0.7f;
    static const Ray_FluidShaper_Spikes::ScalerPartDesc scalerParts_Reference[Ray_FluidShaper_Spikes::scalerPartCount_Reference]=
    {
        { 1.0f, 0.7f, 0.15f },
        { 1.0f, 0.7f, 0.2f },
        { 0.9f, 0.7f, 0.3f },
        { 0.9f, 0.7f, 0.4f },
        { 1.0f, 0.7f, 0.5f },
        { 1.0f, 0.7f, 0.6f },
        { 1.05f, 0.7f, 0.7f },
        { 1.2f, 0.7f, 0.7f },
        { 1.2f, scalerAmplitudeBeforeMax_Reference, scalerWeightBeforeMax_Reference },
        { 0.9f, 1.0f, 1.0f },
        { 0.9f, 1.0f, 1.0f },
        { -1.05f, 0.7f, 0.7f },
        { -1.05f, 0.7f, 0.7f },
        { -1.05f, 0.7f, 0.7f },
        { -1.0f, 0.7f, 0.6f },
        { -1.0f, 0.7f, 0.5f },
        { -0.95f, 0.7f, 0.4f },
        { -0.95f, 0.7f, 0.3f },
        { -1.0f, 0.7f, 0.2f },
        { -1.0f, 0.7f, 0.15f },
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_FluidShaper_Spikes::computeLinearPartCount()
    {
        m_linearPartCount = (u32)(m_linearRadius*(20/1.5f));
        m_linearPartCount = std::min(m_linearPartCount, (const u32)maxScalerPartCount);
        const u32 minLinearPartCount=5;
        ITF_ASSERT(minLinearPartCount>=4);
        if (m_linearPartCount<minLinearPartCount)
            m_linearPartCount = minLinearPartCount;

        u32 rampIndexCount = (u32) ((m_rampLength*(f32)m_linearPartCount)/(2*m_linearRadius));
        u32 maxRampIndexCount = (m_linearPartCount>>1)-1 ;
        rampIndexCount = std::min(rampIndexCount, maxRampIndexCount);

        u32 middle = m_linearPartCount>>1;
        f32 invRampIndexCount = 1;
        if (rampIndexCount>1)
        {
            invRampIndexCount = 1.f/((f32)rampIndexCount-1.f);
        }
        for (u32 i=0; i<m_linearPartCount; i++)
        {
            u32 referenceIndex = (i*scalerPartCount_Reference)/m_linearPartCount;

            u32 distFromExtremum;
            if (i<=middle)
            {
                distFromExtremum = i;
            }
            else
            {
                distFromExtremum = m_linearPartCount-1-i;
            }

            f32 weight = 1.f;
            f32 amplitude = 1.f;

            if (distFromExtremum>=rampIndexCount)
            {
            }
            else
            {
                f32 rampInterpolator = ((f32)distFromExtremum)*invRampIndexCount;

                if (rampInterpolator>1)
                {
                    rampInterpolator = 1.f;
                }
                ITF_ASSERT(rampInterpolator>=0);
                weight = scalerWeightAtStart*(1-rampInterpolator)+scalerWeightBeforeMax_Reference*rampInterpolator;
                amplitude = scalerAmplitudeBeforeMax_Reference;

            }
            ScalerPartDesc scalerPartDesc = scalerParts_Reference[referenceIndex];
            scalerPartDesc.m_weight = weight;
            scalerPartDesc.m_amplitude = amplitude;
            m_scalerParts[i] = scalerPartDesc;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_FluidShaper_Spikes::sample(f32 _x, bbool _high, f32 _relativeX, i32 _pointIndex, f32 &_outputWeight) const
    {
        f32 scalerPhase;
        f32 scalerAmplitude;

        i32 scalerPartIndex = _pointIndex;
        if (scalerPartIndex<0)
        {
            scalerPartIndex = 0;
        }
        else if (scalerPartIndex >= (i32)m_linearPartCount)
        {
            scalerPartIndex = (i32)m_linearPartCount-1;
        }

        const ScalerPartDesc& scalerPartDesc = m_scalerParts[scalerPartIndex];
        scalerPhase = scalerPartDesc.m_phaseMultiplier*m_scalerPhase*0.5f;
        scalerAmplitude = scalerPartDesc.m_amplitude;
        _outputWeight = scalerPartDesc.m_weight;

        if (_relativeX<0) _relativeX = 0;
        else if (_relativeX>1) _relativeX = 1;

        const f32 shapeCosScaleX = 1;

        f32 minY, maxY;
        if (_high)
        {
            minY = minHighY;
            maxY = maxHighY;
        }
        else
        {
            minY = minLowY;
            maxY = maxLowY;
        }

        f32 scalerValue = scalerAmplitude*f32_Cos(scalerPhase);

        if (scalerValue>0)
        {
#ifdef ITF_WII
            scalerValue = f32_Sqrt(scalerValue);
#else
            scalerValue = powf(scalerValue,0.4f);
#endif
        }
        else
        {
#ifdef ITF_WII
            scalerValue = -f32_Sqrt(-scalerValue);
#else
            scalerValue = -powf(-scalerValue,0.4f);
#endif
        }
        //scalerValue = 1;

        f32 c = f32_Cos((_x+m_offsetX) * shapeCosScaleX);

        f32 scaledCos = (minY + (maxY - minY) * (c + 1) * 0.5f)*2-1;

        return scaledCos * scalerValue;

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_FluidShaper_Spikes::getHeight(f32 _linearX, bbool &_found, f32 &_waveWeight, bbool &_hasColor, bbool &_hasGameMaterial) const
    {
        f32 x = _linearX - std::max(std::min(m_linearOffset, m_maxAnnouncer), m_minAnnouncer);

        if (x<-(m_linearRadius+m_transitionLen) || x>(m_linearRadius+m_transitionLen))
        {
            _hasColor = bfalse;
            _hasGameMaterial = bfalse;
            _found = bfalse;
            _waveWeight = 0;
            return 0;
        }
        _found = btrue;


        f32 relativeX = (x+m_linearRadius)/(2*m_linearRadius);
        f32 scaledX = (relativeX * m_linearPartCount);
        i32 partIndex = (i32)floorf(scaledX);
        f32 lambda = scaledX - partIndex;
        f32 y0, y1;
        f32 weight0,weight1;
        f32 y;


        if ((partIndex & 1) != 0)
        {
            //partIndex = partIndex - 1;
            y0 = sample((f32)partIndex, true, partIndex/(f32)m_linearPartCount, partIndex, weight0);
            y1 = sample((f32)partIndex + 1, false, (partIndex+1)/(f32)m_linearPartCount, partIndex + 1, weight1);
        }
        else
        {
            y0 = sample((f32)partIndex, false, partIndex/(f32)m_linearPartCount, partIndex, weight0);
            y1 = sample((f32)partIndex + 1, true, (partIndex+1)/(f32)m_linearPartCount, partIndex+1, weight1);
        }
        y = y0 * (1 - lambda) + y1 * lambda;
        _waveWeight = weight0*(1-lambda)+weight1*lambda;

        y += offsetY;
        y *= _waveWeight;
        y *= m_yScale;
        ITF_ASSERT(_waveWeight>=0);
        _waveWeight = std::min(_waveWeight,1.f);
        _hasColor = (relativeX>=0.f && relativeX<=1.f) && getIsColored();
        _hasGameMaterial = _hasColor && hasGameMaterial();

        return y;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_FluidShaper_Spikes::addTimeSlice(f32 /*_dt*/) 
    {
        m_offsetX += offsetAngleStep;
        m_scalerPhase += scalerPhaseStep;
    }

}
