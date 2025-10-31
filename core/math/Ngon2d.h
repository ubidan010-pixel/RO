#ifndef _NGON2D_H_
#define _NGON2D_H_
//#include "DEFINES.H"

#define MAXPOLYLINES_PO2 10
#define MAXPOLYLINES (1<<MAXPOLYLINES_PO2)

/* ********************************************************************************
FLAGTYPE will determine the prec of tha BV culling strategy
Less bits mean smaller
More bits mean more prec
******************************************************************************** */
#define FLAGTYPE u32
//#define FLAGTYPE u16
//#define FLAGTYPE u8
#define NUMBER_OF_BITS (sizeof(FLAGTYPE)<<3)

namespace ITF
{
    FLAGTYPE GetMarge(u32 BitMin , u32 BitMax);
    FLAGTYPE GetBit(float Scalar);

    /* ********************************************************************************
    This class will manage a bit encoded BV(or kind of) for one line.
    Encode BV aligned on world axis AND BV turned 45°
    see FLAGTYPE for precision used 
    ******************************************************************************** */
    class Line_HACH
    {
        FLAGTYPE XMask,YMask;
        FLAGTYPE XYMask,YXMask;
    public:
        void operator |= (const Line_HACH &Src) ;
        void Compute_Line_HACH(Vec2d &pA,Vec2d &pB);
        u32 Test(Line_HACH &LH);
    };


    /* ********************************************************************************
    This class will help management of lines for avoiding N² computation
    main function if u32 GetNextTouch(Line_HACH	&LH,u32 Base) for parsing 
    ******************************************************************************** */
    class Space_HACH_2DLInes
    {
    public:
        Line_HACH	Buffer[MAXPOLYLINES * 2];
        Line_HACH	*MaskLines[MAXPOLYLINES_PO2];
        Vec2d		*Ngon;
        Vec2d		BMin,BMax;
        Vec2d		Norm_X,Norm_Add;
        u32			Line_INDEXES[2][MAXPOLYLINES];
        u32			ulNumberOfHach;
        float		Surface;

        Space_HACH_2DLInes() ;
        void ComputeOneHash(Vec2d &P1,Vec2d &P2,Line_HACH &Result);
        void ComputeOneHashOr(Vec2d &P1,Vec2d &P2,Line_HACH &ResultOr);
        void ComputeHash(u32 Base = 0);
        void Build(Vec2d *NgonSource,u32 ulNumberOfSeg);
        void AddALine(u32 Index1 , u32 Index2);
        u32 GetNextTouch(Line_HACH	&LH,u32 Base);
        u32 IsCut(Vec2d &Va,Vec2d &Vb,u32 Ic,u32 Id);
        u32 IsCut(u32 Ia,u32 Ib);
        u32 GetNumberOfCut(Vec2d &Va,Vec2d &Vb);
    };

    /* This function will triangulate any ngon */
    u32 FillNgon(Vec2d *pCD,u32 ulNumberOfPoints,u16 *pDstIndexes);

    /* 
    This will indicate how to connect 2 ngon by 
    giving index 1 in ngon 1 and index 2 ngon 2 
    return != 0 mean impossible 
    */
    u32 ConnectNgon(Vec2d *pCD1,u32 ulNumberOfPoints1,Vec2d *pCD2,u32 ulNumberOfPoints2,u16 *pIndexin1,u16 *pIndexin2);

    /* 
    This will say 
    0: they are separated 
    1: 1 is completely in 2
    2: 2 is completely in 1
    3: some edge are overlap
    */
    u32 CompareNgon(Vec2d *pCD1,u32 ulNumberOfPoints1,Vec2d *pCD2,u32 ulNumberOfPoints2);

} // namespace ITF

#endif // _NGON2D_H_

