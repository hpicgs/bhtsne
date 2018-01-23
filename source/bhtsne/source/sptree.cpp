/*
 *
 * Copyright (c) 2014, Laurens van der Maaten (Delft University of Technology)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Delft University of Technology.
 * 4. Neither the name of the Delft University of Technology nor the names of
 *    its contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LAURENS VAN DER MAATEN ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL LAURENS VAN DER MAATEN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <cmath>
#include <limits>
#include <algorithm>

#include <bhtsne/sptree.h>

using namespace bhtsne;

// Constructs cell
SPTree::Cell::Cell() = default;

SPTree::Cell::Cell(unsigned int dimensions, std::vector<double> centers, std::vector<double> radii)
    : m_dimensions(dimensions)
    , m_centers(std::move(centers))
    , m_radii(std::move(radii))
{
}

// Checks whether a point lies in a cell
bool SPTree::Cell::containsPoint(const double* point)
{
    for(auto d = 0; d < m_dimensions; ++d)
    {
        if (m_centers[d] - m_radii[d] > point[d])
        {
            return false;
        }
        if (m_centers[d] + m_radii[d] < point[d])
        {
            return false;
        }
    }
    return true;
}


// Default constructor for SPTree -- build tree, too!
SPTree::SPTree(const Vector2D<double> &data) : m_data(data)
{
    auto dimensions = data.width();
    auto numberOfPoints = data.height();
    // Compute mean, width, and height of current map (boundaries of SPTree)
    std::vector<double> mean_Y = std::vector<double>(dimensions);
    std::vector<double> min_Y = std::vector<double>(dimensions);
    std::vector<double> max_Y = std::vector<double>(dimensions);
    for (auto d = 0u; d < dimensions; ++d)
    {
        min_Y[d] = std::numeric_limits<double>::max();
        max_Y[d] = std::numeric_limits<double>::min();
    }
    for(auto n = 0u; n < numberOfPoints; ++n) {
        for(auto d = 0u; d < dimensions; ++d) {
            auto value = data[n][d];
            mean_Y[d] += value;
            min_Y[d] = std::min(min_Y[d], value);
            max_Y[d] = std::max(max_Y [d], value);
        }
    }
    for (auto d = 0u; d < dimensions; ++d)
    {
        mean_Y[d] /= numberOfPoints;
    }

    // Construct SPTree
    auto width = std::vector<double>(dimensions);
    auto delta = 1e-5;
    for (int d = 0; d < dimensions; d++)
    {
        width[d] = std::max(max_Y[d] - mean_Y[d], mean_Y[d] - min_Y[d]) + delta;
    }

    init(mean_Y, width);
    fill(numberOfPoints);
}


// Constructor for SPTree with particular size (do not fill the tree)
SPTree::SPTree(const Vector2D<double> &data, std::vector<double> centers, std::vector<double> radii) : m_data(data)
{
    init(centers, radii);
}


// Main initialization function
void SPTree::init(std::vector<double> centers, std::vector<double> radii)
{
    m_dimensions = m_data.width();
    m_numberOfChildren = 2;
    for (auto d = 1u; d < m_dimensions; ++d)
    {
        m_numberOfChildren *= 2;
    }
    m_isLeaf = true;
    m_cumulativeSize = 0;

    m_boundary = Cell(m_dimensions, centers, radii);

    m_children = std::vector<std::unique_ptr<SPTree>>(m_numberOfChildren);
    m_centerOfMass = std::vector<double>(m_dimensions, .0);
    m_pointIndices = std::vector<double>();
}

// Insert a point into the SPTree
bool SPTree::insert(unsigned int new_index)
{
    // Ignore objects which do not belong in this quad tree
    const auto point = m_data[new_index];
    if (!m_boundary.containsPoint(point))
    {
        return false;
    }

    // Online update of cumulative size and center-of-mass
    m_cumulativeSize++;
    auto avgAdjustment = (m_cumulativeSize - 1.0) / m_cumulativeSize;
    for (auto d = 0u; d < m_dimensions; ++d)
    {
        m_centerOfMass[d] *= avgAdjustment;
        m_centerOfMass[d] += point[d] / m_cumulativeSize;
    }

    // If there is space in this quad tree and it is a leaf, add the object here
    if(m_isLeaf && m_pointIndices.size() < QT_NODE_CAPACITY) {
        m_pointIndices.push_back(new_index);
        return true;
    }

    // Don't add duplicates for now (this is not very nice)
    auto anyDuplicate = false;
    for(auto otherPoint : m_pointIndices) {
        bool duplicate = true;
        for(unsigned int d = 0; d < m_dimensions; d++) {
            if(point[d] != m_data[otherPoint][d])
            {
                duplicate = false;
                break;
            }
        }
        if (duplicate)
        {
            anyDuplicate = true;
            break;
        }
    }
    if (anyDuplicate)
    {
        return true;
    }

    // Otherwise, we need to subdivide the current cell
    if (m_isLeaf)
    {
        subdivide();
    }

    // Find out where the point can be inserted
    for (auto& child : m_children)
    {
        if (child->insert(new_index))
        {
            return true;
        }
    }

    // Otherwise, the point cannot be inserted (this should never happen)
    return false;
}


// Create four children which fully divide this cell into four quads of equal area
void SPTree::subdivide() {

    // Create new children
    auto centers = std::vector<double>(m_dimensions);
    auto radii  = std::vector<double>(m_dimensions);
    for(auto i = 0u; i < m_numberOfChildren; ++i)
    {
        auto level = 1u;
        for(auto d = 0u; d < m_dimensions; ++d)
        {
            auto halfRadius = m_boundary.m_radii[d] / 2;
            radii[d] = halfRadius;
            if ((i / level) % 2 == 1)
            {
                centers[d] = m_boundary.m_centers[d] - halfRadius;
            }
            else
            {
                centers[d] = m_boundary.m_centers[d] + halfRadius;
            }
            level *= 2;
        }
        m_children[i] = std::make_unique<SPTree>(m_data, centers, radii);
    }

    // Move existing points to correct children
    for(auto point : m_pointIndices)
    {
        bool success = false;
        for (auto& child : m_children)
        {
            if (!success)
            {
                success = child->insert(point);
            }
        }
    }

    // Empty parent node
    m_pointIndices.empty();
    m_isLeaf = false;
}


// Build SPTree on dataset
void SPTree::fill(unsigned int numberOfPoints)
{
    for (auto i = 0u; i < numberOfPoints; ++i)
    {
        insert(i);
    }
}


// Compute non-edge forces using Barnes-Hut algorithm
void SPTree::computeNonEdgeForces(unsigned int pointIndex, double theta, double forces[], double& forceSum)
{
    // Make sure that we spend no time on empty nodes or self-interactions
    if (m_cumulativeSize == 0 || (m_isLeaf && m_pointIndices.size() == 1 && m_pointIndices[0] == pointIndex))
    {
        return;
    }

    auto distances = std::vector<double>(m_dimensions);
    auto sumOfSquaredDistances = 0.0;
    auto maxRadius = 0.0;
    auto radius = 0.0;
    for (auto d = 0u; d < m_dimensions; ++d)
    {
        // Compute distance between point and center-of-mass
        distances[d] = m_data[pointIndex][d] - m_centerOfMass[d];
        sumOfSquaredDistances += distances[d] * distances[d];
        // Check whether we can use this node as a "summary"
        radius = m_boundary.m_radii[d];
        maxRadius = std::max(radius, maxRadius);
    }

    if(m_isLeaf || maxRadius / std::sqrt(sumOfSquaredDistances) < theta)
    {
        // Compute and add t-SNE force between point and current node
        auto inverseDistSum = 1.0 / (1.0 + sumOfSquaredDistances);
        auto force = m_cumulativeSize * inverseDistSum;
        forceSum += force;
        force *= inverseDistSum;
        for (auto d = 0u; d < m_dimensions; ++d)
        {
            forces[d] += force * distances[d];
        }
    }
    else
    {
        // Recursively apply Barnes-Hut to children
        for (auto& child : m_children)
        {
            child->computeNonEdgeForces(pointIndex, theta, forces, forceSum);
        }
    }
}


// Computes edge forces
void SPTree::computeEdgeForces(const std::vector<unsigned int>& rows, const std::vector<unsigned int>& columns, const std::vector<double>& values, Vector2D<double>& forces)
{
    // Loop over all edges in the graph
    auto sumOfSquaredDistances = 0.0;
    auto force = 0.0;
    auto distances = std::vector<double>(m_dimensions);
    for(auto n = 0u; n < forces.height(); ++n)
    {
        for(auto i = rows[n]; i < rows[n + 1]; ++i)
        {
            // Compute pairwise distance and Q-value
            sumOfSquaredDistances = 1.0;
            for (auto d = 0u; d < m_dimensions; ++d)
            {
                distances[d] = m_data[n][d] - m_data[columns[i]][d];
                sumOfSquaredDistances += distances[d] * distances[d];
            }
            force = values[i] / sumOfSquaredDistances;

            // Sum positive force
            for(auto d = 0u; d < m_dimensions; ++d)
            {
                forces[n][d] += force * distances[d];
            }
        }
    }
}
