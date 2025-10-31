#ifndef _ITF_EDITORDRAW_H_
#define _ITF_EDITORDRAW_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

#define FriezeEditor_LineSize	4.0f
#define AABB_LineSize           2.0f
#define Anchor_DefaultSize      PickingShape::m_disc_picking_tolerance  // display size is the same asd picking distance size
#define Connection_IconSize     Anchor_DefaultSize*2.f

    class Actor;
    class PolyLine;
    class Frise;
    class AABB;

    class EditorDraw : public ActorDrawEditInterface
    {
    public:
        enum PickableDetailMode {PickableDetails_HideAll=0, PickableDetails_ShowDbg, PickableDetails_ShowAnchors, PickableDetails_ShowAll, PickableDetails_Count};

        static void     drawCurrentMode();
        static void     drawActor(Actor* _pActor, bbool _bMouseIsOver, bbool _bSubElement = bfalse);
		static void     drawPolyPointList(bbool _isSelected, ObjectRef _ref, const PolyPointList* _pPolyLine, f32 _lineSize, bbool _bMouseIsOver, bbool _bSubElement = bfalse, Matrix44 *_transform = NULL, f32 _heightMult = 1.f, const String& _addPath = String::emptyString, const String& _name = String::emptyString);
        static void     drawFrise(const Frise* _pFrise, f32 _lineSize, bbool _bMouseIsOver);
        static void     drawCells();
        static void     drawCursor();
        static void     drawSelectionZone( const Vec2d& _vStart, const Vec2d& _vEnd );
        static void     drawOrigin();
        static void     drawGrid(const Vec3d& _gridPoint, f32 _step);

        static void     drawHelp();
		static void		drawMemory();
        static void     startDraw();
		

        // Draw switches
        static bbool    getHighlightOnMouseOver()           { return m_bHighlightOnMouseOver; }
        static void     setHighlightOnMouseOver(bbool val)  { m_bHighlightOnMouseOver = val; }
        static void     showAnchors(bbool _bShow)           { m_bShowAnchors = _bShow; }
        static void     showTextTips(bbool _bShow)          { m_bShowTextTips = _bShow; }
        static bbool    getShowTextTips()                   { return m_bShowTextTips;}
        static void     showFriseCollisions(bbool _bShow)   { m_bShowFriseCollisions = _bShow; }
        static void     showFriseTriangles(bbool _bShow)    { m_bShowFriseTriangles = _bShow; }
        static bbool    getShowFriseTriangles()             { return m_bShowFriseTriangles;}
        static void     showObjectAABBs(bbool _bShow)       { m_bShowObjectAABBs = _bShow; }
        static bbool    getShowObjectAABBs()                { return m_bShowObjectAABBs;}
        static void     showCells(u32  _layer)              { m_showCells  = _layer;}
        static void     toggleDrawCells()                   { m_showCells++; if (m_showCells > CELL_LAYERS_COUNT) m_showCells = 0;}
        static void     showPolylineEdgeIndices(bbool _bShow){ m_bShowPolylineEgdeIndices = _bShow; }
        static bbool    getShowPolylineEdgeIndices()        { return m_bShowPolylineEgdeIndices;}
        static void     setPickableDetailModes(PickableDetailMode _mode);
        static void     cyclePickableDetailModes();
        static PickableDetailMode getPickableDetailMode()   {return m_pickableDetailsMode;}

        //Testing pickable detail level
        static bbool pickableDetail_CanShowDbg()            {return m_pickableDetailsMode>=PickableDetails_ShowDbg;}
        static bbool pickableDetail_CanShowAll()            {return m_pickableDetailsMode==PickableDetails_ShowAll;}
        static bbool PickableDetail_CanShowAnchors()        {return m_pickableDetailsMode >= PickableDetails_ShowAnchors;}
        static bbool pickableDetail_canShowAnyDetail()      {return m_pickableDetailsMode > PickableDetails_HideAll;}


        static u32 getDrawCellsEnable()                    { return m_showCells;}

        virtual void drawPolyLine(const PolyLine* _pPolyLine, bbool _bSubElement = bfalse);
        virtual bbool showTextTips() const                  { return m_bShowTextTips; }


        //Predefined colors
        static const u32 uNormalColor;
        static const u32 uSelectedColor;
        static const u32 uSubEditedColor;
        static const u32 uDarkColor;

        static ITF_VECTOR<AABB> m_loadedCells;
        static ITF_VECTOR<AABB> m_updatedCells;

    private:

        static bbool checkMustHidePickableDetails(const Pickable *_pickable);        

        static bbool m_bShowAnchors;
        static bbool m_bShowTextTips;
        static bbool m_bShowFriseCollisions;
        static bbool m_bShowFriseTriangles;
        static bbool m_bShowObjectAABBs;
        static u32   m_showCells;
        static bbool m_bShowPolylineEgdeIndices;
        static bbool m_bHighlightOnMouseOver;
        static PickableDetailMode m_pickableDetailsMode;

        typedef struct FriezeColorData
        {
            FriezeColorData(ObjectRef _ref, const Color& _col)
            {
                m_frieze = _ref;
                m_meshColor = _col;
            }
            ObjectRef           m_frieze;
            Color               m_meshColor;
        } FriezeColorData;
        static ITF_VECTOR<FriezeColorData> m_modifiedFriezes;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Compute screen projection for grid display
        /// @param bottomRight3d
        /// @param topLeft3d
        /// @param _refPoint
        /// @param _xStep
        /// @param _yStep
        static void    gridCompute(Vec3d& bottomRight3d, Vec3d& topLeft3d, const Vec3d& _refPoint, f32 _xStep, f32 _yStep);
    };

    void editorDraw_cellProcessCB(u32 _frame, const Cell* _loadedCell, u32 _iLayer);

} // namespace ITF

#endif // _ITF_EDITORDRAW_H_