#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_
#include "editor\PickingShape\PickingShapeData_RotationScale.h"
#endif //_ITF_PICKINGSHAPEDATA_ROTATION_SCALE_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_


namespace ITF
{
     // in pixels
    static const f32 sSimpleClickActivationTreshold = 10.0f;
    static const f32 sMinScale = 0.01f;


    PickingShapeData_RotationScale::PickingShapeData_RotationScale(PickingShape *_picking, Plugin *_plugin) : PickingShapePluginData(_picking, _plugin)
    {
        m_areRotationHelpersValid = bfalse;
        m_areScaleHelpersValid = bfalse;
        m_state = State_Default;
        m_maxScale = 32.f; //Means we can scale 1/maxScale to maxScale
        m_longTriangleLength_Scale1 = 64.f; //For scale 1
        m_scaleMaxDelta=48; //Means that maxScale means longTriangleLength+scaleMaxDelta, 1/maxScale means longTriangleLength-scaleMaxDelta,
        //We use logarithmic scale, thus : logScale.log(MaxScale)=scaleMaxDelta
        m_logScale = m_scaleMaxDelta/logf(m_maxScale);
        m_rotationScaleHelpersFlipV = bfalse;
        m_rotationScaleHelpersFlipH = bfalse;
        m_isScalingXByGUI = m_isScalingYByGUI = bfalse;
        m_lastFrameSeenAsSubElement = (u32)-1;

        //Dummy initializations to avoid static verifier warnings
        m_scaleBarWidth = 8;
        m_scaleBarLength = 16;
        m_rotationScaleSquareSize = 16;
        m_rotationScaleHelpersAngle = 0;
        m_uniformScaleSquareSize = 16;
        m_initialAspectRatioForUniformScale = 1;
        m_greenTriangleWidth = 8;
        m_greenTriangleSize = 16;
        m_rotationScaleHelper_RotationWhenClicked = 0;
        m_uniformScaleDistanceOffset = 0;
        m_scaleYWhenStartedGUIScaling = 1;
        m_scaleXWhenStartedGUIScaling  = 1;
        m_scaleDistanceOffset = 0;

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::tagSubElementThisFrame()
    {
        m_lastFrameSeenAsSubElement = CURRENTFRAME;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool PickingShapeData_RotationScale::isTaggedSubElementThisFrame() const
    {
        return m_lastFrameSeenAsSubElement == CURRENTFRAME;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::update()
    {
        Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();
        if (!owner)
            return;

        computeRotationScaleHelpers(owner);
        if (!m_areRotationHelpersValid)
            return;

        const Vec2d& mousePos = EDITOR->getMouse2d();

        if (isInQuad(mousePos, m_ScaleRotationXQuad) || m_state == State_ClickedScaleRotationXBar && m_areScaleHelpersValid)
            EDITOR->requestMouseFeedback(CursorIcon::Scale_X);
        else if (isInQuad(mousePos, m_ScaleRotationYQuad) || m_state == State_ClickedScaleRotationYBar && m_areScaleHelpersValid)
            EDITOR->requestMouseFeedback(CursorIcon::Scale_Y);
        else if (isInQuad(mousePos, m_ScaleXQuad) || m_state == State_ClickedScaleXBar && m_areScaleHelpersValid)
            EDITOR->requestMouseFeedback(CursorIcon::Scale_X);
        else if (isInQuad(mousePos, m_ScaleYQuad) || m_state == State_ClickedScaleYBar && m_areScaleHelpersValid)
            EDITOR->requestMouseFeedback(CursorIcon::Scale_Y);
        else if (isInQuad(mousePos, m_uniformScaleSquareQuad) || m_state == State_ClickedUniformScaleSquare)
            EDITOR->requestMouseFeedback(CursorIcon::Scale_XY);
        else if (isInQuad(mousePos, m_greenTriangleQuad) || m_state == State_ClickedGreenTriangle)
            EDITOR->requestMouseFeedback(CursorIcon::Rotate);
    }

    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::draw()
    {       
        Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();        
        if (!owner)
            return;

        if (!EditorDraw::pickableDetail_canShowAnyDetail())
            return;

        if (!owner->isVisible(CAMERA))
            return;

        if (EDITOR->getSubEditedItem().isValid())
        {
            if (EDITOR->getSubEditedItem() != owner->getRef())
            {
                const BaseObject* pSubEditedObject = GETOBJECT(EDITOR->getSubEditedItem());
                if (!pSubEditedObject)
                    return;
            }
        }

        bbool canShowAnchor = bfalse;

        const bbool bMouseIsOver = (EDITOR->getPickingShapeOfObjectUnderMouse() == m_pickingShape);
        const bbool bSubElement = isTaggedSubElementThisFrame();
        //// bit 0 = shape, bit 1 = anchor, bit 2 = text

        if (EditorDraw::pickableDetail_CanShowAll())
            canShowAnchor = btrue;

        canShowAnchor = EditorDraw::PickableDetail_CanShowAnchors();

        if (owner->isSelected() || bMouseIsOver)
            canShowAnchor = btrue;

        if (!bSubElement && (owner->isSelected() || bMouseIsOver))
            canShowAnchor = btrue;

        // Draw Anchor 
        if(canShowAnchor)
        {
            //helpers
            computeRotationScaleHelpers(owner);
            drawRotationScaleHelpers(owner);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    f32 PickingShapeData_RotationScale::clampScale(f32 _scale)
    {
#ifdef _CLAMPED_SCALE
        if (_scale<0)
            return -std::max(std::min(-_scale, m_maxScale), 1.f/m_maxScale);
        else
            return std::max(std::min(_scale, m_maxScale), 1.f/m_maxScale);
#else
        return _scale;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    ///
    f32 PickingShapeData_RotationScale::getDisplayedRelativeScaleForHelperDistance(Pickable *_pickable, bbool _isXScale)
    {
        //result depends if we are scaling by GUI
        if (_isXScale && m_isScalingXByGUI)
        {
            f32 initialScale = f32_Abs(m_scaleXWhenStartedGUIScaling);
            if (initialScale<sMinScale)
                initialScale = sMinScale;
            return _pickable->getScale().m_x/initialScale;
        }
        else if (!_isXScale && m_isScalingYByGUI)
        {
            f32 initialScale = f32_Abs(m_scaleYWhenStartedGUIScaling);
            if (initialScale<sMinScale)
                initialScale = sMinScale;
            return _pickable->getScale().m_y/initialScale;
        }
        else
        {
            if (_isXScale && _pickable->getScale().m_x<0)
                return -1;
            else if (!_isXScale && _pickable->getScale().m_y<0)
                return -1;

            return 1.f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::computeRotationScaleHelpers(Pickable *_owner)
    {
        f32 fAnchorSize = PickingShape::m_disc_Radius;
        if (!_owner)
        {
            m_areRotationHelpersValid = m_areScaleHelpersValid = bfalse;
            m_isScalingXByGUI = m_isScalingYByGUI = bfalse;
            return;
        }

        bbool showHelpers = _owner->isSelected() && !EDITOR->isSubEdited(_owner->getRef()) && !m_pickingShape->isSubAnchor();

        // Do not show if the object is not the last in the selection
        if (showHelpers && _owner->getObjectType() != BaseObject::eSelection)
        {
            const SafeArray<PickingShape*>& selection = EDITOR->getSelectionList();
            if (selection.size())
            {
                if(selection.size() > 1)
                {
                    showHelpers = bfalse;
                }
                else
                {
                    const PickingShape* pPS = selection.back();
                    if(pPS->getOwner() != _owner->getRef())
                    {
                        showHelpers = bfalse;
                    }
                }
            }
        }

        //skip?
        if (!showHelpers)
        {
            m_areRotationHelpersValid = m_areScaleHelpersValid = bfalse;
            m_isScalingXByGUI = m_isScalingYByGUI = bfalse;
            return;
        }

        //Compute positions and boxes for rotation/scale helpers
        f32 scaleX = clampScale(getDisplayedRelativeScaleForHelperDistance(_owner, btrue));
        f32 scaleY = clampScale(getDisplayedRelativeScaleForHelperDistance(_owner, bfalse));
#ifdef _LOGARITHMIC_SCALE
        f32 longTriangleLengthX = m_longTriangleLength_Scale1 + m_logScale*log(f32_Abs(scaleX));
        f32 longTriangleLengthY = m_longTriangleLength_Scale1 + m_logScale*log(f32_Abs(scaleY));
#else
        f32 longTriangleLengthX = m_longTriangleLength_Scale1*f32_Abs(scaleX);
        f32 longTriangleLengthY = m_longTriangleLength_Scale1*f32_Abs(scaleY);
#endif
        const f32 shortTriangleLength = 16.f;
        const f32 triangleWidth = 16.f; 
        const f32 barLength = 64.f;
        const f32 barWidth = 8.f;
        const f32 squareSize = 12.f; //square for non-uniform scale
        const f32 uniformSquareSize = 16.f;

        f32 angle = _owner->getAngle();

        m_rotationScaleHelpersAngle = angle; 
        m_greenTriangleSize = _owner->isSelected() ? longTriangleLengthY:shortTriangleLength;
        m_greenTriangleWidth = triangleWidth;

        m_rotationScaleHelpersFlipV = scaleY<0;
        m_rotationScaleHelpersFlipH = scaleX<0;
        //green arrow
        computeRotationHelperQuad(_owner, fAnchorSize, angle+MTH_PIBY2, m_greenTriangleSize-barWidth/2, m_greenTriangleWidth, m_greenTriangleCenter, m_greenTriangleQuad, m_rotationScaleHelpersFlipV);
        //green bar
        computeScaleHelperQuad(_owner, fAnchorSize, angle+MTH_PIBY2, m_greenTriangleSize, barLength, barWidth, m_ScaleYBarCenter, m_ScaleYQuad, m_rotationScaleHelpersFlipV);
        //green square
        Vec2d rotationScaleYBarCenter;
        computeScaleHelperQuad(_owner, fAnchorSize, angle+MTH_PIBY2, m_greenTriangleSize, squareSize, squareSize, rotationScaleYBarCenter, m_ScaleRotationYQuad, m_rotationScaleHelpersFlipV);

        //    
        f32 redTriangleSize = showHelpers ? longTriangleLengthX:shortTriangleLength;

        m_areRotationHelpersValid = btrue;

        //red bar
        computeScaleHelperQuad(_owner, fAnchorSize, angle, redTriangleSize, barLength, barWidth, m_ScaleXBarCenter, m_ScaleXQuad, m_rotationScaleHelpersFlipH);
        //red square
        Vec2d rotationScaleXBarCenter;
        computeScaleHelperQuad(_owner, fAnchorSize, angle, redTriangleSize, squareSize, squareSize, rotationScaleXBarCenter, m_ScaleRotationXQuad, m_rotationScaleHelpersFlipH);

        //
        m_scaleBarLength = barLength;
        m_scaleBarWidth = barWidth;
        m_rotationScaleSquareSize = squareSize;


        //Uniform scale
        m_uniformScaleSquareSize = uniformSquareSize; 
        computeUniformScaleHelperQuad(_owner, fAnchorSize, 
            angle, angle+MTH_PIBY2, 
            redTriangleSize, m_greenTriangleSize, 
            uniformSquareSize, 
            m_uniformScaleSquareCenter, 
            m_uniformScaleSquareQuad,
            m_rotationScaleHelpersFlipH, m_rotationScaleHelpersFlipV);



        m_areScaleHelpersValid = showHelpers;   
    }
    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::computeUniformScaleHelperQuad(const Pickable *_pObject, f32 _fAnchorSize, f32 _angleX, f32 _angleY, f32 _triangleXLength, f32 _triangleYLength, f32 _squareSize, Vec2d &_squareCenter, Vec2d pQuad[], bbool _flipH, bbool _flipV)
    {
        Vec2d line[2];

        //
        if (_flipH)
            _angleX += MTH_PI;
        if (_flipV)
            _angleY += MTH_PI;
        Vec2d arrowDirectionX(cosf(_angleX),-sinf(_angleX));
        Vec2d arrowDirectionY(cosf(_angleY),-sinf(_angleY));

        Vec3d projectedAnchor;
        Vec2d projectedAnchor2d;

        GFX_ADAPTER->compute3DTo2D(_pObject->getAnchorPos(), projectedAnchor);
        projectedAnchor.truncateTo2D(projectedAnchor2d);
        _squareCenter =  projectedAnchor2d + 
            (_fAnchorSize+_triangleXLength)*arrowDirectionX +
            (_fAnchorSize+_triangleYLength)*arrowDirectionY;

        m_uniformScaleDistanceOffset = (_fAnchorSize*arrowDirectionX+_fAnchorSize*arrowDirectionY).norm();


        //compute the quad
        Vec2d HorizAxis(cosf(_angleX+MTH_PIBY2), -sinf(_angleX+MTH_PIBY2));

        HorizAxis *= 0.5f;
        Vec2d vertAxis = HorizAxis.getPerpendicular();

        HorizAxis *= _squareSize;
        vertAxis *= _squareSize;

        pQuad[0] = _squareCenter - HorizAxis - vertAxis;
        pQuad[1] = _squareCenter - HorizAxis + vertAxis;
        pQuad[3] = _squareCenter + HorizAxis - vertAxis;
        pQuad[2] = _squareCenter + HorizAxis + vertAxis;

    }
    //////////////////////////////////////////////////////////////////////////
    //When using a uniform scale, we want to translate the mouse distance into a scale
    //This function returns the (signed) y scale in _yScale (given the aspect ratio)
    //and returns false if no solution found
    bbool PickingShapeData_RotationScale::getScaleYFromDistance_UniformScale(const Vec2d &_deltaFromAnchor, f32 _fAnchorSize, f32 _ratio, f32 &_yScale)
    {
        Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();
        if (!owner)
            return bfalse;

        //Equation : distance_from_anchor² = (l0*scaleX+_anchorSize)²+(l0*scaleY+_anchorSize)², with scaleX=absRatio*scaleY
        //(see computeUniformScaleHelperQuad for details)
        //then we solve a 2nd order equation
        f32 l0 = m_longTriangleLength_Scale1;
        f32 absRatio = f32_Abs(_ratio);
        f32 distanceFromAnchor = _deltaFromAnchor.norm();
        f32 a = l0*l0*(absRatio*absRatio+1);
        f32 b = 2*l0*_fAnchorSize*(absRatio+1);
        f32 c = 2*_fAnchorSize*_fAnchorSize-distanceFromAnchor*distanceFromAnchor;

        f32 discriminant = b*b-4*a*c;
        if (discriminant<0)
            return bfalse;
        f32 sqrtDisc = f32_Sqrt(discriminant);
        f32 y1 = (-b+sqrtDisc)/(2*a);
        //f32 y2 = (-b-sqrtDisc)/(2*a);

        //
        f32 angleX = owner->getAngle();
        f32 angleY = angleX + MTH_PIBY2;

        Vec2d arrowDirectionX(cosf(angleX),-sinf(angleX));
        Vec2d arrowDirectionY(cosf(angleY),-sinf(angleY));
        _yScale = y1;
        if (_yScale<0)
            return bfalse;

        //compute simulated vector for y1
        f32 triangleXLength = l0*_ratio*y1;
        f32 triangleYLength = l0*y1;
        Vec2d simulatedDelta = (_fAnchorSize+triangleXLength)*arrowDirectionX +
                                (_fAnchorSize+triangleYLength)*arrowDirectionY;

        //invert if incorrect
        if (simulatedDelta.dot(_deltaFromAnchor)<0)
            _yScale = -_yScale;

        return btrue;

    }

    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::computeScaleHelperQuad(const Pickable *_pObject, f32 _fAnchorSize, f32 _angle, f32 _triangleLength, f32 _barLength, f32 _barWidth, Vec2d &_barCenter, Vec2d pQuad[], bbool _centerSymmetry)
    {
        Vec2d line[2];
        if (_centerSymmetry)
            _angle += MTH_PI;
        Vec2d arrowDirection(cosf(_angle),-sinf(_angle));
        Vec2d barLenDirection(cosf(_angle+MTH_PIBY2), -sinf(_angle+MTH_PIBY2));
        Vec3d projectedAnchor;
        Vec2d projectedAnchor2d;

        GFX_ADAPTER->compute3DTo2D(_pObject->getAnchorPos(), projectedAnchor);
        projectedAnchor.truncateTo2D(projectedAnchor2d);
        _barCenter =  projectedAnchor2d + (_fAnchorSize+_triangleLength)*arrowDirection;
        m_scaleDistanceOffset = _fAnchorSize;
        line[1] = _barCenter + _barLength*barLenDirection*0.5f;    
        line[0] = _barCenter - _barLength*barLenDirection*0.5f;

        //compute the quad
        Vec2d HorizAxis(cosf(_angle+MTH_PIBY2), -sinf(_angle+MTH_PIBY2));

        HorizAxis *= 0.5f;
        Vec2d vertAxis = HorizAxis.getPerpendicular();

        HorizAxis *= _barLength;
        vertAxis *= _barWidth;

        pQuad[0] = _barCenter - HorizAxis - vertAxis;
        pQuad[1] = _barCenter - HorizAxis + vertAxis;
        pQuad[3] = _barCenter + HorizAxis - vertAxis;
        pQuad[2] = _barCenter + HorizAxis + vertAxis;

    }
    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::computeRotationHelperQuad(const Pickable *_pObject, f32 _fAnchorSize, f32 _angle, f32 _triangleLength, f32 _triangleWidth, Vec2d &_triangleCenter, Vec2d pQuad[], bbool _centerSymmetry)
    {
        Vec2d line[2];
        if (_centerSymmetry)
            _angle += MTH_PI;
        Vec2d lineDirection(cosf(_angle),-sinf(_angle));
        Vec2d widthVector(cosf(_angle+MTH_PIBY2), -sinf(_angle+MTH_PIBY2));
        Vec3d projectedAnchor;
        Vec2d projectedAnchor2d;

        GFX_ADAPTER->compute3DTo2D(_pObject->getAnchorPos(), projectedAnchor);
        projectedAnchor.truncateTo2D(projectedAnchor2d);
        line[0] =  projectedAnchor2d + _fAnchorSize*lineDirection;
        line[1] = line[0] + _triangleLength*lineDirection*0.5f;
        _triangleCenter = line[1];

        //compute the quad
        Vec2d HorizAxis(cosf(_angle), -sinf(_angle));

        HorizAxis *= 0.5f;
        Vec2d vertAxis = HorizAxis.getPerpendicular();

        HorizAxis *= _triangleLength;
        vertAxis *= _triangleWidth;

        pQuad[0] = _triangleCenter - HorizAxis - vertAxis;
        pQuad[1] = _triangleCenter - HorizAxis + vertAxis;
        pQuad[3] = _triangleCenter + HorizAxis - vertAxis;
        pQuad[2] = _triangleCenter + HorizAxis + vertAxis;

    }
    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::drawRotationScaleHelpers(Pickable *_owner)
    {
        if (!m_areRotationHelpersValid)
            computeRotationScaleHelpers(_owner);
        if (!m_areRotationHelpersValid)
            return; //nothing to draw

        f32 angleOffset = m_rotationScaleHelpersFlipV?MTH_PI:0;
        GFX_ADAPTER->drawDBGQuad(m_greenTriangleCenter,
                                 m_greenTriangleSize, m_greenTriangleWidth, 
                                 (Texture*)EDITOR->getIconStock().getAtlasTexture(),
                                 m_rotationScaleHelpersAngle+MTH_PIBY2+angleOffset,
                                 0,
                                 0,
                                 (i32)Editor_IconStock::Icon_GreenArrow);

        if (m_areScaleHelpersValid)
        {
            f32 scaleXAngleOffset = m_rotationScaleHelpersFlipH?MTH_PI:0;
            f32 scaleYAngleOffset = m_rotationScaleHelpersFlipV?MTH_PI:0;

            //scale bars
            GFX_ADAPTER->drawDBGQuad(m_ScaleYBarCenter,
                m_scaleBarLength, m_scaleBarWidth, 
                (Texture*)EDITOR->getIconStock().getAtlasTexture(),
                m_rotationScaleHelpersAngle+scaleYAngleOffset,
                0,
                0,
                (i32)Editor_IconStock::Icon_GreenBar);
            GFX_ADAPTER->drawDBGQuad(m_ScaleXBarCenter,
                m_scaleBarLength, m_scaleBarWidth, 
                (Texture*)EDITOR->getIconStock().getAtlasTexture(),
                m_rotationScaleHelpersAngle+MTH_PIBY2+scaleXAngleOffset,
                0,
                0,
                (i32)Editor_IconStock::Icon_RedBar);


            //scale squares
            GFX_ADAPTER->drawDBGQuad(m_ScaleYBarCenter,
                m_rotationScaleSquareSize, m_rotationScaleSquareSize, 
                (Texture*)EDITOR->getIconStock().getAtlasTexture(),
                m_rotationScaleHelpersAngle+scaleYAngleOffset,
                0,
                0,
                (i32)Editor_IconStock::Icon_GreenSquare);
            GFX_ADAPTER->drawDBGQuad(m_ScaleXBarCenter,
                m_rotationScaleSquareSize, m_rotationScaleSquareSize, 
                (Texture*)EDITOR->getIconStock().getAtlasTexture(),
                m_rotationScaleHelpersAngle+MTH_PIBY2+scaleXAngleOffset,
                0,
                0,
                (i32)Editor_IconStock::Icon_RedSquare);


            //uniform scale square
            GFX_ADAPTER->drawDBGQuad(m_uniformScaleSquareCenter,
                m_uniformScaleSquareSize, m_uniformScaleSquareSize, 
                (Texture*)EDITOR->getIconStock().getAtlasTexture(),
                m_rotationScaleHelpersAngle,
                0,
                0,
                (i32)Editor_IconStock::Icon_YellowSquare);

        }
    }

    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::handleMousePressed(const Vec2d &_mousePos)
    {
        Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();
        if (!owner)
            return;
        computeRotationScaleHelpers(owner);

        if (!m_areRotationHelpersValid)
            return;

        ObjectRef ownerRef = owner->getRef();

        //Hit tests
        if (isInQuad(_mousePos, m_ScaleRotationYQuad) && m_areScaleHelpersValid)
        {
            if (!EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->startRotationScaleHelperMode(ownerRef);
                preChange();

                m_state = State_ClickedScaleRotationYBar;
                m_scaleYWhenStartedGUIScaling = owner->getScale().m_y;
                m_isScalingYByGUI = btrue;
                m_rotationScaleHelper_ClickedPosition = _mousePos;
                m_initialCursorOffsetFromBarCenter = m_rotationScaleHelper_ClickedPosition - m_ScaleYBarCenter;
                m_rotationScaleHelper_RotationWhenClicked = owner->getAngle();
            }
        }
        else if (isInQuad(_mousePos, m_ScaleRotationXQuad) && m_areScaleHelpersValid)
        {
            if (!EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->startRotationScaleHelperMode(ownerRef);
                preChange();

                m_state = State_ClickedScaleRotationXBar;
                m_scaleXWhenStartedGUIScaling = owner->getScale().m_x;
                m_isScalingXByGUI = btrue;
                m_rotationScaleHelper_ClickedPosition = _mousePos;
                m_initialCursorOffsetFromBarCenter = m_rotationScaleHelper_ClickedPosition - m_ScaleXBarCenter;
                m_rotationScaleHelper_RotationWhenClicked = owner->getAngle();
            }
        }
        else if (isInQuad(_mousePos, m_ScaleXQuad) && m_areScaleHelpersValid)
        {
            if (!EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->startRotationScaleHelperMode(ownerRef);
                preChange();

                m_state = State_ClickedScaleXBar;
                m_scaleXWhenStartedGUIScaling = owner->getScale().m_x;
                m_isScalingXByGUI = btrue;
                m_rotationScaleHelper_ClickedPosition = _mousePos;
                m_initialCursorOffsetFromBarCenter = m_rotationScaleHelper_ClickedPosition - m_ScaleXBarCenter;
            }
        }
        else if (isInQuad(_mousePos, m_ScaleYQuad) && m_areScaleHelpersValid)
        {
            if (!EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->startRotationScaleHelperMode(ownerRef);
                preChange();

                m_state = State_ClickedScaleYBar;
                m_scaleYWhenStartedGUIScaling = owner->getScale().m_y;
                m_isScalingYByGUI = btrue;
                m_rotationScaleHelper_ClickedPosition = _mousePos;
                m_initialCursorOffsetFromBarCenter = m_rotationScaleHelper_ClickedPosition - m_ScaleYBarCenter;
            }
        }
        else if (isInQuad(_mousePos, m_uniformScaleSquareQuad))
        {
            if (!EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->startRotationScaleHelperMode(ownerRef);
                preChange();

                m_state = State_ClickedUniformScaleSquare;
                m_scaleXWhenStartedGUIScaling = owner->getScale().m_x;
                m_isScalingXByGUI = btrue;
                m_scaleYWhenStartedGUIScaling = owner->getScale().m_y;
                m_isScalingYByGUI = btrue;
                m_rotationScaleHelper_ClickedPosition = _mousePos;
                m_initialCursorOffsetFromUniformSquareCenter = m_rotationScaleHelper_ClickedPosition - m_uniformScaleSquareCenter;
                Vec2d scale = owner->getScale();
                if (f32_Abs(scale.m_y)<sMinScale)
                    scale.m_y=sMinScale;
                m_initialAspectRatioForUniformScale = scale.m_x/scale.m_y;
            }
        }
        else if (isInQuad(_mousePos, m_greenTriangleQuad))
        {
            if (!EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->startRotationScaleHelperMode(ownerRef);
                preChange();

                m_state = State_ClickedGreenTriangle;
                m_rotationScaleHelper_ClickedPosition = _mousePos;
                m_rotationScaleHelper_RotationWhenClicked = owner->getAngle();
            }
        }
        
        getCurrentHelperUsedPos(m_prevHelperPos);
    } 
    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::handleMouseReleased(const Vec2d &_mousePos)
    {
        if (!m_areRotationHelpersValid)
        {
            if (EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->endRotationScaleHelperMode();
                m_state = State_Default;
                m_isScalingXByGUI = m_isScalingYByGUI = bfalse;
            }            
            return;
        }

        if (m_state == State_ClickedGreenTriangle || 
            m_state == State_ClickedScaleXBar || 
            m_state == State_ClickedScaleYBar ||
            m_state == State_ClickedScaleRotationYBar ||
            m_state == State_ClickedScaleRotationXBar ||
            m_state == State_ClickedUniformScaleSquare)
        {                    
            EDITOR->endRotationScaleHelperMode();
            m_state = State_Default;
            m_isScalingXByGUI = m_isScalingYByGUI = bfalse;

            finalChange();
        }


    }
    //////////////////////////////////////////////////////////////////////////
    void PickingShapeData_RotationScale::handleMouseMove(const Vec2d &_mousePos)
    {
        Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();
        if (!m_areRotationHelpersValid || owner==NULL)
        {
            if (EDITOR->isInRotationScaleHelperMode())
            {
                EDITOR->endRotationScaleHelperMode();
                m_state = State_Default;
                m_isScalingXByGUI = m_isScalingYByGUI = bfalse;
            }            
            return;
        }

        Vec3d pos2d;//projected position of the shape
        Vec2d pos2d_2d; //only x,y
        GFX_ADAPTER->compute3DTo2D(static_cast<PickingShape_Disc*>(m_pickingShape)->getPos(), pos2d);
        if (pos2d.m_z<0 || pos2d.m_z>1)
            return;
        pos2d.truncateTo2D(pos2d_2d);

        //Mouse move & scale
        if (m_state == State_ClickedScaleXBar || 
            m_state == State_ClickedScaleYBar || 
            m_state == State_ClickedScaleRotationYBar || 
            m_state == State_ClickedScaleRotationXBar)
        {
            bool isScalingX = (m_state == State_ClickedScaleXBar) || (m_state == State_ClickedScaleRotationXBar);
            Vec2d deltaMouse = _mousePos-pos2d_2d;
            Vec2d scaleDirection;

            Vec2d *scaleBarCenter;
            bbool scaleFlip;
            if (isScalingX)
            {
                scaleBarCenter = &m_ScaleXBarCenter;
                scaleFlip = m_rotationScaleHelpersFlipH;
            }
            else
            {
                scaleBarCenter = &m_ScaleYBarCenter;
                scaleFlip = m_rotationScaleHelpersFlipV;
            }
            
            scaleDirection = *scaleBarCenter-pos2d_2d;
            scaleDirection.normalize();
            f32 projectedIdealPointOnScaleAxis = scaleDirection.dot(deltaMouse-m_initialCursorOffsetFromBarCenter);
            f32 scaleDeltaDistance;
#ifdef _LOGARITHMIC_SCALE
            scaleDeltaDistance =  f32_Abs(projectedIdealPointOnScaleAxis) - m_longTriangleLength_Scale1-m_scaleDistanceOffset;
            f32 fScale = expf(scaleDeltaDistance/m_logScale);
#else
            scaleDeltaDistance =  f32_Abs(projectedIdealPointOnScaleAxis) - m_scaleDistanceOffset;
            f32 fScale = scaleDeltaDistance/m_longTriangleLength_Scale1;
#endif
            if(fScale != 0.0f)
            {
                if (projectedIdealPointOnScaleAxis<0)
                    scaleFlip = !scaleFlip;
                if (scaleFlip)
                    fScale = -fScale;
                fScale = clampScale(fScale);

                Vec2d currentScale = owner->getScale();

                f32 *pScaleWhenStartedGUIScaling = isScalingX?&m_scaleXWhenStartedGUIScaling:&m_scaleYWhenStartedGUIScaling;
                f32 *pCurrentScale = isScalingX?&currentScale.m_x:&currentScale.m_y;

                    //if the scale is in the same direction as when we started scaling, 
                    //we take m_scale?WhenStartedGUIScaling's sign,
                    //otherwise we take -m_scale?WhenStartedGUIScaling

                f32 initialSign = (*pScaleWhenStartedGUIScaling)<0?-1.f:1.f;

                *pCurrentScale = f32_Abs(*pScaleWhenStartedGUIScaling * fScale) * initialSign;
                if (fScale* (*pScaleWhenStartedGUIScaling)<0)
                    *pCurrentScale = - *pCurrentScale;
                    
                scale(currentScale);
                postChange();
            }
        }
        
        //rotation
        if (m_state == State_ClickedGreenTriangle || 
            m_state == State_ClickedScaleRotationYBar || 
            m_state == State_ClickedScaleRotationXBar)
        {        
            f32 cursorAngle = atan2f(-(_mousePos.m_y-pos2d.m_y), _mousePos.m_x-pos2d.m_x);
            f32 initialCursorAngle = atan2f(-(m_rotationScaleHelper_ClickedPosition.m_y-pos2d.m_y), m_rotationScaleHelper_ClickedPosition.m_x-pos2d.m_x);

            f32 angle = m_rotationScaleHelper_RotationWhenClicked + cursorAngle-initialCursorAngle;            
            angle = getSign(angle) * f32_Modulo(f32_Abs(angle), MTH_2PI);
            EDITOR->snapAngle(angle);

            const f32 deltaAngle = angle - owner->getAngle();

            if(f32_Abs(deltaAngle) > MTH_EPSILON)
            {
                rotateAroundAxisZ(deltaAngle, NULL);
                postChange();
            }
        }

        //uniform scale
        if (m_state == State_ClickedUniformScaleSquare)
        {
            f32 scaleY=1.f;
            f32 aspectRationSign = (m_initialAspectRatioForUniformScale<0)?-1.f:1.f;
            bbool r = getScaleYFromDistance_UniformScale(_mousePos-pos2d_2d-m_initialCursorOffsetFromUniformSquareCenter, m_scaleDistanceOffset, aspectRationSign, scaleY);
            if (r)
            {
                //for each direction : if the scale is in the same direction as when we started scaling, 
                //we take m_scale?WhenStartedGUIScaling's sign,
                //otherwise we take -m_scale?WhenStartedGUIScaling
                f32 initialSignX = (m_scaleXWhenStartedGUIScaling<0)?-1.f:1.f;
                f32 initialSignY = (m_scaleYWhenStartedGUIScaling<0)?-1.f:1.f;
                f32 scaleX = m_initialAspectRatioForUniformScale*scaleY;

                f32 currentScaleX = f32_Abs(m_scaleYWhenStartedGUIScaling*scaleX)*initialSignX;
                f32 currentScaleY = f32_Abs(m_scaleYWhenStartedGUIScaling*scaleY)*initialSignY;

                if (scaleX*m_scaleXWhenStartedGUIScaling < 0)
                    currentScaleX = -currentScaleX;
                if (scaleY*m_scaleYWhenStartedGUIScaling < 0)
                    currentScaleY = -currentScaleY;

                scale(Vec2d(currentScaleX, currentScaleY));
                postChange();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bbool PickingShapeData_RotationScale::isInQuad(const Vec2d &_point, const Vec2d _pQuad[4])
    {
        Vec2d delta, normal;

        delta = _pQuad[1]-_pQuad[0];
        normal = delta.getPerpendicular();
        f32 sign0 = (_point-_pQuad[0]).dot(normal);

        delta = _pQuad[2]-_pQuad[1];
        normal = delta.getPerpendicular();
        f32 sign1 = (_point-_pQuad[1]).dot(normal);

        delta = _pQuad[3]-_pQuad[2];
        normal = delta.getPerpendicular();
        f32 sign2 = (_point-_pQuad[2]).dot(normal);

        delta = _pQuad[0]-_pQuad[3];
        normal = delta.getPerpendicular();
        f32 sign3 = (_point-_pQuad[3]).dot(normal);

        f32 nonZeroSign = sign0;
        if (sign1!=0)
            nonZeroSign = sign1;
        else if (sign2!=0)
            nonZeroSign = sign2;
        else if (sign3!=0)
            nonZeroSign = sign3;
        if (nonZeroSign*sign0<0 || nonZeroSign*sign1<0 || nonZeroSign*sign2<0 || nonZeroSign*sign3<0)
            return bfalse;
        return btrue;
    }

    void PickingShapeData_RotationScale::preChange()
    {
        UNDOREDO_MANAGER->preChange(static_cast<Pickable*>(m_pickingShape->getOwnerPtr()));
    }

    void PickingShapeData_RotationScale::postChange()
    {
        Pickable* pickable = static_cast<Pickable*>(m_pickingShape->getOwnerPtr());
        pickable->onEditorMove(EDITOR->isAllowStorePickableInitData(pickable));
    }

    void PickingShapeData_RotationScale::finalChange()
    {
        if (m_pickingShape && m_pickingShape->getOwnerPtr())
        {
            Pickable *owner = (Pickable*)m_pickingShape->getOwnerPtr();
            PLUGINGATEWAY->onObjectChanged(owner);
        }
    }

    bbool PickingShapeData_RotationScale::getCurrentHelperUsedPos( Vec2d& _pos2D ) const
    {
        switch(m_state)
        {
        case State_ClickedScaleXBar:
        case State_ClickedScaleRotationXBar:
            _pos2D = m_ScaleXBarCenter + m_initialCursorOffsetFromBarCenter;
            break;
        case State_ClickedScaleYBar:
        case State_ClickedScaleRotationYBar:
            _pos2D = m_ScaleYBarCenter + m_initialCursorOffsetFromBarCenter;
            break;
        case State_ClickedUniformScaleSquare:
            _pos2D = m_uniformScaleSquareCenter + m_initialCursorOffsetFromUniformSquareCenter;        
            break;

        default:
            return bfalse;
            break;
        }
        return btrue;
    }

} // namespace ITF
