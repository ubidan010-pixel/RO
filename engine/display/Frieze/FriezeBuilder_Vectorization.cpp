#include "precompiled_engine.h"

#if 0

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "gameplay/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{

    typedef struct Edge
    {
        Edge()
        {
            m_addedToFrieze = bfalse;
            m_erase = bfalse;
            m_pixelNb     = 0;
            m_state       = 0;
        }

        bbool       m_addedToFrieze;
        Vec2d        m_pos;
        //       u32          m_ref;   
        u32          m_pixelNb;
        //        u32          m_idEdge;
        i32          m_idEdgeNext;
        i32          m_idEdgeLast;
        i32          m_state;
        bbool        m_erase;
    }Edge;

    struct EdgeList
    {

        EdgeList(Frise* _owner)

        {
            m_bitMapSmooth = 2;
            m_bitMapScale  = 0.1f;
            m_FriseRef = _owner;
        };

        void    edgeSetNext                 (   u32 _idEdge, u32 _idNext);        
        u32     edgeAdd                     (   i32 _idLast, i32 _idNext);
        void    edgeDrawDebug               (   );
        i32     edgeFindStart               (   );
        void    edgeIncrPixelNb             (   u32 _idEdge);
        void    edgeSetLast                 (   u32 _idEdge, u32 _idLast);
        void    edgeSetPosition             (   u32 _idEdge, u32 _x, u32 _y);
        void    edgeTestToErase             (   u32 _idEdge);
        void    edgeSetState                (   u32 _idEdge, i32 _state);
        void    buildBordsInVectorization   (  ITF_VECTOR<Frise::Frieze> & _Friezes, ITF_VECTOR<Frise::Bord> & _borders );

        ITF_VECTOR<Edge>      m_edge;
        ITF_VECTOR<u32>  m_edgeToErase;

        u32     m_bitMapSmooth;
        float   m_bitMapScale;
        Frise   *m_FriseRef;
        
    };


    void Frise::buildFriezeInVectorization()
    {

        static u32 bitMapDrawSizeX = 0;
        static u32 bitMapDrawSizeY = 0;

        static u32 *bitMap = NULL;
        static Texture *bitMapDraw = NULL;

        //  reload de la texture
        if( !bitMap /*|| (INPUT_ADAPTER->isKeyPressed(KEY_F8)== 1)*/)
        {
            static bbool bitMapDrawLoaded = bfalse;

            //  load texture 
            if (!bitMapDrawLoaded)
            {
                bitMapDrawLoaded  = btrue;
                bitMapDraw = GFX_ADAPTER->loadTexture("..\\data\\bitmap.bmp", 0);
                RESOURCE_MANAGER->addUserToResourcePhysicalData(bitMapDraw);
            }

            if ( bitMap )
            {
                Memory::free(bitMap);
                bitMap    = NULL;
            }

            bitMapDraw->reload();
            u8* buffer = GFX_ADAPTER->grabTexture(bitMapDraw);

            bitMapDrawSizeX = bitMapDraw->m_datasizeX + 2;
            bitMapDrawSizeY = bitMapDraw->m_datasizeY + 2;

            u32 sizeBitMapdraw  = bitMapDrawSizeX*bitMapDrawSizeY*sizeof(u32);
            bitMap = (u32 *)Memory::malloc(sizeBitMapdraw);
            memset(bitMap, 0, sizeBitMapdraw);

            u32 writeOfs;
            u32 readOfs = 0;
            for (u32 y = 0; y < bitMapDrawSizeY-2; y++)
            {
                writeOfs = (y+1) * bitMapDrawSizeX + 1;
                for (u32 x = 0; x < bitMapDrawSizeX-2; x++)
                {
                    ITF_ASSERT(writeOfs < sizeBitMapdraw);

                    u32 data = buffer[readOfs++];
                    data += buffer[readOfs++];
                    data += buffer[readOfs++];
                    readOfs++;  // skip alpha
                    if (data)
                        bitMap[writeOfs] = 1;
                    writeOfs++;
                }
            }

            delete[](buffer);
        }



#define ARRAY(_arr, _x, _y, _size) _arr[_x + _size * (_y)]

        //  move
        //  bitMapRandom();

        //  tableau de refs pour la texture, taille 2 fois plus grande que le tableau de bitmap
        const u32 refSizeX = (bitMapDrawSizeX-2)*2;
        const u32 refSizeY = (bitMapDrawSizeY-2)*2;
        i32* ref = (i32*)Memory::malloc(refSizeX * refSizeY * sizeof(i32));

        EdgeList list(this);

        //  parsing du tableau de bitmap 
        for (u32 y=1; y<=bitMapDrawSizeY-2; y++)
        {
            for (u32 x=1; x<=bitMapDrawSizeX-2; x++)
            {
                int index_x = (x-1)*2;
                int index_y = (y-1)*2;

                if ( ARRAY(bitMap,x,y, bitMapDrawSizeX) == 0 )
                {
                    ARRAY(ref, index_x,     index_y , refSizeX)      = -1;
                    ARRAY(ref, index_x,     index_y+1 , refSizeX)    = -1;
                    ARRAY(ref, index_x+1,   index_y , refSizeX)      = -1;
                    ARRAY(ref, index_x+1,   index_y+1 , refSizeX)    = -1;
                }
                else
                {
                    u32     res;

                    //  Haut Gauche
                    res = ARRAY(bitMap, x-1, y, bitMapDrawSizeX) +ARRAY(bitMap, x-1, y-1, bitMapDrawSizeX)*2 +ARRAY(bitMap, x, y-1, bitMapDrawSizeX)*4;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        ARRAY(ref,index_x,index_y , refSizeX)   = 0;                    
                        break;
                    case 4:
                    case 6:
                        ARRAY(ref,index_x,index_y , refSizeX)   = 11;
                        break;
                    case 5:
                        ARRAY(ref,index_x,index_y , refSizeX)   = 5;
                        break;
                    case 1:
                    case 3:
                        ARRAY(ref,index_x,index_y , refSizeX)   = 8;
                        break;
                    case 7:
                        ARRAY(ref,index_x,index_y , refSizeX)   = 12;
                        break;
                    }

                    //  Haut Droite
                    res = ARRAY(bitMap, x, y-1, bitMapDrawSizeX) +ARRAY(bitMap, x+1, y-1, bitMapDrawSizeX)*2 +ARRAY(bitMap, x+1, y, bitMapDrawSizeX)*4;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        ARRAY(ref,index_x+1,index_y , refSizeX)   = 1;
                        break;
                    case 1:
                    case 3:
                        ARRAY(ref,index_x+1,index_y , refSizeX)   = 9;
                        break;
                    case 5:
                        ARRAY(ref,index_x+1,index_y , refSizeX)   = 4;
                        break;
                    case 4:
                    case 6:
                        ARRAY(ref,index_x+1,index_y , refSizeX)   = 8;
                        break;
                    case 7:
                        ARRAY(ref,index_x+1,index_y , refSizeX)   = 12;
                        break;
                    }

                    //  Bas Droite
                    res = ARRAY(bitMap, x+1, y, bitMapDrawSizeX) +ARRAY(bitMap, x+1, y+1, bitMapDrawSizeX)*2 +ARRAY(bitMap, x, y+1, bitMapDrawSizeX)*4;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        ARRAY(ref,index_x+1,index_y+1 , refSizeX)   = 2;
                        break;
                    case 4:
                    case 6:
                        ARRAY(ref,index_x+1,index_y+1, refSizeX)   = 9;
                        break;
                    case 5:
                        ARRAY(ref,index_x+1,index_y+1, refSizeX)   = 6;
                        break;
                    case 1:
                    case 3:
                        ARRAY(ref,index_x+1,index_y+1, refSizeX)   = 10;
                        break;
                    case 7:
                        ARRAY(ref,index_x+1,index_y+1, refSizeX)   = 12;
                        break;
                    }

                    //  Bas gauche
                    res = ARRAY(bitMap, x, y+1, bitMapDrawSizeX) +ARRAY(bitMap, x-1, y+1, bitMapDrawSizeX)*2 +ARRAY(bitMap, x-1, y, bitMapDrawSizeX)*4;

                    switch(res)
                    {
                    case 0:
                    case 2:
                        ARRAY(ref,index_x,index_y+1, refSizeX)   = 3;
                        break;
                    case 1:
                    case 3:
                        ARRAY(ref,index_x,index_y+1, refSizeX)   = 11;
                        break;
                    case 5:
                        ARRAY(ref,index_x,index_y+1, refSizeX)   = 7;
                        break;
                    case 4:
                    case 6:
                        ARRAY(ref,index_x,index_y+1, refSizeX)   = 10;
                        break;
                    case 7:
                        ARRAY(ref,index_x,index_y+1, refSizeX)   = 12;
                        break;
                    }
                }
            }
        }

        //  création de deux tableaux de relay
        u32* relayH = (u32*)Memory::malloc(refSizeX * refSizeY * sizeof(u32));
        u32* relayV = (u32*)Memory::malloc(refSizeX * refSizeY * sizeof(u32));

        u32 relay;


        //  création des edges, listing des edges a supprimer
        for (u32 x=0; x<refSizeX; x++)
        {
            for (u32 y=0; y<refSizeY; y++)
            {
                switch ( ARRAY(ref,x,y, refSizeX) )
                {
                case 0:
                    //  horizontal
                    relay           = list.edgeAdd( list.m_edge.size()+1, -1);
                    ARRAY(relayH,x+1,y, refSizeX)  = relay;
                    list.edgeIncrPixelNb(relay);
                    list.edgeSetPosition(relay, x, y);
                    //edgeSetRef(relay, 0);
                    //  vertical
                    ARRAY(relayV,x,y+1, refSizeX)  = list.edgeAdd( -1, relay );
                    list.edgeIncrPixelNb(ARRAY(relayV,x,y+1, refSizeX));
                    break;
                case 1:
                    //  horizontal
                    list.edgeSetNext( ARRAY(relayH,x,y, refSizeX), list.m_edge.size());
                    list.edgeIncrPixelNb(ARRAY(relayH,x,y, refSizeX));
                    list.edgeTestToErase(ARRAY(relayH,x,y, refSizeX));
                    //  vertical
                    relay           = list.edgeAdd( ARRAY(relayH,x,y, refSizeX), -1);
                    ARRAY(relayV,x,y+1, refSizeX)  = relay;
                    list.edgeIncrPixelNb(relay);
                    list.edgeSetPosition(relay, x+1, y);
                    //edgeSetRef(relay, 1);
                    break;
                case 2:
                    //  horizontal
                    list.edgeSetLast( ARRAY(relayH,x,y, refSizeX), ARRAY(relayV,x,y, refSizeX));
                    list.edgeIncrPixelNb(ARRAY(relayH,x,y, refSizeX));
                    list.edgeSetPosition(ARRAY(relayH,x,y, refSizeX), x+1, y+1);
                    list.edgeTestToErase(ARRAY(relayH,x,y, refSizeX));
                    //edgeSetRef(ARRAY(relayH,x,y, refSizeX), 2);
                    //  vertical
                    list.edgeSetNext( ARRAY(relayV,x,y, refSizeX), ARRAY(relayH,x,y, refSizeX));
                    list.edgeIncrPixelNb(ARRAY(relayV,x,y, refSizeX));
                    list.edgeTestToErase(ARRAY(relayV,x,y, refSizeX));
                    break;
                case 3:
                    //  horizontal
                    relay           = list.edgeAdd( -1, ARRAY(relayV,x,y, refSizeX));
                    ARRAY(relayH,x+1,y, refSizeX)  = relay;
                    list.edgeIncrPixelNb(relay);
                    //  vertical
                    list.edgeSetLast(ARRAY(relayV,x,y, refSizeX), relay);
                    list.edgeIncrPixelNb(ARRAY(relayV,x,y, refSizeX));
                    list.edgeSetPosition(ARRAY(relayV,x,y, refSizeX), x, y+1);
                    // edgeSetRef(ARRAY(relayV,x,y, refSizeX), 3);
                    list.edgeTestToErase(ARRAY(relayV,x,y, refSizeX));
                    break;
                case 4:
                    //  horizontal
                    relay           = list.edgeAdd( ARRAY(relayV,x,y, refSizeX), -1);
                    ARRAY(relayH,x+1,y, refSizeX)  = relay;
                    list.edgeSetPosition(relay, x+1, y);
                    // edgeSetRef(relay, 4);
                    //  vertical
                    list.edgeSetNext( ARRAY(relayV,x,y, refSizeX), relay);
                    list.edgeTestToErase(ARRAY(relayV,x,y, refSizeX));
                    break;
                case 5:
                    //  horizontal
                    list.edgeSetNext( ARRAY(relayH,x,y, refSizeX), ARRAY(relayV,x,y, refSizeX));
                    list.edgeTestToErase(ARRAY(relayH,x,y, refSizeX));
                    //  vertical
                    list.edgeSetLast( ARRAY(relayV,x,y, refSizeX), ARRAY(relayH,x,y, refSizeX));
                    list.edgeSetPosition(ARRAY(relayV,x,y, refSizeX), x, y);
                    // edgeSetRef(ARRAY(relayV,x,y, refSizeX), 5);
                    list.edgeTestToErase(ARRAY(relayV,x,y, refSizeX));
                    break;
                case 6:
                    //  horizontal
                    relay           = list.edgeAdd( -1, list.m_edge.size()+1);
                    ARRAY(relayH,x+1,y, refSizeX)  = relay;
                    //  vertical
                    ARRAY(relayV,x,y+1, refSizeX)  = list.edgeAdd( relay, -1);
                    list.edgeSetPosition(ARRAY(relayV,x,y+1, refSizeX), x+1, y+1);
                    //  edgeSetRef(ARRAY(relayV,x,y+1, refSizeX), 6);
                    break;
                case 7:
                    //  horizontal
                    list.edgeSetLast( ARRAY(relayH,x,y, refSizeX), list.m_edge.size());
                    list.edgeSetPosition(ARRAY(relayH,x,y, refSizeX), x, y+1);
                    //  edgeSetRef(ARRAY(relayH,x,y, refSizeX), 7);
                    list.edgeTestToErase(ARRAY(relayH,x,y, refSizeX));
                    //  vertical
                    ARRAY(relayV,x,y+1, refSizeX)  =list. edgeAdd( -1, ARRAY(relayH,x,y, refSizeX));
                    break;
                case 8:
                    //  horizontal                
                    ARRAY(relayH,x+1,y, refSizeX)  = ARRAY(relayH,x,y, refSizeX);
                    list.edgeIncrPixelNb(ARRAY(relayH,x,y, refSizeX));
                    break;
                case 9:
                    //  vertical
                    ARRAY(relayV,x,y+1, refSizeX)  = ARRAY(relayV,x,y, refSizeX);
                    list.edgeIncrPixelNb(ARRAY(relayV,x,y, refSizeX));
                    break;
                case 10:
                    //  horizontal
                    ARRAY(relayH,x+1,y, refSizeX)  = ARRAY(relayH,x,y, refSizeX);
                    list.edgeIncrPixelNb(ARRAY(relayH,x,y, refSizeX));
                    break;
                case 11:
                    //  vertical
                    ARRAY(relayV,x,y+1, refSizeX)  = ARRAY(relayV,x,y, refSizeX);
                    list.edgeIncrPixelNb(ARRAY(relayV,x,y, refSizeX));
                    break;
                case 12:
                    break;

                default:
                    break;
                }
            }
        }

        //  DEBUG
        //     for (u32 i=0; i<m_Edge.size(); i++)
        //     {
        //         const Edge& e       = m_Edge[i];
        //         const Edge& eNext   = m_Edge[e.m_idEdgeNext];
        // 
        //         GFX_ADAPTER->drawDBGLine(e.m_pos, eNext.m_pos, 0,1,0,0);
        //     }


        //     for (u32 i=0; i<m_edgeToErase.size(); i++)
        //     {   
        //         Edge e  = m_Edge[m_edgeToErase[i]]; 
        //         GFX_ADAPTER->drawDBGCircle(e.m_pos.m_x, e.m_pos.m_y, 0.1f,1,0,0,0.f);
        // 
        //     }


        // STATE
        //  0  neutre
        // -1  deja fait
        //  1  a faire


        u32 nbEdgetoErase   = list.m_edgeToErase.size();

        //  smooth
        for (u32 i=0; i<nbEdgetoErase; i++)
        {
            u32   idEdgeCur = list.m_edgeToErase[i];
            Edge* eCur      = &list.m_edge[idEdgeCur]; 
            Edge* eLast     = NULL;
            Edge* eNext     = NULL;
            Edge* eNextNext = NULL;
            u32   idNewEdge = 0;

            i32 res = list.m_edge[eCur->m_idEdgeLast].m_state + list.m_edge[eCur->m_idEdgeNext].m_state*3; 

            list.edgeSetState(idEdgeCur,-1);

            switch (res)
            {
            case  0:
            case -1:
            case -3:
            case -4:
                //  edge suivant a effacer
                list.m_edge[eCur->m_idEdgeNext].m_erase = btrue;

                idNewEdge   = list.edgeAdd(eCur->m_idEdgeLast, list.m_edge[eCur->m_idEdgeNext].m_idEdgeNext);
                list.edgeSetState(idNewEdge,-1);

                eCur      = &list.m_edge[idEdgeCur]; 
                eLast     = &list.m_edge[eCur->m_idEdgeLast];
                eNext     = &list.m_edge[eCur->m_idEdgeNext];
                eNextNext = &list.m_edge[eNext->m_idEdgeNext];

                //  last
                eLast->m_idEdgeNext        = idNewEdge;
                //  next next
                eNextNext->m_idEdgeLast    = idNewEdge;

                break;

            case  1:
            case -2:

                //  edge suivant a effacer
                list.m_edge[eCur->m_idEdgeNext].m_erase = btrue;

                idNewEdge   = list.edgeAdd(idEdgeCur, list.m_edge[eCur->m_idEdgeNext].m_idEdgeNext);
                list.edgeSetState(idNewEdge,-1);

                eCur      = &list.m_edge[idEdgeCur]; 
                eNext     = &list.m_edge[eCur->m_idEdgeNext];
                eNextNext = &list.m_edge[eNext->m_idEdgeNext];

                //  cur
                eCur->m_idEdgeNext         = idNewEdge;
                //  next next
                eNextNext->m_idEdgeLast    = idNewEdge;

                break;

            case  2:
            case  3:
                idNewEdge   = list.edgeAdd(eCur->m_idEdgeLast, eCur->m_idEdgeNext);
                list.edgeSetState(idNewEdge,-1);

                eCur      = &list.m_edge[idEdgeCur]; 
                eLast     = &list.m_edge[eCur->m_idEdgeLast];
                eNext     = &list.m_edge[eCur->m_idEdgeNext];

                //  last
                eLast->m_idEdgeNext    = idNewEdge;
                //  next 
                eNext->m_idEdgeLast    = idNewEdge;
                //GFX_ADAPTER->drawDBGLine(eCur->m_pos, eNext->m_pos, 0,0,1,0.f);
                break;

            case  4:
                idNewEdge   = list.edgeAdd(idEdgeCur, eCur->m_idEdgeNext);
                list.edgeSetState(idNewEdge,-1);

                eCur      = &list.m_edge[idEdgeCur]; 
                eNext     = &list.m_edge[eCur->m_idEdgeNext];

                //  cur
                eCur->m_idEdgeNext     = idNewEdge;
                //  next 
                eNext->m_idEdgeLast    = idNewEdge;
                //GFX_ADAPTER->drawDBGLine(eCur->m_pos, eNext->m_pos, 1,1,1,0.f);
                break;


            }

            //  position
            list.m_edge[idNewEdge].m_pos = eCur->m_pos +(eNext->m_pos -eCur->m_pos)*0.5f;//float(eCur->m_pixelNb)/float(eCur->m_pixelNb+m_bitMapSmooth);


        }

        for (u32 i=0; i<list.m_edge.size(); i++)
        {
            const Edge& eCur       = list.m_edge[i];
            if (!eCur.m_erase)
            {
                const Edge& eNext   = list.m_edge[eCur.m_idEdgeNext];
                GFX_ADAPTER->drawDBGLine(eCur.m_pos, eNext.m_pos, 1,0,0,0.f);
            } 
        }



        Memory::free(relayH);
        Memory::free(relayV);
        Memory::free(ref);

#undef ARRAY


        if(!m_buildCollision)
            return;

        //    CONSTRUCTION
        list.buildBordsInVectorization(m_Friezes, m_Bords);
        buildCoins();
        setBordsTextureConfigBySlope();
        setStateCoins();
        //     addOutLine();
        setPositionsCoins();
        setArrondiCoinsPositifs();
        addThickness();
        buildRoundness();
        setArrondiCoinsNegatifs();
        createVertexsBords();
        createVertexsCoins();
        createVertexsAndUvsRoundness();
        setLengthBordsAndCoins();


        //  pour chaque frieze
        for ( u32 i=0; i<m_Friezes.size(); i++ )
        {
            u32 friezeIndexBordStart   = m_Friezes[i].m_idStartBord;
            u32 friezeIndexBordStop    = m_Friezes[i].m_idStopBord;

            if ( isLoopingMultiFrieze(i) )
                loopUvsMultiFrieze(i);
            else
                snapUVsMultiFrieze(i);


            ////////////////////////
            /////////////////////
            //    UVS
            /////////////////////
            ////////////////////////

            //************************************
            //    calcul du uvXstep de chaque bord
            //************************************
            for (u32 i=friezeIndexBordStart; i<friezeIndexBordStop; i++)
            {
                Bord& b  = m_Bords[i];
                b.m_uvXstep = b.m_long *m_uvXcoeffGlobal * b.m_textureConfig->m_xScaleFinal * b.m_uvXcoeff;// * float(b.m_vertexNb-2)/2.f;
            }

            //************************************
            //    calcul du uvXstep de chaque coin
            //************************************
            for (u32 i=friezeIndexBordStart; i<friezeIndexBordStop; i++)
            {
                Coin& c = m_Coins[i];
                if ( c.m_state==Actif )
                    c.m_uvXstep = c.m_long *m_uvXcoeffGlobal *MTH_FloatSign(c.m_angle) *m_txt_scaleCoinCoeff * c.m_textureConfig->m_xScaleFinal;

            }

            //*************************************
            //**********************************
            //    position des uvs pour les bords
            //**********************************
            //*************************************

            //  a modifier, les uvs des vertexs communs sont caluclés deux fois ici
            float   uvXcur      = 0.f;
            float   uvYdown     = 0.75f;
            float   uvYup       = uvYdown +0.25f;

            u32     index   = friezeIndexBordStart;

            do
            {
                //  get
                Bord& b = m_Bords[index];

                for ( u32 pt=0; pt<b.m_vertexNb; pt+=2)
                {
                    setUVpos(m_vertexsIndexs[b.m_vertexIndexDeb +pt],   uvXcur, uvYdown);
                    setUVpos(m_vertexsIndexs[b.m_vertexIndexDeb +pt +1], uvXcur, uvYup);
                    uvXcur  += b.m_uvXstep;
                }
                uvXcur  -= b.m_uvXstep;

                //  bord suivant
                index   = (u32)b.m_next;
            }while(index!=friezeIndexBordStart);

            //*************************************
            //**********************************
            //    position des uvs pour les coins
            //    décallage des uvs de bord
            //**********************************
            //*************************************
            for ( u32 i=friezeIndexBordStart; i<friezeIndexBordStop; i++)
            {
                const Coin& c = m_Coins[i];

                if ( c.m_state==Actif )
                {

                    int indexVertexG, indexVertexD; 

                    const Bord& bVoisin  = m_Bords[m_Bords[i].m_last];

                    float   uvX, uvY;

                    if (c.m_angle >0.f)
                    {
                        indexVertexG    = m_vertexsIndexs[bVoisin.m_vertexIndexDeb +bVoisin.m_vertexNb -1];
                        indexVertexD    = m_vertexsIndexs[m_Bords[i].m_vertexIndexDeb +1];
                        uvY = getUVpos(m_vertexsIndexs[m_Bords[i].m_vertexIndexDeb +1]).m_y;
                    }
                    else
                    {
                        indexVertexG    = m_vertexsIndexs[bVoisin.m_vertexIndexDeb +bVoisin.m_vertexNb -2];
                        indexVertexD    = m_vertexsIndexs[m_Bords[i].m_vertexIndexDeb];
                        uvY = getUVpos(m_vertexsIndexs[m_Bords[i].m_vertexIndexDeb]).m_y;
                    }

                    //    ecart entre le centre du coin et une extremite
                    float    uvEcart    = (1.f -(float(c.m_vertexNb)*0.5f) )*c.m_uvXstep;

                    //  décallage des uvs
                    Vec2d uvG   = getUVpos(indexVertexG);
                    Vec2d uvD   = getUVpos(indexVertexD);
                    uvX  = uvD.m_x;

                    setUVpos(indexVertexG, uvG.m_x +uvEcart, uvG.m_y);
                    setUVpos(indexVertexD, uvD.m_x -uvEcart, uvD.m_y);

                    //  uv du centre
                    setUVpos(m_vertexsIndexs[c.m_vertexIndexDeb+2], uvX, uvY);

                }
            }
        }
    }

    i32 EdgeList::edgeFindStart()
    {
        for (u32 i=0; i<m_edge.size(); i++)
            if( !m_edge[i].m_erase && !m_edge[i].m_addedToFrieze)
                return i;

        return -1;
    }


    void EdgeList::buildBordsInVectorization(ITF_VECTOR<Frise::Frieze> & _Friezes,ITF_VECTOR<Frise::Bord> & _borders )
    {
        //    normales, sight et pos des bords
        Frise::Bord    b;

        _borders.clear();

        _borders.reserve(m_edge.size());


        //  FRIEZE INDEX
        Frise::Frieze friezeIndex;
        friezeIndex.m_loop  = btrue;

        //  pour chaque frise a construire
        for(;;)
        {
            //  recherche d'une frise qui n'a pas deja été traitée
            i32 idEdgeStart   = edgeFindStart();
            if(idEdgeStart<0)
                break;

       //     GFX_ADAPTER->drawDBGCircle(m_edge[idEdgeStart].m_pos.m_x,m_edge[idEdgeStart].m_pos.m_y,0.1f,1,0,0,0.f,getDepth());

            //  add frieze start id
            friezeIndex.m_idStartBord = _borders.size();

            u32 idEdgeCur     = idEdgeStart;
            u32 idBordStart = _borders.size();


            //  test des bords a angle quasi nul entre eux
            Vec2d   sightNormOld(2.f,0.f);
            u32     idBordStartOptim = idBordStart;
            u32     cpt = 0;

            //  pour chaque edge
            for(;;)
            {
                Edge& e = m_edge[idEdgeCur];
                e.m_addedToFrieze   = btrue;

                b.m_pos         = e.m_pos;
                b.m_sight       = m_edge[e.m_idEdgeNext].m_pos -e.m_pos;
                b.m_sightNorm   = b.m_sight;
                b.m_sightNorm.normalize();
                b.m_normal      = b.m_sightNorm.getPerpendicular();
                b.m_last        = _borders.size()-1;


                //  fin de la frise??
                if (e.m_idEdgeNext==idEdgeStart )
                {
                    b.m_next = idBordStart;
                    _borders[idBordStart].m_last  = _borders.size();
                    _borders.push_back(b); 

                    friezeIndex.m_idStopBord = _borders.size();
                    _Friezes.push_back(friezeIndex);
                    break;
                }
                else
                {                
                    idEdgeCur   = e.m_idEdgeNext;

                    //  test si l'angle entre les bords est quasi nul
                    float   dot = b.m_sightNorm.dot(sightNormOld);

                    if ( dot<=0.9995f || dot>=1.0005f )
                    {
                        if(cpt>0)
                        {
                            Frise::Bord& bStart            = _borders[idBordStartOptim]; 
                            bStart.m_sight          = b.m_pos -bStart.m_pos;
                            bStart.m_sightNorm      = bStart.m_sight;
                            bStart.m_sightNorm.normalize();
                            bStart.m_normal         = bStart.m_sightNorm.getPerpendicular();
                        }

                        sightNormOld        = b.m_sightNorm;
                        idBordStartOptim    = _borders.size();
                        cpt                 = 0;

                        b.m_next    = _borders.size()+1;
                        _borders.push_back(b); 
                    }
                    else
                    {
                        cpt++;
                    }
                }
            }
        }
    }


    void EdgeList::edgeIncrPixelNb(u32 _idEdge)
    {
        m_edge[_idEdge].m_pixelNb++;
    }


    void EdgeList::edgeTestToErase(u32 _idEdge)
    {
        if ( m_edge[_idEdge].m_pixelNb<= m_bitMapSmooth )
        {
            m_edgeToErase.push_back(_idEdge);
            edgeSetState(_idEdge,1);
            m_edge[_idEdge].m_erase = btrue;
        }
    }


    void EdgeList::edgeDrawDebug()
    {


        for (u32 i=0; i<m_edge.size(); i++)
        {
            const Edge& e       = m_edge[i];
            const Edge& eNext   = m_edge[e.m_idEdgeNext];

            bbool edgeErase = bfalse;

            for (u32 j=0; j<m_edgeToErase.size(); j++)
                if ( i==m_edgeToErase[j] )
                    edgeErase   = btrue;

            if ( edgeErase)        
                GFX_ADAPTER->drawDBGLine(e.m_pos, eNext.m_pos, 1,0,0,0);
            else
                GFX_ADAPTER->drawDBGLine(e.m_pos, eNext.m_pos, 0,1,0,0);
        }
    }

    u32 EdgeList::edgeAdd( i32 _idLast, i32 _idNext)
    {
        Edge e;
        e.m_idEdgeLast  = _idLast;
        e.m_idEdgeNext  = _idNext;
        m_edge.push_back(e);

        return m_edge.size() -1;
    }

    void EdgeList::edgeSetState(u32 _idEdge, i32 _state)
    {
        m_edge[_idEdge].m_state= _state;
    }

    void EdgeList::edgeSetNext(u32 _idEdge, u32 _idNext)
    {
        m_edge[_idEdge].m_idEdgeNext  = _idNext;
    }

    void EdgeList::edgeSetLast(u32 _idEdge, u32 _idLast)
    {
        m_edge[_idEdge].m_idEdgeLast  = _idLast;
    }

    void EdgeList::edgeSetPosition(u32 _idEdge, u32 _x, u32 _y)
    {
        const PolyLineEdge&  edge0 = m_FriseRef->getEdgeAt(0);

        m_edge[_idEdge].m_pos = Vec2d(float(_x)*m_bitMapScale, -float(_y)*m_bitMapScale) + edge0.getPos();
    }

} // namespace ITF

#endif