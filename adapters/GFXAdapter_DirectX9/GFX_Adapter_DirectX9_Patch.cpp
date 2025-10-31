#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_


namespace ITF
{
#define USE_OPTIMIZED_PATCH_VERSION 1  //prototype optimization

//Assume this function is optimized with LHS Removed
//-----------------------------------------------------------------------------------------------------------------------------------------    
#ifdef USE_OPTIMIZED_PATCH_VERSION 

    void GFXAdapter_Directx9::shaderDrawPatch32(const Texture *texture, const Vec2d *PointsNS, const ColorInteger* _lighttab)
    {
        ITF_ASSERT(_lighttab);

        f32 ratio = 1.f;

        /// set Texture;
        if (texture)
        {
            SetTextureBind(0, texture->m_adapterimplementationData);
            ratio = texture->getRatioXY();
            setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        }

        /// The stride of float Buffer is:
        /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS+16 that is for UV.
        /// this is for Vector4 shader: xy = vertex zw = uv.

//        shaderBuffer localShaderBuffer;
        u32 moveOp= mp_shaderManager.getCurBuffFloatPos(0);
        const Vec2d * curPointNS  = PointsNS;
        const Vec2d * curPointNS8 = PointsNS + 8;
        for (u32 i = 0; i < 8; i++, curPointNS++, curPointNS8++)
        {
            mp_shaderManager.setBufferFloatAt(0, moveOp++, curPointNS->m_x);
            mp_shaderManager.setBufferFloatAt(0, moveOp++, curPointNS->m_y);
            mp_shaderManager.setBufferFloatAt(0, moveOp++, curPointNS8->m_x);
            mp_shaderManager.setBufferFloatAt(0, moveOp++, curPointNS8->m_y * ratio);
        }

        mp_shaderManager.setCurBuffFloatPos(0,moveOp);

        /// number constant for control points is: f32 to vector4 -> 32 float / 4
        mp_shaderManager.addVector4ConstToFlush(0, 8);

        /// color.
        moveOp= mp_shaderManager.getCurBuffFloatPos(1);
        const ColorInteger *curLight = _lighttab;
        for (u32 i = 0; i < 4; i++, curLight++)
        {
            mp_shaderManager.setBufferFloatAt(1, moveOp++, curLight->getF32Red());
            mp_shaderManager.setBufferFloatAt(1, moveOp++, curLight->getF32Green());
            mp_shaderManager.setBufferFloatAt(1, moveOp++, curLight->getF32Blue());
            mp_shaderManager.setBufferFloatAt(1, moveOp++ ,curLight->getF32Alpha());
        }

        mp_shaderManager.setCurBuffFloatPos(1,moveOp);

        mp_shaderManager.addVector4ConstToFlush(1, 4);

    }


#else

    /* Original version*/


void GFXAdapter_Directx9::shaderDrawPatch32(const Texture *texture, const Vec2d *PointsNS, const ColorInteger* _lighttab)
{
    ITF_ASSERT(_lighttab);

    f32 ratio = 1.f;

    /// set Texture;
    if (texture)
    {
        SetTextureBind(0, texture->m_adapterimplementationData);
        ratio = texture->getSizeX()/(f32)texture->getSizeY();//texture->getRatioXY();
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
    }

    /// The stride of float Buffer is:
    /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS+16 that is for UV.
    /// this is for Vector4 shader: xy = vertex zw = uv.
    
    for (u32 i = 0; i < 8; i++)
    {
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_y);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 8].m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 8].m_y * ratio);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
    }

    /// number constant for control points is: f32 to vector4 -> 32 float / 4
    mp_shaderManager.addVector4ConstToFlush(0, 8);

    /// color.
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getRed());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getGreen());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getBlue());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getAlpha());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
    }

    mp_shaderManager.addVector4ConstToFlush(1, 4);

}


#endif // USE_OPTIMIZED_PATCH_VERSION
}