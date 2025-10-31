#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#define _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

namespace ITF
{
    class Pickable;

    class PickingShapeData_RotationScale : public PickingShapePluginData
    {
    public:
        enum State 
        {
            State_Default, 
            State_ClickedGreenTriangle, 
            State_ClickedScaleXBar, 
            State_ClickedScaleYBar,
            State_ClickedScaleRotationXBar,
            State_ClickedScaleRotationYBar,
            State_ClickedUniformScaleSquare,
            ENUM_FORCE_SIZE_32(0)
        };

        //////////////////////////////////////////////////////////////////////////
        ///Construction
        PickingShapeData_RotationScale(PickingShape*, Plugin*);

        //////////////////////////////////////////////////////////////////////////
        ///Update shape data
        virtual void        update();

        //////////////////////////////////////////////////////////////////////////
        ///Draw shape data
        virtual void        draw();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse Pressed
        /// @param _mousePos position of mouse
        virtual void        handleMousePressed(const Vec2d & _mousePos); //NB : in this function, CAMERA is supposed to be applied

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse move
        /// @param _mousePos position of mouse ( may be stationary )
        virtual void        handleMouseMove(const Vec2d & _mousePos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// handle Mouse Released
        /// @param _mousePos
        virtual void        handleMouseReleased(const Vec2d & _mousePos);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// compute Rotation Scale Helpers positions, boxes, etc
        void                computeRotationScaleHelpers(Pickable *_owner);

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Display the helpers
        void                drawRotationScaleHelpers(Pickable *_owner);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the "relative" scale, as displayed by helper. usually returns 1, except when editing
        /// @param _pickable the actor
        /// @param _isXScale btrue if X, bfalse if Y
        float               getDisplayedRelativeScaleForHelperDistance(Pickable *_pickable, bbool _isXScale);

        bbool               getCurrentHelperUsedPos(Vec2d& _pos2D)const;

        //////////////////////////////////////////////////////////////////////////
        Vec2d               m_rotationScaleHelper_ClickedPosition;

        bbool               m_areRotationHelpersValid, m_areScaleHelpersValid;

        bbool               m_rotationScaleHelpersFlipH, m_rotationScaleHelpersFlipV;
        f32                 m_greenTriangleWidth, m_greenTriangleSize; 
        Vec2d               m_greenTriangleCenter;
        Vec2d               m_greenTriangleQuad[4];

        Vec2d               m_ScaleXBarCenter, m_ScaleYBarCenter; //Also center for scale quad
        Vec2d               m_uniformScaleSquareCenter;
        Vec2d               m_ScaleXQuad[4], m_ScaleYQuad[4];
        Vec2d               m_uniformScaleSquareQuad[4];
        Vec2d               m_ScaleRotationXQuad[4], m_ScaleRotationYQuad[4];
        float               m_isScalingXByGUI, m_isScalingYByGUI;
        float               m_scaleXWhenStartedGUIScaling, m_scaleYWhenStartedGUIScaling;
        f32                 m_scaleBarWidth, m_scaleBarLength;
        f32                 m_rotationScaleSquareSize;
        f32                 m_uniformScaleSquareSize;
        f32                 m_longTriangleLength_Scale1;
        f32                 m_scaleDistanceOffset; //The center of the scale bar is at m_scaleDistanceOffset+ m_longTriangleLength_Scale1 (for scale1) from anchor center
        f32                 m_uniformScaleDistanceOffset;
        f32                 m_maxScale;
        f32                 m_scaleMaxDelta;
        f32                 m_logScale;

        f32                 m_rotationScaleHelpersAngle;
        f32                 m_rotationScaleHelper_RotationWhenClicked;
        Vec2d               m_initialCursorOffsetFromBarCenter;
        Vec2d               m_initialCursorOffsetFromUniformSquareCenter;
        f32                 m_initialAspectRatioForUniformScale;
        State               m_state;
        u32                 m_lastFrameSeenAsSubElement;

    protected:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called before a change is applied on the owner
        virtual void        preChange();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called after a change is applied on the owner
        virtual void        postChange();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Called just after the last postChange
        virtual void        finalChange();

        Vec2d               m_prevHelperPos;

        //
        bbool               isTaggedSubElementThisFrame() const;
        void                tagSubElementThisFrame();
        //
        bbool               isInQuad(const Vec2d &_point, const Vec2d _pQuad[4]);
        void                computeRotationHelperQuad(const Pickable *_pOwner, f32 _fAnchorSize, f32 _angle, f32 _triangleLength, f32 _triangleWidth, Vec2d &_triangleCenter, Vec2d pQuad[], bbool _centerSymmetry);
        void                computeScaleHelperQuad(const Pickable *_pOwner, f32 _fAnchorSize, f32 _angle, f32 _triangleLength, f32 _barLength, f32 _barWidth, Vec2d &_barCenter, Vec2d pQuad[], bbool _centerSymmetry);
        void                computeUniformScaleHelperQuad(const Pickable *_pOwner, f32 _fAnchorSize, f32 _angleX, f32 _angleY, f32 _triangleXLength, f32 _triangleYLength, f32 _squareSize, Vec2d &_squareCenter, Vec2d pQuad[], bbool _flipH, bbool _flipV);
        f32                 clampScale(f32 _scale);
        bbool               getScaleYFromDistance_UniformScale(const Vec2d &_deltaFromAnchor, f32 _fAnchorSize, f32 _ratio, f32 &_yScale);

    };

} // namespace ITF


#endif // _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
