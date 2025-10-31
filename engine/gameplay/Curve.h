#ifndef _ITF_CURVE_H_
#define _ITF_CURVE_H_

namespace ITF
{

#ifdef ITF_FINAL
    #define DEBUG_DRAW_CURVE(_MacroParam, _MacroOr)
    #define DEBUG_DRAW_CURVE_OFS(_MacroParam, _MacroOr, _OffsetX, _OffsetY, _OffsetZ)
#else // ITF_FINAL
    #define DEBUG_DRAW_CURVE(_MacroParam, _MacroOr)          {_MacroParam.m_debugDraw = btrue; _MacroParam.m_followObject = _MacroOr;}
    #define DEBUG_DRAW_CURVE_OFS(_MacroParam, _MacroOr, _OffsetX, _OffsetY, _OffsetZ) {_MacroParam.m_debugDraw = btrue; _MacroParam.m_followObject = _MacroOr; _MacroParam.m_drawOffset.m_x = _OffsetX; _MacroParam.m_drawOffset.m_y = _OffsetY; _MacroParam.m_drawOffset.m_z = _OffsetZ;}
#endif // ITF_FINAL


class BaseCurveParams : public IRTTIObject
{
    public:
        DECLARE_OBJECT_CHILD_RTTI(BaseCurveParams, IRTTIObject,2655109923);
        DECLARE_SERIALIZE()
        f32                     m_xofs;      // added to _x (phase)
        f32                     m_yofs;      // start y for the curve
        f32                     m_xScale;    // scale in x of the curve's shape (frequency)
        f32                     m_yScale;    // scale in y of the curve's shape (amplitude)
        f32                     m_xMin;      // below this x, the out of bound behaviour is used
        f32                     m_xMax;      // above this x, the out of bound behaviour is used
        f32                     m_yMin;      // y min clamping value (saturation)
        f32                     m_yMax;      // y max clamping value (saturation)
        bbool                   m_loop;      // out of bound behaviour (loop or return nearest value)
        bbool                   m_invert;    // true if the curve's shape must be Y mirrored
#ifndef ITF_FINAL
        bbool                   m_debugDraw;
        ObjectRef               m_followObject;
        Vec3d                   m_drawOffset;   // relative to followed object
#endif // ITF_FINAL

        BaseCurveParams();
        f32                     getValue(f32 _x);

protected:
    virtual f32             computeValue(f32 _x) {ITF_ASSERT(bfalse); return 0;} // must be implemented by children
        void                    clampXInput(f32& _x);
        void                    clampValue(f32& _value);
        SafeArray<f32>          m_set;
#ifndef ITF_FINAL
        virtual void            buildSet(f32 _centerVal);
        virtual void            drawSet();
        virtual f32             getDrawPeriod() {return MTH_PI * 2.f;}
#endif // ITF_FINAL
};



class SinusParams : public BaseCurveParams
{
    public:
        DECLARE_OBJECT_CHILD_RTTI(SinusParams, BaseCurveParams,4003190388);
        DECLARE_SERIALIZE()
        SinusParams();

private:
        virtual f32             computeValue(f32 _x);
        virtual f32             getDrawPeriod() {return MTH_PI * 2.f;}
};

class ExpParams : public BaseCurveParams
{
    public:
        DECLARE_OBJECT_CHILD_RTTI(ExpParams, BaseCurveParams,3480470020);
        DECLARE_SERIALIZE()
        ExpParams();

private:
        virtual f32             computeValue(f32 _x);
        virtual f32             getDrawPeriod() {return 3.f;}
};

class LogisticParams : public BaseCurveParams
{
    public:
        DECLARE_OBJECT_CHILD_RTTI(LogisticParams, BaseCurveParams,4082536952);
        DECLARE_SERIALIZE()
        LogisticParams();

        f32     m_Sheight;
private:
        virtual f32             computeValue(f32 _x);
        virtual f32             getDrawPeriod() {return (m_xMax - m_xMin) * 2.f;}
};


class GaussParams : public BaseCurveParams
{
    public:
        DECLARE_OBJECT_CHILD_RTTI(GaussParams, BaseCurveParams,845029638);
        DECLARE_SERIALIZE()

        f32 m_curveHeight;
        f32 m_bellCenter;
        f32 m_bellWidth;


        GaussParams();

private:
        virtual f32             computeValue(f32 _x);
        virtual f32             getDrawPeriod() {return (m_xMax - m_xMin) * 2.f;}
};



}
#endif // _ITF_CURVE_H_


