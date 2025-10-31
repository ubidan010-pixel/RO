#ifndef _CELLLAYERSSETTINGS_H_
#define _CELLLAYERSSETTINGS_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // PLATFORM NON DEPENDANT COMMON CODE
    ///////////////////////////////////////////////////////////////////////////////////////////////

    class CellsLayersSettings
    {
    public:
        enum Target
        {
            CellTgt_PC,
            CellTgt_XBOX,
            CellTgt_PS3,
            CellTgt_WII,
            CellTgt_VITA,
            CellTgt_3DS
        };
        static Target m_currentTarget;

        static int getLoadResourceMaxRange(int _layer)
        {
            switch(m_currentTarget)
            {
                case CellTgt_XBOX:
                    return getLoadResourceMaxRange_XBOX(_layer);
                case CellTgt_PS3:
                    return getLoadResourceMaxRange_PS3(_layer);
                case CellTgt_WII:
                    return getLoadResourceMaxRange_WII(_layer);
                case CellTgt_VITA:
                    return getLoadResourceMaxRange_VITA(_layer);
                case CellTgt_3DS:
                    return getLoadResourceMaxRange_3DS(_layer);
                case CellTgt_PC:
                default:
                    return getLoadResourceMaxRange_PC(_layer);
            }
        }

        static int getUpdateCellsRange(int _layer)
        {
            switch(m_currentTarget)
            {
                case CellTgt_XBOX:
                    return getUpdateCellsRange_XBOX(_layer);
                case CellTgt_PS3:
                    return getUpdateCellsRange_PS3(_layer);
                case CellTgt_WII:
                    return getUpdateCellsRange_WII(_layer);
                case CellTgt_VITA:
                    return getUpdateCellsRange_VITA(_layer);
                case CellTgt_3DS:
                    return getUpdateCellsRange_3DS(_layer);
                case CellTgt_PC:
                default:
                    return getUpdateCellsRange_PC(_layer);
            }
        }

        static int getResourceCellsRange(int _layer)
        {
            switch(m_currentTarget)
            {
                case CellTgt_XBOX:
                    return getResourceCellsRange_XBOX(_layer);
                case CellTgt_PS3:
                    return getResourceCellsRange_PS3(_layer);
                case CellTgt_WII:
                    return getResourceCellsRange_WII(_layer);
                case CellTgt_VITA:
                    return getResourceCellsRange_VITA(_layer);
                case CellTgt_3DS:
                    return getResourceCellsRange_3DS(_layer);
                case CellTgt_PC:
                default:
                    return getResourceCellsRange_PC(_layer);
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // PC ONLY
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static int getLoadResourceMaxRange_PC(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return  0;
            default:
                return 200;
            }
        }
         
        static int getUpdateCellsRange_PC(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return 1;
                break;
            default:
                return 0;
            }
        }

        static int getResourceCellsRange_PC(int _layer)
        {
            switch(_layer)
            {
            case 0:
                return 3;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
            }
        }
 
        ///////////////////////////////////////////////////////////////////////////////////////////////
        // XBOX ONLY
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static int getLoadResourceMaxRange_XBOX(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return  0;
            default:
                return 200;
            }
        }
         
        static int getUpdateCellsRange_XBOX(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return 1;
                break;
            default:
                return 0;
            }
        }

        static int getResourceCellsRange_XBOX(int _layer)
        {
            switch(_layer)
            {
            case 0:
                return 3;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////////////////////
        // PS3 ONLY
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static int getLoadResourceMaxRange_PS3(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return  0;
            default:
                return 200;
            }
        }
         
        static int getUpdateCellsRange_PS3(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return 1;
                break;
            default:
                return 0;
            }
        }

        static int getResourceCellsRange_PS3(int _layer)
        {
            switch(_layer)
            {
            case 0:
                return 3;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // WII ONLY
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static int getLoadResourceMaxRange_WII(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return  0;
            default:
                return 200;
            }
        }
         
        static int getUpdateCellsRange_WII(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return 1;
                break;
            default:
                return 0;
            }
        }

        static int getResourceCellsRange_WII(int _layer)
        {
            switch(_layer)
            {
            case 0:
                return 3;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
            }
        }


        ///////////////////////////////////////////////////////////////////////////////////////////////
        // VITA ONLY
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static int getLoadResourceMaxRange_VITA(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return  0;
            default:
                return 200;
            }
        }
         
        static int getUpdateCellsRange_VITA(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return 1;
                break;
            default:
                return 0;
            }
        }

        static int getResourceCellsRange_VITA(int _layer)
        {
            switch(_layer)
            {
            case 0:
                return 3;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // 3DS ONLY
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static int getLoadResourceMaxRange_3DS(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return  0;
            default:
                return 200;
            }
        }
         
        static int getUpdateCellsRange_3DS(int _layer)
        {
            switch(_layer)
            {
            case 0:
            case 1:
                return 1;
                break;
            default:
                return 0;
            }
        }

        static int getResourceCellsRange_3DS(int _layer)
        {
            switch(_layer)
            {
            case 0:
                return 3;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 1;
                break;
            default:
                return 0;
            }
        }
    };
} //namespace ITF

#endif _CELLLAYERSSETTINGS_H_
