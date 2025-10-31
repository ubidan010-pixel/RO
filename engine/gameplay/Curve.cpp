#include "precompiled_engine.h"

#ifndef _ITF_CURVE_H_
#include "engine/gameplay/Curve.h"
#endif // _ITF_CURVE_H_

#ifndef ITF_FINAL
    #ifndef _ITF_GFX_ADAPTER_H_
    #include "engine/AdaptersInterfaces/GFXAdapter.h"
    #endif //_ITF_GFX_ADAPTER_H_
#endif // ITF_FINAL

namespace ITF
{
//////////////////////////////// BASECURVEPARAMS ///////////////////////////////
    IMPLEMENT_OBJECT_RTTI(BaseCurveParams)
    BEGIN_SERIALIZATION(BaseCurveParams)
        SERIALIZE_MEMBER("xofs", m_xofs);
        SERIALIZE_MEMBER("yofs", m_yofs);
        SERIALIZE_MEMBER("xScale", m_xScale);
        SERIALIZE_MEMBER("yScale", m_yScale);
        SERIALIZE_MEMBER("xMin", m_xMin);
        SERIALIZE_MEMBER("xMax", m_xMax);
        SERIALIZE_MEMBER("yMin", m_yMin);
        SERIALIZE_MEMBER("yMax", m_yMax);
        SERIALIZE_MEMBER("loop", m_loop);
    END_SERIALIZATION()

    BaseCurveParams::BaseCurveParams()
    {
        m_xofs      =   0.f;
        m_yofs      =   0.f;
        m_xScale    =   1.f;
        m_yScale    =   1.f;
        m_xMin      =   0.f;
        m_xMax      =   1.f;
        m_yMin      =   0.f;
        m_yMax      =   1.f;
        m_loop      =   bfalse;
        m_invert    =   bfalse;

#ifndef ITF_FINAL
        m_drawOffset = Vec3d::Zero;
        m_debugDraw =   bfalse;
        m_followObject  =   ITF_INVALID_OBJREF;
#endif // ITF_FINAL
    }

    void BaseCurveParams::clampXInput(f32& _x)
    {
        f32 deltaMin = _x - m_xMin;
        if (deltaMin < 0.f)
        {
            if (m_loop)
            {
                deltaMin = f32_Modulo(-deltaMin, m_xMax-m_xMin);
                _x = m_xMax - deltaMin;
            }
            else
                _x = m_xMin;
        }
        f32 deltaMax = m_xMax - _x;
        if (deltaMax < 0.f)
        {
            if (m_loop)
            {
                deltaMax = f32_Modulo(-deltaMax, m_xMax-m_xMin);
                _x = m_xMin + deltaMax;
            }
            else
                _x = m_xMax;
        }
    }

    void BaseCurveParams::clampValue(f32& _value)
    {
        _value *= m_yScale;
        _value += m_yofs;
        if (m_invert)
            _value = m_yMax - _value;
        if (_value < m_yMin)
            _value = m_yMin;
        else if (_value > m_yMax)
            _value = m_yMax;
    }

#ifndef ITF_FINAL
    void BaseCurveParams::buildSet(f32 _centerVal)
    {
        if (m_debugDraw)
        {
            const u32 ptCount = 256;
            f32 curInput = _centerVal - getDrawPeriod()*0.5f;
            f32 slope = getDrawPeriod() / (f32)ptCount;
            m_set.clear();
            for (u32 i = 0; i < ptCount; i++, curInput += slope)
            {
                m_set.push_back(computeValue(curInput));
            }
            m_set.push_back(computeValue(_centerVal));
        }
    }

    void BaseCurveParams::drawSet()
    {
        Pickable* pObj = (Pickable*)GETOBJECT(m_followObject);
        const u32 setSize = m_set.size();
        if (pObj && setSize > 1)
        {
            const u32 ptCount = setSize-1;
            const f32 displayWidth = 1.5f;
            const f32 displayHeight = 1.5f;
            const f32 xslope = displayWidth / (f32)ptCount;
            f32 curX = 0.f;
            Vec3d pos = pObj->getPos() -  Vec3d(displayWidth * 0.5f, m_set[ptCount], 0.f) + m_drawOffset;
            GFX_ADAPTER->drawDBGBox(pObj->getPos().truncateTo2D() + m_drawOffset.truncateTo2D(), 0.f, Vec2d(displayWidth, displayHeight), 0.3f, 0.3f, 0.3f, 0.f, pos.m_z + m_drawOffset.m_z, btrue);
            for (u32 i = 1; i < ptCount; i++, curX += xslope)
                GFX_ADAPTER->drawDBG3DLine(pos + Vec3d(curX, m_set[i], 0.f), pos + Vec3d(curX-xslope, m_set[i-1], 0.f), Color::green());
            GFX_ADAPTER->drawDBGCircle(pos + Vec3d(displayWidth * 0.5f, m_set[ptCount], 0.f), 0.1f, Color::red(), 0.f, 8);
            String str;
            str.setTextFormat("Value: %f", m_set[ptCount] * m_yScale + m_yofs);
            Vec3d pos2D;
            GFX_ADAPTER->compute3DTo2D(m_drawOffset + pObj->getPos() - Vec3d(displayWidth*0.4f, displayHeight * 0.8f, 0.f) , pos2D);
            GFX_ADAPTER->drawDBGText(str, pos2D.m_x, pos2D.m_y, 1.f, 1.f, 1.f, btrue);
        }
    }
#endif // ITF_FINAL


    f32 BaseCurveParams::getValue(f32 _x)
    {
        _x = _x * m_xScale + m_xofs;

        clampXInput(_x);

        f32 value = computeValue(_x);

        clampValue(value);

#ifndef ITF_FINAL
        if (m_debugDraw)
        {
            buildSet(_x);
            drawSet();
        }
#endif // ITF_FINAL

        return value;
    }


//////////////////////////////// SINUS ///////////////////////////////
    IMPLEMENT_OBJECT_RTTI(SinusParams)
    BEGIN_SERIALIZATION_CHILD(SinusParams)
    END_SERIALIZATION()

    f32 SinusParams::computeValue(f32 _x)
    {
        return f32_Sin(_x);
    }

    SinusParams::SinusParams() : BaseCurveParams()
    {
        m_xMax      =   MTH_PI*2.f;
        m_loop      =   btrue;
    }



//////////////////////////////// EXP ///////////////////////////////
    IMPLEMENT_OBJECT_RTTI(ExpParams)
    BEGIN_SERIALIZATION_CHILD(ExpParams)
    END_SERIALIZATION()

    f32 ExpParams::computeValue(f32 _x)
    {
        return expf(_x);
    }

    ExpParams::ExpParams() : BaseCurveParams()
    {
        m_xMax      =   2.f;
    }

//////////////////////////////// LOGISTIC ///////////////////////////////
    IMPLEMENT_OBJECT_RTTI(LogisticParams)
    BEGIN_SERIALIZATION_CHILD(LogisticParams)
    END_SERIALIZATION()

    f32 LogisticParams::computeValue(f32 _x)
    {
        // logistic(0) == 0.0024726237, logistic(1) == 0.99752742 --> normalize to 1
        static const f32 value0 = 1.f / (1.f + powf(MTH_EULER, 6.f));
        static const f32 value1 = 1.f / (1.f + powf(MTH_EULER, -6.f));
        static const f32 normalizeFactor = 1.f / (value1 - value0);
        _x *= 12.f;
        _x -= 6.f;
        return m_Sheight/(1.f+powf(MTH_EULER, -_x)) * normalizeFactor - value0;
    }

    LogisticParams::LogisticParams()  : BaseCurveParams()
    {
        m_Sheight = 1.f;
    }

//////////////////////////////// GAUSS ///////////////////////////////
    IMPLEMENT_OBJECT_RTTI(GaussParams)
    BEGIN_SERIALIZATION_CHILD(GaussParams)
        SERIALIZE_MEMBER("curveHeight", m_curveHeight);
        SERIALIZE_MEMBER("bellCenter", m_bellCenter);
        SERIALIZE_MEMBER("bellWidth", m_bellWidth);
    END_SERIALIZATION()

    GaussParams::GaussParams() : BaseCurveParams()
    {
        m_curveHeight   =   1.f;
        m_bellCenter    =   0.f;
        m_bellWidth     =   0.5f;
    }


    f32 GaussParams::computeValue(f32 _x)
    {
        f32 quotient = powf(_x - m_bellCenter, 2) / (2.f * m_bellWidth *m_bellWidth);
        return m_curveHeight * powf(MTH_EULER, -quotient);
    }
}

