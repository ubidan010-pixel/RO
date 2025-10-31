#ifndef _ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_
#define _ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_

#ifndef _ITF_CHARACTERDBUGGERCOMPONENT_H_
#include "gameplay/components/Common/CharacterDebuggerComponent.h"
#endif //_ITF_CHARACTERDBUGGERCOMPONENT_H_

namespace ITF
{
    class Ray_CharacterDebuggerComponent : public CharacterDebuggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CharacterDebuggerComponent, CharacterDebuggerComponent,1706648044)

    public:
        DECLARE_SERIALIZE()

        Ray_CharacterDebuggerComponent();
        virtual ~Ray_CharacterDebuggerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        Update( f32 _deltaTime );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        editor_draw();
#endif // ITF_SUPPORT_EDITOR

    protected:
        ObjectRef           m_hangingPolyline;
        u32                 m_hangingEdge;
        f32                 m_currentEfficiency;

    };

    //---------------------------------------------------------------------------------------------------

    class Ray_CharacterDebuggerComponent_Template : public CharacterDebuggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CharacterDebuggerComponent_Template,CharacterDebuggerComponent_Template,2645970871);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CharacterDebuggerComponent);

    public:

        Ray_CharacterDebuggerComponent_Template() {}
        ~Ray_CharacterDebuggerComponent_Template() {}

    private:

    };
}

#endif //_ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_