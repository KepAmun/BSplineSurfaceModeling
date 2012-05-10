#include <iostream>
#include <sstream>
#include <math.h>
#include <limits>

#include <QTimer>
#include <QDateTime>

#include <QDebug>

#include "ModelView.h"

const float S_TO_MS = 1000;

ModelView::ModelView(QWidget *parent) :
    QGLWidget(parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(Timer()));

    m_quad = gluNewQuadric();

    double PI = 3.1415926;
    m_camAnglePhi = PI/4;
    m_camAngleTheta = PI/4;
    m_camDist = 150;

    m_camRotating = false;
    m_camZooming = false;
    m_draggingPoint = false;

    m_lastUpdate = -1;
    m_lastTimer = -1;

    m_maxFps = 30; // , 1000ms/30fps = 33.333ms delay between frames
    m_frameSlip = 0;

    m_renderMode = GL_RENDER;
    m_pickedName = -1;


    UpdateCamPosition();
    //UpdateClicked();
}


void ModelView::initializeGL()
{
    glClearColor(0,0,0,1.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    glShadeModel (GL_SMOOTH);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);


    glSelectBuffer( PICK_BUFFER_SIZE, PickBuffer );
}

void ModelView::resizeGL(int width, int height)
{
    glViewport(0,0,(GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void ModelView::UpdateClicked()
{
    if(m_maxFps > 0)
    {
        m_frameDeltaMs = S_TO_MS / (float)m_maxFps; // (ms/s) / (f/s) = (ms/s) * (s/f) = ms / f
        //m_frameSlip = 0;
        m_timer.start(m_frameDeltaMs+0.5);
    }
}


void ModelView::Timer()
{
    //qint64 now = QDateTime::currentMSecsSinceEpoch();

    //qint64 timeDelta = 0;

    //int nextFrameDelay = 0;

    //if(m_lastTimer > 0)
    //{
        //timeDelta = now - m_lastTimer;

        //clock_t compensation = m_frameDelayMs - timeDelta;

//        nextFrameDelay = m_frameDelayMs + m_frameSlip;// + compensation; //TODO: try to compensate for any lag durring the last frame.

//        while(m_frameSlip > 1)
//            m_frameSlip -= 1;

//        if(m_frameSlip < 0)
//            m_frameSlip = 0;

//        m_frameSlip += m_frameDelayMs - nextFrameDelay;

//        //cout << m_frameDelayMs << ", " << m_frameSlip << ", "<< delay << endl;

//        if(nextFrameDelay < 0)
//            nextFrameDelay = 0;
    //}



    //glDraw();
    update();

    //m_lastTimer = now;
}


void ModelView::paintGL()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();

    //std::cout << "now - m_lastUpdate == " << now - m_lastUpdate << endl;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };


    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    if(m_renderMode == GL_SELECT)
    {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT,viewport);

        gluPickMatrix( (double)m_lastMouseX, (double)(viewport[3] - m_lastMouseY), PICK_TOL, PICK_TOL, viewport );
    }


    gluPerspective(70,1,m_camDist*0.1,m_camDist*5);


    if( m_renderMode == GL_SELECT )
    {
        glInitNames();
        glPushName(-1);
    }


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    gluLookAt(m_camX, m_camY, m_camZ, 0, 0, 0, 0, 1, 0);


    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glPushMatrix();
    glTranslatef(60,60,60);
    GLfloat glowMaterial[] = {1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glowMaterial);
    gluSphere(m_quad,1,10,10);
    glPopMatrix();


    glPushMatrix();

    Render(now);

    glPopMatrix();


    //DrawHud(now);

    glFlush();


    //cout<<"now-(m_lastUpdate+34) == "<<now-(m_lastUpdate+34)<<endl;
    float a = now-(m_lastUpdate+34);
    if(a<0)
        a=0;
    //m_timer.start(34-a);//m_frameDeltaMs+0.5);

    m_lastUpdate = now; // 'now' is still the time Display() started
}


void ModelView::DrawHud(qint64 now)
{
    qint64 timeDelta = 0;
    if(m_lastUpdate > 0)
    {
        timeDelta = now - m_lastUpdate;
    }

    float fps = S_TO_MS/(float)timeDelta;

    emit fpsUpdated(fps);

}


void ModelView::mousePressEvent(QMouseEvent * event)
{
    if(m_pickedName != -1)
    {
        m_draggingPoint = true;
        NewPickedObject();
    }
    else if(event->button()==Qt::LeftButton && m_camRotating == false)
    {
        m_camRotating = true;
    }
    else if(event->button()==Qt::RightButton && m_camZooming == false)
    {
        m_camZooming = true;
    }

    m_lastMouseX = event->x();
    m_lastMouseY = event->y();
}


void ModelView::mouseMoveEvent(QMouseEvent * event)
{
    int dx = event->x() - m_lastMouseX;
    int dy = -(event->y() - m_lastMouseY);

    m_lastMouseX = event->x();
    m_lastMouseY = event->y();

    const float speed = 0.01f;

    if(m_draggingPoint)
    {
        UpdatePickedObject(dx,dy);
    }
    else if(!m_camRotating && !m_camZooming)
    {
        int lastPickedName = m_pickedName;
        m_pickedName = -1;
        int hits = 0;
        m_renderMode = GL_SELECT;
        glRenderMode(GL_SELECT);

        paintGL();

        m_renderMode = GL_RENDER;
        hits = glRenderMode(GL_RENDER);
        uint minHit = UINT_MAX;
        for( uint i = 0, index = 0; i < hits; i++ )
        {
            uint nitems = PickBuffer[index++];
            uint zmin = PickBuffer[index++];
            uint zmax = PickBuffer[index++];

            //qDebug()<<"Hit # "<<i<<": found "<<nitems<<" items on the name stack\n";
            //qDebug()<<"\tZmin = "<<zmin<<", Zmax = "<<zmax<<"\n";

            for( int j = 0; j < nitems; j++ )
            {
                int item = PickBuffer[index++];

                if(minHit > zmin)
                {
                    m_pickedName = item;
                    minHit = zmin;
                }
            }
        }

        if(lastPickedName != m_pickedName)
        {
            update();
        }
    }
    else if (m_camRotating)
    {
        m_camAngleTheta = (m_camAngleTheta + (dx * speed));
        m_camAnglePhi = (m_camAnglePhi + (dy * speed));

        const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844;
        const double TWOPI = 2*PI;

        if(m_camAnglePhi > PI-0.1)
            m_camAnglePhi = PI-0.1;
        if(m_camAnglePhi < 0.1)
            m_camAnglePhi = 0.1f;

        while(m_camAngleTheta > TWOPI)
            m_camAngleTheta -= TWOPI;
        while(m_camAngleTheta < 0)
            m_camAngleTheta += TWOPI;

        UpdateCamPosition();
    }
    else if(m_camZooming)
    {
        m_camDist *= 1 + (dy * speed);

        UpdateCamPosition();
    }

}

void ModelView::mouseReleaseEvent(QMouseEvent * event)
{
    if(m_draggingPoint == true)
    {
        m_draggingPoint = false;
        UpdatePickedObject(0,0);
    }
    else if(event->button()==Qt::LeftButton && m_camRotating == true)
    {
        m_camRotating = false;
    }
    else if(event->button()==Qt::RightButton && m_camZooming == true)
    {
        m_camZooming = false;
    }
}


void ModelView::UpdateCamPosition()
{
    m_camX = m_camDist * cos(m_camAngleTheta) * sin(m_camAnglePhi);
    m_camY = m_camDist * cos(m_camAnglePhi);
    m_camZ = m_camDist * sin(m_camAngleTheta) * sin(m_camAnglePhi);

    update();
}

