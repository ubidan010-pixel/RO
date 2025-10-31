#include "precompiled_editor.h"

#ifndef _ITF_SELECTION_H_
#include "editor/selection.h"
#endif //_ITF_SELECTION_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_EDITOR_PLUGIN_H_
#include "editor/EditorPlugin.h"
#endif //_ITF_EDITOR_PLUGIN_H_

namespace ITF
{

    Selection::Selection()
        : m_pickShape(NULL)
    {
        setObjectType(BaseObject::eSelection);
        setAlwaysVisible(btrue);
        m_pickShape = newAlloc(mId_Editor,PickingShape_Disc(NULL));
        m_pickShape->setOwner(getRef());
        m_pickShape->allocatePluginData();
    }

    Selection::~Selection()
    {
        SF_DEL(m_pickShape);
    }

    void Selection::update( f32 /*_deltaTime*/ )
    {
        static bbool initPosSet = bfalse;
        if(EDITOR->getSelectionSize() > 1)
        {
            if(!initPosSet)
            {
                m_displayOffset = Vec3d::Zero;
                initPosSet = btrue;
            }
            
            updatePositionAndRotationFromSelection();

            setSelected(m_pickShape->getPickingInfoPtr());
        }
        else
        {
            initPosSet = bfalse;
            setSelected(NULL);
        }

        m_pickShape->update();
    }

    void Selection::previewPosition( const Vec3d& _worldPos ) const
    {
        EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_CameraPan, _worldPos, PickingShape::m_disc_sqRadius / 4.0f);
    }

    void Selection::updatePositionAndRotationFromSelection()
    {
        if(const u32 uShapeCount = EDITOR->getSelectionSize())
        {
            f32 angle = 0.0f;
            Vec3d pos = Vec3d::Zero;
            f32 fCount = 0.0f;
            for(u32 i = 0; i < uShapeCount; ++i)
            {
                PickingShape* pShape = EDITOR->getSelectionAt(i);

                if(pShape->getShapeType() == PickingShape::ShapeType_Disc)
                {
                    pos += static_cast<PickingShape_Disc*>(pShape)->getPos();
                    angle += static_cast<PickingShape_Disc*>(pShape)->getAngle();
                    fCount++;
                }
            }
            if(fCount > 0.0f)
            {
                pos /= fCount;
                angle /= fCount;

                setPos(pos);
                setAngle(angle);
            }
        }
    }

    void Selection::setDisplayOffset( const Vec3d& _worldOffset )
    {
        Vec3d localOffset = _worldOffset;
        Vec3d_Rotate(&localOffset, -getAngle());
        m_displayOffset = localOffset;
    }

    Vec3d Selection::getDisplayOffset() const
    {
        Vec3d worldOffset = m_displayOffset;
        Vec3d_Rotate(&worldOffset, getAngle());
        return worldOffset;
    }

    Vec3d Selection::getAnchorPos() const
    {
        return getPos() + getDisplayOffset();
    }

    bbool Selection::isMouseOver() const
    {
        Vec3d ssDisplayPos;
        GFX_ADAPTER->compute3DTo2D(getAnchorPos(), ssDisplayPos);

        return (ssDisplayPos.truncateTo2D() - EDITOR->getMouse2d()).sqrnorm() < PickingShape::m_disc_sqRadius / 2.0f;
    }
} //namespace ITF
