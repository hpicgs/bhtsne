#pragma once

#include <vector>

#include <bhtsne/bhtsne_api.h>


namespace bhtsne {


class BHTSNE_API Vector2D {
public:
    Vector2D() = default;
    Vector2D(size_t height, size_t width, double initValue = 0.0);
    Vector2D(Vector2D && other);
    Vector2D(std::vector<std::vector<double>> vec);

    Vector2D & operator=(Vector2D && other);

    void initialize(size_t height, size_t width, double initValue = 0.0);
    void appendRow(std::vector<double>& row);

    size_t size() const;
    size_t width() const;
    size_t height() const;

    std::vector<double>::iterator begin();
    std::vector<double>::iterator end();

    inline double * operator[](size_t i);
    inline const double * operator[](size_t i) const;
    inline double & at(size_t i, size_t j);

protected:
    std::vector<double> m_vector;
    size_t m_width;
};


} // namespace bhtsne