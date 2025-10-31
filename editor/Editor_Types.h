#ifndef _ITF_EDITOR_TYPES_H_
#define _ITF_EDITOR_TYPES_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

namespace ITF
{
    class PickingShape;

    typedef ITF_VECTOR<PickingShape*> PickingShapeVector;

    struct TabEditInfo
    {
        String              m_worldUniqueName;
        bbool               m_initialized;
        Vector<Vec3d>       m_playersPos;
        Vec3d               m_camPos;
        u32                 m_camMode;
        ObjectRefList       m_editedScenes;
        UndoRedoWorldData   m_undoRedoData;
        ObjectPath          m_currentCheckpoint;
        ObjectRef           m_owner;
        ObjectRef           m_subsceneCurrentlySubEdited;
        ObjectRef           m_forcedWorkingSubScene;

        TabEditInfo()
            : m_initialized(bfalse)
            , m_camMode(U32_INVALID)
        {
        }

        bool operator ==(const String& _uniqueName) const
        {
            return _uniqueName == m_worldUniqueName;
        };

    };

    struct SMultiFilter
    {
        enum FilterOp
        {
            FilterOp_none,
            FilterOp_includeFriendly,
            FilterOp_excludeFriendly,
            FilterOp_includePath,
            FilterOp_excludePath,
            FilterOp_includeComponent,
            FilterOp_excludeComponent,
            FilterOp_includeZ,
            FilterOp_excludeZ,
            FilterOp_includeCellLayer,
            FilterOp_excludeCellLayer,
            FilterOp_includeTarget,
            FilterOp_excludeTarget,
        };

        enum ArithmeticOp
        {
            Op_Bigger,
            Op_Lower,
            Op_Equals,
        };

        String8         m_label;
        FilterOp        m_op;
        ArithmeticOp    m_arithmeticOp;
        f32             m_arithmeticArg;
        bbool           m_isIncludeQuerry;
    };
} // namespace ITF

#endif // _ITF_EDITOR_TYPES_H_