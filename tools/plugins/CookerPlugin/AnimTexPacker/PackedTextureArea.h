#ifndef _ITF_PACKEDTEXTUREAREA_H_
#define _ITF_PACKEDTEXTUREAREA_H_

#ifndef _ITF_ASSERTMANAGER_H_
#include "core/AssertManager.h"
#endif //_ITF_ASSERTMANAGER_H_

namespace ITF
{
    namespace AnimTexPacker
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        struct PointI
        {
            int x,y;
        };
        ///////////////////////////////////////////////////////////////////////////////////////////
        struct BoxI
        {
            BoxI() {}
            BoxI(int _minx, int _miny, int _maxx, int _maxy) :
            minx(_minx)
                ,maxx(_maxx)
                ,miny(_miny)
                ,maxy(_maxy)
            {}

            bool operator==(const BoxI& _other) const
            {
                return minx ==_other.minx &&
                    miny == _other.miny &&
                    maxx == _other.maxx &&
                    maxy == _other.maxy;
            }
            bool operator!=(const BoxI& _other) const
            {
                return !operator==(_other);
            }

            int minx, miny, maxx, maxy;
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Area where we place subtextures
        class PackedTextureArea
        {
        public:
            PackedTextureArea(int _width, int _height) : 
              m_width(_width)
                  , m_height(_height) 
              {
                  m_board = newAlloc(mId_Temporary, bool[_width*_height]);
                  ITF_MemSet(m_board, 0, sizeof(m_board[0])*_width*_height);
                  m_remarkableXs.insert(0);
                  m_remarkableYs.insert(0);
                  m_remarkableXs.insert(m_width);
                  //m_remarkableYs.insert(m_height);
              }
              ~PackedTextureArea() {SF_DEL(m_board);}

              inline int getWidth() const {return m_width;}
              inline int getHeight() const {return m_height;}

              inline bool getBoard(int x, int y) const
              {
                  ITF_ASSERT(x>=0 && x<m_width && y>=0 && y<m_height); 
                  return m_board[x+y*m_width];
              }
              inline void setBoard(int x, int y, bool _value)
              {
                  ITF_ASSERT(x>=0 && x<m_width && y>=0 && y<m_height); 
                  m_board[x+y*m_width] = _value;
              }        

              inline bool testBorder(int x, int y) const
              {
                  if (x>=0 && x<m_width && y>=0 && y<m_height)
                      return m_board[x+y*m_width];
                  return true;
              }

              inline void computeRemarkablePlacesForBox(ITF_SET<int> &_placesX, ITF_SET<int> &_placesY, int w, int h)
              {
                  _placesX.clear();
                  _placesY.clear();
                  ITF_SET<int>::const_iterator x_iter, y_iter;
                  for (y_iter=m_remarkableYs.begin(); y_iter!=m_remarkableYs.end(); y_iter++)
                  {
                      int y1 = *y_iter;
                      int y2 = y1-h;
                      if (y1>=0 && y1<m_height)
                          _placesY.insert(y1);
                      if (y2>=0 && y2<m_height)
                          _placesY.insert(y2);
                  }
                  for (x_iter=m_remarkableXs.begin(); x_iter!=m_remarkableXs.end(); x_iter++)
                  {
                      int x1 = *x_iter;
                      int x2 = x1-w;
                      if (x1>=0 && x1<m_width)
                          _placesX.insert(x1);
                      if (x2>=0 && x2<m_width)
                          _placesX.insert(x2);
                  }

              }

              void fillBox(BoxI &_b)
              {
                  int x, y;

                  x = _b.minx;
                  if (x>=0)
                      m_remarkableXs.insert(x);

                  y = _b.miny;
                  if (y>=0)
                      m_remarkableYs.insert(y);

                  x = _b.maxx+1;
                  if (x<=m_width)
                      m_remarkableXs.insert(x);

                  y = _b.maxy+1;
                  if (y<=m_height)
                      m_remarkableYs.insert(y);

                  fillArea(_b, btrue);

              }


              bool isAreaFree(int offsetx, int offsety, int width, int height)
              {
                  if (offsetx + width > m_width)
                      return false;
                  if (offsety + height > m_height)
                      return false;
                  if (offsetx <0)
                      return false;
                  if (offsety <0)
                      return false;
                  for (int y = offsety; y < height + offsety; y++)
                      for (int x = offsetx; x < width + offsetx; x++)
                      {
                          if (getBoard(x, y))
                              return false;
                      }
                      return true;
              }

        private:
            void fillArea(BoxI &_b, bool _v)
            {
                for (int y = _b.miny; y <= _b.maxy; y++)
                    for (int x = _b.minx; x <= _b.maxx; x++)
                        setBoard(x, y, _v);
            }


            int m_width, m_height;
            ITF_SET<int> m_remarkableXs, m_remarkableYs;
            bool *m_board;
        };
    }
}

#endif

