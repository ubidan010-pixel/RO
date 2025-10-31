#include "precompiled_gameplay.h"

#ifndef _ITF_PATCHCURVECOMPONENT_H_
#include "gameplay/Components/Misc/PatchCurveComponent.h"
#endif //_ITF_PATCHCURVECOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PatchCurveComponent)

    BEGIN_SERIALIZATION_CHILD(PatchCurveComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(PatchCurveComponent)
        VALIDATE_COMPONENT_PARAM("childOrientationInfluence", 0 <= getTemplate()->getChildOrientationInfluence() && getTemplate()->getChildOrientationInfluence() <= 1, "should in the [0,1] range");
    END_VALIDATE_COMPONENT()


    PatchCurveComponent::PatchCurveComponent()
        : m_animComponent(NULL)
        , m_timer(0.f)
    {
    }

    PatchCurveComponent::~PatchCurveComponent()
    {
        clear();
    }

    void PatchCurveComponent::clear()
    {
        if (!m_actor)
        {
            return;
        }

        if (m_texture.isValidResourceId())
        {
            m_actor->removeResource(m_texture);
            m_texture.invalidateResourceId();
        }
    }


    void PatchCurveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();


        if (!getTemplate()->getTextureFile().isEmpty())
        {
            m_texture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());
        }
    }

    void PatchCurveComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_timer += _dt;
    }


    void PatchCurveComponent::computeCommonData( const Vec2d& _p2, const Vec2d& _p1,
        f32& patchLength, f32& uvLength, Vec2d& startUV, Vec2d& endUV )
    {
        patchLength = (_p2 - _p1).norm();

        uvLength = 1.f;
        if (getTemplate()->getTileLength())
        {
            uvLength  = patchLength / getTemplate()->getTileLength();
        }

        f32 uvOffset = 0.f;
        if (getTemplate()->getUVScrollSpeed())
        {
            uvOffset = m_timer * -getTemplate()->getUVScrollSpeed();
            // NOTE: using minus scroll speed so that a positive speed gives left-to-right scroll
        }

        if (getTemplate()->getAttachToChild())
        {
            startUV.m_x = uvOffset - uvLength;
            endUV.m_x = uvOffset;
        }
        else
        {
            startUV.m_x = uvOffset;
            endUV.m_x = uvOffset + uvLength;
        }
    }

    void PatchCurveComponent::drawLink(
        const Vec2d& _p1, const Vec2d& _n1,
        const Vec2d& _p2, const Vec2d& _n2,
        PatchCurve& _patchCurve, const f32 _alpha /*= 1.f */ )
    {
        ITF_ASSERT_MSG(m_texture.getResource(), "nothing to draw!");
        if (!m_texture.getResource())
        {
            return;
        }


        Texture* texture = (Texture*)m_texture.getResource();
        f32 textureRatio = texture->getSizeX() ? f32(texture->getSizeY()) / f32(texture->getSizeX()) : 1.f;


        f32 patchLength, uvLength;
        Vec2d startUV, endUV;
        computeCommonData(_p1, _p2, patchLength, uvLength, startUV, endUV);

        if (patchLength <= MTH_EPSILON)
        {
            return;
        }


        PatchCurve::Params params;

        params.m_p1 = _p1;
        params.m_n1 = _n1;
        params.m_p2 = _p2;
        params.m_n2 = _n2;
        params.m_width = getTemplate()->getWidth();
        params.m_startUV = Vec2d(startUV.m_x, 0.f);
        params.m_endUV = Vec2d(endUV.m_x, textureRatio);


        const f32 z = m_actor->getDepth() + getTemplate()->getZOffset();

        Color color = getColorFactor();
        color.m_a *= _alpha;
        ColorInteger colorU32 = color.getAsU32();
        const ColorInteger colors[4] = { colorU32, colorU32, colorU32, colorU32 };

        _patchCurve.Draw(params, texture, z, colors);
    }

    void PatchCurveComponent::drawLink(
        const Vec2d& _p1, const Vec2d& _n1,
        const Vec2d& _p2, const Vec2d& _n2,
        BezierPatch& _bezierPatch, const f32 _alpha /*= 1.f */ )
    {
        ITF_ASSERT_MSG(m_texture.getResource(), "nothing to draw!");
        if (!m_texture.getResource())
        {
            return;
        }

        BezierPatchParams params;

        params.m_texture = (Texture*)m_texture.getResource();

        f32 patchLength, uvLength;
        computeCommonData(_p1, _p2, patchLength, uvLength, params.m_startUV, params.m_endUV);

        if (patchLength <= MTH_EPSILON)
        {
            return;
        }


        params.m_p0 = _p1;
        params.m_p1 = _p1 + _n1 * patchLength * 0.5f;
        params.m_p2 = _p2 + _n2 * patchLength * 0.5f;
        params.m_p3 = _p2;


        params.m_startWidth = params.m_endWidth = getTemplate()->getWidth();
        params.m_z = m_actor->getDepth() + getTemplate()->getZOffset();

        Color color = getColorFactor();
        color.m_a *= _alpha;
        params.m_startColor = params.m_endColor = color;
        params.m_fogColor = getColorFog();

        if (getTemplate()->getTessellationRatio() > 0)
        {
            Bezier4<Vec2d> bezier;
            bezier.m_p0 = params.m_p0;
            bezier.m_p1 = params.m_p1;
            bezier.m_p2 = params.m_p2;
            bezier.m_p3 = params.m_p3;

            f32 length = bezier.getLength(10);
            params.m_vdivToDraw = (u32)(length * getTemplate()->getTessellationRatio());
        }

        _bezierPatch.drawPatch(params);
    }


    Vec2d getAttachPosition( Actor* _actor, AnimLightComponent* _animComponent, const StringID& _boneName )
    {
        if ( _animComponent )
        {
            u32 boneIndex = _animComponent->getBoneIndex(_boneName);
            if (boneIndex != U32_INVALID)
            {
                Vec2d bonePos;
                _animComponent->getBonePos(boneIndex, bonePos);
                return bonePos;
            }
        }

        return _actor->get2DPos();
    }

    Vec2d PatchCurveComponent::getParentAttachPosition()
    {
        return getAttachPosition(m_actor, m_animComponent, getTemplate()->getParentBone());
    }

    Vec2d PatchCurveComponent::getChildAttachPosition( Actor* _child )
    {
        return getAttachPosition(_child, _child->GetComponent<AnimLightComponent>(), getTemplate()->getChildBone());
    }

    Vec2d PatchCurveComponent::getParentAttachNormal()
    {
        return Vec2d::Up.Rotate(m_actor->getAngle());
    }

    Vec2d PatchCurveComponent::getChildAttachNormal( Actor* _child, const Vec2d& _parentPos, const Vec2d& _childPos )
    {
        //return -Vec2d::Up.Rotate(_child->getAngle()) );
        //return (_parentPos - _childPos).normalize();

        Vec2d childDown = -Vec2d::Up.Rotate(_child->getAngle());
        Vec2d dirToChild = (_parentPos - _childPos).normalize();
        return childDown * getTemplate()->getChildOrientationInfluence() + dirToChild * (1 - getTemplate()->getChildOrientationInfluence());
    }



    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PatchCurveComponent_Template)
    BEGIN_SERIALIZATION_CHILD(PatchCurveComponent_Template)
        SERIALIZE_MEMBER("texture", m_textureFile);
        SERIALIZE_MEMBER("width", m_width);
        SERIALIZE_MEMBER("tileLength", m_tileLength);
        SERIALIZE_MEMBER("uvScrollSpeed", m_uvScrollSpeed);
        SERIALIZE_MEMBER("zOffset", m_zOffset);
        SERIALIZE_MEMBER("tessellationLength", m_tessellationLength);
        SERIALIZE_MEMBER("parentBone", m_parentBone);
        SERIALIZE_MEMBER("childBone", m_childBone);
        SERIALIZE_MEMBER("childOrientationInfluence", m_childOrientationInfluence);
        SERIALIZE_MEMBER("attachToChild", m_attachToChild);
    END_SERIALIZATION()

    PatchCurveComponent_Template::PatchCurveComponent_Template()
        : m_width(1.f)
        , m_tileLength(0.f) // 0 = auto
        , m_uvScrollSpeed(0.f)
        , m_zOffset(0.f)
        , m_tessellationLength(0.1f)
        , m_tessellationRatio(0.f)
        , m_parentBone("SnapPoint")
        , m_childBone("SnapPoint")
        , m_childOrientationInfluence(0.5f)
        , m_attachToChild(bfalse)
    {
    }

    void PatchCurveComponent_Template::onTemplateLoaded()
    {
        if (m_tessellationLength > 0)
        {
            m_tessellationRatio = f32_Inv(m_tessellationLength);
        }
    }

};