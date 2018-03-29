#pragma once

#include <vector>

namespace bhtsne {


template<typename T>
class Vector2D {
public:
    Vector2D() = default;
    Vector2D(size_t height, size_t width, T initValue = 0.0);
    Vector2D(Vector2D<T> && other);
    Vector2D(const std::vector<std::vector<double>> & vec);

    Vector2D<T> & operator=(Vector2D<T> && other);

    void initialize(size_t height, size_t width, T initValue = 0.0);
    void appendRow(const std::vector<T>& row);

    inline size_t size() const;
    inline size_t width() const;
    inline size_t height() const;

    inline typename std::vector<T>::iterator begin();
    inline typename std::vector<T>::iterator end();

    inline T * operator[](size_t i);
    inline const T * operator[](size_t i) const;
    inline T & at(size_t i, size_t j);

protected:
    std::vector<T> m_vector;
    size_t m_width;
};


}

#include <bhtsne/Vector2D.inl>
