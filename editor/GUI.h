#ifndef _ITF_EDITOR_GUI_TEXTINPUT_H_
#define _ITF_EDITOR_GUI_TEXTINPUT_H_

#if defined(ITF_WINDOWS)

namespace ITF
{

    class GUI_TextInput
    {
    public:
        static bbool inputText(const String& _title, const String& _message, String& _text);
    };

} // namespace ITF

#endif // ITF_WINDOWS

#endif // _ITF_EDITOR_GUI_TEXTINPUT_H_
