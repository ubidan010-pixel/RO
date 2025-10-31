
#ifndef _ITF_AFTERFXHANDLER_H_
#define _ITF_AFTERFXHANDLER_H_

#ifndef _ITF_AFTERFX_H_
#include "engine/display/AfterFx.h"
#endif //_ITF_AFTERFX_H_

namespace ITF
{

class AfterFxHandler
{
public:

    AfterFxHandler();
    ~AfterFxHandler();

    void                        init();
    void                        end();
    void                        update( f32 _dt );
    void                        draw();

    void                        startFade( const Color& _from, const Color& _to, f32 _duration, bbool _removeOnFinished );
    bbool                       isFinished();

private:
    
    class IEffect
    {
    public:

        virtual void            update( f32 _dt, AFTERFX* _afterFx ) = 0;
        virtual AFTERFX_Type    getType() const = 0;
        virtual bbool           isFinished() const = 0;
		virtual bbool           isPost2D() const = 0;
    };

    class Fade : public IEffect
    {
    public:

        Fade();
        void                    start( const Color& _from, const Color& _to, f32 _duration, bbool _removeOnFinished );
        void                    update( f32 _dt, AFTERFX* _afterFx );
        bbool                   isFinished() const { return m_fadeCounter >= m_fadeDuration && m_removeOnFinished; }
		bbool                   isPost2D() const { return btrue; }
        AFTERFX_Type            getType() const { return AFX_Fade; }

    private:

        Color                   m_startColor;
        Color                   m_targetColor;
        f32                     m_fadeCounter;
        f32                     m_fadeDuration;
        bbool                   m_removeOnFinished;
    };

    void                        setEffect( IEffect* _effect );

    AFTERFX*                    m_afterFx;
    IEffect*                    m_currentFX;
    Fade                        m_fade;
};

}

#endif // _ITF_AFTERFXHANDLER_H_

