#ifndef _ITF_DATAERRORCOMPONENT_H_
#define _ITF_DATAERRORCOMPONENT_H_


#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class UITextBox;
    class DataErrorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DataErrorComponent,ActorComponent,541031563);

    public:
        DataErrorComponent();
        ~DataErrorComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        const String&       getTextLabel() {return m_TextLabel;}
        void                setTextLabel(const String& _label) {m_TextLabel = _label;}
        virtual void        Draw();

    private:
        String      m_TextLabel;
/*        f32         m_drawBoxWidth, m_drawBoxHeight;
        UITextBox*  m_textBox;
        f32         m_textSize;

        f32         m_textAlpha;
        f32         m_textRed;
        f32         m_textGreen;
        f32         m_textBlue;

        f32         m_backgroundAlpha;
        f32         m_backgroundRed;
        f32         m_backgroundGreen;
        f32         m_backgroundBlue;*/
    };

}


#endif // _ITF_DATAERRORCOMPONENT_H_

