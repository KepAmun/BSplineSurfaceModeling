#include "Vertex3d.h"

Vertex3d::Vertex3d()
{
    m_v[0] = m_v[1] = m_v[2] = 0;
}

Vertex3d::Vertex3d(double *v)
{
    m_v[0] = v[0];
    m_v[1] = v[1];
    m_v[2] = v[2];
}

Vertex3d::Vertex3d(double x, double y, double z)
{
    m_v[0] = x;
    m_v[1] = y;
    m_v[2] = z;
}

Vertex3d::Vertex3d(Vertex3d* v)
{
    m_v[0] = v->m_v[0];
    m_v[1] = v->m_v[1];
    m_v[2] = v->m_v[2];
}

double *Vertex3d::Get3dv()
{
    return m_v;
}

double Vertex3d::X() const
{
    return m_v[0];
}

double Vertex3d::Y() const
{
    return m_v[1];
}

double Vertex3d::Z() const
{
    return m_v[2];
}

double *Vertex3d::XRef()
{
    return &m_v[0];
}

double *Vertex3d::YRef()
{
    return &m_v[1];
}

double *Vertex3d::ZRef()
{
    return &m_v[2];
}


double Vertex3d::length() const
{
    return sqrt(m_v[0]*m_v[0] + m_v[1]*m_v[1] + m_v[2]*m_v[2]);
}

void Vertex3d::normalize()
{
    double len = length();
    if (len != 0)
    {
        m_v[0] /= len;
        m_v[1] /= len;
        m_v[2] /= len;
    }
}

Vertex3d& Vertex3d::operator = (const Vertex3d& v)
{
    if(this != &v)
    {
        m_v[0] = v.m_v[0];
        m_v[1] = v.m_v[1];
        m_v[2] = v.m_v[2];
    }

    return *this;
}

Vertex3d& Vertex3d::operator = (double *pt)
{
    m_v[0] = pt[0];
    m_v[1] = pt[1];
    m_v[2] = pt[2];

    return *this;
}

Vertex3d Vertex3d::operator + (const Vertex3d& v)
{
    Vertex3d result = Vertex3d(this);

    return result += v;
}

Vertex3d& Vertex3d::operator += (const Vertex3d& v)
{
    m_v[0] += v.m_v[0];
    m_v[1] += v.m_v[1];
    m_v[2] += v.m_v[2];

    return *this;
}

Vertex3d Vertex3d::operator -(const Vertex3d &v)
{
    Vertex3d result = Vertex3d(this);

    return result -= v;
}

Vertex3d& Vertex3d::operator -=(const Vertex3d &v)
{
    m_v[0] -= v.m_v[0];
    m_v[1] -= v.m_v[1];
    m_v[2] -= v.m_v[2];

    return *this;
}

Vertex3d Vertex3d::operator * (const double &d)
{
    Vertex3d result = Vertex3d(this);

    result.m_v[0] *= d;
    result.m_v[1] *= d;
    result.m_v[2] *= d;

    return result;
}

Vertex3d& Vertex3d::operator *= (const double &d)
{
    m_v[0] *= d;
    m_v[1] *= d;
    m_v[2] *= d;

    return *this;
}

Vertex3d& Vertex3d::operator *= (const Vertex3d& v)
{
    m_v[0] *= v.m_v[0];
    m_v[1] *= v.m_v[1];
    m_v[2] *= v.m_v[2];

    return *this;
}

double& Vertex3d::operator [] (int i)
{
    return m_v[i];
}

static Vertex3d operator * (const double d, const Vertex3d v)
{
    return d * v;
}

static ostream& operator << (ostream& stream, const Vertex3d& v)
{
    stream << "(" << v.X()
           << ", " << v.Y()
           << ", " << v.Z() << ")";

    return stream;
}
