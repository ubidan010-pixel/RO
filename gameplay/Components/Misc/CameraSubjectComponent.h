#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#define _ITF_CAMERASUBJECTCOMPONENT_H_

namespace ITF
{
    class CameraSubjectComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraSubjectComponent, ActorComponent,2693571618);
        DECLARE_SERIALIZE()

    public:

        CameraSubjectComponent();
        virtual ~CameraSubjectComponent();

        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        const Vec2d&                getBoundingBoxMin() const;
        const Vec2d&                getBoundingBoxMax() const;
        const Vec2d&                getOffset() const;

        AABB                        getAABB() const;
        bbool                       isSubjectOnScreen( bbool _invalidValue = bfalse ) const;

    private:
 
        const class CameraSubjectComponent_Template* getTemplate() const;
    };

    class CameraSubjectComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CameraSubjectComponent_Template,TemplateActorComponent,1531192225);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CameraSubjectComponent);

    public:

        CameraSubjectComponent_Template();
        ~CameraSubjectComponent_Template();

        const Vec2d&    getBoundingBoxMin() const { return m_boundingBoxMin; }
        const Vec2d&    getBoundingBoxMax() const { return m_boundingBoxMax; }
        const Vec2d&    getOffset() const { return m_offset; }

    private:
        Vec2d           m_boundingBoxMin;              // Minimum point of the bounding box
        Vec2d           m_boundingBoxMax;              // Maximum point of the bounding box
        Vec2d           m_offset;
    };

    ITF_INLINE const CameraSubjectComponent_Template* CameraSubjectComponent::getTemplate() const
    {
        return static_cast<const CameraSubjectComponent_Template*>(m_template);
    }

    ITF_INLINE const Vec2d& CameraSubjectComponent::getBoundingBoxMin() const { return getTemplate()->getBoundingBoxMin(); }
    ITF_INLINE const Vec2d& CameraSubjectComponent::getBoundingBoxMax() const { return getTemplate()->getBoundingBoxMax(); }
    ITF_INLINE const Vec2d& CameraSubjectComponent::getOffset() const { return getTemplate()->getOffset(); }
  
}

#endif // _ITF_CAMERASUBJECTCOMPONENT_H_