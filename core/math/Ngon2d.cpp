//#include "TRACK.h"
//#include "NGON2D.h"

#include "precompiled_core.h"

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_NGON2D_H_
#include "Core/math/Ngon2d.h"
#endif // _ITF_NGON2D_H_

// void DEBUG_DRAWLINE(u32 Color, Vec2d *A,  Vec2d *B, int with) {};
// void DEBUG_DrawCross(u32 Color, Vec2d *A, int with) {};
//#pragma optimize("",off)

namespace ITF
{
    float DistanceToLineXY(const Vec2d &X,const Vec2d &A,const Vec2d &B)  
    { 
        Vec2d D,E;
        float R;
        D = X;
        D -= A;
        E = B;
        E -= A;
        //R = ((float)D.m_y * (float)E.m_x) - ((float)D.m_x * (float)E.m_y);
        Vec2d::SwapXmY(&E, &E);
        R = Vec2d::Dot(&D, &E);
        return R;
    };

    u32 IsRightXY(const Vec2d &X,const Vec2d &A,const Vec2d &B, u32 IsLeft = 0)  
    { 
        Vec2d D,E;
        float R;
        D = X;
        D -= A;
        E = B;
        E -= A;
        //R = ((float)D.m_y * (float)E.m_x) - ((float)D.m_x * (float)E.m_y);
        Vec2d::SwapXmY(&E, &E);
        R = Vec2d::Dot(&D, &E);
        if (IsLeft)
            return R > 0 ? 0 : 1;
        else
            return R < 0 ? 0 : 1;
    };




    /* ********************************************************************************
    FLAGTYPE will determine the prec of tha BV culling strategy
    Less bits mean smaller
    More bits mean more prec
    ******************************************************************************** */
    //#define FLAGTYPE u64
#define FLAGTYPE u32
    //#define FLAGTYPE u16
    //#define FLAGTYPE u8

#define NUMBER_OF_BITS (sizeof(FLAGTYPE)<<3)
    FLAGTYPE GetMarge(u32 BitMin , u32 BitMax)
    {
        if (BitMin>BitMax)
            return (((((FLAGTYPE)1<<BitMin)-(FLAGTYPE)1)<<1)+(FLAGTYPE)1 ) - (((((FLAGTYPE)1<<BitMax)-(FLAGTYPE)1)));
        else
            return (((((FLAGTYPE)1<<BitMax)-(FLAGTYPE)1)<<1)+(FLAGTYPE)1 ) - (((((FLAGTYPE)1<<BitMin)-(FLAGTYPE)1)));
    }

    FLAGTYPE GetBit(float Scalar)
    {
        if (Scalar < (float)1) return 0;
        if (Scalar >= (float)NUMBER_OF_BITS) return NUMBER_OF_BITS-1;//*/
        return (u32)(Scalar);// <- this MUST be a floor, not a round
    }

    /* ********************************************************************************
    This class will manage a bit encoded BV(or kind of) for one line.
    Encode BV aligned on world axis AND BV turned 45°
    see FLAGTYPE for precision used 
    ******************************************************************************** */
    void Line_HACH::operator |= (const Line_HACH &Src) 
    { 
        XMask |= Src.XMask; 
        YMask |= Src.YMask; 
        XYMask |= Src.XYMask; 
        YXMask |= Src.YXMask;
    };
    void Line_HACH::Compute_Line_HACH(Vec2d &pA,Vec2d &pB)
    {
        XMask = GetMarge(GetBit(pA.m_x), GetBit(pB.m_x));
        YMask = GetMarge(GetBit(pA.m_y), GetBit(pB.m_y));
        XYMask = GetMarge(GetBit(0.707106781186f * (pA.m_x+pA.m_y)), GetBit(0.707106781186f * (pB.m_x+pB.m_y)));
        YXMask = GetMarge(GetBit(0.707106781186f * (pA.m_x+(float)NUMBER_OF_BITS-pA.m_y)), GetBit(0.707106781186f * (pB.m_x+(float)NUMBER_OF_BITS-pB.m_y)));
    }
    u32 Line_HACH::Test(Line_HACH &LH)
    {
        if ((LH.XMask & XMask) && (LH.YMask & YMask) && (LH.XYMask & XYMask) && (LH.YXMask & YXMask)) return 1;
        return 0;
    }


    /* ********************************************************************************
    This class will help management of lines for avoiding N² computation
    main function if u32 GetNextTouch(Line_HACH	&LH,u32 Base) for parsing 
    ******************************************************************************** */
    Space_HACH_2DLInes::Space_HACH_2DLInes() 
        : Ngon(NULL)
    {
        Line_HACH *pCurrentBuffer = Buffer;
        for (u32 i = 0 ; i < MAXPOLYLINES_PO2 ; i ++)
        {
            MaskLines[i] = pCurrentBuffer;
            pCurrentBuffer += 1ull << (MAXPOLYLINES_PO2 - i);
        }
        ulNumberOfHach = 0;
    };
    void Space_HACH_2DLInes::ComputeOneHash(Vec2d &P1,Vec2d &P2,Line_HACH &Result)
    {
        Vec2d Last = P1;
        Vec2d New = P2;
        Last -= Norm_Add;
   //     Last &= Norm_X;
        Last *= Norm_X;
        New -= Norm_Add;
      //  New &= Norm_X;
        New *= Norm_X;

        Result.Compute_Line_HACH(Last,New);
    }
    void Space_HACH_2DLInes::ComputeOneHashOr(Vec2d &P1,Vec2d &P2,Line_HACH &ResultOr)
    {
        Line_HACH Result;
        ComputeOneHash(P1,P2,Result);
        ResultOr |= Result;
    }
    void Space_HACH_2DLInes::ComputeHash(u32 Base)
    {
        for (u32 Counter = Base ; Counter < ulNumberOfHach ; Counter++ )
        {
            Line_HACH Local;
            ComputeOneHash(Ngon[Line_INDEXES[0][Counter]],Ngon[Line_INDEXES[1][Counter]],Local);
            MaskLines[0][Counter] = Local;
            if ((Counter & 3) == 0) 
                MaskLines[1][Counter>>2] = Local;
            else
                MaskLines[1][Counter>>2] |= Local;
            if ((Counter & 15) == 0) 
                MaskLines[2][Counter>>4] = Local;
            else
                MaskLines[2][Counter>>4] |= Local;
            if ((Counter & 63) == 0) 
                MaskLines[3][Counter>>6] = Local;
            else
                MaskLines[3][Counter>>6] |= Local;
        }
    }
    void Space_HACH_2DLInes::Build(Vec2d *NgonSource,u32 ulNumberOfSeg)
    {
        Ngon = NgonSource;
        ulNumberOfHach = ulNumberOfSeg;
        BMin = BMax = *Ngon;
        for (u32 Counter = 0 ; Counter < ulNumberOfSeg ; Counter++ )
        {
            BMin.m_x = BMin.m_x < Ngon[Counter].m_x ? BMin.m_x : Ngon[Counter].m_x;
            BMin.m_y = BMin.m_y < Ngon[Counter].m_y ? BMin.m_y : Ngon[Counter].m_y;
            BMax.m_x = BMax.m_x > Ngon[Counter].m_x ? BMax.m_x : Ngon[Counter].m_x;
            BMax.m_y = BMax.m_y > Ngon[Counter].m_y ? BMax.m_y : Ngon[Counter].m_y;
        }
        Norm_Add = BMin;
        Norm_X = BMax - BMin;
        Norm_X.m_x = (float)NUMBER_OF_BITS / Norm_X.m_x;
        Norm_X.m_y = (float)NUMBER_OF_BITS / Norm_X.m_y;
        u32 Last = ulNumberOfSeg-1;
        Surface = 0;
        for (u32 Counter = 0 ; Counter < ulNumberOfSeg ; Counter++ )
        {
            Line_INDEXES[0][Counter] = Counter;
            Line_INDEXES[1][Counter] = Last;
            Surface -= (Ngon[Counter].m_x - Ngon[Last].m_x) * (Ngon[Counter].m_y + Ngon[Last].m_y);
            Last = Counter;
        }
        Surface *= 0.5f;
        ComputeHash();
    }
    void Space_HACH_2DLInes::AddALine(u32 Index1 , u32 Index2)
    {
        Line_INDEXES[0][ulNumberOfHach] = Index1;
        Line_INDEXES[1][ulNumberOfHach++] = Index2;
        ComputeHash(ulNumberOfHach-1);
    }

    u32 Space_HACH_2DLInes::GetNextTouch(Line_HACH	&LH,u32 Base)
    {
        /* Square search */
        if (Base >= ulNumberOfHach) return U32_INVALID;
        else return Base;
        
        /* Fast Search */
     /*   while (Base < ulNumberOfHach)
        {
            if (((Base & 3) == 0) && (!MaskLines[1][Base>>2].Test(LH)))
            {
                if (((Base & 15) == 0) && (!MaskLines[2][Base>>4].Test(LH)))
                {
                    if (((Base & 63) == 0) && (!MaskLines[3][Base>>6].Test(LH)))
                        Base += 64;
                    else
                        Base += 16;
                }
                else
                    Base += 4;
            }
            else
            {
                if (MaskLines[0][Base].Test(LH)) 
                    return Base;
                else
                    Base++;
            }
        }
        return -1;*/
    };

    u32 Space_HACH_2DLInes::IsCut(Vec2d &Va,Vec2d &Vb,u32 Ic,u32 Id)
    {
        double D1,D2;
#define Epsilon 0.0005f
        D1 = DistanceToLineXY(Va,Ngon[Ic],Ngon[Id]);
        D2 = DistanceToLineXY(Vb,Ngon[Ic],Ngon[Id]);
        if ((D1 > Epsilon)&&(D2 > Epsilon)) return 0;
        if ((D1 < -Epsilon)&&(D2 < -Epsilon)) return 0;
        D1 = DistanceToLineXY(Ngon[Ic],Va,Vb);
        D2 = DistanceToLineXY(Ngon[Id],Va,Vb);
        if ((D1 > Epsilon)&&(D2 > Epsilon)) return 0;
        if ((D1 < -Epsilon)&&(D2 < -Epsilon)) return 0;
        return 1;
    }

    u32 Space_HACH_2DLInes::IsCut(u32 Ia,u32 Ib)
    {
        Line_HACH	LH;
        ComputeOneHash(Ngon[Ia],Ngon[Ib],LH);

        u32 Counter = GetNextTouch(LH,0);
        while (Counter != U32_INVALID)
        {
            if (((Ngon[Ia] - Ngon[Line_INDEXES[0][Counter]]).sqrnorm()) && 
                ((Ngon[Ib] - Ngon[Line_INDEXES[0][Counter]]).sqrnorm()) &&
                ((Ngon[Ia] - Ngon[Line_INDEXES[1][Counter]]).sqrnorm()) &&
                ((Ngon[Ib] - Ngon[Line_INDEXES[1][Counter]]).sqrnorm()))
            {
                if (IsCut(Ngon[Ia],Ngon[Ib],Line_INDEXES[0][Counter],Line_INDEXES[1][Counter])) 
                    return 1;
            }
            Counter = GetNextTouch(LH,Counter + 1);
        }
        return 0;
    }

    u32 Space_HACH_2DLInes::GetNumberOfCut(Vec2d &pA,Vec2d &pB)
    {
        Line_HACH	LH;
        u32 ulNumberOfCut = 0;

        ComputeOneHash(pA,pB,LH);

        u32 Counter = GetNextTouch(LH,0);
        while (Counter != U32_INVALID)
        {
            if (((pA - Ngon[Line_INDEXES[0][Counter]]).sqrnorm()) && 
                ((pB - Ngon[Line_INDEXES[0][Counter]]).sqrnorm()) &&
                ((pA - Ngon[Line_INDEXES[1][Counter]]).sqrnorm()) &&
                ((pB - Ngon[Line_INDEXES[1][Counter]]).sqrnorm()))
            {
                if (IsCut(pA,pB,Line_INDEXES[0][Counter],Line_INDEXES[1][Counter])) 
                    ulNumberOfCut ++;
            }
            Counter = GetNextTouch(LH,Counter + 1);
        }
        return ulNumberOfCut;
    }

    /* ********************************************************************************
    This function will triangulate the "ulNumberOfPoints" points int the "pCD" source 
    Try to make a LINEAR time computation, with help of Space_HACH_2DLInes
    Return NumberofIndex Created
    return 0 if fail (ex bubbles, lines crosses , number > (1<<MAXPOLYLINES_PO2), etc..)
    ******************************************************************************** */
    u32 FillNgon(Vec2d *pCD,u32 ulNumberOfPoints,u16 *pDstIndexes)
    {
        u32 Killed[((1<<MAXPOLYLINES_PO2)>>5) + 1];
        u32 CCW = 1 , Inifinite = 0, Current = 0 , Nm2 = ulNumberOfPoints - 2, Np1 = 1 , Nm1 = ulNumberOfPoints - 1,Diags = ulNumberOfPoints - 3;
        u32 TNUm = 0;
        if (ulNumberOfPoints > 1<<MAXPOLYLINES_PO2) return 0;
        if (ulNumberOfPoints < 3) return 0;
        memset(Killed,0,sizeof(Killed));

        Space_HACH_2DLInes SH;
        SH.Build(pCD,ulNumberOfPoints);
        if (SH.Surface < 0) CCW ^= 1;
        //if (!CCW) return 0;
        u32 KKKFf = 0;
        while (Diags)
        {
            if (!(Killed[Current>>5] & (1<<(Current&31))))
            {
                if	((((KKKFf & 7)== 0) || (Inifinite > 2)) && 
                    (IsRightXY(pCD[Current],pCD[Nm1],pCD[Nm2],CCW) && (!SH.IsCut(Current,  Nm2))))
                {
                    if (IsRightXY(pCD[Np1],pCD[Nm2],pCD[Current],CCW) && 
                        IsRightXY(pCD[Np1],pCD[Nm1],pCD[Nm2],CCW) &&
                        IsRightXY(pCD[Np1],pCD[Current],pCD[Nm1],CCW))
                    {
                        Nm2 = Nm1;
                        Nm1 = Current;
                    } else
                    {
                        *(pDstIndexes++) = (u16)Current;
                        pDstIndexes[1-CCW] = (u16)Nm1;
                        pDstIndexes[CCW] = (u16)Nm2;
                        SH.AddALine(Nm1,Nm2);
                        KKKFf = 0;

                        pDstIndexes+=2;
                        TNUm ++;
                        //	DEBUG_DRAWLINE(0xff005000, pCD + Current,  pCD + Nm2, 0);
                        Killed[Nm1>>5] |= 1<<(Nm1&31);
                        Diags--;
                        Inifinite = 0;
                        Nm1 = Current;
                    }
                } else
                {
                    Nm2 = Nm1;
                    Nm1 = Current;
                }
                KKKFf++;
            }

            Current = Np1 ;
            Np1 ++;
            if (Np1 == ulNumberOfPoints) 
            {
                Np1 = 0;	
                Inifinite++;
            }
            if (Inifinite > 5) 
                Diags = 0;
        }

        if (Inifinite > 5)
        {
            // #define DEBUG_INFO
            // #ifdef DEBUG_INFO
            // 			for (int i = 0; i < ulNumberOfPoints ; i++)
            // 			if (!(Killed[i>>5] & (1<<(i&31))))
            // 			{
            // 				Nm1 = Current;
            // 				Current = i;
            // 			}
            // 			for (int i = 0; i < ulNumberOfPoints ; i++)
            // 			if (!(Killed[i>>5] & (1<<(i&31))))
            // 			{
            // 				DEBUG_DRAWLINE(0xff0000FF, pCD + Current, pCD +  Nm1, 2);
            // 				DEBUG_DrawCross(0xff0000FF, pCD + Current, 2);
            // 				DEBUG_DrawCross(0xff0000FF, pCD + Nm1, 2);
            // 				Nm1 = Current;
            // 				Current = i;
            // 			} else
            // 				DEBUG_DrawCross(0xffFF00FF, pCD + i, 2);
            // #endif
            return 0;
        }

        Current = Nm2 = Nm1 = U32_INVALID;
        for (u32 k = 0; k < 2; k++)
        {
            for (u32 i = 0; i < ulNumberOfPoints; i++)
            {
                if (!(Killed[i >> 5] & (1 << (i & 31))))
                {
                    Nm2 = Nm1;
                    Nm1 = Current;
                    Current = i;
                }
            }
        }

        if (Nm2 != U32_INVALID)
        {
            *(pDstIndexes++) = (u16)Current;
            pDstIndexes[1-CCW] = (u16)Nm1;
            pDstIndexes[CCW] = (u16)Nm2;
            pDstIndexes+=2;
            TNUm ++;
        }

        return TNUm;
    }

    /* 
    This will indicate how to connect 2 ngon by 
    giving index 1 in ngon 1 and index 2 ngon 2 
    return != 0 mean impossible 
    */
    u32 ConnectNgon(Vec2d *pCD1,u32 ulNumberOfPoints1,Vec2d *pCD2,u32 ulNumberOfPoints2,u16 *pIndexin1,u16 *pIndexin2)
    {
        return 1;
    }

    /* 
    This will say 
    0: they are separated 
    1: 1 is completely in 2
    2: 2 is completely in 1
    3: some edge are overlap
    */
    u32 CompareNgon(Vec2d *pCD1,u32 ulNumberOfPoints1,Vec2d *pCD2,u32 ulNumberOfPoints2)
    {
        Space_HACH_2DLInes SH;
        if (!ulNumberOfPoints2) return 0;
        if (!ulNumberOfPoints1) return 0;
        SH.Build(pCD2,ulNumberOfPoints2);
        Vec2d ToTest;
        ToTest = SH.BMax + Vec2d(10.f,10.f);
        u32 NoC = SH.GetNumberOfCut(ToTest,*pCD1);
        if (NoC & 1)
            return 1; /* 1 Is inside 2 */
        else
        {
            SH.Build(pCD1,ulNumberOfPoints1);
            ToTest = *pCD2;
            ToTest = SH.BMax + Vec2d(10.f,10.f);
            u32 NoC = SH.GetNumberOfCut(ToTest,*pCD2);
            if (NoC & 1) return 2; /* 2 Is inside 1 */
            return 0; /* Not Cut */
        }
    }

} // namespace ITF

