#ifndef BSPLINESURFACEVIEW_H
#define BSPLINESURFACEVIEW_H

#include "ModelView.h"
#include "BSplineSurface.h"

#include <vector>
#include "Vertex3d.h"

class BSplineSurfaceView : public ModelView{
    Q_OBJECT

public:
    explicit BSplineSurfaceView(QWidget *parent = 0);

public slots:
    void AddRow();
    void AddColumn();

    void RemoveRow();
    void RemoveColumn();

private:
    virtual void Render(qint64 now);
    virtual void UpdatePickedObject(double dx, double dy);
    virtual void NewPickedObject();

    int m_dragOffsetX,m_dragOffsetY;

    BSplineSurface m_bSpline;

    vector<vector<Vertex3d> > m_controlPoints;

};

#endif // BSPLINESURFACEVIEW_H
