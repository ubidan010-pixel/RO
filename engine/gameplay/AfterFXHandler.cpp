#include "precompiled_engine.h"

#ifndef _ITF_AFTERFXHANDLER_H_
#include "engine/gameplay/AfterFXHandler.h"
#endif // _ITF_AFTERFXHANDLER_H_

namespace ITF
{

AfterFxHandler::AfterFxHandler()
: m_afterFx(NULL)
, m_currentFX(NULL)
{
}

AfterFxHandler::~AfterFxHandler()
{
    end();
}

void AfterFxHandler::init()
{
    ITF_ASSERT(!m_afterFx);

    m_afterFx = newAlloc(mId_GfxAdapter, AFTERFX);
}

void AfterFxHandler::end()
{
    SF_DEL(m_afterFx);
}

void AfterFxHandler::update( f32 _dt )
{
    if ( m_currentFX )
    {
        if ( m_currentFX->isFinished() )
        {
            m_afterFx->setType(AFX_None);
            m_currentFX = NULL;
        }
        else
        {
            m_afterFx->setType(m_currentFX->getType());
            m_currentFX->update(_dt,m_afterFx);
        }
    }
    else
    {
        m_afterFx->setType(AFX_None);
    }
}

bbool AfterFxHandler::isFinished()
{
    return m_currentFX != NULL ? m_currentFX->isFinished() : btrue;
}

void AfterFxHandler::draw()
{
    if ( m_currentFX )
    {
        if (m_afterFx->isPost2D())
		{
			GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, 0, GFX_ITF_AFTERFX, NULL, NULL, NULL, m_afterFx);
		}
		else
		{
			m_afterFx->setZRender(1000.f);
            GFX_ADAPTER->addPrimitiveInZList((void*)m_afterFx, GFX_ITF_AFTERFX, NULL, m_afterFx->getZRender(),0,ObjectRef::InvalidRef);
		}
	}
}

void AfterFxHandler::setEffect( IEffect* _effect )
{
    m_currentFX = _effect;
    m_afterFx->setType(m_currentFX->getType());
	m_afterFx->setPost2D(m_currentFX->isPost2D());
}

void AfterFxHandler::startFade( const Color& _from, const Color& _to, f32 _duration, bbool _removeOnFinished )
{
	m_fade.start(_from,_to,_duration,_removeOnFinished);
    setEffect(&m_fade);
}

//=====================================================================================================

AfterFxHandler::Fade::Fade()
: m_startColor(Color::white())
, m_targetColor(Color::white())
, m_fadeCounter(0.f)
, m_fadeDuration(0.f)
, m_removeOnFinished(bfalse)
{

}

void AfterFxHandler::Fade::start( const Color& _from, const Color& _to, f32 _duration, bbool _removeOnFinished )
{
    m_fadeCounter = 0.f;
    m_fadeDuration = _duration;
    m_startColor = _from;
    m_targetColor = _to;
    m_removeOnFinished = _removeOnFinished;
}

void AfterFxHandler::Fade::update( f32 _dt, AFTERFX* _afterFx )
{
    f32 t = m_fadeCounter / m_fadeDuration;
    Color currentColor = Color::Interpolate(m_startColor,m_targetColor,t);

    _afterFx->setParamcAt(0, currentColor);

    m_fadeCounter += _dt;
}

}