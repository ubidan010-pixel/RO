#ifndef _ITF_ANIMTEXPACKER_PATCH_H_
#define _ITF_ANIMTEXPACKER_PATCH_H_

#ifndef _ITF_PACKEDTEXTUREAREA_H_
#include "tools/plugins/CookerPlugin/AnimTexPacker/PackedTextureArea.h"
#endif

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#pragma warning(push)
#pragma warning(disable : 4244 4127 4512)
#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

#include <nvimage/Image.h>
#include <nvmath/Color.h>
#include <nvimage/DirectDrawSurface.h>
#include <nvimage/ImageIO.h>

#include <nvtt/nvtt.h>
#pragma warning(pop)


namespace ITF
{
    class AnimBone;
    class AnimTemplate;
    class AnimSkeleton;
    class AnimPatch;
    class AnimPatchPoint;
    class AnimPatchBank;

    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        class ChainedBones
        {
        public:
            SafeArray<AnimBone*> m_AnimBones;
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        class BoneExtent
        {
        public:
            BoneExtent() {initEmpty();}

            void initEmpty() 
            {
                m_worldExtent.setMin(Vec2d(1,1));
                m_worldExtent.setMax(Vec2d(0,0)); 
                m_uvExtent.setMin(Vec2d(1,1));
                m_uvExtent.setMax(Vec2d(0,0));
            }
            void grow(const Vec2d &_xy, const Vec2d &_uv)
            {
                if (m_worldExtent.isValid())
                    m_worldExtent.grow(_xy);
                else
                    m_worldExtent = AABB(_xy,_xy);

                if (m_uvExtent.isValid())
                    m_uvExtent.grow(_uv);
                else
                    m_uvExtent = AABB(_uv,_uv);
            }

            AABB m_worldExtent, m_uvExtent;

        };
        class BoneMaxSize
        {
        public:
            BoneMaxSize() {initEmpty();}

            void initEmpty() { m_worldSize= AABB(Vec2d(0,0)); m_uvSize = AABB(Vec2d(0,0)); }

            void grow(const Vec2d &_worldSize, const Vec2d &_uvSize)
            {
                m_worldSize.grow(_worldSize);
                m_uvSize.grow(_uvSize);
            }

            f32 getMaxWorldSize() const {return std::max(m_worldSize.getMax().m_x, m_worldSize.getMax().m_y);}

            AABB m_worldSize;
            AABB m_uvSize;
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///A patch : closed shape containing a texture. More or less an AnimTemplate+texture data+placement, but only 1 independent shape if bone is cut
        class Patch
        {
        public:
            Patch() : 
              m_pixels(NULL)
                  , m_UVBox(0,0,0,0)
                  , m_finalPos(0,0,0,0)
                  , m_subImageWidth(0)
                  , m_subImageHeight(0)
                  , m_isRotated(bfalse)
                  , m_animTemplate(NULL)
                  , m_initialImageWidth(0)
                  , m_initialImageHeight(0)
                  , m_skeleton(NULL)
                  , m_sourceOfAlias(NULL)
              {
                  static int currentSerial=0;
                  m_serialNumber = currentSerial;
                  m_uvlocalToRotatedLocalTransform.setIdentity();
                  m_imageStretchTransform.setIdentity();
                  currentSerial++;
              }
              Patch(const Patch&) {ITF_ASSERT(bfalse);}
              Patch& operator=(const Patch&) {ITF_ASSERT(bfalse);return *this;}

              ~Patch()
              {
                  SF_DEL_ARRAY(m_pixels);
              }
              ///////////////////////////////////////////////////////////////////////////////////////////
              void setAliasOf(Patch *_sourceOfAlias) {m_sourceOfAlias = _sourceOfAlias;}
              Patch *getSourceOfAlias() const {return m_sourceOfAlias;}

              ///////////////////////////////////////////////////////////////////////////////////////////
              const BoxI &getUVBox() const {return m_UVBox;}

              ///////////////////////////////////////////////////////////////////////////////////////////
              void setFinalPos(const BoxI &_finalBox, bbool _isRotated, const transform2d &_uvTransform);
              ///////////////////////////////////////////////////////////////////////////////////////////
              void setFinalPosFromAlias();

              ///////////////////////////////////////////////////////////////////////////////////////////
              const StringID &getTemplateID() const {return m_templateID;}

              ///////////////////////////////////////////////////////////////////////////////////////////
              const String8 &getImagePath() const {return m_imagePath;}

              ///////////////////////////////////////////////////////////////////////////////////////////
              int computePlacementHeuristic(PackedTextureArea *_area, BoxI &b);

              ///////////////////////////////////////////////////////////////////////////////////////////
              bbool Place(PackedTextureArea *_area, BoxI &b, bbool &_rotated, transform2d &_subImageRotation);

              ///////////////////////////////////////////////////////////////////////////////////////////
              bbool isEqual(const Patch *_other);

              ///////////////////////////////////////////////////////////////////////////////////////////
              ///Initialize from animation data
              void initialize(const StringID &_templateID, AnimTemplate *_animTemplate, ChainedBones &_chainedBones, nv::Image &_image, const String8 &_imagePath, AnimSkeleton *_skeleton);

              int getUVBoundingWidth() const {return m_UVBox.maxx-m_UVBox.minx+1;}
              int getUVBoundingHeight() const {return m_UVBox.maxy-m_UVBox.miny+1;}
              u32 getSubImageWidth() const {return m_subImageWidth;}
              u32 getSubImageHeight() const {return m_subImageHeight;}

              ///////////////////////////////////////////////////////////////////////////////////////////
              void extractPixels(nv::Image &_image);

              const SafeArray<u32> &getBoneIndices() const {return m_boneIndices;}
              const ITF_VECTOR<StringID> &getBoneIDs() const {return m_boneIDs;}

              ///////////////////////////////////////////////////////////////////////////////////////////
              void debugDump();

              ///////////////////////////////////////////////////////////////////////////////////////////
              void blitToFinalPos(nv::Image &_destImage);

              ///////////////////////////////////////////////////////////////////////////////////////////
              void growPatchMaxSize(const BoneMaxSize& _toAdd );

              ///////////////////////////////////////////////////////////////////////////////////////////
              const BoneMaxSize &getPatchMaxSize() const;

              nv::Color32 getPixel(int _x, int _y) const
              {
                  int w = (int)getSubImageWidth();
                  ITF_ASSERT(_x>=0 && _y>=0 && _x<w && _y<(int)getSubImageHeight());
                  return m_pixels[_x+_y*w];
              }
              nv::Color32 getPixel_WithBorder(int _x, int _y) const
              {
                  int w = (int)getSubImageWidth();
                  if(_x>=0 && _y>=0 && _x<w && _y<(int)getSubImageHeight())
                      return m_pixels[_x+_y*w];
                  else
                      return nv::Color32(0,0,0,0);
              }

              ///////////////////////////////////////////////////////////////////////////////////////////
              void resizeTexture(int _newSize);

              ///////////////////////////////////////////////////////////////////////////////////////////
              nv::Color32 samplePixel_Linear(f32 _x, f32 _y);

              ///////////////////////////////////////////////////////////////////////////////////////////
              void           modifyUVs(int mapWidth, int mapHeight, ITF_SET<AnimPatchPoint*> &_alreadyProcessedPoints);

              ///////////////////////////////////////////////////////////////////////////////////////////
              AnimTemplate * getAnimTemplate(    ) { return m_animTemplate; }
              
              int getInitialImageWidth      (    ) { return m_initialImageWidth; }
              int getInitialImageHeight     (    ) { return m_initialImageHeight; }

        private:

            ///////////////////////////////////////////////////////////////////////////////////////////
            bbool computePatchUVAABB(AnimPatch   *_patch, AABB &UVs, nv::Image &_image);
            ///////////////////////////////////////////////////////////////////////////////////////////
            ///compute uv aabb from animation data
            void computeUVAABB(AnimTemplate *_template, ChainedBones &_chainedBones, nv::Image &_image, const String8 &_imagePath);

            ///////////////////////////////////////////////////////////////////////////////////////////
            AnimTemplate *m_animTemplate;
            BoxI m_UVBox;
            BoxI m_finalPos;
            bbool m_isRotated;
            nv::Color32 *m_pixels;
            int m_subImageWidth, m_subImageHeight;
            int m_serialNumber;
            String8 m_imagePath;
            StringID m_templateID;
            SafeArray<u32> m_boneIndices;
            ITF_VECTOR<StringID> m_boneIDs;
            static const u32 m_PatchsDegree = 4;
            static const u32 m_PatchsNumberOfCurves = 2;
            BoneMaxSize m_boneMaxSize;
            AnimSkeleton *m_skeleton;
            int m_initialImageWidth, m_initialImageHeight;
            Patch *m_sourceOfAlias;
            //Transforms the m_pPixels in UV coordinates in the same coordinate frame, but with possibly a rotation (eg a 90 deg)
            transform2d m_uvlocalToRotatedLocalTransform, m_imageStretchTransform;

        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// List of patches
        class PatchList
        {
        public:
            String m_patchImagePath;
            AnimPatchBank * m_patchBank;
            ITF_VECTOR<StringID> m_usedPatches;
            ITF_LIST<Patch*> m_patches;

            ///////////////////////////////////////////////////////////////////////////////////////////
            PatchList()
            {
                m_patchBank = NULL;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////
            void destroy(void)
            {
                ITF_LIST<Patch*>::iterator iter;
                for (iter=m_patches.begin(); iter!=m_patches.end(); iter++)
                {
                    delete *iter; 
                }
                m_patches.clear();
                m_usedPatches.clear();
            }

        };

    }
}

#endif
