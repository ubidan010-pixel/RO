#include "precompiled_GFXAdapter_WII.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <revolution.h>
#include <revolution/sc.h>
#include <revolution/mtx.h>

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#include "core/memory/memory_WII.h"

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ENGINEMONITORPLUGIN_H_
#include "tools/plugins/EngineMonitorPlugin/EngineMonitorPlugin.h"
#endif //_ITF_ENGINEMONITORPLUGIN_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_
#ifndef _ITF_TRCADAPTER_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_WII.h"
#endif //_ITF_TRCADAPTER_WII_H_


#define DEFAULT_FIFO_SIZE (320 * 1024)

namespace ITF
{
//----

/*---------------------------------------------------------------------------*
 * Quick and dirty queue implementation.
 *---------------------------------------------------------------------------*/

typedef struct QItem_
{
    void* writePtr;
    void* dataPtr;
    void* copyXFB;
} QItem;

#define QUEUE_MAX 5
#define QUEUE_EMPTY QUEUE_MAX

typedef struct Queue_
{
    QItem entry[QUEUE_MAX];
    u16 top;
    u16 bot;
} Queue;

/*---------------------------------------------------------------------------*
 * Data needed for triple-buffering.
 *---------------------------------------------------------------------------*/

static Queue RenderQ;   // Queue for frames in FIFO
static Queue DoneQ;     // Queue for frames finished already

void* myXFB1 = 0;    // Pointers to the two XFB's
void* myXFB2 = 0;
void* copyXFB;   // Which XFB to copy to next
void* dispXFB;   // Which XFB is being displayed now

volatile GXBool BPSet  = GX_FALSE;        // Is the FIFO breakpoint set?
volatile GXBool BPWait = GX_FALSE;        // Is breakpoint reset waiting on VBlank?
volatile GXBool BPGo   = GX_FALSE;        // Indicates breakpoint should be released

volatile u32  DoneCount = 0; // Value to use for new sync token.

static u8 *CUThreadStack = NULL;       // Stack for clean-up thread
static OSThreadQueue waitingDoneRender; // Threads waiting for frames to finish
static OSThread CUThread;               // OS data for clean-up thread

void   BPCallback      ( void );
void   VIPreCallback   ( ::u32 retraceCount );
void   VIPostCallback  ( ::u32 retraceCount );
void*  CleanupThread   ( void* param );
void   SetNextBreakPt  ( void );

#ifndef ITF_FINAL
/*---------------------------------------------------------------------------*
 * Compute GPU time.
 *---------------------------------------------------------------------------*/
volatile f32 gf32_DBG_gpu = 0.f;
void Tick_GPUFrame(bool end)
{
    static volatile f64 lastUpdate     = 0.0;
    static volatile f64 gpuTOPt0       = 0.0;

    static volatile f64 cumulativeTime = 0.0;
    static volatile u32 numCalls       = 0;

    if(!end)
    {
        gpuTOPt0 = SYSTEM_ADAPTER->getTime();
    }
    else
    {
        f64 gpuTOPt1 = SYSTEM_ADAPTER->getTime();

        cumulativeTime += (gpuTOPt1 - gpuTOPt0);
        numCalls++;

        if((gpuTOPt1 - lastUpdate) > 1.0)
        {
            gf32_DBG_gpu   = (f32)((f64)numCalls / cumulativeTime);

            cumulativeTime = 0.0;
            numCalls       = 0;
            lastUpdate     = gpuTOPt1;
        }
    }
}

//----------------------------------------------------------------------------//
// To flip endianess and store
static void SetLeU32(u8* ptr, u32 data)
{
    *ptr     = (u8)(data & 0x000000FF);
    *(ptr+1) = (u8)((data >> 8)  & 0x000000FF);
    *(ptr+2) = (u8)((data >> 16) & 0x000000FF);
    *(ptr+3) = (u8)((data >> 24) & 0x000000FF);
}

static void SetLeU16(u8* ptr, u16 data)
{
    *ptr     = (u8)(data & 0x00FF);
    *(ptr+1) = (u8)((data >> 8) & 0x00FF);
}

//----------------------------------------------------------------------------//
static void doScreenshot(u32 _h, const char *_szFileName)
{
    if(!PLUGINGATEWAY)
        return;
    EngineMonitorPlugin *plug = (EngineMonitorPlugin*) PLUGINGATEWAY->getPluginByName(EngineMonitorPlugin::getPluginName());
    if(!plug)
        return;

    char szFullFileName[256];
    sprintf(szFullFileName, "%s.bmp", _szFileName);
    char lineBuff[3*640];

    //
    // bmp header file
    //
    #define BMP_HEADER_SIZE     54
    unsigned long col;
    u8* dp = (u8*)lineBuff;
    u32 bfSize = 640*_h*3+BMP_HEADER_SIZE;

    // Create file header

    dp[0] = 'B';                        // bfType
    dp[1] = 'M';

    SetLeU32(&dp[2], bfSize);           // bfSize
    SetLeU16(&dp[6], 0);                // bfReserved1
    SetLeU16(&dp[8], 0);                // bfReserved2
    SetLeU32(&dp[10], BMP_HEADER_SIZE); // bfOffBits


    // Create information header

    SetLeU32(&dp[14], 40);   // biSize
    SetLeU32(&dp[18], 640);  // biWidth
    SetLeU32(&dp[22], _h);   // biHeight

    SetLeU16(&dp[26], 1);    // biPlanes
    SetLeU16(&dp[28], 24);   // biBitCount

    SetLeU32(&dp[30], 0);    // biCompression
    SetLeU32(&dp[34], 0);    // biSizeimage
    SetLeU32(&dp[38], 0);    // biXPelsPerMeter
    SetLeU32(&dp[42], 0);    // biYPelsPerMeter
    SetLeU32(&dp[46], 0);    // biClrUsed
    SetLeU32(&dp[50], 0);    // biClrImportant

    // Send bitmap header
    plug->sendFile(szFullFileName, lineBuff, BMP_HEADER_SIZE, bfalse, btrue);

    GFX_ADAPTER->syncGPU();
    
    //
    // bmp raw data
    //
    for(int y=_h-1; y>=0; --y)
    {
        unsigned char *pbuff = (unsigned char *)lineBuff;
        for(int x = 0; x < 640; ++x)
        {
            GXPeekARGB( x, y, &col );
            col &= 0xFFFFFF;
            *pbuff++ = (unsigned char)(col);
            *pbuff++ = (unsigned char)(col >> 8);
            *pbuff++ = (unsigned char)(col >> 16);
        }
        // Send one bitmap line.
        plug->sendFile(szFullFileName, lineBuff, 3*640, btrue, btrue);
    }
}
#endif // ITF_FINAL

/*---------------------------------------------------------------------------*
 * Quick and dirty queue implementation.
 *---------------------------------------------------------------------------*/

void init_queue(Queue *q)
{
    q->top = QUEUE_EMPTY;
}

void enqueue(Queue *q, QItem *qitm)
{
    if (q->top == QUEUE_EMPTY)
    {
        q->top = q->bot = 0;
    }
    else
    {
        q->top = (u16) ((q->top+1) % QUEUE_MAX);

        if (q->top == q->bot)
        {   // error, overflow
            OSHalt("queue overflow");
        }
    }

    q->entry[q->top] = *qitm;
}

QItem dequeue(Queue *q)
{
    u16 bot = q->bot;

    if (q->top == QUEUE_EMPTY)
    {   // error, underflow
        OSHalt("queue underflow");
    }

    if (q->bot == q->top)
    {
        q->top = QUEUE_EMPTY;
    }
    else
    {
        q->bot = (u16) ((q->bot+1) % QUEUE_MAX);
    }

    return q->entry[bot];
}

QItem queue_front(Queue *q)
{
    if (q->top == QUEUE_EMPTY)
    {   // error, queue empty
        OSHalt("queue_top: queue empty");
    }

    return q->entry[q->bot];
}

GXBool queue_empty(Queue *q)
{
    return q->top == QUEUE_EMPTY;
}

u32 queue_length(Queue *q)
{
    if (q->top == QUEUE_EMPTY) return 0;

    if (q->top >= q->bot)
        return (u32) ((s32) q->top - q->bot + 1);
    else
        return (u32) ((s32) (q->top + QUEUE_MAX) - q->bot + 1);
}


/*---------------------------------------------------------------------------*
   Breakpoint Interrupt Callback
 *---------------------------------------------------------------------------*/

void BPCallback ( void )
{
    QItem qitm;

    qitm = queue_front(&RenderQ);

    DoneCount++;

    // Check whether or not the just-finished frame can be
    // copied already or if it must wait (due to lack of a
    // free XFB).  If it must wait, set a flag for the VBlank
    // interrupt callback to take care of it.

    if( qitm.copyXFB == dispXFB )
    {
        BPWait = GX_TRUE;
    }
    else
    {
        SetNextBreakPt();
    }

#ifndef ITF_FINAL
    Tick_GPUFrame(true);
#endif // ITF_FINAL
}

/*---------------------------------------------------------------------------*
   Routine to move breakpoint ahead, deal with finished frames.
 *---------------------------------------------------------------------------*/

void SetNextBreakPt ( void )
{
    QItem qitm;

    // Move entry from RenderQ to DoneQ.

    qitm = dequeue(&RenderQ);

    enqueue(&DoneQ, &qitm);

    OSWakeupThread( &waitingDoneRender );

    // Move breakpoint to next entry, if any.

    if (queue_empty(&RenderQ))
    {
        GXDisableBreakPt();
        BPSet = GX_FALSE;
    }
    else
    {
        qitm = queue_front(&RenderQ);
        GXEnableBreakPt( qitm.writePtr );
    }
}

/*---------------------------------------------------------------------------*
   VI Pre Callback (VBlank interrupt)

   The VI Pre callback should be kept minimal, since the VI registers
   must be set before too much time passes.  Additional bookkeeping is
   done in the VI Post callback.

 *---------------------------------------------------------------------------*/

void VIPreCallback ( ::u32 retraceCount )
{
#pragma unused (retraceCount)
    if (DoneCount > 0)
    {
        DoneCount--;

        dispXFB = (dispXFB == myXFB1) ? myXFB2 : myXFB1;

        VISetNextFrameBuffer( dispXFB );
        VIFlush();

        BPGo = GX_TRUE;
    }
}

/*---------------------------------------------------------------------------*
   VI Post Callback (VBlank interrupt)
 *---------------------------------------------------------------------------*/

void VIPostCallback ( ::u32 retraceCount )
{
#pragma unused (retraceCount)
    if (BPWait && BPGo)
    {
        BPWait = GX_FALSE;
        BPGo = GX_FALSE;

        SetNextBreakPt();

        return;
    }
}

/*---------------------------------------------------------------------------*
   Cleanup Thread
 *---------------------------------------------------------------------------*/

void* CleanupThread ( void* param )
{
    #pragma unused (param)

    QItem qitm;
    while(1)
    {
        OSSleepThread( &waitingDoneRender );
        // Take qitm.dataPtr and do any necessary cleanup.
        // That is, free up any data that only needed to be
        // around for the GP to read while rendering the frame.
        qitm = dequeue(&DoneQ);

        GFX_ADAPTER_WII->OnGPUFrameEnd();
    }
}

bbool GFXAdapter_WII::WaitRenderQueueLen(u32 _len)
{
    // We must keep latency down while still keeping the FIFO full.
    // We allow only two frames to be in the FIFO at once.

    bbool res = bfalse;

#ifndef ITF_FINAL
    f64 time = SYSTEM_ADAPTER->getEllaspedFromStart();
#endif // ITF_FINAL

    // This is a critical section that requires no interrupts to
    // happen in between the "if" and the "sleep".  The sleep will
    // re-enable interrupts, allowing one to wake up this thread.
    int enabled = OSDisableInterrupts();

    // Wait for vertical retrace and GX execution end.
    while(queue_length(&RenderQ) > _len)
    {
        OSSleepThread( &waitingDoneRender );
        res = btrue;
    }

    OSRestoreInterrupts(enabled);

#ifndef ITF_FINAL
    mf64_VBLSyncTime += SYSTEM_ADAPTER->getEllaspedFromStart() - time;
#endif // ITF_FINAL

    return res;
}

bbool GFXAdapter_WII::waitPreviousGPUframeEnd()
{
    return WaitRenderQueueLen(0);
}

void GFXAdapter_WII::syncGPU()
{
    LockRendering();

    #ifndef ITF_FINAL
    f64 time = SYSTEM_ADAPTER->getEllaspedFromStart();
    #endif // ITF_FINAL

    OSTick CurrentTick = OSGetTick();
    u16 token;
    GXSetDrawSync(0xcafe); // add token.
    GXFlush();
    while (0xcafe != (token = GXReadDrawSync())) // wait token.
    {
        if(OSTicksToMilliseconds(OSDiffTick(OSGetTick(), CurrentTick)) > 500)
        {
            #ifdef ASSERT_ENABLED
            ITF_ASSERT_CRASH(0,"GPU hang!");
            #elif !defined(ITF_FINAL)
            int *a = NULL;
            *a = 2;
            #endif
            break;
        }
    }
    GXSetDrawSync(0xcaff); // replace current token.

    #ifndef ITF_FINAL
    // $$$$ TODO increment an other sync counter.
    mf64_VBLSyncTime += SYSTEM_ADAPTER->getEllaspedFromStart() - time;
    #endif // ITF_FINAL

    UnlockRendering();
}

bbool GFXAdapter_WII::setScreenProtection(bbool _enable)
{
    return VIEnableDimming(_enable ? TRUE : FALSE) ? btrue : bfalse;
}

//----------------------------------------------------------------------------//

ITF_INLINE void MATRIXconvert4x4TOWii(Mtx &_wiiMtx, const GMatrix44* _matrix4x4)
{
	MTXRowCol(_wiiMtx,0,0) = _matrix4x4->_m11;
	MTXRowCol(_wiiMtx,1,0) = _matrix4x4->_m12;
	MTXRowCol(_wiiMtx,2,0) = _matrix4x4->_m13;
	MTXRowCol(_wiiMtx,0,1) = _matrix4x4->_m21;
	MTXRowCol(_wiiMtx,1,1) = _matrix4x4->_m22;
	MTXRowCol(_wiiMtx,2,1) = _matrix4x4->_m23;
	MTXRowCol(_wiiMtx,0,2) = _matrix4x4->_m31;
	MTXRowCol(_wiiMtx,1,2) = _matrix4x4->_m32;
	MTXRowCol(_wiiMtx,2,2) = _matrix4x4->_m33;
	MTXRowCol(_wiiMtx,0,3) = _matrix4x4->_m41;
	MTXRowCol(_wiiMtx,1,3) = _matrix4x4->_m42;
	MTXRowCol(_wiiMtx,2,3) = _matrix4x4->_m43;
}

ITF_INLINE void MATRIXconvert4x4TOWii(Mtx44 &_wiiMtx, const GMatrix44* _matrix4x4)
{
	MTXRowCol(_wiiMtx,0,0) = _matrix4x4->_m11;
	MTXRowCol(_wiiMtx,1,0) = _matrix4x4->_m12;
	MTXRowCol(_wiiMtx,2,0) = _matrix4x4->_m13;
	MTXRowCol(_wiiMtx,3,0) = _matrix4x4->_m14;
	MTXRowCol(_wiiMtx,0,1) = _matrix4x4->_m21;
	MTXRowCol(_wiiMtx,1,1) = _matrix4x4->_m22;
	MTXRowCol(_wiiMtx,2,1) = _matrix4x4->_m23;
	MTXRowCol(_wiiMtx,3,1) = _matrix4x4->_m24;
	MTXRowCol(_wiiMtx,0,2) = _matrix4x4->_m31;
	MTXRowCol(_wiiMtx,1,2) = _matrix4x4->_m32;
	MTXRowCol(_wiiMtx,2,2) = _matrix4x4->_m33;
	MTXRowCol(_wiiMtx,3,2) = _matrix4x4->_m34;
	MTXRowCol(_wiiMtx,0,3) = _matrix4x4->_m41;
	MTXRowCol(_wiiMtx,1,3) = _matrix4x4->_m42;
	MTXRowCol(_wiiMtx,2,3) = _matrix4x4->_m43;
	MTXRowCol(_wiiMtx,3,3) = _matrix4x4->_m44;
}

//----------------------------------------------------------------------------//

GFXAdapter_WII::GFXAdapter_WII()
    : m_FifoBuffer(0)
    , m_FifoObj(0)
    , Rmode(0)
    , m_VFilterEnable(GX_TRUE)
    , m_Texture(NULL)
    , m_CurrentTevType(TEV_T)
    , m_CurrentRTPass(RT_PASS_COLOR)
    , mu32_CurClearColor(COLOR_ZERO)
    , m_zTest(bfalse)
    , m_zFuncComp(GX_ALWAYS)
    , m_zWrite(bfalse)
	, FakeAAFlag(0)
#ifndef ITF_FINAL
    , mf64_VBLSyncTime(0.0)
    , m_fPsNoVsync(0.f)
#endif // ITF_FINAL
{
    mg_UVmat.setIdentity();

    setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);

	Synchronize::createCriticalSection(&m_csRendering);

    for(u32 i=0 ; i<AFX_MAX_RT ; ++i)
        mp_AFX_RT[i] = NULL;
    mb_AFXBfrLocked = bfalse;
    mb_ClearOnCopy  = btrue;
    m_AFXCurRT      = AFX_RT;

    mf32_EFBScale[0] = 1.f;
    mf32_EFBScale[1] = 1.f;

    mv_StaticUtilScreenSize.m_x = (f32)getScreenWidth();
    mv_StaticUtilScreenSize.m_y = (f32)getScreenHeight();

    mv_RealScreenSize.m_x = (f32)getScreenWidth();
    mv_RealScreenSize.m_y = (f32)getScreenHeight();
}

//----------------------------------------------------------------------------//

GFXAdapter_WII::~GFXAdapter_WII()
{
    ITF_ASSERT(m_NormalTex.size() == 0);
    m_NormalTex.clear();

    if(myXFB1)
    {
        Memory::alignedFree(myXFB1);
        myXFB1 = NULL;
    }
    if(myXFB2)
    {
        Memory::alignedFree(myXFB2);
        myXFB2 = NULL;
    }

    OSCancelThread(&CUThread);
    if(CUThreadStack)
    {
        Memory::alignedFree(CUThreadStack);
        CUThreadStack = NULL;
    }

    if(m_FifoBuffer)
    {
        Memory::alignedFree(m_FifoBuffer);
        m_FifoBuffer = NULL;
    }

    if(FontData)
    {
        Memory::alignedFree(FontData);
        FontData = NULL;
    }

    removeInternalBuffers();

    Synchronize::destroyCriticalSection(&m_csRendering);
}


//----------------------------------------------------------------------------//
// FakeAA
void GFXAdapter_WII::forceDisableFakeAA(bbool _disable)
{
    if(_disable)
    {
        if(isFakeAAEnabled())
        {
            GFX_RECT Viewport;
            getViewport(&Viewport);

            addFakeAAFlag(FAKEAA_FLAG_DISABLE);

            setupViewport(&Viewport);
        }
    }
    else if(FakeAAFlag & FAKEAA_FLAG_DISABLE)
    {
        GFX_RECT Viewport;
        getViewport(&Viewport);

        subFakeAAFlag(FAKEAA_FLAG_DISABLE);

        setupViewport(&Viewport);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setCamera(float _x, float _y, float _z, Camera* _cam)
{
    addFakeAAFlag(FAKEAA_FLAG_PERSP);

    GFXAdapter::setCamera(_x, _y, _z, _cam);

    f32 nearPlane = _cam->isBiased() ? 1.f : _cam->m_near;
    f32 farPlane  = _cam->m_far;

    _cam->forcePosition(Vec3d(_x,_y,_z));

    Vec3d vEyePt(_cam->getX(), _cam->getY(), _cam->getZ() * 1.0f);
    Vec3d vLookatPt(_cam->getX(), _cam->getY(), _cam->getZ());
    Vec3d vUpVec(0.0f, 1.0f, 0.0f);

    vLookatPt.m_z -= 1.f;

	//-----

    mg_World.setIdentity();

    GFX_computeMatrixLookAtRH(&mg_View, &vEyePt, &vLookatPt, &vUpVec);

    if (_cam->getRotateCam() && !_cam->isBiased() && (_cam->getMode() == Camera::Camera_InGame))
    {
        m_useRotateZCamera = btrue;
        Matrix44 rz;
        M44_setMatrixRotationZ(&rz, _cam->getRotateZAngle());
        M44_mul4x4Out((Matrix44*)&mg_ViewRender, (const Matrix44*)&mg_View, &rz);
    }
    else
    {
        m_useRotateZCamera = bfalse;
    }

    GFX_computeMatrixPerspectiveFovRH(&mg_Proj, _cam->getFocale(), _cam->m_screen_ratio, nearPlane, farPlane);

    _cam->m_lookAt = vLookatPt;

    _cam->m_resultingBiasedPosition = vEyePt;
	//_cam->m_resultingBiasedPosition.m_z = 8.0f;

    computeRenderMatrix();
	updateCameraFrustumPlanes(_cam);

    //----

    // Wii Projection
    Mtx44 mproj;
    MATRIXconvert4x4TOWii(mproj, &mg_Proj);
    MTXRowCol(mproj, 2, 2) = MTXRowCol(mproj, 2, 3) * f32_Inv(farPlane);
    GXSetProjection(mproj, GX_PERSPECTIVE);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::updateCameraFrustumPlanes(Camera *_cam)
{
	Quaternion leftPlane, rightPlane, topPlane, bottomPlane, nearPlane;
	extractFrustumPlanes(mg_WorldViewProj, leftPlane, rightPlane, topPlane, bottomPlane, nearPlane);

	Plane &left = _cam->m_frustumPlanes[Camera::CameraPlane_Left];
	Plane &right = _cam->m_frustumPlanes[Camera::CameraPlane_Right];
	Plane &bottom = _cam->m_frustumPlanes[Camera::CameraPlane_Bottom];
	Plane &top = _cam->m_frustumPlanes[Camera::CameraPlane_Top];
	Plane &nearP = _cam->m_frustumPlanes[Camera::CameraPlane_Near];

	left.m_normal = Vec3d(leftPlane.x, leftPlane.y, leftPlane.z);
	left.m_constant = -leftPlane.w;

	right.m_normal = Vec3d(rightPlane.x, rightPlane.y, rightPlane.z);
	right.m_constant = -rightPlane.w;

	top.m_normal = Vec3d(topPlane.x, topPlane.y, topPlane.z);
	top.m_constant = -topPlane.w;

	bottom.m_normal = Vec3d(bottomPlane.x, bottomPlane.y, bottomPlane.z);
	bottom.m_constant = -bottomPlane.w;

	nearP.m_normal = Vec3d(nearPlane.x, nearPlane.y, nearPlane.z);
	const Vec3d& pos = _cam->getPos();
	nearP.m_constant = nearPlane.x * pos.m_x + nearPlane.y * pos.m_y + nearPlane.z * pos.m_z;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::extractFrustumPlanes
	(
		const GMatrix44	&_m,
		Quaternion		&_leftPlane,
		Quaternion		&_rightPlane,
		Quaternion		&_topPlane,
		Quaternion		&_bottomPlane,
		Quaternion		&_nearPlane
	)
{
    Quaternion xColumn = {_m._m11, _m._m21, _m._m31, _m._m41};
    Quaternion yColumn = {_m._m12, _m._m22, _m._m32, _m._m42};
    Quaternion zColumn = {_m._m13, _m._m23, _m._m33, _m._m43};
    Quaternion wColumn = {_m._m14, _m._m24, _m._m34, _m._m44};

	QUATAdd(&wColumn, &xColumn, &_leftPlane); QUATScale(&_leftPlane, &_leftPlane, -1.f);
	QUATSubtract(&xColumn, &wColumn, &_rightPlane);
	QUATAdd(&wColumn, &yColumn, &_bottomPlane); QUATScale(&_bottomPlane, &_bottomPlane, -1.f);
	QUATSubtract(&yColumn, &wColumn, &_topPlane);
	QUATScale(&zColumn, &_nearPlane, -1.f);

	normalizePlane(_leftPlane);
	normalizePlane(_rightPlane);
	normalizePlane(_bottomPlane);
	normalizePlane(_topPlane);
	normalizePlane(_nearPlane);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::normalizePlane(Quaternion &_plane)
{
    const f32 n2 = Vec3d(_plane.x, _plane.y, _plane.z).sqrnorm();
    if(n2 != 0.f)
    {
        QUATScale(&_plane, &_plane, f32_InvSqrt(n2));
    }
    else
    {
        _plane.x = _plane.y = _plane.z = _plane.w = 0.f;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::init2DRender()
{
    /// Set 2D Matrix.
    setOrthoView(0.f, (f32) getScreenWidth(), 0.f, (f32) getScreenHeight());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t)
{
    subFakeAAFlag(FAKEAA_FLAG_PERSP);

    m_useRotateZCamera = bfalse;

    mg_World.setIdentity();
    mg_View.setIdentity();
    GFX_computeMatrixOrthoOffCenterRH(&mg_Proj, _l, _r, _b, _t, -1.f, 1.f);

    computeRenderMatrix();
	updateCameraFrustumPlanes(CAMERA);

    setObjectMatrix(GMatrixIdentity);

    // Wii Projection
    Mtx44 mproj;
    MATRIXconvert4x4TOWii(mproj, &mg_Proj);
    MTXRowCol(mproj, 2, 3) = - MTXRowCol(mproj, 2, 3);
    GXSetProjection(mproj, GX_ORTHOGRAPHIC);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setResolution_WII(u32 _screenWidth, u32 _screenHeight, u8 _aspectRatio)
{
    mf32_EFBScale[0] = (f32)Rmodeobj.fbWidth * f32_Inv((f32)_screenWidth);
    if(SCGetAspectRatio() == _aspectRatio)
    {
        mf32_EFBScale[1] = 1.f;
    }
    else
    {
        mf32_EFBScale[1] = 0.75f;//(9.f/16.f) * (4.f/3.f);
        if(SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
            mf32_EFBScale[1] = f32_Inv(mf32_EFBScale[1]);
    }

    mv_RealScreenSize.m_x = (f32)_screenWidth;
    mv_RealScreenSize.m_y = (f32)_screenHeight;
}

//----------------------------------------------------------------------------//

void* GFXAdapter_WII::createFramebuffer(u32 _size)
{
    MEM_M_PushExt(MEM_C_MEM2_ALLOC | MEM_C_HARD_ALLOC);
	void* bfr = Memory::alignedMallocCategory(sizeof(u8) * _size, 32, MemoryId::mId_GfxAdapter);
    MEM_M_PopExt();
	ITF_MemSet(bfr,0x10801080, _size);
	DCStoreRange(bfr, _size);
	return bfr;
}

#ifndef ITF_FINAL
//----------------------------------------------------------------------------//

extern "C" void verifyCallback(GXWarningLevel level, unsigned long id, const char* msg)
{
    switch(id){
        case 95: // * has a value of (# #), which might be unintentionally large.
            return;
        default :
            break;
    }
    LOG("GXError : Level %d, Warning %03d: %s", level, id, msg);
}
#endif

//----------------------------------------------------------------------------//

void GFXAdapter_WII::configureMem()
{
	/// FIFO.
	MEM_M_PushExt(MEM_C_MEM1_ALLOC | MEM_C_HARD_ALLOC);
	m_FifoBuffer = Memory::alignedMallocCategory(DEFAULT_FIFO_SIZE, 32, MemoryId::mId_GfxAdapter);
    m_FifoObj = GXInit(m_FifoBuffer, DEFAULT_FIFO_SIZE);
    MEM_M_PopExt();

	/// FrameBuffers
	allocatedFrameBufferSize = VIPadFrameBufferWidth(Rmode->fbWidth) * Rmode->xfbHeight * (u32)VI_DISPLAY_PIX_SZ;
	myXFB1 = createFramebuffer(allocatedFrameBufferSize);
	myXFB2 = createFramebuffer(allocatedFrameBufferSize);

    dispXFB = myXFB1;
    copyXFB = myXFB2;
    VISetNextFrameBuffer(dispXFB);

    /// System Font.
    if (OSGetFontEncode() == OS_FONT_ENCODE_SJIS)
    {
        FontData = (OSFontHeader*)Memory::alignedMallocCategory(OS_FONT_SIZE_SJIS, 32, MemoryId::mId_GfxAdapter);
    }
    else
    {
        FontData = (OSFontHeader*)Memory::alignedMallocCategory(OS_FONT_SIZE_ANSI, 32, MemoryId::mId_GfxAdapter);
    }
    OSInitFont(FontData);

#ifndef ITF_FINAL
    GXSetVerifyCallback(verifyCallback);
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::initGX()
{
    resetGX();

    //
    // GP sync init
    //

    GXDisableBreakPt();
    init_queue(&RenderQ);
    init_queue(&DoneQ);
    BPSet  = GX_FALSE;
    BPWait = GX_FALSE;
    BPGo   = GX_FALSE;
    DoneCount = 0;

    OSInitThreadQueue( &waitingDoneRender );

    // Creates a new thread. The thread is suspended by default.
    const u32 CUThreadStackSize = 8*1024;
    if(!CUThreadStack)
    {
    	MEM_M_PushExt(MEM_C_MEM1_ALLOC);
    	CUThreadStack = (u8*) Memory::alignedMallocCategory(CUThreadStackSize, 32, MemoryId::mId_GfxAdapter);
        MEM_M_PopExt();
    }

    OSCreateThread(
        &CUThread,
        CleanupThread,                      // ptr to the start routine
        NULL,
        CUThreadStack+CUThreadStackSize,    // stack bas addr
        CUThreadStackSize,                  // stack size
        0,                                  // highest scheduling priority
        OS_THREAD_ATTR_DETACH);             // detached by default

    OSResumeThread(&CUThread);

    // callbacks
    VISetPreRetraceCallback(VIPreCallback);
    VISetPostRetraceCallback(VIPostCallback);
    GXSetBreakPtCallback(BPCallback);
}

void GFXAdapter_WII::resetGX()
{
    /*----------------------------------------------------------------*
     *  GX configuration by a render mode obj                         *
     *----------------------------------------------------------------*/
    // These are necessary function calls to take a render mode
    // object and set up relevant GX configuration.

    // viewport
    GXSetViewport(0.f, 0.f, (f32)Rmode->fbWidth, (f32)Rmode->efbHeight, -1.f, 1.f);
    GXSetScissor(0, 0, (u32)Rmode->fbWidth, (u32)Rmode->efbHeight);

    // DispCopy config
    GXSetDispCopySrc(0, 0, Rmode->fbWidth, Rmode->efbHeight);
    const u16 n_xfb_lines = GXSetDispCopyYScale(GXGetYScaleFactor(Rmode->efbHeight, Rmode->xfbHeight)); // call yscale after setting display copy source
    GXSetDispCopyDst(Rmode->fbWidth, n_xfb_lines);

    // extra params
    GXSetCopyFilter(Rmode->aa, Rmode->sample_pattern, m_VFilterEnable, Rmode->vfilter);
    GXSetDispCopyGamma(GX_GM_1_0);

    if (Rmode->aa)
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    GXSetFieldMode(Rmode->field_rendering, Rmode->xfbHeight < Rmode->viHeight);

    // Clear embedded framebuffer for the first frame
    clear(GFX_CLEAR_COLOR|GFX_CLEAR_DEFERRED, COLOR_ZERO);
    GXCopyDisp(myXFB1, GX_TRUE);
    GXCopyDisp(myXFB1, GX_FALSE);
    GXCopyDisp(myXFB2, GX_FALSE);

    // Set swap mode tables.
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED,   GX_CH_GREEN, GX_CH_BLUE,  GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED,   GX_CH_GREEN, GX_CH_BLUE,  GX_CH_GREEN);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_RED,   GX_CH_BLUE,  GX_CH_BLUE);

    // Set swap modes.
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setRenderMode(GXRenderModeObj* _mode)
{
	// If an application specific render mode is provided,
	// override the default render mode
	if (_mode != NULL)
	{
		Rmodeobj = *_mode;
	}
	else
	{
		GXRenderModeObj Pal574IntDfScale =
		{
			VI_TVMODE_PAL_INT,      		// viDisplayMode
			640,            				// fbWidth
			480,             				// efbHeight
			574,             				// xfbHeight
			(VI_MAX_WIDTH_PAL - 640)/2,		// viXOrigin
			(VI_MAX_HEIGHT_PAL - 574)/2,	// viYOrigin
			640,             				// viWidth
			574,             				// viHeight
			VI_XFBMODE_DF,   				// xFBmode
			GX_FALSE,        				// field_rendering
			GX_FALSE,        				// aa

			// sample points arranged in increasing Y order
			 6,  6,  6,  6,  6,  6,  // pix 0, 3 sample points, 1/12 units, 4 bits each
			 6,  6,  6,  6,  6,  6,  // pix 1
			 6,  6,  6,  6,  6,  6,  // pix 2
			 6,  6,  6,  6,  6,  6,  // pix 3

			// vertical filter[7], 1/64 units, 6 bits each
			 8,         // line n-1
			 8,         // line n-1
			10,         // line n
			12,         // line n
			10,         // line n
			 8,         // line n+1
			 8          // line n+1
		};

		u32   oldInter = OSDisableInterrupts();
		bbool bDTVStatus = VIGetDTVStatus();
		bbool bProgressive = SCGetProgressiveMode() & bDTVStatus;

		/*----------------------------------------------------------------*
		 *  Set up default rendering mode                                 *
		 *  (used by GX/VI configurations and XFB size below)             *
		 *----------------------------------------------------------------*/
		switch (VIGetTvFormat())
		{
			case VI_NTSC:
                setMonitorRefreshRate(60.f);
				if(bProgressive)
				{
				    ITF_Memcpy(&Rmodeobj, &GXNtsc480ProgSoft, sizeof(GXRenderModeObj));
				    LOG("TV Format = NTSC, Render mode is GXNtsc480ProgSoft");
				}
				else
				{
				    ITF_Memcpy(&Rmodeobj, &GXNtsc480IntDf, sizeof(GXRenderModeObj));
				    LOG("TV Format = NTSC, Render mode is GXNtsc480IntDf");
				}

				// Values recommended by VI doc
				Rmodeobj.xfbHeight  = 456;
				Rmodeobj.viWidth    = (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 686 : 670;
				Rmodeobj.viHeight   = 456;
				Rmodeobj.viXOrigin  = (VI_MAX_WIDTH_NTSC - Rmodeobj.viWidth)/2;
				Rmodeobj.viYOrigin  = (VI_MAX_HEIGHT_NTSC - Rmodeobj.viHeight)/2;
				break;

			case VI_PAL:
				if(bProgressive)
				{
                    setMonitorRefreshRate(60.f);
					ITF_Memcpy(&Rmodeobj, &GXEurgb60Hz480ProgSoft, sizeof(GXRenderModeObj));
					LOG("TV Format = PAL, Render mode is GXEurgb60Hz480ProgSoft");

					// Values recommended by VI doc
					Rmodeobj.xfbHeight  = 456;
					Rmodeobj.viWidth    = (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 686 : 670;
					Rmodeobj.viHeight   = 456;
					Rmodeobj.viXOrigin  = (VI_MAX_WIDTH_EURGB60 - Rmodeobj.viWidth)/2;
					Rmodeobj.viYOrigin  = (VI_MAX_HEIGHT_EURGB60 - Rmodeobj.viHeight)/2;
				}
				else if(SCGetEuRgb60Mode() == SC_EURGB60_MODE_ON)
				{
                    setMonitorRefreshRate(60.f);
					ITF_Memcpy(&Rmodeobj, &GXEurgb60Hz480IntDf, sizeof(GXRenderModeObj));
					LOG("TV Format = PAL, Render mode is GXEurgb60Hz480IntDf");

					// Values recommended by VI doc
					Rmodeobj.xfbHeight  = 456;
					Rmodeobj.viWidth    = (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 686 : 670;
					Rmodeobj.viHeight   = 456;
					Rmodeobj.viXOrigin  = (VI_MAX_WIDTH_EURGB60 - Rmodeobj.viWidth)/2;
					Rmodeobj.viYOrigin  = (VI_MAX_HEIGHT_EURGB60 - Rmodeobj.viHeight)/2;
				}
				else
				{
                    setMonitorRefreshRate(50.f);
					ITF_Memcpy(&Rmodeobj, &Pal574IntDfScale, sizeof(GXRenderModeObj));
					LOG("TV Format = PAL, Render mode is Pal574IntDfScale");

					// Values recommended by VI doc
					Rmodeobj.xfbHeight  = 542;
					Rmodeobj.viWidth    = (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 682 : 666;
					Rmodeobj.viHeight   = 542;
					Rmodeobj.viXOrigin  = (VI_MAX_WIDTH_PAL - Rmodeobj.viWidth)/2;
					Rmodeobj.viYOrigin  = (VI_MAX_HEIGHT_PAL - Rmodeobj.viHeight)/2;
				}
				break;

			case VI_MPAL:
                setMonitorRefreshRate(60.f);
				if(bProgressive)
				{
					ITF_Memcpy(&Rmodeobj, &GXMpal480ProgSoft, sizeof(GXRenderModeObj));
					LOG("TV Format = MPAL, Render mode is GXMpal480ProgSoft");
				}
				else
				{
					ITF_Memcpy(&Rmodeobj, &GXMpal480IntDf, sizeof(GXRenderModeObj));
					LOG("TV Format = MPAL, Render mode is GXMpal480IntDf");
				}

				// Values recommended by VI doc
				Rmodeobj.xfbHeight  = 456;
				Rmodeobj.viWidth    = (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 686 : 670;
				Rmodeobj.viHeight   = 456;
				Rmodeobj.viXOrigin  = (VI_MAX_WIDTH_MPAL - Rmodeobj.viWidth)/2;
				Rmodeobj.viYOrigin  = (VI_MAX_HEIGHT_MPAL - Rmodeobj.viHeight)/2;
				break;

			case VI_EURGB60:
                setMonitorRefreshRate(60.f);
				if(bProgressive)
				{
					ITF_Memcpy(&Rmodeobj, &GXEurgb60Hz480ProgSoft, sizeof(GXRenderModeObj));
					LOG("TV Format = EURGB60, Render mode is GXEurgb60Hz480ProgSoft");
				}
				else
				{
					ITF_Memcpy(&Rmodeobj, &GXEurgb60Hz480IntDf, sizeof(GXRenderModeObj));
					LOG("TV Format = EURGB60, Render mode is GXEurgb60Hz480IntDf");
				}

				// Values recommended by VI doc
				Rmodeobj.xfbHeight  = 456;
				Rmodeobj.viWidth    = (SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 686 : 670;
				Rmodeobj.viHeight   = 456;
				Rmodeobj.viXOrigin  = (VI_MAX_WIDTH_EURGB60 - Rmodeobj.viWidth)/2;
				Rmodeobj.viYOrigin  = (VI_MAX_HEIGHT_EURGB60 - Rmodeobj.viHeight)/2;
				break;

			default:
				ITF_ASSERT_CRASH(0, "Unknown TV format");
				break;
		}

		if (!bProgressive)
			FakeAAFlag = 0xffff;
		else
			FakeAAFlag = 0;
		
        if(bProgressive)
        {
            Rmodeobj.vfilter[0] = 4;
            Rmodeobj.vfilter[1] = 4;
            Rmodeobj.vfilter[2] = 14;
            Rmodeobj.vfilter[3] = 20;
            Rmodeobj.vfilter[4] = 14;
            Rmodeobj.vfilter[5] = 4;
            Rmodeobj.vfilter[6] = 4;
        }
        else
        {
            Rmodeobj.vfilter[0] = 6;
            Rmodeobj.vfilter[1] = 6;
            Rmodeobj.vfilter[2] = 12;
            Rmodeobj.vfilter[3] = 16;
            Rmodeobj.vfilter[4] = 12;
            Rmodeobj.vfilter[5] = 6;
            Rmodeobj.vfilter[6] = 6;
        }

		// Values recommended by VI doc
        Rmodeobj.fbWidth    = 640;
        Rmodeobj.efbHeight  = 456;

        if(bProgressive)
        {
            Rmodeobj.aa     = GX_FALSE;
            m_VFilterEnable = GX_FALSE;
        }

        OSRestoreInterrupts(oldInter);
    }

	Rmode = &Rmodeobj;

    setResolution_WII(getScreenWidth(), getScreenHeight(), SC_ASPECT_RATIO_16x9);

    mv_StaticUtilScreenSize.m_x = (f32)getScreenWidth()  * mf32_EFBScale[0];
    mv_StaticUtilScreenSize.m_y = (f32)getScreenHeight() * mf32_EFBScale[1];
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::startVI(void)
{
    // Compute RenderMode
    setRenderMode(NULL);

    // Start check
    VISetVSyncTimingTest();

    // Configure VI with given render mode
    VIConfigure(Rmode);
    VIFlush();

	// Wait for retrace to start first frame
	VIWaitForRetrace();

	// Because of hardware restriction, we need to wait one more
	// field to make sure mode is safely changed when we change
	// INT->DS or DS->INT. (VIInit() sets INT mode as a default)
	VIWaitForRetrace();

    // Check
	u32 uVSyncTest = VIGetVSyncTimingTest();
	if(uVSyncTest)
		LOG("\nWarning GetVSyncTimingTest = %u\n", uVSyncTest);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::init()
{
    LockRendering();

    // Configure VI
    startVI();

    // Allocate buffers
    configureMem();
    createInternalBuffers();

    u32 ringVBSize = 500 * 1024;
#ifdef VBMANAGER_USE_DYNAMICRING_VB
    ringVBSize += 1 * 1024 * 1024;
#endif // VBMANAGER_USE_DYNAMICRING_VB
    MEM_M_PushExt(MEM_C_MEM2_ALLOC);
    m_VertexBufferManager.createDynamicRingVB(ringVBSize,32);
    MEM_M_PopExt();
    AFTERFX_startRenderTarget(Color::zero());

    // Init GX
    initGX();

    // Default configuration
    GXSetNumChans(1); // default, color = vertex color
	GXSetNumTexGens(0); // no texture in this demo
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);

    renderContext_SetDefault();
    setCamera(0.f, 0.f, 1.f, CAMERA);

    resetSceneResolvedFlag();

    setGlobalColor(Color::white());

	depthMask(0);
	depthTest(0);
	depthFunc(GFX_CMP_ALWAYS);
	setCullMode(GFX_CULL_NONE);

    VISetBlack(FALSE);

    UnlockRendering();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::clear(u32 _buffer, u32 _Col)
{
    GXSetCopyClear(*(GXColor*)&_Col, GX_MAX_Z24);

    if(mu32_CurClearColor != _Col)
    {
        mu32_CurClearColor = _Col;

        if(!(_buffer & GFX_CLEAR_DEFERRED))
        {
            // Immediate Clear
            init2DRender();
            setAlphaBlend(GFX_BLEND_COPY);
            drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, _Col, btrue);

            // Reset Projection
            CAMERA->apply();
        }
   }
}

void GFXAdapter_WII::clear(u32 _buffer, f32 _r, f32 _g, f32 _b, f32 _a)
{
    clear(_buffer, Color(_a,_r,_g,_b).getAsU32());
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setAlphaBlend(GFX_BLENDMODE _Blend)
{
    u_CurrentBlendingMode = _Blend;
	GXPokeBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ONE, GX_LO_SET);

    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    switch(_Blend)
    {
        case GFX_BLEND_COPY:
        GXSetZCompLoc(GX_TRUE);
        GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ALPHA:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ALPHAPREMULT:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ALPHADEST:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_INVDSTALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ALPHADESTPREMULT:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_ONE, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ADD:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ONE, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ADDALPHA:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
        break;

        case GFX_BLEND_SUBALPHA:
		LOG("GFX_BLEND_SUBALPHA not supported");
        break;

        case GFX_BLEND_SUB:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_INVSRCCLR, GX_LO_CLEAR);
        break;

        case GFX_BLEND_MUL:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_SRCCLR, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ALPHAMUL:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_SRCALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_IALPHAMUL:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ZERO, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_IALPHA:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_INVSRCALPHA, GX_BL_SRCALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_IALPHAPREMULT:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_SRCALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_IALPHADEST:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_DSTALPHA, GX_LO_CLEAR);
        break;

        case GFX_BLEND_IALPHADESTPREMULT:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_ONE, GX_LO_CLEAR);
        break;

        case GFX_BLEND_MUL2X:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTCLR, GX_BL_SRCCLR, GX_LO_CLEAR);
        break;

        case GFX_BLEND_ALPHATOCOLOR:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_DSTALPHA, GX_BL_ZERO, GX_LO_CLEAR);
        break;

        case GFX_BLEND_IALPHATOCOLOR:
        GXSetBlendMode(GX_BM_BLEND, GX_BL_INVDSTALPHA, GX_BL_ZERO, GX_LO_CLEAR);
        break;

		default:
		LOG("Blend mode not supported");
		break;
    }
}

bbool GFXAdapter_WII::useSrcAlphaForBlend(GFX_BLENDMODE _Blend)
{
    switch(_Blend)
    {
        case GFX_BLEND_ALPHA:
        case GFX_BLEND_ALPHAPREMULT:
        case GFX_BLEND_ADDALPHA:
        case GFX_BLEND_ALPHAMUL:
        case GFX_BLEND_IALPHAMUL:
        case GFX_BLEND_IALPHA:
        case GFX_BLEND_IALPHAPREMULT:
            return btrue;
            break;

        case GFX_BLEND_COPY:
        case GFX_BLEND_ALPHADEST:
        case GFX_BLEND_ALPHADESTPREMULT:
        case GFX_BLEND_ADD:
        case GFX_BLEND_SUBALPHA:
        case GFX_BLEND_SUB:
        case GFX_BLEND_MUL:
        case GFX_BLEND_IALPHADEST:
        case GFX_BLEND_IALPHADESTPREMULT:
        case GFX_BLEND_MUL2X:
        case GFX_BLEND_ALPHATOCOLOR:
        case GFX_BLEND_IALPHATOCOLOR:
        default:
            return bfalse;
            break;
    }

    return bfalse;
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setCullMode(GFX_CULLMODE _Cull)
{
    switch(_Cull)
    {
        case GFX_CULL_NONE:
            GXSetCullMode(GX_CULL_NONE);
            break;
        case GFX_CALL_CCW:
            GXSetCullMode(GX_CULL_FRONT);
            break;
        case GFX_CULL_CW:
            GXSetCullMode(GX_CULL_BACK);
            break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::depthMask(u32 _depthMask)
{
    if(_depthMask)
        m_zWrite = btrue;
    else
        m_zWrite = bfalse;

	GXSetZMode(m_zTest, m_zFuncComp, m_zWrite);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::depthFunc(u32 _func)
{
    switch(_func)
    {
	    case GFX_CMP_NEVER:
            m_zFuncComp = GX_NEVER;
            break;
	    case GFX_CMP_LESS:
            m_zFuncComp = GX_LESS;
            break;
	    case GFX_CMP_EQUAL:
            m_zFuncComp = GX_EQUAL;
            break;
	    case GFX_CMP_LESSEQUAL:
            m_zFuncComp = GX_LEQUAL;
            break;
	    case GFX_CMP_GREATER:
            m_zFuncComp = GX_GREATER;
            break;
	    case GFX_CMP_NOTEQUAL:
            m_zFuncComp = GX_NEQUAL;
            break;
	    case GFX_CMP_GREATEREQUAL:
            m_zFuncComp = GX_GEQUAL;
            break;
	    case GFX_CMP_ALWAYS:
            m_zFuncComp = GX_ALWAYS;
            break;
    }

	GXSetZMode(m_zTest, m_zFuncComp, m_zWrite);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::depthTest(u32 _test)
{
    if (_test)
   		m_zTest = btrue;
    else
       	m_zTest = bfalse;

	GXSetZMode(m_zTest, m_zFuncComp, m_zWrite);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setMatrixTransform(MATRIXMODE _type, const GMatrix44* _Matrix)
{
    switch(_type)
    {
    default:
    case GFX_MWORLD:
        setObjectMatrix(*_Matrix);
        break;

    case GFX_MWORLDTOWIEW:
    case GFX_MVIEW:
        mg_View = *_Matrix;
        computeRenderMatrix();
        break;

    case GFX_PROJECTION:
        mg_Proj = *_Matrix;
        computeRenderMatrix();
        break;

    case GFX_TEXTURE:
        {
            mg_UVmat = *_Matrix;

            Mtx muv;
            MATRIXconvert4x4TOWii(muv, &mg_UVmat);
            GXLoadTexMtxImm(muv, GX_TEXMTX0, GX_MTX3x4);
            GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);
            GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);
        }
        break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setupViewport(GFX_RECT* _Viewport)
{
    f32 x = _Viewport->left * mf32_EFBScale[0];
    f32 y = _Viewport->top  * mf32_EFBScale[1];
    f32 w = (_Viewport->right  - _Viewport->left) * mf32_EFBScale[0];
    f32 h = (_Viewport->bottom - _Viewport->top)  * mf32_EFBScale[1];
	if (isFakeAAEnabled()) y += 0.5f;

    // GXSetViewport can't be called if another thread is rendering
    // It's the case for the DVDError Thread
    LockRendering();

    if(Rmode->field_rendering)
        GXSetViewportJitter(x, y, w, h, -1.f, 1.f, VIGetNextField());
    else
        GXSetViewport(x, y, w, h, -1.f, 1.f);

    GXSetScissor((u32)x, (u32)y, (u32)w, (u32)h);

    UnlockRendering();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::getViewport(GFX_RECT* _RectOut)
{
	f32 vp[GX_VIEWPORT_SZ];
	GXGetViewportv(vp);
	_RectOut->left = vp[0] * f32_Inv(mf32_EFBScale[0]);
	_RectOut->top =  vp[1] * f32_Inv(mf32_EFBScale[1]);
    if (isFakeAAEnabled()) _RectOut->top -= 0.5f;
	_RectOut->right =  _RectOut->left + (vp[2] * f32_Inv(mf32_EFBScale[0]));
	_RectOut->bottom = _RectOut->top  + (vp[3] * f32_Inv(mf32_EFBScale[1]));
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setScissorRect(GFX_RECT* _clipRect)
{
    GXSetScissor
    (
        (u32)(_clipRect->left * mf32_EFBScale[0]),
        (u32)(_clipRect->top  * mf32_EFBScale[1]),
        (u32)((_clipRect->right  - _clipRect->left) * mf32_EFBScale[0]),
        (u32)((_clipRect->bottom - _clipRect->top)  * mf32_EFBScale[1])
    );
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::restoreDefaultShader()
{
    setGlobalColor(Color::white());
    disableFog();
    setTev(TEV_T);
    setAlphaBlend(GFX_BLEND_ALPHA);
    setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::startRendering()
{
    WaitRenderQueueLen(0);

	LockRendering();
    cleanBufferEndOfFrame();

    CheckDeprecatedDL();

#ifndef ITF_FINAL
    static GXWarningLevel warnLevel = GX_WARN_NONE;
    GXSetVerifyLevel(warnLevel);
#endif

    // Invalidate vertex cache in GP
    GXInvalidateVtxCache();
    // Invalidate texture cache in GP
    GXInvalidateTexAll();

    GXSetNumChans(1); // default, color = vertex color
	GXSetNumTexGens(0); // no texture in this demo
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    renderContext_SetDefault();

    m_VertexBufferManager.DynamicRingVBstartFrame();

#ifndef ITF_FINAL
    Tick_GPUFrame(false);
#endif // ITF_FINAL
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::endRendering()
{
#ifndef ITF_DISABLE_DEBUGINFO
    // Debug Infos
    #ifndef ITF_FINAL
    if(!(getOverDrawMode()&1) || getStrSceenshotResquest())
    #endif // ITF_FINAL
    {
        #ifndef ITF_FINAL
        //RastersManager::get().rastersDraw();
		#ifdef RASTER_ENABLED
			RastersManager::get().rastersMinimalDraw(0.09f);
		#endif // !ITF_FINAL

        #endif // ITF_FINAL
        drawDebugText();
    }
#endif // ITF_DISABLE_DEBUGINFO

    m_VertexBufferManager.DynamicRingVBendFrame();

    resetSceneResolvedFlag();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::present()
{
#ifndef ITF_FINAL
    if(getStrSceenshotResquest())
    {
        doScreenshot((u32)((f32)Rmode->efbHeight * mf32_EFBScale[1]), getStrSceenshotResquest());
        resetScreenShotRequest();
    }
#endif // ITF_FINAL


	if (!SYSTEM_ADAPTER_WII->isInHomeMenu() && !SYSTEM_ADAPTER_WII->isInSplashScreen()) // NO FLICKERING 
	{
		if ((!TRC_ADAPTER_WII) || !TRC_ADAPTER_WII->isDisplayingError()) // NO FLICKERING 
		{
			if (m_monitorRefreshRate > 55.0f) 
			{
				if (m_monitorRefreshRate - getfPs()<2.0f) 
					// Philippe FAKE AA
					xorFakeAAFlag(FAKEAA_FLAG_ACTIVE);
			}
		}
	}

    //// Prepare CopyDisp

    // Set Z/Color update to make sure eFB will be cleared at GXCopyDisp.
    GXSetZMode(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);

    // DispCopy config
    const u16 srcH = (u16)((f32)Rmode->efbHeight * mf32_EFBScale[1]);
    GXSetDispCopySrc(0, 0, Rmode->fbWidth, srcH);
    const u16 n_xfb_lines = GXSetDispCopyYScale(GXGetYScaleFactor(Rmode->efbHeight, Rmode->xfbHeight)); // call yscale after setting display copy source
    GXSetDispCopyDst(Rmode->fbWidth, n_xfb_lines);

    GXSetCopyFilter(Rmode->aa, Rmode->sample_pattern, m_VFilterEnable, Rmode->vfilter);

    //-----------------------------

    //// Syncro + CopyDisp + Swap

    GXFifoObj   fifoInfo;
    void* tmp_read;
    void* tmp_write;
    QItem qitm;
    int   enabled;

    GXFlush();
    GXGetCPUFifo(&fifoInfo);
    GXGetFifoPtrs(&fifoInfo, &tmp_read, &tmp_write);

    // Create new render queue item
    qitm.writePtr = tmp_write;
    qitm.dataPtr = NULL;        // pointer to frame-related user data
    qitm.copyXFB = copyXFB;

    // Technically, you can work this such that you don't
    // need the OSDisabled interrupts.  You need to rework
    // the enqueue/dequeue routines a bit, though, to make
    // them non-interfere with each other.

    enabled = OSDisableInterrupts();
    {
        enqueue(&RenderQ, &qitm);
        if (BPSet == GX_FALSE)
        {
            BPSet = GX_TRUE;
            GXEnableBreakPt( tmp_write );
        }
    }
    OSRestoreInterrupts(enabled);

    // Issue display copy command
    GXCopyDisp((u16*)copyXFB + (u32)((f32)(Rmode->xfbHeight - n_xfb_lines) * 0.5f) * Rmode->fbWidth, GX_TRUE);
    GXFlush();

    copyXFB = (copyXFB == myXFB1) ? myXFB2 : myXFB1;

    // Unlock mutex
    UnlockRendering();

    // Give a chance to loading threads to run
    OSYieldThread();
    WaitRenderQueueLen(1);
}

//----------------------------------------------------------------------------//
void GFXAdapter_WII::OnGPUFrameEnd()
{
    m_VertexBufferManager.DynamicRingVBendDraw();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::LockRendering()
{
	Synchronize::enterCriticalSection(&m_csRendering);

    // Set the thread that will be interrupted if the FIFO reaches its high watermark
    GXSetCurrentGXThread();
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::UnlockRendering()
{
	Synchronize::leaveCriticalSection(&m_csRendering);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::updateWiiObjectMatrix()
{
    Mtx matw;
    MATRIXconvert4x4TOWii(matw, &mg_ObjViewMatrix);
    GXLoadPosMtxImm(matw, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

void GFXAdapter_WII::setObjectMatrix(const GMatrix44& _matrix)
{
    if(m_useRotateZCamera)
        M44_mul4x4Out((Matrix44*)&mg_ObjViewMatrix, (const Matrix44*)&_matrix, (const Matrix44*)&mg_ViewRender);
    else
        M44_mul4x4Out((Matrix44*)&mg_ObjViewMatrix, (const Matrix44*)&_matrix, (const Matrix44*)&mg_View);

    mg_ObjMatrix = _matrix;
    updateWiiObjectMatrix();
}

void GFXAdapter_WII::setObjectMatrix(const GMatrix44& _matrix, GXPosNrmMtx _mtxID)
{
    GMatrix44 mat44;
    if(m_useRotateZCamera)
        M44_mul4x4Out((Matrix44*)&mat44, (const Matrix44*)&_matrix, (const Matrix44*)&mg_ViewRender);
    else
        M44_mul4x4Out((Matrix44*)&mat44, (const Matrix44*)&_matrix, (const Matrix44*)&mg_View);

    Mtx matw;
    MATRIXconvert4x4TOWii(matw, &mat44);
    GXLoadPosMtxImm(matw, _mtxID);
}

///----------------------------------------------------------------------------//
/// UV.
///----------------------------------------------------------------------------//

void GFXAdapter_WII::setUVAnim(const GFX_UVANIM& _uvanim)
{
    setMatrixTransform(GFX_TEXTURE, &_uvanim.m_UVMat);
}

void GFXAdapter_WII::resetUVAnim()
{
    mg_UVmat.setIdentity();
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}

///----------------------------------------------------------------------------//
/// Render Context.
///----------------------------------------------------------------------------//
void GFXAdapter_WII::renderContext_Set()
{
    if (rC_getZWRITE())
        depthMask(1);
    if (rC_getZWRITE())
        depthTest(1);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::renderContext_SetDefault()
{
    //depthMask(0);
    //depthTest(0);
    //depthFunc(GFX_CMP_ALWAYS);
    m_zWrite = bfalse;
    m_zTest = bfalse;
    m_zFuncComp = GX_ALWAYS;
    GXSetZMode(m_zTest, m_zFuncComp, m_zWrite);
        
    GXSetZTexture( GX_ZT_DISABLE, GX_TF_Z8, 0 );    

    disableFog();
}

///----------------------------------------------------------------------------//
/// Gfx Material.
///----------------------------------------------------------------------------//
void GFXAdapter_WII::setGfxMaterial(const GFX_MATERIAL& _gfxMat)
{
    m_drawFlag = 0;

    switch (_gfxMat.m_matType)
    {
    default:
    case GFX_MAT_DEFAULT:
        #ifdef SUPPORT_STATS_MANAGER
        StatsManager::getptr()->materialDefaultInc();
        #endif
        setGfxMatDefault();
        break;

    case GFX_MAT_FRIEZEANIM:
        #ifdef SUPPORT_STATS_MANAGER
        StatsManager::getptr()->materialFrizeAnimInc();
        #endif
        setGfxMatDefault();
        m_VecOverlay = _gfxMat.m_matParams.getvParamsAt(0);
        break;

    case GFX_MAT_REFRACTION:
        #ifdef SUPPORT_STATS_MANAGER
        StatsManager::getptr()->materialRefractionInc();
        #endif
        setGfxMatRefraction(_gfxMat);
        break;

    case GFX_MAT_GLOW:
        ITF_ASSERT_MSG(0, "GFX_MAT_GLOW not implemented");
        #ifdef SUPPORT_STATS_MANAGER
        StatsManager::getptr()->materialGlowInc();
        #endif
        //AFTERFX_prepareObjectGlow();
        //setGfxMatDefault();
        //m_drawFlag |= DRWF_COPYTOTARGET;
        break;

    case GFX_MAT_ALPHAFADE:
        #ifdef SUPPORT_STATS_MANAGER
        StatsManager::getptr()->materialAlphaFadeInc();
        #endif
        setGfxMatAlphaFade(_gfxMat);
        break; 
    }

    /// set alpha blend.
    setAlphaBlend(_gfxMat.m_blendMode);

    /// set Texture if present
    setTextureAdressingMode(0, _gfxMat.m_TexAdressingModeU, _gfxMat.m_TexAdressingModeV);
    SetTextureResource(0, _gfxMat.m_textureDiffuse);
}

void GFXAdapter_WII::unsetGfxMaterial(const GFX_MATERIAL& _gfxMat, Tev_Type _curTevType)
{
    switch (_gfxMat.m_matType)
    {
    case GFX_MAT_REFRACTION:
        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE2);
        GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
        setTev(_curTevType);

        // Restore FakeAA status
        forceDisableFakeAA(bfalse);
        break;

    case GFX_MAT_GLOW:
        ITF_ASSERT_MSG(0, "GFX_MAT_GLOW not implemented");
        break;

    case GFX_MAT_FRIEZEANIM:
    case GFX_MAT_ALPHAFADE:
    case GFX_MAT_DEFAULT:
    default:
        // NOTHING
        break;
    }
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::setGfxMatDefault()
{
    // Nothing
}

void GFXAdapter_WII::setGfxMatRefraction(const GFX_MATERIAL& _gfxMat)
{
    if(!getSceneResolvedFlag())
    {
        // Copy frame buffer
        const bbool saveFlag = mb_ClearOnCopy;
        mb_ClearOnCopy = bfalse;
        copyCurrentColorBuffer(AFX_FB);
        setSceneResolvedFlag(btrue);
        mb_ClearOnCopy = saveFlag;
    }

    // Frame buffer as texture
    setTextureAdressingMode(2, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
    SetTexture(2, mp_AFX_RT[AFX_FB]->getTexture());

    // Try normal texture
    Texture *tex_normal = (Texture *)_gfxMat.m_textureNormal.getResource();
    setTextureAdressingMode(3, _gfxMat.m_TexAdressingModeU, _gfxMat.m_TexAdressingModeV);
    if(!SetTextureResource(3, _gfxMat.m_textureNormal))
    {
        // if normal doesn't work
        // try texture Diffuse
        tex_normal = (Texture *)_gfxMat.m_textureDiffuse.getResource();
        SetTextureResource(3, _gfxMat.m_textureDiffuse);
    }

    // Get or Create normal texture at wii format (= map RG to IA channels)
    {
        Texture *tex_normal_wii = NULL;
        ITF_MAP<Texture*,Texture*>::iterator it = m_NormalTex.find(tex_normal);
        if(it != m_NormalTex.end())
        {
            tex_normal_wii = it->second;
        }
        else
        {
            // Create
            tex_normal_wii = (Texture*)RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture);
            RESOURCE_MANAGER->addUserToResourcePhysicalData(tex_normal_wii);
            createDynamicTexture(tex_normal_wii, tex_normal->getSizeX(), tex_normal->getSizeY(), Texture::PF_LA8);
            m_NormalTex[tex_normal] = tex_normal_wii;

            // Save matrices
            const GMatrix44 saveObjmat = mg_ObjViewMatrix;
            const GMatrix44 saveUVmat  = mg_UVmat;

            // Render
            f32 w = (f32)tex_normal->getSizeX();
            f32 h = (f32)tex_normal->getSizeY();

            setOrthoView(0.f, w, 0.f, h);
            resetUVAnim();

            GFX_RECT saveViewport;
            getViewport(&saveViewport);
            GXSetViewport(0.f, 0.f, w, h, -1.f, 1.f);
            GXSetScissor(0, 0, (u32)w, (u32)h);

            GFX_BLENDMODE saveBlendMode = u_CurrentBlendingMode;
            setAlphaBlend(GFX_BLEND_COPY);

            Color saveFogColor = m_colorFog;
            setFogColor(Color::zero());
            GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP2);

            SetTexture(0, tex_normal);
            drawScreenQuadInternal(0.f, 0.f, w, h, 0.f, COLOR_WHITE, bfalse, TEV_T);
            copyCurrentColorBuffer(tex_normal_wii, 0, 0, (u16)w, (u16)h);

            // Restore
            GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
            setupViewport(&saveViewport);

            // Redraw frame buffer
            init2DRender();
            setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
            SetTexture(0, mp_AFX_RT[AFX_FB]->getTexture());
            drawScreenQuadInternal(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE, bfalse, TEV_T);

            // Restore
            setFogColor(saveFogColor);
            setAlphaBlend(saveBlendMode);

            CAMERA->apply();

            mg_ObjViewMatrix = saveObjmat;
            updateWiiObjectMatrix();

            setMatrixTransform(GFX_TEXTURE, &saveUVmat);
        }
        ITF_ASSERT(tex_normal_wii);
        SetTexture(4, tex_normal_wii);
    }

    // Prepare Tev settings
    setTev(TEV_REFRACTION);

    // Decal factors
    const f32 f32_P0   = _gfxMat.m_matParams.getfParamsAt(0);
    const f32 f32_P1_2 = _gfxMat.m_matParams.getfParamsAt(1) * 0.5f;
    const f32 f32_P2   = _gfxMat.m_matParams.getfParamsAt(2);
    const f32 f32_P3   = _gfxMat.m_matParams.getfParamsAt(3);

    // Scale Matrix for diffuse
    if(f32_P0 != 1.f)
    {
        Matrix44 mat(mg_UVmat);
        mat.mulScale(Vec3d(f32_P0, f32_P0, 1.f));
        setMatrixTransform(GFX_TEXTURE, (const GMatrix44 *)&mat);
    }

    // Matrix for refraction
    // Setting 1.0 is not possible in indirect matrix so we scale the values,
    // then the inverse scale is perform by GX with scale_exp parameter of GXSetIndTexMtx function
    {
        const f32 indMtx[2][3] =
        {
            f32_P1_2, 0.f,      (f32_P2 - f32_P1_2) * 0.25f,
            0.f,      f32_P1_2, (f32_P3 - f32_P1_2) * 0.25f,
        };
        GXSetIndTexMtx(GX_ITM_0, indMtx, 2);
    }

    // Matrix for frame buffer look up
    {
        // Start with projection matrix
        Matrix44 mproj(mg_Proj);

        // Convert from clip-space [-1;1] to texture-space [0;1]
        // + invert Y
        mproj._m11 *=  0.5f;
        mproj._m22 *= -0.5f;
        mproj._m31 -=  0.5f;
        mproj._m32 -=  0.5f;

        // WorldView[3][4]
        Matrix44 mwv(mg_ObjViewMatrix);
        mwv._m14 = mwv._m24 = mwv._m34 = mwv._m44 = 0.f;

        // Compute WorldViewProj final matrix
        Matrix44 mwvp(GMatrixIdentity);
        M44_mul4x4Out(&mwvp, &mwv, &mproj);

        // Convert to Wii format
        Mtx muv;
        MATRIXconvert4x4TOWii(muv, (const GMatrix44 *)&mwvp);
        GXLoadTexMtxImm(muv, GX_TEXMTX1, GX_MTX3x4);
    }

    // Disable FakeAA for this object because it's already done with other objects render before
    // and get in AFX_FB texture
    forceDisableFakeAA(btrue);
}

///----------------------------------------------------------------------------//
/// Light.
///----------------------------------------------------------------------------//

void GFXAdapter_WII::setGlobalColor(const Color& _color)
{
    setGFXGlobalColor(_color);
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::setFogColor(const Color& _color)
{
#ifndef ITF_FINAL
    static bbool s_ForceDisableFog = bfalse;
#endif // ITF_FINAL

	m_colorFog = _color;

#ifndef ITF_FINAL
    if(s_ForceDisableFog)
        m_colorFog.m_a = 0.f;
#endif // ITF_FINAL

	setUseFog(m_colorFog.getAlpha() != 0.f);
}

///----------------------------------------------------------------------------//

const GMatrix44* GFXAdapter_WII::getViewMatrix()
{
    return &mg_View;
}


} // namespace ITF
