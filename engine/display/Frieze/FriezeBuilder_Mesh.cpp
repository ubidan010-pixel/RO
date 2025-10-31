#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    void Frise::setVtxColor( const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown ) const
    {
        _vtxColorDown = _texConfig.m_color.getAsU32();
        _vtxColorUp = ITFCOLOR_SET_A(_texConfig.m_color.getAsU32(), _texConfig.m_alphaUp);
    }

    void Frise::setVtxColorWithExtrem( const FriseTextureConfig& _texConfig, u32& _vtxColorUp, u32& _vtxColorDown, u32& _vtxColorUpExt, u32& _vtxColorDownExt, bbool _forceAlpha ) const
    {
        setVtxColor( _texConfig, _vtxColorUp, _vtxColorDown );

        if ( isLooping() && !_forceAlpha )
        {
            _vtxColorDownExt = _vtxColorDown;
            _vtxColorUpExt = _vtxColorUp;
        }
        else
        {
            // down
            _vtxColorDownExt = ITFCOLOR_SET_A(_texConfig.m_color.getAsU32(), _texConfig.m_alphaBorder);

            // up
            u8 alphaTexture = _texConfig.m_color.m_rgba.m_a;
            u8 alphaUp;

            if ( _texConfig.m_alphaUp == alphaTexture )
                alphaUp = _texConfig.m_alphaBorder;
            else if ( _texConfig.m_alphaBorder == alphaTexture )
                alphaUp = _texConfig.m_alphaUp;
            else
                alphaUp = u8( ( _texConfig.m_alphaBorder + _texConfig.m_alphaUp ) *0.5f);

            _vtxColorUpExt = ITFCOLOR_SET_A(_texConfig.m_color.getAsU32(), alphaUp);
        }
    }

    void Frise::setMeshElement( ITF_MeshElement& _elem, const FriseConfig* _friseConfig, u32 _indexTableSize, u32 _idTexConfig) const
    {
        _elem.m_indexBuffer = GFX_ADAPTER->createIndexBuffer(_indexTableSize , bfalse);
        _elem.m_startIndex = 0;
        _elem.m_count = _indexTableSize;                
        _elem.m_material = _friseConfig->m_textureConfigs[_idTexConfig].m_gfxMat;

        if (_friseConfig->m_textureConfigs[_idTexConfig].m_configType == FriseTextureConfig::FriseConfig_Animation)
        {
            _elem.m_material.m_textureDiffuse.invalidateResourceId();
        }
        else
        {
            _elem.m_material.m_textureDiffuse = _friseConfig->m_textureConfigs[_idTexConfig].m_textureData;
            if (_elem.m_material.m_matType == GFX_MAT_REFRACTION)
                _elem.m_material.m_textureNormal = _friseConfig->m_textureConfigs[_idTexConfig].m_textureNormal;
        }

        // scrolling
        if ( _friseConfig->m_textureConfigs[_idTexConfig].m_scrollingSpeedTrans.m_x != 0.f || _friseConfig->m_textureConfigs[_idTexConfig].m_scrollingSpeedTrans.m_y != 0.f )
        {
            _elem.m_material.m_uvAnim.m_uvFlagAnim |= UVANIM_F_TRANS;
            _elem.m_material.m_uvAnim.m_speedTrans.m_x = _friseConfig->m_textureConfigs[_idTexConfig].m_scrollingSpeedTrans.m_x;
            _elem.m_material.m_uvAnim.m_speedTrans.m_y = _friseConfig->m_textureConfigs[_idTexConfig].m_scrollingSpeedTrans.m_y;
        }
        // rotation
        if ( _friseConfig->m_textureConfigs[_idTexConfig].m_scrollingSpeedRotate != 0.f )
        {
            _elem.m_material.m_uvAnim.m_uvFlagAnim |= UVANIM_F_ROTATE;
            _elem.m_material.m_uvAnim.m_speedRotate = _friseConfig->m_textureConfigs[_idTexConfig].m_scrollingSpeedRotate;
        }
    }

    void Frise::setQuadPos_StartExtremity( edgeFrieze& _edge, Vec2d* _pos, f32 _visualOffset, f32 _heightMid, f32 _width, bbool _isSnaped ) const
    {
        _pos[0] = _edge.m_points[0] - _edge.m_sightNormalized *_width; 
        _pos[1] = _edge.m_points[1] - _edge.m_sightNormalized *_width;

        if ( _isSnaped )
        {
            _pos[2] = _edge.m_pos -_edge.m_normal *_visualOffset *_heightMid;
            _pos[3] = _pos[2] +_edge.m_normal *_heightMid;
        }
        else
        {
            _pos[2] = _edge.m_points[0];
            _pos[3] = _edge.m_points[1];
        }
    }

    void Frise::setQuadPos_StopExtremity( edgeFrieze& _edge, Vec2d* _pos, f32 _visualOffset, f32 _heightMid, f32 _width, bbool _isSnaped ) const
    {
        if ( _isSnaped)
        {
            _pos[0] = _edge.m_pos -_edge.m_normal *_visualOffset *_heightMid;
            _pos[1] = _pos[0] +_edge.m_normal *_heightMid;
            _pos[2] = _edge.m_pos + _edge.m_sightNormalized *_width -_edge.m_normal *_visualOffset *_edge.m_heightStop;
            _pos[3] = _pos[2] +_edge.m_normal *_edge.m_heightStop;
        }
        else
        {
            _pos[0] = _edge.m_points[2];
            _pos[1] = _edge.m_points[3];
            _pos[2] = _pos[0] + _edge.m_sightNormalized *_width;
            _pos[3] = _pos[1] + _edge.m_sightNormalized *_width;
        }
    }

    void Frise::setQuadColor_StartExtremity( u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const
    {
        _color[0] = _vtxColorDownExtremity;
        _color[1] = _vtxColorUpExtremity;
        _color[2] = _vtxColorDown;
        _color[3] = _vtxColorUp;
    }

    void Frise::setQuadColor_StopExtremity( u32* _color, u32 _vtxColorDown, u32 _vtxColorUp, u32 _vtxColorDownExtremity, u32 _vtxColorUpExtremity ) const
    {
        _color[0] = _vtxColorDown;
        _color[1] = _vtxColorUp;
        _color[2] = _vtxColorDownExtremity;
        _color[3] = _vtxColorUpExtremity;
    }

    void Frise::setVtxBorderLeftPosDown( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.m_x = _edge.m_points[0].m_x;
        _vtxPos.m_y = _edge.m_points[0].m_y;
        _vtxPos.m_z = m_recomputeData.m_zVtxDown;
    }

    void Frise::setVtxBorderLeftPosUp( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.m_x = _edge.m_points[1].m_x;
        _vtxPos.m_y = _edge.m_points[1].m_y;
        _vtxPos.m_z = m_recomputeData.m_zVtxUp;
    }

    void Frise::setVtxBorderRightPosDown( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.m_x = _edge.m_points[2].m_x;
        _vtxPos.m_y = _edge.m_points[2].m_y;
        _vtxPos.m_z = m_recomputeData.m_zVtxDown;
    }

    void Frise::setVtxBorderRightPosUp( const edgeFrieze& _edge, Vec3d& _vtxPos) const
    {
        _vtxPos.m_x = _edge.m_points[3].m_x;
        _vtxPos.m_y = _edge.m_points[3].m_y;
        _vtxPos.m_z = m_recomputeData.m_zVtxUp;
    }

    void Frise::setVtxPosUp( const Vec2d& _pos, Vec3d& _vtxPos) const
    {
        _vtxPos.m_x = _pos.m_x;
        _vtxPos.m_y = _pos.m_y;
        _vtxPos.m_z = m_recomputeData.m_zVtxUp;
    }

    void Frise::setVtxPosDown( const Vec2d& _pos, Vec3d& _vtxPos) const
    {
        _vtxPos.m_x = _pos.m_x;
        _vtxPos.m_y = _pos.m_y;
        _vtxPos.m_z = m_recomputeData.m_zVtxDown;
    }
 
    void Frise::setVtxPos( const Vec2d& _pos, Vec3d& _vtxPos, f32 _z) const
    {
        _vtxPos.m_x = _pos.m_x;
        _vtxPos.m_y = _pos.m_y;
        _vtxPos.m_z = _z;
    }

    void Frise::setFlipQuadWithAlphaBorder( bbool& _flip, u32 _colorLeft, u32 _colorRight) const
    {
        f32 alphaLeft = Color::getAlphaFromU32(_colorLeft);
        f32 alphaRight = Color::getAlphaFromU32(_colorRight);

        if ( alphaLeft == alphaRight)
            return;

        if ( alphaLeft < alphaRight)
            _flip = btrue;
        else
            _flip = bfalse;
    }

} // namespace ITF
