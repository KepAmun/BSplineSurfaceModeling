#include "BSplineSurface.h"
#include <QDebug>
#include <math.h>

BSplineSurface::BSplineSurface()
{
    m_quad = gluNewQuadric();

//    m_knots.push_back(0);
//    m_knots.push_back(0);
//    m_knots.push_back(0);
//    m_knots.push_back(0);

//    //m_knots.push_back(0.25);
//    m_knots.push_back(0.5);
//    //m_knots.push_back(0.75);

//    m_knots.push_back(1);
//    m_knots.push_back(1);
//    m_knots.push_back(1);
//    m_knots.push_back(1);

//    for(int i=0;i<4+4+1;i++)
//    {
//        //m_knots.push_back(i/8.0);
//    }

}

vector<double> BSplineSurface::GetKnots(int numControlPoints)
{
    vector<double> knots;
    int numKnots = numControlPoints + 4;

    for(int i=0;i<numKnots/2 && i<4;i++)
    {
        knots.push_back(0);
    }

    int innerKnots = numKnots - 8;
    if(innerKnots<0 && numKnots%2 != 0)
        innerKnots = 1;

    for(int i=1;i<=innerKnots;i++)
    {
        knots.push_back(i/(float)(innerKnots+1));
    }

    for(int i=0;i<numKnots/2 && i<4;i++)
    {
        knots.push_back(1);
    }


    return knots;
}

void BSplineSurface::RefreshPoints(vector<vector<Vertex3d> > controlPoints, int resolution)
{
    vector<double> knotsX = GetKnots(controlPoints[0].size());
    vector<double> knotsZ = GetKnots(controlPoints.size());

    m_verteces.clear();

    double delta = 1.0/resolution;

    for(double u=0; u<=1; u+=delta)
    {
        vector<Vertex3d> row;

        for(double v=0; v<=1; v+=delta)
        {
            Vertex3d vertex;

            for(int i=0;i<controlPoints.size();i++)
            {
                for(int j=0;j<controlPoints[0].size();j++)
                {
                    double b1 = BasisFunction(u,i,knotsZ);
                    double b2 = BasisFunction(v,j,knotsX);

                    double cx = controlPoints[i][j].X();
                    double cy = controlPoints[i][j].Y();
                    double cz = controlPoints[i][j].Z();

                    vertex += controlPoints[i][j] * (b1 * b2);
                }
            }

            //qDebug() << row.size() << "," << m_verteces.size()  << "; " << u  << "," << v <<" : " << vertex.X() << "," << vertex.Y() << "," << vertex.Z() << endl;

            row.push_back(vertex);
        }

        m_verteces.push_back(row);
    }

}


void BSplineSurface::Render()
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glBegin(GL_QUADS);

    glVertex3f(-40,-15,40);
    glVertex3f(40,-15,40);
    glVertex3f(40,-15,-40);
    glVertex3f(-40,-15,-40);

    glEnd();

    glPolygonMode( GL_FRONT, GL_FILL );
    glPolygonMode( GL_BACK, GL_LINE );


    GLfloat surfaceAmbient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat surfaceDiffuse[] = {0.6, 0.6, 0.6, 1.0};
    GLfloat surfaceSpecular[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat blankColor[] = {0.0, 0.0, 0.0, 0.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, surfaceAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, surfaceDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, surfaceSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blankColor);
    glMaterialf(GL_FRONT, GL_SHININESS, 40);

    glBegin(GL_QUADS);
    {
        for(int x=0;x<m_verteces.size()-1;x++)
        {
            for(int y=0;y<m_verteces[x].size()-1;y++)
            {
                Vertex3d v0 = m_verteces[x][y];
                Vertex3d v1 = m_verteces[x][y+1];
                Vertex3d v2 = m_verteces[x+1][y+1];
                Vertex3d v3 = m_verteces[x+1][y];

                // TODO: precalculate normals
                Vertex3d l1 = v1-v0;
                Vertex3d l2 = v3-v0;
                float nx = (l1.Y()*l2.Z()) - (l1.Z()*l2.Y());
                float ny = (l1.Z()*l2.X()) - (l1.X()*l2.Z());
                float nz = (l1.X()*l2.Y()) - (l1.Y()*l2.X());
                Vertex3d n = Vertex3d(nx,ny,nz);

                glNormal3dv(n.Get3dv());
                glVertex3dv(v0.Get3dv());
                glVertex3dv(v1.Get3dv());
                glVertex3dv(v2.Get3dv());
                glVertex3dv(v3.Get3dv());
            }
        }
    }
    glEnd();

}

double BSplineSurface::BasisFunction(double u, int i, vector<double> &knots, int k)
{
    double result = 0;

    if(k == 1)
    {
        if((knots[i]<knots[i+1] && knots[i] <= u && u < knots[i+1]) || (u==1 && knots[i+1]==1))
        {
            result = 1;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        double r1, r2;

        double knotDelta1 = knots[i+k-1] - knots[i];
        double knotDelta2 = knots[i+k]   - knots[i+1];

        if(knotDelta1 != 0)
        {
            r1 = ((u-knots[i]) / knotDelta1) * BasisFunction(u,i,knots,k-1);
        }
        else
        {
            r1 = 0;
        }

        if(knotDelta2 != 0)
        {
            r2 = ((knots[i+k]-u) / knotDelta2) * BasisFunction(u,i+1,knots,k-1);
        }
        else
        {
            r2 = 0;
        }

        result = r1 + r2;
    }

    return result;
}


