
#include <bhtsne/Vector2D.h>

#include <cassert>


namespace bhtsne 
{


Vector2D::Vector2D(size_t height, size_t width, double initValue)
{
    initialize(height, width, initValue);
}

Vector2D::Vector2D(Vector2D && other)
{
    m_vector = std::move(other.m_vector);
    m_width = other.m_width;
    m_internal_width = other.m_internal_width;
}

Vector2D::Vector2D(std::vector<std::vector<double>> vec)
{
    if (vec.size() > 0)
    {
        m_width = vec.front().size();
        m_internal_width = m_width % 4 ? m_width + 4 - (m_width % 4) : m_width;
        for (auto & each : vec)
        {
            appendRow(each);
        }
    }
}

Vector2D & Vector2D::operator=(Vector2D && other)
{
    m_vector = std::move(other.m_vector);
    m_width = other.m_width;
    m_internal_width = other.m_internal_width;
    return *this;
}

void Vector2D::initialize(size_t height, size_t width, double initValue)
{
    // next internal width geq width and is a multiple of 4 (so 4*sizeof(double) == 32) 
    m_internal_width = width % 4 ? width + 4 - (width % 4) : width;
    assert(m_internal_width % 32 == 0 && m_internal_width > width); 
    m_width = width;
    m_vector.resize(height*m_internal_width, initValue);
}

void Vector2D::appendRow(std::vector<double>& row)
{
    assert(m_width == row.size());
    m_vector.insert(m_vector.end(), row.begin(), row.end());
    int i = m_internal_width - m_width;
    while (i--)
    {
        m_vector.push_back(0.0);
    }
}

size_t Vector2D::size() const
{
    return height() * width();
}

size_t Vector2D::width() const
{
    return m_width;
}

size_t Vector2D::height() const
{
    assert(m_internal_width != 0);
    return m_vector.size() / m_internal_width;
}

Vector2DIterator Vector2D::begin()
{
    return Vector2DIterator(m_vector.data(), m_width, m_internal_width-m_width);
}

Vector2DIterator Vector2D::end()
{
    return Vector2DIterator(m_vector.data() + m_vector.size(), m_width, m_internal_width - m_width);
}

double * Vector2D::operator[](size_t i)
{
    assert(i < height());
    return m_vector.data() + (i * m_internal_width);
}

const double * Vector2D::operator[](size_t i) const
{
    assert(i < height());
    return m_vector.data() + (i * m_internal_width);
}

double & Vector2D::at(size_t i, size_t j)
{
    assert(i < height());
    return m_vector.at(i * m_internal_width + j);
}


} //bhtsne