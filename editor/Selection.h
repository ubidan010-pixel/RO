#ifndef _ITF_SELECTION_H_
#define _ITF_SELECTION_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

namespace ITF
{
    class PickingShape_Disc;

    class Selection : public Pickable
    {
    public:
        Selection();
        virtual ~Selection();

        void                setDisplayOffset(const Vec3d& _offset);
        Vec3d               getDisplayOffset()const;

        virtual Vec3d       getAnchorPos() const;

        bbool               isMouseOver()const;
        PickingShape_Disc*  getShape()const                         { return m_pickShape; }

        virtual void        update(f32 _deltaTime);
        void                previewPosition(const Vec3d& _worldPos)const;

    private:
        void                updatePositionAndRotationFromSelection();

        Vec3d               m_displayOffset; // X Y only
        PickingShape_Disc*  m_pickShape;
    };

}

#endif // _ITF_SELECTION_H_
