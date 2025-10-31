#ifndef _ITF_LOADINGSCREEN_H_
#define _ITF_LOADINGSCREEN_H_

namespace ITF
{


    class Loading_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(Loading_Template, TemplateObj,675203100);
        DECLARE_SERIALIZE()

    public:
        Loading_Template()
            : Super()
        {
            m_cameraz = 0.0f;
        }
        Loading_Template(const Path& _path)
            : Super(_path)
        {
             m_cameraz = 0.0f;
        }

        const ITF_INLINE f32 getCameraZ() const         {return m_cameraz;}
        const ITF_INLINE String& getFilename() const    {return m_filename;}

    private:


        String  m_filename;
        f32     m_cameraz;
    };


    class LoadingScreen
    {
    public:

        LoadingScreen():m_bVisible(bfalse),m_pWorld(NULL),m_enabled(bfalse),m_LoadingTemplate(NULL) {};
        virtual ~LoadingScreen();

        bbool isEnabled() const { return m_enabled; }
        void enable() {m_enabled = btrue;}
        void show();
        void hide();
        void load( const Path& loadingScreenPath );
        void update(f32 _elapsed);
        void draw();
        void destroy();
        bbool isVisible() const { return m_bVisible && m_enabled; }

        ObjectRef getRef() const ;
    protected:

        World*  m_pWorld;
        Path    m_loadingScreenPath;
        bbool   m_bVisible;
        bbool   m_enabled;

        const Loading_Template* m_LoadingTemplate;
        
    };


} // namespace ITF

#endif // _ITF_LOADINGSCREEN_H_
