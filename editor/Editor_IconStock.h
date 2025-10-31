#ifndef _ITF_EDITOR_ICONSTOCK_H_
#define _ITF_EDITOR_ICONSTOCK_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

class Vec2d;
class Vec3d;
class Texture;

//////////////////////////////////////////////////////////////////////////
/// Contains list of icons for editor
class Editor_IconStock
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// List of icon IDs
    enum EditorIconID
    {
        Icon_PolyLinePoint = 0,
        Icon_PolyLinePoint_Selected = 1,
        Icon_PolyLinePoint_Delete = 7,
        Icon_PolyLine_Cut = 8,
        Icon_PolyLine_NewPoint = 9,
        Icon_PolyLine_Hole = 10,
        Icon_CameraPan = 11, //Missing
        Icon_CameraRotate = 12, //Missing
        Icon_TriggerSub = 13, //Missing
        Icon_Trajectory_Camera = 14,//Missing
        Icon_CameraActor = 15, //Missing
        Icon_RedSquare = 16,
        Icon_GreenSquare = 17,
        Icon_YellowSquare = 18,
        Icon_Interrogation = 19, //Missing
        Icon_Plus = 20, 
        Icon_WallRunSTop = 21,
        Icon_RedArrow = 32,
        Icon_GreenArrow = 33,
        Icon_RedBar = 36,
        Icon_GreenBar = 37,
        Icon_Actor = 40, 
        Icon_SelectedActor = 41,
        Icon_Frise = 42, 
        Icon_SelectedFrise = 43, 
        Icon_Group = 44, 
        Icon_SubEditActor = 45, 
        Icon_ActorWithWarnings = 46,
        Icon_GroupSelected = 47,
        Icon_Light = 48,
        Icon_SelectedLight = 49,
        Icon_ConstraintCamera = 50,
        Icon_ConstraintCamera_Selected = 51,
        Icon_Cinema_Clap = 52,
        Icon_SelectedCinema_Clap = 53,
        Icon_Save = 56,
        Icon_Ok = 57,
        Icon_New = 58,
        Icon_Polyline_Connected = 59,
        Icon_0 = 60,
        Icon_1 = 61,
        Icon_2 = 62,
        Icon_3 = 63,
        Icon_4 = 64,
        Icon_5 = 65,
        Icon_6 = 66,
        Icon_7 = 67,
        Icon_8 = 68,
        Icon_9 = 69,
        Icon_DynamicFrise = 70,
        Icon_SelectedDynamicFrise = 71,
        Icon_DisabledActor = 72,
        Icon_SelectedDisabledActor = 73,
        Icon_TrajectoryNode = 74,
        Icon_TrajectoryTangNode = 75,
        Icon_SelectedTrajectoryNode = 76,
        Icon_SelectedTrajectoryTangNode = 77,
        Icon_SubSceneReference = 78,
        Icon_SubSceneReferenceSelected = 79,
        Icon_colorComputer = 80,
        Icon_colorComputerSelected = 81,
        Icon_DummyActor = 82,
        Icon_SelectedDummyActor = 83,
        Icon_TrajectoryNodeActor = 84,
        Icon_SelectedTrajectoryNodeActor = 85,
		
		Icon_Death = 86,
		Icon_BubbleExit = 87,
		Icon_Hit_Receive = 88,

        Icon_Count,
        ENUM_FORCE_SIZE_32(0)
    };


    //////////////////////////////////////////////////////////////////////////
    ///Constructor
    Editor_IconStock();
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// show given icon at given position. Intended for mouse (special display attributes)
    /// @param _id the icon ID
    /// @param _mousePos the mouse position
    /// @param _angle the icon's angle
    void                    showMouseIcon   (EditorIconID _id, const Vec2d &_mousePos, f32 _angle);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// draw icon at given position, with z=0
    /// @param _pt
    /// @param _t
    /// @param _size
    void                    drawIcon(EditorIconID _id, const Vec3d& _pt, f32 _size = 50.f, f32 _angle=0.f, bbool _flipH=bfalse, bbool _flipV=bfalse, u32 _color = COLOR_WHITE);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// draw Icon with given texture and given sizes
    /// @param _id the icon ID
    /// @param _pt 3D position
    /// @param _w 2D width
    /// @param _h 2D height
    /// @param _angle horizontal rotation
    void                    drawIcon2D(EditorIconID _id, const Vec3d& _pt, f32 _w, f32 _h, f32 _angle=0.f, bbool _flipH=bfalse, bbool _flipV=bfalse, u32 _color = COLOR_WHITE);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// draw icon at given position, with given texture&size
    /// @param _id the icon ID
    /// @param _pt
    /// @param _z
    /// @param _t
    /// @param _size
    void                    drawIcon(EditorIconID _id, const Vec2d& _pt, f32 _z, f32 _size = 50.f, f32 _angle=0.f, bbool _flipH=bfalse, bbool _flipV=bfalse, u32 _color = COLOR_WHITE);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get texture atlas (contains all icons)
    Texture                 *getAtlasTexture();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get texture for trajectory (cannot be in atlas because og tilling)
    Texture                 *getTrajectoryTexture();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// LoadIcons into stock
    void                    loadIcons();
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// UnloadIcons into stock
    void                    unloadIcons();

    void                    draw();

private:
    static const u32        m_maxIconsDisplayed = 512;

    /// draw icon at given position, with given texture&size
    void                    drawIcon(const Vec2d& _pt, f32 _z, Texture* _t, int _indexInAtlas, f32 _size = 50.f, f32 _angle=0.f, bbool _flipH=bfalse, bbool _flipV=bfalse, u32 _color = COLOR_WHITE);
    /// draw icon at given position
    void                    drawIcon(const Vec3d& _pt, Texture* _t, int _indexInAtlas, f32 _size = 50.f, f32 _angle=0.f, bbool _flipH=bfalse, bbool _flipV=bfalse, u32 _color = COLOR_WHITE);
    /// draw Icon with given texture and given sizes
    void                    drawIcon2D(const Vec3d& _pt, Texture* _t, int _indexInAtlas, f32 _w, f32 _h, f32 _angle, bbool _flipH, bbool _flipV, u32 _color = COLOR_WHITE);
    ///
    void                    createMeshVertexBuffer();

    ITF_VECTOR<Vertex>      m_mouseIconVtx;
    ResourceID              m_atlasTextureID, m_trajectoryTextureID, m_iconsGroup;
    ITF_Mesh                m_mesh;
    SafeArray<VertexPCT, 6 * m_maxIconsDisplayed> m_vertex;
};

}

#endif

