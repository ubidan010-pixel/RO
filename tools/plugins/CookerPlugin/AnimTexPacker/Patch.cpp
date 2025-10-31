#include "core/enginecommon.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_ANIMTEXPACKER_PATCH_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/Patch.h"
#endif

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONDRAW_H_
#include    "engine/animation/AnimationDraw.h"
#endif // _ITF_ANIMATIONDRAW_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        class SimplePatch32Data
        {
        public:
            static const int maxVertexCount=MAX_VERTEX_BY_MESH;
            SimplePatch32Data() {vertexCount=0;indexCount=0;}

            VertexPT vertexListPT[maxVertexCount];
            u32 vertexCount;
            u32 indexCount;
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::setFinalPos(const BoxI &_finalBox, bbool _isRotated, const transform2d &_uvTransform)
        {
            m_finalPos = _finalBox; 
            m_isRotated = _isRotated;
            m_uvlocalToRotatedLocalTransform = _uvTransform;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::setFinalPosFromAlias()
        {
            ITF_ASSERT(getSourceOfAlias());
            m_finalPos = m_sourceOfAlias->m_finalPos;
            m_isRotated = m_sourceOfAlias->m_isRotated;
            m_uvlocalToRotatedLocalTransform = m_sourceOfAlias->m_uvlocalToRotatedLocalTransform;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        int Patch::computePlacementHeuristic(PackedTextureArea *_area, BoxI &b)
        {
            int x,y;
            int count=0;

            //upper edge
            y=b.miny-1;
            for (x = b.minx - 1; x <= b.maxx + 1; x++)
                if (_area->testBorder(x, y))
                    count++;
            //lower edge
            y=b.maxy+1;
            for (x = b.minx - 1; x <= b.maxx + 1; x++)
                if (_area->testBorder(x, y))
                    count++;
            //left edge
            x = b.minx - 1;
            for (y = b.miny; y <= b.maxy; y++)
                if (_area->testBorder(x, y))
                    count++;
            //right edge
            x = b.maxx + 1;
            for (y = b.miny; y <= b.maxy; y++)
                if (_area->testBorder(x, y))
                    count++;
            return count;            
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool Patch::Place(PackedTextureArea *_area, BoxI &b, bbool &_rotated, transform2d &_subImageRotation)
        {
            b = BoxI(0, 0, m_UVBox.maxx-m_UVBox.minx, m_UVBox.maxy-m_UVBox.miny);
            int bestHeuristic = -1;
            int bestx=0, besty=0;
            int bestPhase = 0;
            int w = getSubImageWidth();
            int h = getSubImageHeight();
            ITF_SET<int> placeableX, placeableY;
            ITF_SET<int>::const_iterator xiter, yiter;

            ITF_ASSERT(!getSourceOfAlias());

            //regular orientation
            _area->computeRemarkablePlacesForBox(placeableX, placeableY, w,h);
            for (yiter=placeableY.begin(); yiter!=placeableY.end(); yiter++)
                for (xiter = placeableX.begin(); xiter!=placeableX.end();xiter++)
                {
                    int x = *xiter;
                    int y = *yiter;
                    b.minx = x;
                    b.miny = y;
                    b.maxx = x + w - 1;
                    b.maxy = y + h - 1;
                    int heur = computePlacementHeuristic(_area, b);
                    if (heur > bestHeuristic)
                    {
                        if (_area->isAreaFree(x, y, w, h))
                        {

                            bestHeuristic = heur;
                            bestx = x;
                            besty = y;
                            bestPhase = 0;
                        }
                    }
                }
#if 0
                //rotated orientation
                _area->computeRemarkablePlacesForBox(placeableX, placeableY, h,w);
                for (yiter=placeableY.begin(); yiter!=placeableY.end(); yiter++)
                    for (xiter = placeableX.begin(); xiter!=placeableX.end();xiter++)
                    {
                        int x = *xiter;
                        int y = *yiter;

                        b.minx = x;
                        b.miny = y;
                        b.maxx = x + h - 1;
                        b.maxy = y + w - 1;
                        int heur = computePlacementHeuristic(_area, b);
                        if (heur > bestHeuristic)
                        {
                            if (_area->isAreaFree(x, y, h, w))
                            {

                                bestHeuristic = heur;
                                bestx = x;
                                besty = y;
                                bestPhase = 1;
                            }
                        }
                    }
#endif
                    //
                    if (bestPhase == 0)
                    {
                        b.minx = bestx;
                        b.miny = besty;
                        b.maxx = bestx + w - 1;
                        b.maxy = besty + h - 1;
                        _rotated = bfalse;
                        _subImageRotation.setIdentity();
                    }
                    else if (bestPhase == 1)
                    {
                        b.minx = bestx;
                        b.miny = besty;
                        b.maxx = bestx + h - 1;
                        b.maxy = besty + w - 1;
                        _rotated = btrue;
                        Vec2d nextZero(0,(f32)m_subImageHeight-1);
                        _subImageRotation.m_rot.set(0.f,1.f,-1.f,0.f);
                        _subImageRotation.m_pos = -mulMatrix2d(_subImageRotation.m_rot, nextZero);                        
                    }
                    else
                    {
                        ITF_ASSERT(bfalse);
                    }

                    if (bestHeuristic < 0)
                        return bfalse;
                    _area->fillBox(b);
                    return btrue;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////        
        bbool Patch::isEqual(const Patch *_other)
        {
            if (m_imagePath!=_other->m_imagePath)
                return bfalse;
            if (m_UVBox != _other->m_UVBox)
                return bfalse;

            return btrue;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::initialize(const StringID &_templateID, AnimTemplate *_animTemplate, ChainedBones &_chainedBones, nv::Image &_image, const String8 &_imagePath, AnimSkeleton *_skeleton)
        {
            computeUVAABB(_animTemplate, _chainedBones, _image, _imagePath);

            u32 animBoneCount=_chainedBones.m_AnimBones.size();
            m_imagePath = _imagePath;
            m_templateID = _templateID;
            m_boneIndices.resize(0);
            m_boneIDs.resize(0);
            m_boneIDs.reserve(animBoneCount);
            m_boneIndices.reserve(animBoneCount);
            m_initialImageWidth = (int)_image.width();
            m_initialImageHeight = (int)_image.height();
            m_animTemplate = _animTemplate;
            m_skeleton = _skeleton;

            for (u32 i=0; i<animBoneCount; i++)
            {
                AnimBone *bone = _chainedBones.m_AnimBones[i];
                u32 boneIndex = (u32)_skeleton->getBoneIndex(bone->m_Name);
                m_boneIndices.push_back(boneIndex);
                m_boneIDs.push_back(bone->m_Name);
            }
            extractPixels(_image);
        }


        void Patch::extractPixels(nv::Image &_image)
        {
            ITF_ASSERT(m_pixels==NULL );
            m_subImageWidth = getUVBoundingWidth();
            m_subImageHeight = getUVBoundingHeight();
            m_pixels = newAlloc(mId_Temporary,nv::Color32[m_subImageHeight*m_subImageWidth]);
            nv::Color32 emptyColor(0,0,0,0);
            nv::Color32 *dest = m_pixels;
            for (int y=m_UVBox.miny; y<=m_UVBox.maxy; y++)
            {
                const nv::Color32 *c = _image.scanline(y);
                bbool isYInFrame=(y>=0 && y<m_initialImageHeight);
                for (int x=m_UVBox.minx; x<=m_UVBox.maxx; x++)
                {
                    if (isYInFrame && x>=0 && x<m_initialImageWidth)
                        *dest++ = c[x];
                    else
                        *dest++ = emptyColor;
                }
            }
            ITF_ASSERT(dest==m_pixels+m_subImageWidth*m_subImageHeight);
        }


        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::debugDump()
        {
            nv::Image newImage;
            newImage.setFormat(nv::Image::Format_ARGB);
            newImage.allocate(getSubImageWidth(), getSubImageHeight());
            memcpy(newImage.pixels(), m_pixels, getSubImageWidth()*getSubImageHeight()*sizeof(nv::Color32));
            bbool saved = nv::ImageIO::save("d:\\tmp\\atlas\\__dump.tga", &newImage);
            ITF_ASSERT(saved);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::blitToFinalPos(nv::Image &_destImage)
        {
            ITF_ASSERT(!getSourceOfAlias());
            int finalW=m_finalPos.maxx-m_finalPos.minx+1;
            int finalH=m_finalPos.maxy-m_finalPos.miny+1;

            int destWidth = _destImage.width();
            int destHeight = _destImage.height();

            nv::Color32 *dest = _destImage.pixels();

            if (m_isRotated)
            {
                for (int y=0; y<m_subImageHeight; y++)
                    for (int x=0; x<m_subImageWidth; x++)
                    {
                        nv::Color32 c = getPixel_WithBorder(x,y);

                        int newX = m_subImageHeight-1-y + m_finalPos.minx;
                        int newY = x + m_finalPos.miny;
                        int offset = newX + newY*destWidth;
                        ITF_ASSERT(offset<destWidth*destHeight);
                        dest[offset] = c;
                    }
            }
            else
            {
                for (int y=0; y<finalH; y++)
                    for (int x=0; x<finalW; x++)
                    {
                        nv::Color32 c = getPixel_WithBorder(x,y);
                        int offset = x+m_finalPos.minx + (y+m_finalPos.miny)*destWidth;
                        ITF_ASSERT(offset<destWidth*destHeight);
                        dest[offset] = c;
                    }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::growPatchMaxSize(const BoneMaxSize& _toAdd )
        {            
            m_boneMaxSize.grow(_toAdd.m_worldSize.getMax(), _toAdd.m_uvSize.getMax());
        }

        const BoneMaxSize &Patch::getPatchMaxSize() const
        {
            ITF_ASSERT(!getSourceOfAlias());
            return m_boneMaxSize;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::resizeTexture(int _newSize)
        {
            ITF_ASSERT(!getSourceOfAlias());
            int w = getSubImageWidth();
            int h = getSubImageHeight();
            if (w>=h)
            {
                float ratio = h/(float)w;
                w = _newSize;
                h = (int) (_newSize*ratio);
                if (h<1)
                    h = 1;
            }
            else
            {
                float ratio = w/(float)h;
                h = _newSize;
                w = (int) (_newSize*ratio);
                if (w<1)
                    w = 1;
            }
            int srcWidth = getSubImageWidth();
            int srcHeight = getSubImageHeight();

            m_imageStretchTransform.m_rot.set(w/(f32)srcWidth, 0,0, h/(f32)srcHeight);
            m_imageStretchTransform.m_pos.set(0,0);

            ITF_ASSERT(w<srcWidth && h<srcHeight);

            nv::Color32 *newPixels = new nv::Color32[w*h];
            for (int y=0; y<h; y++)
                for (int x=0; x<w; x++)
                {
                    f32 xInSource_Start = x*(srcWidth/(f32)w);
                    f32 xInSource_End = (x+1)*(srcWidth/(f32)w);
                    xInSource_End = std::min(xInSource_End, (f32)srcWidth);
                    f32 yInSource_Start = y*(srcHeight/(f32)h);
                    f32 yInSource_End = (y+1)*(srcHeight/(f32)h);
                    yInSource_End = std::min(yInSource_End, (f32)srcHeight);
                    int pixelCount = 0;
                    int pixelR=0, pixelG=0, pixelB=0, pixelA=0;

                    for (float subY=yInSource_Start; subY<yInSource_End; subY = floorf(subY)+1.f)
                    {
                        for (float subX=xInSource_Start; subX<xInSource_End; subX = floorf(subX)+1.f)
                        {
                            nv::Color32 c = samplePixel_Linear(subX, subY);
                            pixelR += c.r;
                            pixelG += c.g;
                            pixelB += c.b;
                            pixelA += c.a;
                            pixelCount++;
                        }
                    }

                    nv::Color32 mean(0,0,0,0);
                    ITF_ASSERT(pixelCount!=0);
                    if (pixelCount!=0)
                    {
                        mean.r = (u8)(pixelR/pixelCount);
                        mean.g = (u8)(pixelG/pixelCount);
                        mean.b = (u8)(pixelB/pixelCount);
                        mean.a = (u8)(pixelA/pixelCount);
                    }

                    newPixels[x+y*w] = mean;

                }
                //
                delete [] m_pixels;
                m_pixels = newPixels;
                m_subImageWidth = w;
                m_subImageHeight = h;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        nv::Color32 Patch::samplePixel_Linear(f32 _x, f32 _y)
        {
            f32 xInSource = _x;
            f32 yInSource = _y;
            double dummyDouble;
            int x0 = (int)xInSource;
            int srcWidth = (int)getSubImageWidth();
            int srcHeight = (int)getSubImageHeight();
            int x1 = std::min(srcWidth-1, x0+1);
            int y0 = (int)yInSource;
            int y1 = std::min(srcHeight-1, y0+1);
            f32 fracX = (float)modf(xInSource, &dummyDouble);
            f32 fracY = (float)modf(yInSource, &dummyDouble);
            f32 invFracX = 1-fracX;
            f32 invFracY = 1-fracY;
            nv::Color32 p00 = getPixel(x0,y0);
            nv::Color32 p10 = getPixel(x1,y0);
            nv::Color32 p01 = getPixel(x0,y1);
            nv::Color32 p11 = getPixel(x1,y1);
            float w00 = invFracX*invFracY;
            float w10 = fracX*invFracY;
            float w01 = invFracX*fracY;
            float w11 = fracX*fracY;

            int r = (int)(p00.r*w00 + p10.r*w10 + p01.r*w01 + p11.r*w11);
            int g = (int)(p00.g*w00 + p10.g*w10 + p01.g*w01 + p11.g*w11);
            int b = (int)(p00.b*w00 + p10.b*w10 + p01.b*w01 + p11.b*w11);
            int a = (int)(p00.a*w00 + p10.a*w10 + p01.a*w01 + p11.a*w11);

            return nv::Color32((u8)r,(u8)g,(u8)b,(u8)a);
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        void Patch::modifyUVs(int mapWidth, int mapHeight, ITF_SET<AnimPatchPoint*> &_alreadyProcessedPoints)
        {            

            //Compute, for UV space : OriginalUV->truncatedInteger->rotatedInteger->placedInteger->final
            transform2d originalToTruncated, truncatedRotatedToStretched, truncatedToRotated, rotatedToPlaced, placedToFinal;

            originalToTruncated.m_rot.set((f32)m_initialImageWidth, 0, 0, (f32)m_initialImageWidth);
            originalToTruncated.m_pos = - Vec2d((f32)m_UVBox.minx, (f32)m_UVBox.miny);

            Patch *transformSource = getSourceOfAlias();
            if (!transformSource)
                transformSource = this;
            truncatedToRotated = transformSource->m_uvlocalToRotatedLocalTransform;
            truncatedRotatedToStretched = transformSource->m_imageStretchTransform;

            rotatedToPlaced.m_rot.setIdentity();
            rotatedToPlaced.m_pos = Vec2d((f32)m_finalPos.minx+0.5f, (f32)m_finalPos.miny+0.5f);

            placedToFinal.m_rot.set(1.f/mapWidth, 0, 0, 1.f/mapWidth);
            placedToFinal.m_pos.set(0,0);

            transform2d uvFinalTransform = placedToFinal*rotatedToPlaced*truncatedRotatedToStretched*truncatedToRotated*originalToTruncated;

            //we compute an uniform tranform, which doesnt take whole texture shapes, because rectangular textures can make UV squashed
            transform2d uvVectorTranformUniform = rotatedToPlaced*truncatedRotatedToStretched*truncatedToRotated;

            //We modify Pos, PosEnd for each bone seed (hierarchy start). Seed is at index 0 for m_boneIndices
            for (u32 boneIndexInList=0; boneIndexInList<m_boneIDs.size(); boneIndexInList++)
            {
                i32 boneIDIndex = m_animTemplate->m_NameToBone.find(m_boneIDs[boneIndexInList]);
                i32 boneIndex = -1;
                if (boneIDIndex>=0)
                    boneIndex = m_animTemplate->m_NameToBone.getAt(boneIDIndex);
                ITF_ASSERT(boneIndex>=0 && boneIndex<(i32)m_animTemplate->m_BonesCurFrameDyn.size());
                AnimBoneDyn *seedBone = &m_animTemplate->m_BonesCurFrameDyn[boneIndex];
                ITF_ASSERT(seedBone);

                //f32 localAngleRad = seedBone->m_AngleLocal;
                f32 localAngleRad = seedBone->m_Angle;
                Vec2d boneDirection(cosf(localAngleRad), -sinf(localAngleRad)); //we negate y because the rotation is actually with y up (world space)

                if (boneIndexInList==0)
                {
                    seedBone->m_PosEnd = mulTransform2d(uvFinalTransform, seedBone->m_Pos + seedBone->m_Length*boneDirection);
                    seedBone->m_Pos    = mulTransform2d(uvFinalTransform, seedBone->m_Pos);
                    seedBone->m_Angle  = localAngleRad;
                }

                //the following code may have problems with non-square textures or rotation. we should use X ratio instead
                Vec2d boneLenVec(seedBone->m_Length, 0);
                boneLenVec = mulMatrix2d(uvFinalTransform.m_rot, boneLenVec);
                seedBone->m_Length = boneLenVec.m_x;

            }

            //each AnimTemplate is broken in several Patch,
            //so we need to iterate each AnimPatch in it to check it's connected to a bone related to the Patch
            //If so, we can modify the points in the AnimPatch
            for (u32 patchIndex=0; patchIndex<m_animTemplate->m_Patch.size(); patchIndex++)
            {                
                AnimPatch *patch = &m_animTemplate->m_Patch[patchIndex];
                if (patch->m_nbPatchPoints!=0)
                {
                    AnimBone *boneToFind = patch->m_AnimPatchPoints[0]->m_mainRefs.m_BonePtr;
                    u32 boneIndex=0;
                    ITF_ASSERT(boneToFind);
                    StringID idToFind = boneToFind->m_Name;

                    //In the bone in current patch list?
                    bbool isFound = bfalse;
                    for (boneIndex=0; boneIndex<m_boneIDs.size(); boneIndex++)
                    {
                        if (idToFind==m_boneIDs[boneIndex])
                        {
                            isFound = btrue;


                            break;
                        }
                    }
                    //if we are in right AnimPatch
                    if (isFound)
                    {   
                        //
                        //modify points
                        for (u32 pointIndex=0; pointIndex<patch->m_nbPatchPoints; pointIndex++)
                        {
                            AnimPatchPoint *p = patch->m_AnimPatchPoints[pointIndex];
                            if (std::find(_alreadyProcessedPoints.begin(), _alreadyProcessedPoints.end(), p)!=_alreadyProcessedPoints.end())
                                continue;
                            _alreadyProcessedPoints.insert(p);

                            p->m_PosUV = mulTransform2d(uvFinalTransform, p->m_PosUV);
                            p->m_NormaleUV = mulMatrix2d(uvVectorTranformUniform.m_rot, p->m_NormaleUV);
                            p->m_NormaleUV.normalize();

                            //the following code may have problems with non-square textures or rotation. we should use Y ratio instead
                            Vec2d boneLenVec(0, p->m_mainRefs.m_Pos.m_y);
                            boneLenVec = mulMatrix2d(uvFinalTransform.m_rot, boneLenVec);
                            p->m_mainRefs.m_Pos.m_y = boneLenVec.m_y;
                            //p->m_mainRefs.m_Pos.m_y *= initialToFinalHeight;

                            p->m_mainRefs.m_Normale = mulMatrix2d(uvVectorTranformUniform.m_rot, p->m_mainRefs.m_Normale);
                            p->m_mainRefs.m_Normale.normalize();
                        }
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool Patch::computePatchUVAABB(AnimPatch   *_patch, AABB &UVs, nv::Image &_image)
        {
            struct TexDesc
            {
                u32 m_sizeX, m_sizeY;
                f32 getRatioXY() {return (f32) m_sizeX/(f32) m_sizeY;}
            } textureDesc;
            UVs = AABB(Vec2d(0,0));
            bbool isBoxInit=bfalse;
            TexDesc *texture = &textureDesc;
            SimplePatch32Data data;
            SimplePatch32Data *_pData = &data;
            Vec2d PointsNS[16];
            AnimPatchPointDyn *ppDynTab[4];
            AnimPatchPoint *ppTab[4];
            AnimPatchPointDyn dummyDyn[4];
            const u32 _hdiv = 4;
            const u32 _vdiv = 4;
            const f32 _z = 0;
            textureDesc.m_sizeX = _image.width();
            textureDesc.m_sizeY = _image.height();
            ITF_MemSet(dummyDyn, 0, sizeof(dummyDyn));

            if (_patch->m_nbPatchPoints == 4)
            {
                ppTab[0] = _patch->m_AnimPatchPoints[1];
                ppTab[1] = _patch->m_AnimPatchPoints[3];
                ppTab[2] = _patch->m_AnimPatchPoints[0];
                ppTab[3] = _patch->m_AnimPatchPoints[2];
            } else
            {
                ITF_ASSERT(_patch->m_nbPatchPoints == 2);
                ppTab[0] = _patch->m_AnimPatchPoints[0];
                ppTab[1] = _patch->m_AnimPatchPoints[1];
                ppTab[2] = _patch->m_AnimPatchPoints[1];
                ppTab[3] = _patch->m_AnimPatchPoints[0];
            }

            ppDynTab[0] = &dummyDyn[ppTab[0]->m_Index];
            ppDynTab[1] = &dummyDyn[ppTab[1]->m_Index];
            ppDynTab[2] = &dummyDyn[ppTab[2]->m_Index];
            ppDynTab[3] = &dummyDyn[ppTab[3]->m_Index];

            AnimDraw::computePatchControlPoints(&PointsNS[0],ppDynTab, ppTab, _hdiv, _vdiv, bfalse);
#undef PATCH32_USE_PCT
#undef PATCH32_USE_COLOR
#define PATCH32_DO_NOT_RETURN
#define PATCH32_IGNORE_MESHELEMENTS
#if PATCH32_WORKINGBUFF == 0
        pData->vertexListPT = (VertexPT*)pData->LockVBuff()
#endif // PATCH32_WORKINGBUFF
#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h"

            VertexPT	* vertices = _pData->vertexListPT;
            for (u32 vertIndex=0; vertIndex<_pData->vertexCount; vertIndex++)
            {
                const Vec2d &uv = vertices[vertIndex].m_uv;
                /*int x = (int)(uv.m_x *_image.width());
                int y = (int)(uv.m_y *_image.height());
                if (x>=0 && x<_image.width() && y>=0 && y<_image.height())
                _image.pixels()[x+y*_image.width()] = nv::Color32(0,255,0,255);*/
                if (!isBoxInit)
                {
                    UVs = AABB(uv);
                    isBoxInit = btrue;
                }
                else
                {
                    UVs.grow(uv);
                }
            }
#if PATCH32_WORKINGBUFF == 0
            pData->UnlockVBuff();
#endif // PATCH32_WORKINGBUFF
            ITF_ASSERT(_pData->vertexCount<SimplePatch32Data::maxVertexCount);
            return isBoxInit;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///compute uv aabb from animation data
        void Patch::computeUVAABB(AnimTemplate *_template, ChainedBones &_chainedBones, nv::Image &_image, const String8 &_imagePath)
        {
            AABB UVs(Vec2d(0,0));
            bbool isUVInit=bfalse;

            for (int boneIndex=0; boneIndex<(int)_chainedBones.m_AnimBones.size(); boneIndex++)
            {
                AnimBone *bone = _chainedBones.m_AnimBones[boneIndex];

                //for each patch linked to this bone
                for (u32 patchIndex=0; patchIndex<_template->m_Patch.size(); patchIndex++)
                {
                    AnimPatch *patch = &_template->m_Patch[patchIndex];
                    if (patch->m_nbPatchPoints!=0 && 
                        patch->m_AnimPatchPoints[0]->m_mainRefs.m_BonePtr == bone)
                    {
                        AABB patchBB;
                        if (computePatchUVAABB(patch, patchBB, _image))
                        {
                            if (isUVInit)
                            {
                                UVs.grow(patchBB);
                            }
                            else
                            {
                                UVs = patchBB;
                                isUVInit = btrue;
                            }
                        }
                        //
                    }
                }
            }


            int width = _image.width();
            int height = _image.height();
            //knowing we have bilinear filtering, we get every pixel concerned
            UVs.setMin(UVs.getMin()*Vec2d((f32)width, (f32)height)-Vec2d(0.5f,0.5f));
            UVs.setMax(UVs.getMax()*Vec2d((f32)width, (f32)height)+Vec2d(0.5f,0.5f));

            m_UVBox.minx = (int)floorf(UVs.getMin().m_x);
            m_UVBox.miny = (int)floorf(UVs.getMin().m_y);
            m_UVBox.maxx = (int)ceilf(UVs.getMax().m_x);
            m_UVBox.maxy = (int)ceilf(UVs.getMax().m_y);                    

        }
    }
}
#endif
