#ifndef _WIISPECIFICCONSTANTS_
#define _WIISPECIFICCONSTANTS_

#undef _MSL_DEBUG_ERROR
#undef _MSL_ERROR
#define _MSL_DEBUG_ERROR(a,b) {}
#define _MSL_ERROR(a,b) {}

#ifndef HOLLYWOOD_REV
#define HOLLYWOOD_REV	1
#endif // HOLLYWOOD_REV

#define ITF_PSI

#define NB_FLUID_LEVELS (128 + 1)

#define MAX_VERTEX_BY_MESH      1500
#define MAX_INDEX_BY_ELEMENT    8192
#define NB_ELEMENT_IN_VERTEX_BUFFER 16

#define MESH_DEFAULT_HDIV           4
#define MESH_DEFAULT_VDIV           128

#define PATCH32_WORKINGBUFF    0
#define VBMANAGER_USE_DYNAMICRING_VB

#define LAYER_0_CELL_SIZE           20.0f
#define LAYER_0_MAX_HORIZ_CELLS     250
#define LAYER_0_MAX_VERT_CELLS      250
#define CELL_LAYERS_COUNT           4
#define CELL_LAYERS_SCALE           4
//#define USE_LOAD_RESOURCE_MAX_RANGE
#define CELLLAYERSSETTINGS_TARGET   CellsLayersSettings::CellTgt_WII

//#define SKIP_NODRAW_UPDATE 0.16f

#define RESOURCE_PHYSICAL_DEFAULTLIFETIME 300 //keep it for 5 secs assuming we are at 60fps

#endif // _WIISPECIFICCONSTANTS_