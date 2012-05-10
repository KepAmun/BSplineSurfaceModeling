#ifndef BSPLINESURFACE_H
#define BSPLINESURFACE_H

#include <GL/glu.h>
#include <vector>
#include "Vertex3d.h"

using namespace std;

class BSplineSurface
{
public:
    BSplineSurface();
    double BasisFunction(double u, int i, vector<double> &knots, int k=4);
    void RefreshPoints(vector<vector<Vertex3d> > controlPoints, int resolution = 128);
    void Render();

private:
    GLUquadric* m_quad;
    vector<vector<Vertex3d> > m_verteces;
    //vector<double> m_knots;
    vector<double> GetKnots(int numControlPoints);
};

#endif // BSPLINESURFACE_H
