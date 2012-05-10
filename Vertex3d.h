#ifndef VERTEX3D_H
#define VERTEX3D_H

#include <iostream>
#include <cmath>

using namespace std;

class Vertex3d
{
private:
    double m_v[3];

public:
    Vertex3d();
    Vertex3d(double*v);
    Vertex3d(double x, double y, double z);
    Vertex3d(Vertex3d* v);

    double* Get3dv();
    double X() const;
    double Y() const;
    double Z() const;

    double* XRef();
    double* YRef();
    double* ZRef();

    double length() const;
    void normalize();

    Vertex3d& operator = (const Vertex3d& v);
    Vertex3d& operator = (double *d);
    Vertex3d operator + (const Vertex3d& v);
    Vertex3d& operator += (const Vertex3d& v);
    Vertex3d operator - (const Vertex3d& v);
    Vertex3d& operator -= (const Vertex3d& v);
    Vertex3d operator * (const double& d);
    Vertex3d& operator *= (const double& d);
    Vertex3d& operator *= (const Vertex3d& v);
    double& operator [] (int i);

};


#endif // VERTEX3D_H
