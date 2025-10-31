#ifndef _TRIANGULATE_H_
#define _TRIANGULATE_H_

namespace ITF
{

class Triangulate
{
public:

  static bbool Process(const ITF_VECTOR<Vec2d> &contour,
                      ITF_VECTOR<Vec2d> &result);

  static bbool ProcessIndexes(const ITF_VECTOR<Vec2d> &contour,
                      ITF_VECTOR<u16> &result);

  // compute area of a contour/polygon
  static float Area(const ITF_VECTOR<Vec2d> &contour);

  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bbool InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py);


private:
  static bbool Snip(const ITF_VECTOR<Vec2d> &contour,int u,int v,int w,int n,int *V);

};

}

#endif // _TRIANGULATE_H_
