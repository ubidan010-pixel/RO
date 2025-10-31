#ifndef _ITF_DEPTH_RANGE_H_
#define _ITF_DEPTH_RANGE_H_

namespace ITF
{

    class DepthRange
    {
    public:
        DepthRange();
        DepthRange(f32 _depth);


        ITF_INLINE f32      getReferenceDepth()const    { return m_referenceDepth; }
        ITF_INLINE f32      getMinDepth()const          { return getReferenceDepth() - 0.5f; }
        ITF_INLINE f32      getMaxDepth()const          { return getReferenceDepth() + 0.5f; }
        ITF_INLINE bbool    contains(f32 _z)const       { return _z >= getMinDepth() && getMaxDepth() > _z; }
        ITF_INLINE bbool    isValid()const              { return m_referenceDepth != F32_INFINITY; }

        bool operator==(const DepthRange &other) const  { return getReferenceDepth() == other.getReferenceDepth(); }
        bool operator!=(const DepthRange &other) const  { return !(*this == other); }

        static const DepthRange InvalidDepthRange;

    private:
        f32 m_referenceDepth;
    };

} // ITF

#endif // _ITF_DEPTH_RANGE_H_
