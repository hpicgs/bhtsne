#pragma once

#include <vector>
#include <iterator>

#include "Allocator.h"
#include <bhtsne/bhtsne_api.h>


namespace bhtsne {

class BHTSNE_API AlignedVector2DIterator : public std::iterator<std::forward_iterator_tag, double>
{
    double* p;
    int data;
    int padding;
    int current;
public:
    AlignedVector2DIterator(double* np, int ndata, int npadding)
        : p(np), data(ndata), padding(npadding), current(0) {}
    AlignedVector2DIterator(const AlignedVector2DIterator& mit) 
        : p(mit.p), data(mit.data), padding(mit.padding), current(mit.current) {}
    AlignedVector2DIterator& operator++() { 
        ++p; 
        if (++current == data) {
            current = 0;
            p += padding;
        }
        return *this;
    }
    AlignedVector2DIterator operator++(int) {
        auto old = *this;
        ++(*this);
        return old;
    }
    bool operator==(const AlignedVector2DIterator& rhs) const { return p == rhs.p; }
    bool operator!=(const AlignedVector2DIterator& rhs) const { return p != rhs.p; }
    double& operator*() { return *p; }
};


class BHTSNE_API AlignedVector2D {
public:
    AlignedVector2D() = default;
    AlignedVector2D(size_t height, size_t width, double initValue = 0.0);
    AlignedVector2D(AlignedVector2D && other);
    AlignedVector2D(std::vector<std::vector<double>> vec);

    AlignedVector2D & operator=(AlignedVector2D && other);

    void initialize(size_t height, size_t width, double initValue = 0.0);
    void appendRow(std::vector<double>& row);

    size_t size() const;
    size_t width() const;
    size_t height() const;

    AlignedVector2DIterator begin();
    AlignedVector2DIterator end();

    double * operator[](size_t i);
    const double * operator[](size_t i) const;
    double & at(size_t i, size_t j);

protected:
    std::vector<double, aligned_allocator<double, 32>> m_vector;
    size_t m_width;
    size_t m_internal_width; // with padding for alignment
};


} // namespace bhtsne