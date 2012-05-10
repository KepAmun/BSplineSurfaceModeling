#ifndef OPENGLVIEW_H
#define OPENGLVIEW_H

#include <QGLWidget>
#include <GL/glu.h>
#include <QTimer>
#include <QMouseEvent>

#define PICK_TOL   1.
#define PICK_BUFFER_SIZE  256

class ModelView: public QGLWidget
{
    Q_OBJECT

private:

    QTimer m_timer;

    float m_frameDeltaMs;
    float m_frameSlip;

    float m_camAnglePhi;
    float m_camAngleTheta;
    float m_camDist;
    float m_camX;
    float m_camY;
    float m_camZ;

    bool m_camRotating;
    bool m_camZooming;


    void UpdateClicked();

    void UpdateCamPosition();

    void DrawHud(qint64 now);


    qint64 m_lastUpdate;
    qint64 m_lastTimer;

public:
    explicit ModelView(QWidget *parent = 0);

protected:
    void initializeGL();
    void resizeGL(int x, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    virtual void Render(qint64 now)=0;
    virtual void UpdatePickedObject(double dx, double dy)=0;
    virtual void NewPickedObject()=0;


    int m_lastMouseX;
    int m_lastMouseY;

    int m_maxFps;
    int m_renderMode;
    unsigned int  PickBuffer[PICK_BUFFER_SIZE];
    int m_pickedName;
    double m_pickedZ;
    bool m_draggingPoint;

    GLUquadric* m_quad;


signals:
    void fpsUpdated(double fps);

public slots:
    void Timer();

};

#endif // OPENGLVIEW_H
