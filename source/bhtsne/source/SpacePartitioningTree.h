
#pragma once

#include <array>
#include <memory>

#include <bhtsne/Vector2D.h>


namespace bhtsne {

    template<unsigned int D>
    class SpacePartitioningTree
    {
        // Axis-aligned bounding box stored as a center with half-dimensions to represent the boundaries of this quad tree
        std::array<double, D> m_centers;
        std::array<double, D> m_radii;

        // Indices in this space-partitioning tree node, corresponding center-of-mass, and list of all children
        const Vector2D<double> & m_data;
        std::array<double, D> m_centerOfMass;
        unsigned int m_pointIndex;

        // Properties of this node in the tree
        bool m_isLeaf;
        unsigned int m_cumulativeSize;

        // Children
        std::array<std::unique_ptr<SpacePartitioningTree>, 1u << D> m_children;
        unsigned int m_numberOfChildren;

    public:
        explicit SpacePartitioningTree(const Vector2D<double> & data);
        SpacePartitioningTree(const Vector2D<double> & data, const std::array<double, D> & centers,
                              const std::array<double, D> & radii, unsigned int new_index);
        SpacePartitioningTree(const SpacePartitioningTree & other) = delete;
        SpacePartitioningTree(SpacePartitioningTree && other) = default;

        void insert(unsigned int new_index);
        void insertIntoChild(unsigned int new_index);
        unsigned int childIndexForPoint(const double * point);

        // TODO make forces return value
        void computeNonEdgeForces(unsigned int pointIndex, double theta, double * forces, double & forceSum) const;
        void computeEdgeForces(const std::vector<unsigned int> & rows, const std::vector<unsigned int> & columns,
                               const std::vector<double> & values, Vector2D<double> & forces);
    };
}

#include "SpacePartitioningTree.inl"