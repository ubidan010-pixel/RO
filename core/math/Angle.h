#ifndef _ITF_ANGLE_H_
#define _ITF_ANGLE_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

namespace ITF
{

class Angle
{
public:
        Angle()
        : m_value(0.0f)
        {
        }

        Angle(bool bDegrees, f32 flIn ) {bDegrees ? SetDegrees(flIn) : SetRadians(flIn); }

        void serialize(ArchiveMemory& _archive) { _archive.serialize(m_value); }

        f32 ToRadians() const {return m_value;}
        f32 ToDegrees() const {return m_value * MTH_RADTODEG;}
        f32 ToDegrees360() const {return ( m_value >= 0.0f ) ? m_value * MTH_RADTODEG : 360.f + ( m_value * MTH_RADTODEG ); }


        f32 Radians() const { return ToRadians(); }
        f32 Degrees180() const { return ToDegrees(); }
        f32 Degrees360() const { return ToDegrees360(); }
        static  Angle  Degrees( f32 flDegrees ) { return Angle( true , flDegrees ); }
        static  Angle  Radians( f32 flRadians ) { return Angle( false , flRadians ); }

        ITF_INLINE void SetRadians(f32 flInRadians)
        {
            m_value = flInRadians;
        }

        ITF_INLINE void SetDegrees(f32 flInDegrees) 
        {
            SetRadians(flInDegrees * MTH_DEGTORAD);
        }

        ITF_INLINE bool operator!=(const Angle &aIn) const
        {
            return ToRadians()!=aIn.ToRadians();
        }

        ITF_INLINE const Angle operator -() const
        {
            Angle angle(false,-ToRadians());
            return angle;
        }

        ITF_INLINE const Angle operator *=(const Angle &aIn)
        {
            SetRadians(ToRadians() * aIn.ToRadians());
            return *this;
        }
        
        ITF_INLINE const Angle operator /=(const Angle &aIn)
        {
            SetRadians(ToRadians() / aIn.ToRadians());
            return *this;
        }

        ITF_INLINE const Angle operator +=(const Angle &aIn)
        {
            //SetRadians(ToRadians() + aIn.ToRadians());
            m_value += aIn.m_value;
            return *this;
        }
        
        ITF_INLINE const Angle operator -=(const Angle &aIn)
        {
            //SetRadians(ToRadians() - aIn.ToRadians());
            m_value -= aIn.m_value;
            return *this;
        }

        ITF_INLINE const Angle operator +=(const f32 flIn)
        {
            m_value += flIn;
            return *this;
        }

        ITF_INLINE const Angle operator -=(const f32 flIn)
        {
            m_value -= flIn;
            return *this;
        }

        ITF_INLINE const Angle operator *=(const f32 flIn)
        {
            //SetRadians(ToRadians() * flIn);
            m_value *= m_value;
            return *this;
        }
        
        ITF_INLINE const Angle operator /=(const f32 flIn)
        {
            SetRadians(ToRadians() / flIn);
            return *this;
        }

        ITF_INLINE bool operator < (const Angle &flIn) const
        {
            return ToRadians()<flIn.ToRadians();
        }

        ITF_INLINE bool operator > (const Angle &flIn) const
        {
            return ToRadians()>flIn.ToRadians();
        }

        ITF_INLINE bool operator <= (const Angle &flIn) const
        {
            return ToRadians()<=flIn.ToRadians();
        }

        ITF_INLINE bool operator >= (const Angle &flIn) const
        {
            return ToRadians()>=flIn.ToRadians();
        }

        ITF_INLINE bool operator == (const Angle &flIn) const
        {
            return ToRadians()==flIn.ToRadians();
        }

        static Angle Atan(f32 In)
        {
            Angle aOut = Angle(false, ::atanf(In));
            return aOut;
        }

        static Angle Atan2(f32 Y, f32 X)
        {
            Angle aOut = Angle(false, ::atan2f(Y, X));
            return aOut;
        }

        static Angle Acos(f32 In)
        {
            Angle aOut = Angle(false, f32_ACos(In));
            return aOut;
        }

        static Angle Asin( f32 nIn )
        {
            Angle aOut = Angle(false, f32_ASin(nIn));
            return aOut;
        }

        bool    IsZero() const { return f32_Abs(m_value)<MTH_EPSILON; }
        bool    IsNegative() const { return (m_value<0.0f); }
        bool    IsPositive() const { return (m_value>0.0f); }

        ITF_INLINE static Angle GetShortestDif( const Angle& angleFrom, const Angle& angleTo )
        {
            return Angle( bfalse, getShortestAngleDelta(angleFrom.ToRadians(),angleTo.ToRadians()) );
        }

        ITF_INLINE bbool EqualWithEpsilon( Angle angle ) const
        {
            return f32_Abs(angle.ToRadians()-ToRadians())<MTH_EPSILON;
        }

        ITF_INLINE Vec2d ToVector() const
        {
            return Vec2d::Right.Rotate(m_value);
        }

        ITF_INLINE void Snap() 
        {
            if( m_value < 0.0f )
            {
                m_value = MTH_2PI + f32_Modulo( m_value,MTH_2PI );
            }
            else
            {
                if( m_value >= MTH_2PI )
                {
                    m_value = f32_Modulo( m_value,MTH_2PI );
                }
            }
        }

        ITF_INLINE void SnapPI() 
        {
	        if( m_value > -MTH_PI && m_value < MTH_PI )
            {
		        return;
            }

	        Snap();

	        if( m_value > MTH_PI )
            {
		        m_value = -( MTH_2PI - m_value );
            }
        }
		
		static Angle Lerp( f32 flLambda , const Angle& PointA , const Angle& PointB )
		{
			f32 Diff = PointB.ToRadians() - PointA.ToRadians();

			Angle Result = PointA;

			if ( Diff > MTH_PI )
				Diff = Diff - MTH_2PI;
			else if ( Diff < - MTH_PI )
				Diff = MTH_2PI + Diff;

			Result.SetRadians( Result.ToRadians() + (Diff * flLambda) );
			return Result;
		}

        static const Angle Zero;
        static const Angle Angle360;
        static const Angle Angle180;

    private:

        f32 m_value;
};


ITF_INLINE Angle operator+(const Angle &aIn1, const Angle &aIn2)
{
    return Angle(false, aIn1.ToRadians() + aIn2.ToRadians());
}

ITF_INLINE Angle operator-(const Angle &aIn1, const Angle &aIn2)
{
    return Angle(false, aIn1.ToRadians() - aIn2.ToRadians());
}

ITF_INLINE Angle operator*(const Angle &aIn1, const Angle &aIn2)
{
    return Angle(false, aIn1.ToRadians() * aIn2.ToRadians());
}

ITF_INLINE Angle operator/(const Angle &aIn1, const Angle &aIn2)
{
    return Angle(false, aIn1.ToRadians() / aIn2.ToRadians());
}

ITF_INLINE Angle operator*(const Angle &aIn1, f32 Val )
{
    return Angle(false, aIn1.ToRadians() * Val);
}

ITF_INLINE Angle operator/(const Angle &aIn1, f32 Val )
{
    return Angle(false, aIn1.ToRadians() / Val);
}

ITF_INLINE Angle operator*( f32 Val, const Angle &aIn1 )
{
    return Angle(false, Val * aIn1.ToRadians());
}

ITF_INLINE Angle operator/( f32 Val, const Angle &aIn1 )
{
    return Angle(false, Val / aIn1.ToRadians());
}

extern Angle ClampAngle( const Angle& _angle, const Angle& _min, const Angle& _max );


// Angle is for 360° max
// AngleAmount is for any amount
class AngleAmount : public Angle
{
public:
    AngleAmount() { SetRadians(0.f); }
    AngleAmount(bool bDegrees, f32 flIn ) {bDegrees ? SetDegrees(flIn) : SetRadians(flIn); }
};

}

#endif // _ITF_ANGLE_H_


