
#include <bhtsne/AlignedVector2D.h>

#include <cassert>


namespace bhtsne 
{


AlignedVector2D::AlignedVector2D(size_t height, size_t width, double initValue)
{
    initialize(height, width, initValue);
}

AlignedVector2D::AlignedVector2D(AlignedVector2D && other)
{
    m_vector = std::move(other.m_vector);
    m_width = other.m_width;
    m_internal_width = other.m_internal_width;
}

AlignedVector2D::AlignedVector2D(std::vector<std::vector<double>> vec)
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

AlignedVector2D & AlignedVector2D::operator=(AlignedVector2D && other)
{
    m_vector = std::move(other.m_vector);
    m_width = other.m_width;
    m_internal_width = other.m_internal_width;
    return *this;
}

void AlignedVector2D::initialize(size_t height, size_t width, double initValue)
{
    // next internal width geq width and is a multiple of 4 (so 4*sizeof(double) == 32) 
    m_internal_width = width % 4 ? width + 4 - (width % 4) : width;
    assert(m_internal_width % 32 == 0 && m_internal_width > width); 
    m_width = width;
    m_vector.resize(height*m_internal_width, initValue);
}

void AlignedVector2D::appendRow(std::vector<double>& row)
{
    assert(m_width == row.size());
    m_vector.insert(m_vector.end(), row.begin(), row.end());
    int i = m_internal_width - m_width;
    while (i--)
    {
        m_vector.push_back(0.0);
    }
}

size_t AlignedVector2D::size() const
{
    return height() * width();
}

size_t AlignedVector2D::width() const
{
    return m_width;
}

size_t AlignedVector2D::height() const
{
    assert(m_internal_width != 0);
    return m_vector.size() / m_internal_width;
}

AlignedVector2DIterator AlignedVector2D::begin()
{
    return AlignedVector2DIterator(m_vector.data(), m_width, m_internal_width-m_width);
}

AlignedVector2DIterator AlignedVector2D::end()
{
    return AlignedVector2DIterator(m_vector.data() + m_vector.size(), m_width, m_internal_width - m_width);
}

double * AlignedVector2D::operator[](size_t i)
{
    assert(i < height());
    return m_vector.data() + (i * m_internal_width);
}

const double * AlignedVector2D::operator[](size_t i) const
{
    assert(i < height());
    return m_vector.data() + (i * m_internal_width);
}

double & AlignedVector2D::at(size_t i, size_t j)
{
    assert(i < height());
    return m_vector.at(i * m_internal_width + j);
}


} //bhtsne