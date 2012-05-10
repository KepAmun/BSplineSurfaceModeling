#include "bsplinesurfaceview.h"

#include <QDebug>

BSplineSurfaceView::BSplineSurfaceView(QWidget *parent) :
    ModelView(parent)
{
    m_maxFps = 0;

    int w=5,h=5;

    float cw = ((w-1)/2.0),ch = ((h-1)/2.0); // center w and h offsets

    for(int x=0;x<w;x++)
    {
        vector<Vertex3d> row;
        for(int z=0;z<h;z++)
        {
            Vertex3d v = Vertex3d(10*(x-cw),-((x-cw)*(x-cw)+(z-ch)*(z-ch))*2,10*(z-ch));
            row.push_back(v);
        }

        m_controlPoints.push_back(row);
    }

    m_bSpline.RefreshPoints(m_controlPoints);
    update();

     m_dragOffsetX = 0;
     m_dragOffsetY = 0;
}


void BSplineSurfaceView::AddRow()
{
    vector<Vertex3d> newRow;

    for(int x=0;x<m_controlPoints[0].size();x++)
    {
        Vertex3d v;

        Vertex3d neighbor = m_controlPoints[m_controlPoints.size()-1][x];
        v = Vertex3d(neighbor.X()+10,neighbor.Y(),neighbor.Z());

        newRow.push_back(v);
    }

    m_controlPoints.push_back(newRow);

    m_bSpline.RefreshPoints(m_controlPoints);
    update();
}

void BSplineSurfaceView::AddColumn()
{
    for(int z=0;z<m_controlPoints.size();z++)
    {
        Vertex3d v;

        Vertex3d neighbor = m_controlPoints[z][m_controlPoints[z].size()-1];
        v = Vertex3d(neighbor.X(),neighbor.Y(),neighbor.Z()+10);

        m_controlPoints[z].push_back(v);
    }

    m_bSpline.RefreshPoints(m_controlPoints);
    update();

}

void BSplineSurfaceView::RemoveRow()
{
    if(m_controlPoints.size() > 4)
    {
        m_controlPoints.pop_back();
        m_bSpline.RefreshPoints(m_controlPoints);
        update();
    }
}

void BSplineSurfaceView::RemoveColumn()
{
    if(m_controlPoints[0].size() > 4)
    {
        for(int z=0;z<m_controlPoints.size();z++)
        {
            m_controlPoints[z].pop_back();
        }
        m_bSpline.RefreshPoints(m_controlPoints);
        update();
    }

}

void BSplineSurfaceView::Render(qint64 now)
{

    if(m_renderMode == GL_RENDER)
    {
        glPushMatrix();

        // render surface
        m_bSpline.Render();

        glPopMatrix();
    }

    GLfloat hightlightAmbient[] = {0.2, 0.2, 0.0, 1.0};
    GLfloat hightlightDiffuse[] = {0.5, 0.5, 0.0, 1.0};
    GLfloat hightlightSpecular[] = {0.5, 0.5, 0.0, 1.0};
    GLfloat hightlightEmission[] = {0.5, 0.5, 0.0, 1.0};

    GLfloat basicAmbient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat basicDiffuse[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat basicSpecular[] = {0.5, 0.5, 0.5, 1.0};

    GLfloat blankMaterial[] = {0.0, 0.0, 0.0, 0.0};

    // render control points

    for(int x=0; x < m_controlPoints.size(); x++)
    {
        for(int y=0; y < m_controlPoints[x].size(); y++)
        {
            int name = (x << 16) | y;
            glLoadName(name);

            if(m_pickedName == name)
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT, hightlightAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, hightlightDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, hightlightSpecular);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, hightlightEmission);
            }
            else
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT, basicAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, basicDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, basicSpecular);
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blankMaterial);
            }

            glPushMatrix();
            glTranslated(m_controlPoints[x][y].X(),m_controlPoints[x][y].Y(),m_controlPoints[x][y].Z());
            gluSphere(m_quad,1,10,10);
            glPopMatrix();
        }
    }

}

void BSplineSurfaceView::UpdatePickedObject(double dx, double dy)
{
    int cx = m_pickedName >> 16;
    int cy = m_pickedName & 0xFFFF;

    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Vertex3d cpWorld = Vertex3d(); // control point in world coordinates
    int x = m_lastMouseX - m_dragOffsetX;
    int y = (viewport[3] - m_lastMouseY) - m_dragOffsetY;
    gluUnProject(x,y,m_pickedZ,model_view,projection,viewport,cpWorld.XRef(),cpWorld.YRef(),cpWorld.ZRef());

    m_controlPoints[cx][cy] = cpWorld;

    //Vertex3d v = Vertex3d(dx*0.1, dy*0.1, 0);
    //m_controlPoints[cx][cy] += v;

    int resolution = 128;
    if(m_draggingPoint)
        resolution = 16;

    m_bSpline.RefreshPoints(m_controlPoints, resolution);
    update();
}

void BSplineSurfaceView::NewPickedObject()
{
    int cx = m_pickedName >> 16;
    int cy = m_pickedName & 0xFFFF;

    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);


    Vertex3d cpWorld = Vertex3d(m_controlPoints[cx][cy]); // control point in world coordinates
    Vertex3d cpScreen = Vertex3d(); // control point in screen coordinates
    gluProject(cpWorld.X(),cpWorld.Y(),cpWorld.Z(),model_view,projection,viewport,cpScreen.XRef(),cpScreen.YRef(),cpScreen.ZRef());

    // Mouse offset to control point to maintain relative distance. - NOT WORKING
    m_dragOffsetX = 0;//m_lastMouseX - cpScreen.X();
    m_dragOffsetY = 0;//(viewport[3]-m_lastMouseY) - cpScreen.Y();

    m_pickedZ = cpScreen.Z();
}


