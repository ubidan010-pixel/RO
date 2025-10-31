#ifndef FASTTRIGO_PS3_H_
#define FASTTRIGO_PS3_H_

namespace FastTrigo
{
inline float Pi() { return 3.14159265358979323846; }
inline float HalfPi() { return 3.14159265358979323846 * 0.5; }
inline float InvPi() { return 1.0 / 3.14159265358979323846; }

// using template specialization to get pure consts without any memory storage
// sin Taylor series coefficients.
template <unsigned int numCoef>
struct SC
{
    operator double() { return 0; }
};
template <> struct SC<0> { operator double() { return -1.6666666666666666666667e-01; }}; // 1/3!
template <> struct SC<1> { operator double() { return +8.3333333333333333333333e-03; }}; // 1/5!
template <> struct SC<2> { operator double() { return -1.9841269841269841269841e-04; }}; // 1/7!
template <> struct SC<3> { operator double() { return +2.7557319223985890652557e-06; }}; // 1/9!
template <> struct SC<4> { operator double() { return -2.5052108385441718775052e-08; }}; // 1/11!
template <> struct SC<5> { operator double() { return +1.6059043836821614599392e-10; }}; // 1/13!
template <> struct SC<6> { operator double() { return -7.6471637318198164759011e-13; }}; // 1/15!
template <> struct SC<7> { operator double() { return +2.8114572543455207631989e-15; }}; // 1/17!

// cos Taylor series coefficients.
template <unsigned int numCoef>
struct CC
{
    operator double() { return 0; }
};
template <> struct CC<0> { operator double() { return -5.0000000000000000000000e-01; }}; // 1/2!
template <> struct CC<1> { operator double() { return +4.1666666666666666666667e-02; }}; // 1/4!
template <> struct CC<2> { operator double() { return -1.3888888888888888888889e-03; }}; // 1/6!
template <> struct CC<3> { operator double() { return +2.4801587301587301587302e-05; }}; // 1/8!
template <> struct CC<4> { operator double() { return -2.7557319223985890652557e-07; }}; // 1/10!
template <> struct CC<5> { operator double() { return +2.0876756987868098979210e-09; }}; // 1/12!
template <> struct CC<6> { operator double() { return -1.1470745597729724713852e-11; }}; // 1/14!
template <> struct CC<7> { operator double() { return +4.7794773323873852974382e-14; }}; // 1/16!

// round
inline double __round(double x)
{
    return __fcfid(__fctid(x));
}

inline double __trunc(double x)
{
    return __fcfid(__fctidz(x));
}

inline double sin(double x)
{
    double absx = __fabs(x);
    double sign = __fsel(x, 1.0, -1.0);
    // Normalise x
    double num_cycles = __round(absx * InvPi());
    // Determine if num_cycles is even or odd
    double halfNumCycles = num_cycles * 0.5;
    double is_even = __trunc(halfNumCycles) - halfNumCycles;
    // -pi/2 <= norm_x < pi/2
    double norm_x = absx - Pi() * num_cycles;
    // Run Taylor series with funky factorisation.
    double y = norm_x * norm_x;
    double taylor = ((((((((SC<7>() * y + SC<6>())
                           * y + SC<5>())
                          * y + SC<4>())
                         * y + SC<3>())
                        * y + SC<2>())
                       * y + SC<1>())
                      * y + SC<0>())
                     * y);
    sign = __fsel(is_even, sign, -sign);
    double result = norm_x + norm_x * taylor;
    result *= sign;
    return result;
}

inline void sincos(double x, double *s, double *c)
{
    double absx = __fabs(x);
    // "Cosine" |x|.
    double cabsx = absx + HalfPi();

    double sign = __fsel(x, 1.0, -1.0);

    double num_cycles  = __round(absx * InvPi());

    // Determine if num_cycles is even or odd.
    double halfNumCycles = num_cycles * 0.5;
    double is_even = __trunc(halfNumCycles) - (halfNumCycles);

    // Normalise.
    double PiNumCycles = Pi() * num_cycles;
    double norm_x  = absx - PiNumCycles;
    double norm_cx = cabsx - PiNumCycles - HalfPi();

    // Run Taylor series with funky factorisation.
    double y  = norm_x * norm_x;
    double cy = norm_cx * norm_cx;

    double taylor = ((((((((SC<7>() * y + SC<6>())
        * y + SC<5>())
        * y + SC<4>())
        * y + SC<3>())
        * y + SC<2>())
        * y + SC<1>())
        * y + SC<0>())
        * y);
    double taylorc = ((((((((CC<7>() * cy + CC<6>())
        * cy + CC<5>())
        * cy + CC<4>())
        * cy + CC<3>())
        * cy + CC<2>())
        * cy + CC<1>())
        * cy + CC<0>())
        * cy);

    sign = __fsel(is_even, sign, -sign);
    double cos_sign = __fsel(is_even, 1.0, -1.0);

    double sine = norm_x + norm_x * taylor;
    double cosine = 1.0 + taylorc;

    sine *= sign;
    cosine *= cos_sign;

    *s = sine;
    *c = cosine;
}

inline double cos(double x)
{
    return sin(x + (Pi()/2.0));
}

inline float cos(float x)
{
    return float(cos(double(x)));
}

inline float sin(float x)
{
    return float(sin(double(x)));
}

inline void sincos(float fx, float *fs, float *fc)
{
    double x = double(fx);
    double s, c;
    sincos(x, &s, &c);
    *fs = float(s);
    *fc = float(c);
}

} // FastTrigo

#endif
