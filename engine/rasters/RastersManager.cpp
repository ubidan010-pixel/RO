#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

//We do not protect this file with include guards, because we want to change have the rastergroup definition
#define ITF_DEFINE_RASTERGROUP_STRINGS
#include "engine/rasters/RasterGroups_Defines.h"

#ifndef _ITF_3DS_DMPGL_DRAWPRIM_H_
#include "adapters/GFXAdapter_3DS_GR/GFXAdapter_3DS_GR.h"
#endif //_ITF_3DS_DMPGL_DRAWPRIM_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

namespace ITF
{
#ifdef RASTER_ENABLED
    void RastersManager::Raster::start()
    {
        m_hit++;
#ifndef ITF_WII
        m_startTime = (f32)SYSTEM_ADAPTER->getTime();
#else
        m_startTime = OSGetTick();
#endif
    }

    void RastersManager::Raster::stop()
    {
#ifndef ITF_WII
        f32 stopTime = (f32)SYSTEM_ADAPTER->getTime();
        m_elapsedTime += stopTime - m_startTime;
#else
        m_elapsedTime += OSDiffTick(OSGetTick(),m_startTime);
#endif
    }

    f32 RastersManager::Raster::getElapsedTime() const
    {
#ifndef ITF_WII
        return m_elapsedTime * 1000.f;
#else
        return ((f32)OSTicksToNanoseconds(m_elapsedTime))/1000000.f;
#endif
    }

    void RastersManager::RasterHistory::getSampleByFrameIndex(u32 _frameIndex, i32 _maxBackSearch, f32 &_sampleValue)
    {
        _sampleValue = 0;
        if (m_sampleCount==0)
        {
            return;
        }
        i32 index = (i32)m_sampleCount-1;
        for (;_maxBackSearch>=0; _maxBackSearch--)
        {
            if (m_sampledFrameIndex[index] == _frameIndex)
            {
                _sampleValue = m_sampledValue[index];
                return;
            }
            index--;
            if (index<0)
                return;
        }
        ITF_ASSERT_MSG(bfalse, "We should have found a sample by searching those frames");
    }
    void RastersManager::RasterHistory::addSample(f32 _sampleValue, u32 _frameIndex)
    {
        if (m_sampleCount>=1)
        {
            //We may add different samples to same frame
            if (m_sampledFrameIndex[m_sampleCount-1] == _frameIndex)
            {
                m_sampledValue[m_sampleCount-1] += _sampleValue;
                return;
            }
        }
        //normal processing : looping buffer
        if (m_sampleCount>=maxSampleCount)
        {
            m_sampleCount = 0;
        }
        m_sampledValue[m_sampleCount] = _sampleValue;
        m_sampledFrameIndex[m_sampleCount] = _frameIndex;
        m_sampleCount++;
    }

    void RastersManager::RasterHistory::dump()
    {
        for (u32 i=0; i<m_sampleCount; i++)
        {
            LOG("[RASTER_HISTORY]: group %d, frame %d  \"%s\" %f ms", m_group, m_sampledFrameIndex[i], m_name.cStr(), (m_sampledValue[i]));
        }
    }

    void RastersManager::timeRasterStart(const RasterID &_rasterID) volatile
    {
        if (!m_bEnable)
            return;

        ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "rasters are only for main thread");

        Raster* ras = find(_rasterID);
        if (ras)
            ras->start();
        else
        {
            Raster newras(_rasterID.m_name, _rasterID.m_group, _rasterID.getColor());
            newras.start();
            ((RasterListType&)m_rasterList).set(_rasterID.getCRC(), newras);
        }
    }

    void RastersManager::timeRasterStop(const RasterID &_rasterID) volatile
    {
        if (!m_bEnable)
            return;

        Raster* ras = find(_rasterID);

        if (ras)
            ras->stop();
    }

    void RastersManager::timeRasterStop(StringID::StringIdValueType _CRC) volatile
    {
        if (!m_bEnable)
            return;

        i32 index = ((RasterListType&)m_rasterList).find(_CRC);
        if (index<0)
        {
            ITF_ASSERT(index>=0);
            return;
        }
        Raster* ras = &((RasterListType&)m_rasterList).getAt((u32)index);

        ras->stop();
    }

    void RastersManager::dump(bbool allGroups)
    {
		String captionText;
		u32 currentFrameIndex = SINGLETONS.m_currentFrame-1;
        LOG("[RASTER_START %s]", allGroups?"ALL":RasterGroup_Names[m_rasterGroupIndex]);
		for (ITF_MAP<u32, RasterHistory>::iterator iter = m_rasterHistoryList.begin();
			iter!=m_rasterHistoryList.end();
			iter++)
		{
			//String captionText;
			RasterHistory &h = iter->second;

            if(!allGroups && !(h.getGroup() & (1<<m_rasterGroupIndex)))
                continue;

			u32 sampleCount = h.getSampleCount();
			if (sampleCount)
			{
                char extraString[512]; *extraString = 0;
				u32 frameIndex = currentFrameIndex;
				f32 sampleValue;
				h.getSampleByFrameIndex(frameIndex, 1, sampleValue);
                BaseObject *pObj = h.m_ObjRef.getObject();
                if(pObj)
                {
                    if(pObj->getObjectType() == BaseObject::eActor)
                    {
                        Actor *pActor = (Actor*)pObj;

                        AnimLightComponent *pAnimLightComp = pActor->GetComponent<AnimLightComponent>();
                        sprintf(extraString, ",REF=0x%X, ACTOR, %u, %s", h.m_ObjRef.getValue(), pActor->m_drawCount, pAnimLightComp?pAnimLightComp->isVisiblePrediction()?"1":"0" :"00");
                    }
                }
                LOG("group %d, %s, %f%s", h.getGroup(), h.m_name.cStr(), sampleValue, extraString);
			}
		}
        LOG("[RASTER_END]");
    }

    void RastersManager::dumpHistory()
    {
        for (ITF_MAP<u32, RasterHistory>::iterator iter = m_rasterHistoryList.begin();
            iter!=m_rasterHistoryList.end();
            iter++)
        {
            iter->second.dump();
        }
    }

    void RastersManager::resetHistory()
    {
        for (ITF_MAP<u32, RasterHistory>::iterator iter = m_rasterHistoryList.begin();
            iter!=m_rasterHistoryList.end();
            iter++)
        {
            iter->second.reset();
        }
    }

    void RastersManager::flushRaster(Raster &_raster, StringID::StringIdValueType _CRC, bbool _reset, ObjectRef _ref)
    {
        if (m_bEnable && m_bEnableHistory)
        {            
            RasterHistory &history = m_rasterHistoryList[_CRC];
            if(!history.m_name.getLen())
            {
                history.m_name = _raster.getName();
                history.m_ObjRef = _ref;
            }
            history.setGroup(_raster.getGroups());
            history.setColor(_raster.getColor());
            history.addSample(_raster.getElapsedTime(), SINGLETONS.m_currentFrame);
        }
        if (_reset)
        {
            _raster.reset();
        }
    }

    void RastersManager::flushDynamicRaster(const RasterID &_rasterID, ObjectRef _ref)
    {
        Raster *r = find(_rasterID);
        if (r) //may not be available if not enabled
        {
            flushRaster(*r, _rasterID.getCRC(), btrue, _ref);
        }
    }

    void RastersManager::rastersReset()
    {
        if (m_bEnable && m_bEnableHistory)
        {
            u32 rasterCount = (u32)m_rasterList.size();
            for (u32 i=0; i<rasterCount; i++)
            {
                Raster &r= m_rasterList.getAt(i);
                flushRaster(r,m_rasterList.getKeyAt(i), bfalse);
            }
        }


        for (u32 i=0; i<m_rasterList.size(); i++)
        {
            m_rasterList.getAt(i).reset();
        }

    }

    void RastersManager::cycleRasterGroup()
    {
        m_rasterGroupIndex++;
        if ((1<<m_rasterGroupIndex) >= RasterGroup_END)
            m_rasterGroupIndex = 0;
        m_rasterGroupChanged = btrue;
        m_lastTimeGroupChanged = SYSTEM_ADAPTER->getTime();
    }

    const Color &RastersManager::getColorFromName(const char *_name, SafeArray<StringID::StringIdValueType> &_colorRegistry)
    {
        static const Color baseColors[]={Color::white(), Color::black(), 
            Color::red(), Color::green(), Color::blue(),
            Color::yellow(), Color::cyan(), Color::magenta(), 
            Color::orange(), Color::violet()};
        const int baseColorCount = sizeof(baseColors)/sizeof(baseColors[0]);
        StringID::StringIdValueType CRC = StringID::StrToCRC(_name);
        i32 index = _colorRegistry.find(CRC);
        if (index<0)
        {
            index = (i32)_colorRegistry.size();
            _colorRegistry.push_back(CRC);
        }
        return baseColors[index%baseColorCount];
    }

    int RastersManager::rasterComparer(const void *p1, const void *p2)
    {
        RasterHistory *h1 = *(RasterHistory**)p1;
        RasterHistory *h2 = *(RasterHistory**)p2;

        u32 sampleCount;

        f32 t1 = 0, t2 = 0;
        u32 currentFrameIndex = SINGLETONS.m_currentFrame-1;
        u32 frameIndex = currentFrameIndex;
        sampleCount = h1->getSampleCount();
        if (sampleCount)
        {
            h1->getSampleByFrameIndex(frameIndex, 1, t1);
        }

        sampleCount = h2->getSampleCount();
        if (sampleCount)
        {
            h2->getSampleByFrameIndex(frameIndex, 1, t2);
        }
        if (t1<t2) return 1;
        if (t1>t2) return -1;
        return 0;

    }

    void RastersManager::rastersDraw()
    {       
        if (!GFX_ADAPTER->isShowRasters())
        {
            return;
        }
        static SafeArray<RasterHistory*> rasterHistoryList;        
        rasterHistoryList.clear();
        static u32 frameSerialNumber = 0;
        static const f64 captionChangeRythm = 1.f;
        static const u32 subdivisionCount = 5;
        static const u32 subdiv_dashStride = 2;
        u32 currentFrameIndex = SINGLETONS.m_currentFrame-1;
        frameSerialNumber++;
        f64 currentTime(SYSTEM_ADAPTER->getTime());
        bbool captionChangesThisTime = bfalse;
        if ((currentTime-m_lastTimeCaptionChanged)>captionChangeRythm)
        {
            captionChangesThisTime = btrue;
            m_lastTimeCaptionChanged = currentTime;
            m_currentCaptionChangeIndex++;
            m_currentCaptionText.clear();
        }

        if (!m_renderTarget)
        {
            m_renderTarget = GFX_ADAPTER->getNewRenderTarget();
            u32 width = RasterHistory::maxSampleCount;
            width = std::min(width, GFX_ADAPTER->getScreenWidth());
            u32 height = m_renderTargetHeight;
            m_renderTarget->create((f32)width, (f32)height);
            GFX_ADAPTER->clearRenderTarget(m_renderTarget, Color::zero().getAsU32());
        }

        if (m_rasterGroupChanged || m_referenceTimeChanged)
        {
            GFX_ADAPTER->clearRenderTarget(m_renderTarget, Color::zero().getAsU32());
            m_rasterGroupChanged = bfalse;
            m_referenceTimeChanged = bfalse;
        }
        u32 width = m_renderTarget->getTexture()->getSizeX();
        u32 height = m_renderTarget->getTexture()->getSizeY();
        u32 currentX = frameSerialNumber % width;

        GFX_RECT Viewport;
        Viewport.left = 0;
        Viewport.top = 0;
        Viewport.right = m_renderTarget->m_area[2];
        Viewport.bottom = m_renderTarget->m_area[3];
        GFX_ADAPTER->setupViewport(&Viewport);

        GFX_ADAPTER->setAlphaBlend(GFX_BLEND_COPY);
        GFX_ADAPTER->setAlphaRef(1);
        GFX_ADAPTER->setAlphaTest(btrue);
        GFX_ADAPTER->startImpostorRendering(m_renderTarget, 0.f, 0.f, f32(width), 1.f, bfalse);
        //GFX_ADAPTER->setOrthoView(0.f, (f32) width, 0.f, 1.f);
        f32 invFrameTime = 1.f/m_referenceTime;
        f32 maxTimeScale = 1.2f; //if >1, we have additional room for times greater than the frame time
        f32 timeToHeightFactor = invFrameTime/maxTimeScale;
        u32 significantRasterCount = 0;

        //find min frame index. Also get significant value
        for (ITF_MAP<u32, RasterHistory>::iterator iter = m_rasterHistoryList.begin();
            iter!=m_rasterHistoryList.end();
            iter++)
        {
            RasterHistory &h = iter->second;
            if (!(h.getGroup() & (1<<m_rasterGroupIndex)))
            {
                continue;
            }
            significantRasterCount++;
        }

        bbool hasError = bfalse;
        f32 cumulatedHeight = 0;
        u32 significantRasterIndex=0;
        
        for (ITF_MAP<u32, RasterHistory>::iterator iter = m_rasterHistoryList.begin();
            iter!=m_rasterHistoryList.end();
            iter++)
        {
            RasterHistory &h = iter->second;
            if (!(h.getGroup() & (1<<m_rasterGroupIndex)))
            {
                continue;
            }
            u32 sampleCount = h.getSampleCount();
            if (sampleCount)
            {
                u32 frameIndex = currentFrameIndex;
                f32 sampleValue;
                h.getSampleByFrameIndex(frameIndex, 1, sampleValue);
                {
                    rasterHistoryList.push_back(&h);

                    u32 dt = currentX;

                    f32 scaledValue = (sampleValue*timeToHeightFactor)*0.001f;
                    const Color &c = h.getColor();

                    f32 halfWidth = 0.5f;
                    f32 halfHeight = scaledValue * 0.5f;
                    GFX_ADAPTER->drawBox(Vec2d((f32)dt+halfWidth, cumulatedHeight+halfHeight),
                        0,
                        Vec2d(halfWidth, halfHeight),
                        c.getRed(),c.getGreen(),c.getBlue(),
                        0.5f,
                        btrue);
                    cumulatedHeight += scaledValue;

                    if (captionChangesThisTime)
                    {
                        if ((m_currentCaptionChangeIndex%significantRasterCount)==significantRasterIndex)
                        {
                            m_currentCaptionColor = c;
                            float scaledSampleValue = sampleValue;
                            m_currentCaptionText.setTextFormat("%s (%.2f ms)", h.m_name.cStr(), scaledSampleValue);
                            /*
                            if (scaledSampleValue<0.1f)
                                m_lastTimeCaptionChanged = 0; //change ASAP
                                */
                        }
                    }
                    significantRasterIndex++;
                }
            }
            else
            {
                //do nothing
            }
        }

        if (!hasError)
        {
            //Display remaining of column
            f32 columnHeight = m_referenceTime*maxTimeScale*timeToHeightFactor;
            u32 dt = currentX;
            if (columnHeight>cumulatedHeight)
            {
                f32 remainingHeight = columnHeight-cumulatedHeight;
                GFX_ADAPTER->drawBox(Vec2d((f32)dt+0.5f, cumulatedHeight+remainingHeight*0.5f),
                    0,
                    Vec2d(0.5f, remainingHeight*0.5f),
                    0.5f,0.5f,0.5f,
                    0.5f,
                    btrue);
            }

            {
                u32 dt;
                //display subdivisions
                dt = currentX;
                
                if (dt%subdiv_dashStride == 0)
                {
                    f32 pixelHeight = 0.5f/height;
                    f32 subdivDeltaValue = m_referenceTime/(f32)subdivisionCount*timeToHeightFactor;
                    f32 currentValue = subdivDeltaValue;
                    for (u32 subdivIndex=1; subdivIndex<subdivisionCount; subdivIndex++)
                    {
                        
                        GFX_ADAPTER->drawBox(Vec2d((f32)dt+0.5f, currentValue),
                            0,
                            Vec2d(0.5f, pixelHeight),
                            0.f,0.f,0.f,
                            0.5f,
                            btrue);
                        currentValue += subdivDeltaValue;
                    }

                }

                //display caret
                dt = currentX+1;
                dt = dt % width;
                GFX_ADAPTER->drawBox(Vec2d((f32)dt+0.5f, (columnHeight)*0.5f),
                    0,
                    Vec2d(0.5f, columnHeight*0.5f),
                    0.f,0.f,0.f,
                    0.5f,
                    btrue);
            }            
        }

        //Display line for frame time
        f32 frameLineY  = m_referenceTime*timeToHeightFactor;
        GFX_ADAPTER->drawBox(Vec2d((f32)width*0.5f, frameLineY),
            0,
            Vec2d((f32)width*0.5f, 1.f/height),
            0.f,0.f,0.f,
            0.5f,
            btrue);
        if (hasError)
        {
            ITF_ASSERT(!hasError);
            GFX_ADAPTER->clearRenderTarget(m_renderTarget, Color::red().getAsU32());
        }
        GFX_ADAPTER->stopImpostorRendering(m_renderTarget);

#ifdef ITF_WII
        /// restore 3d View.
        CAMERA->apply();
#endif // ITF_WII

        GFX_ADAPTER->init2DRender();
        GFX_ADAPTER->setAlphaBlend(GFX_BLEND_ALPHA);
        //grey semitransparent background for empty history
        GFX_ADAPTER->drawScreenQuad(0,0, (f32)width, (f32)height, 0, Color(0.25f,1.f,1.f,1.f).getAsU32(), btrue);
        //history
        GFX_ADAPTER->setAlphaBlend(GFX_BLEND_COPY);
        GFX_ADAPTER->SetTexture(0, m_renderTarget->getTexture());
        GFX_ADAPTER->drawScreenQuad(0,0, (f32)width, (f32)height, 0, COLOR_WHITE, bfalse);
        GFX_ADAPTER->SetTexture(0, NULL);
        //display captions

        f32 captionY = (f32)height;
        f32 x = (f32)width + 10.f;
        if ((currentTime-m_lastTimeGroupChanged)<2.)
        {
            captionY = (f32)height;
            GFX_ADAPTER->drawDBGText(RasterGroup_Names[m_rasterGroupIndex], x, GFX_ADAPTER->getScreenHeight()-captionY, 1.f, 1.f, 1.f);
        }
        else if (!m_currentCaptionText.isEmpty())
        {

            f32 squareSize=16;

            GFX_ADAPTER->drawScreenQuad(x,captionY, squareSize, -squareSize, 0, m_currentCaptionColor.getAsU32(), btrue);            
            GFX_ADAPTER->drawDBGText(m_currentCaptionText, x+ squareSize + 8, GFX_ADAPTER->getScreenHeight()-captionY, 1.f, 1.f, 1.f);

            u32 listSize = std::min((u32)rasterHistoryList.size(), 6U);

            if (captionChangesThisTime)
            {
                m_rasterListText.resize(0);
                String textInList;
                if (rasterHistoryList.size())
                {
                    qsort(&rasterHistoryList[0], rasterHistoryList.size(), sizeof(rasterHistoryList[0]), rasterComparer);
                }
                for (u32 i=0; i<listSize; i++)
                {
                    RasterHistory *h = rasterHistoryList[i];
                    u32 sampleCount = h->getSampleCount();
                    f32 val=0;
                    if (sampleCount)
                    {
                        h->getSampleByFrameIndex(currentFrameIndex, 1, val);
                    }
                    textInList.setTextFormat("%s : %.2f ms", h->m_name.cStr(), val);
                    m_rasterListText.push_back(textInList);
                }
            }
            u32 textCount = m_rasterListText.size();
            captionY -= 4;
            for (u32 i=0; i<textCount; i++)
            {
                captionY -= 12;
                GFX_ADAPTER->drawDBGText(m_rasterListText[i], x+ squareSize + 8, GFX_ADAPTER->getScreenHeight()-captionY, 1.f, 1.f, 1.f);
            }

        }

        //back to normal state
        GFX_ADAPTER->setAlphaTest(bfalse);
        GFX_ADAPTER->setAlphaBlend(GFX_BLEND_COPY);
        rasterHistoryList.clear();
    }
	
	void RastersManager::rastersMinimalDraw(f32 minVal)
	{
		if (!GFX_ADAPTER->isShowRasters())
		{
			return;
		}
		String captionText;
		u32 currentFrameIndex = SINGLETONS.m_currentFrame-1;
		f32 y = 20;
		f32 x = 5;
#ifdef ITF_WII
        x = 50;
#endif // ITF_WII

#ifdef USE_GR
		GFXAdapter_3DS_GR* adapter = static_cast<GFXAdapter_3DS_GR*>(GFX_ADAPTER);
		adapter->DrawString(x, y, RasterGroup_Names[m_rasterGroupIndex]);
#else
        captionText.setTextFormat("%s", RasterGroup_Names[m_rasterGroupIndex]);
		GFX_ADAPTER->drawDBGText(captionText, x, y, 1.f, 1.f, 1.f);
#endif
		y += 13.0f;

		for (ITF_MAP<u32, RasterHistory>::iterator iter = m_rasterHistoryList.begin();
			iter!=m_rasterHistoryList.end();
			iter++)
		{
			//String captionText;
			RasterHistory &h = iter->second;
			if (!(h.getGroup() & (1<<m_rasterGroupIndex)))
			{
				continue;
			}
			u32 sampleCount = h.getSampleCount();
			if (sampleCount)
			{
				u32 frameIndex = currentFrameIndex;
				f32 sampleValue;
				h.getSampleByFrameIndex(frameIndex, 1, sampleValue);
				if(sampleValue < minVal)
                    continue;
#ifdef USE_GR
				GFXAdapter_3DS_GR* adapter = static_cast<GFXAdapter_3DS_GR*>(GFX_ADAPTER);
				adapter->DrawString(x, y, h.m_name.cStr());
				adapter->DrawString(x + 150.0f, y, "(%.1f ms)", sampleValue);
#else
                captionText.setTextFormat("%s (%.2f ms)", h.m_name.cStr(), sampleValue);
				GFX_ADAPTER->drawDBGText(captionText, x, y, 1.f, 1.f, 1.f);
#endif
				y += 13.0f;
			}
		}
	}

    RastersManager::Raster* RastersManager::find(const RasterID &_rasterID) volatile
    {
        i32 index = ((RasterListType&)m_rasterList).find(_rasterID.m_CRC);
        if (index<0)
        {
            return NULL;
        }
        return &((RasterListType&)m_rasterList).getAt(index);
    }


    timeRasterScope::timeRasterScope( const RasterID &_id ) 
    {
        m_CRC = _id.getCRC();
        RastersManager::get().timeRasterStart(_id);
    }

    timeRasterScope::~timeRasterScope()
    {
        RastersManager::get().timeRasterStop(m_CRC);
    }
#endif //RASTER_ENABLED
} //namespace ITF
