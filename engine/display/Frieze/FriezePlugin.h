#ifndef _ITF_FRIEZEPLUGIN_H_
#define _ITF_FRIEZEPLUGIN_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///A plugin for frieze
    class FriezePlugin
    {
    public:
        FriezePlugin(Frise *_frieze) : m_frieze(_frieze) {}
        virtual ~FriezePlugin() {}

        virtual void buildFrieze_static(Frise::MeshStatic_VertexList& _vertexTempList, Frise::MeshStatic_IndexList* _indexTempList) = 0;
        virtual void update(f32 _dt) = 0;
        virtual void onEvent(Event *_event) = 0;

    protected:
        Frise *m_frieze;
    };
}
#endif
