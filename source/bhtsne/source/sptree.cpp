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

#include <math.h>
#include <float.h>
#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <limits>
#include <algorithm>

#include <bhtsne/sptree.h>

using namespace bhtsne;

// Constructs cell
SPTree::Cell::Cell() = default;

SPTree::Cell::Cell(unsigned int dimensions, std::vector<double> centers, std::vector<double> radii)
    : m_dimensions(dimensions)
    , m_centers(centers)
    , m_radii(radii) 
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
SPTree::SPTree(const Vector2D<double> &data) : data(data)
{
    auto dimensions = data.width();
    auto number = data.height();
    // Compute mean, width, and height of current map (boundaries of SPTree)
    std::vector<double> mean_Y = std::vector<double>(dimensions);
    std::vector<double> min_Y = std::vector<double>(dimensions);
    std::vector<double> max_Y = std::vector<double>(dimensions);
    for (auto d = 0u; d < dimensions; ++d)
    {
        min_Y[d] = std::numeric_limits<double>::max();
        max_Y[d] = std::numeric_limits<double>::min();
    }
    for(auto n = 0u; n < number; ++n) {
        for(auto d = 0u; d < dimensions; ++d) {
            auto value = data[n][d];
            mean_Y[d] += value;
            min_Y[d] = std::min(min_Y[d], value);
            max_Y[d] = std::max(max_Y [d], value);
        }
    }
    for (auto d = 0u; d < dimensions; ++d)
    {
        mean_Y[d] /= number;
    }

    // Construct SPTree
    auto width = std::vector<double>(dimensions);
    auto delta = 1e-5;
    for (int d = 0; d < dimensions; d++)
    {
        width[d] = std::max(max_Y[d] - mean_Y[d], mean_Y[d] - min_Y[d]) + delta;
    }

    init(mean_Y, width);
    fill(number);
}


// Constructor for SPTree with particular size (do not fill the tree)
SPTree::SPTree(const Vector2D<double> &data, std::vector<double> centers, std::vector<double> radii) : data(data)
{
    init(centers, radii);
}


// Main initialization function
void SPTree::init(std::vector<double> centers, std::vector<double> radii)
{
    m_dimensions = data.width();
    m_numberOfChildren = 2;
    for(auto d = 1u; d < m_dimensions; ++d) m_numberOfChildren *= 2;
    m_isLeaf = true;
    m_cumulativeSize = 0;

    boundary = Cell(m_dimensions, centers, radii);

    m_children = std::vector<std::unique_ptr<SPTree>>(m_numberOfChildren);

    m_centerOfMass = std::vector<double>(m_dimensions, .0);

    m_pointIndices = std::vector<double>();

    buff = (double*) malloc(m_dimensions * sizeof(double));
}


// Destructor for SPTree
SPTree::~SPTree()
{
    free(buff);
}


// Insert a point into the SPTree
bool SPTree::insert(unsigned int new_index)
{
    // Ignore objects which do not belong in this quad tree
    const auto point = data[new_index];
    if (!boundary.containsPoint(point))
        return false;

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
    auto any_duplicate = false;
    for(auto n = 0u; n < m_pointIndices.size(); ++n) {
        bool duplicate = true;
        for(unsigned int d = 0; d < m_dimensions; d++) {
            if(point[d] != data[m_pointIndices[n]][d]) { duplicate = false; break; }
        }
        any_duplicate = any_duplicate | duplicate;
    }
    if(any_duplicate) return true;

    // Otherwise, we need to subdivide the current cell
    if(m_isLeaf) subdivide();

    // Find out where the point can be inserted
    for(unsigned int i = 0; i < m_numberOfChildren; i++) {
        if(m_children[i]->insert(new_index)) return true;
    }

    // Otherwise, the point cannot be inserted (this should never happen)
    return false;
}


// Create four children which fully divide this cell into four quads of equal area
void SPTree::subdivide() {

    // Create new children
    auto centers = std::vector<double>(m_dimensions);
    auto radii  = std::vector<double>(m_dimensions);
    for(unsigned int i = 0; i < m_numberOfChildren; i++) {
        unsigned int div = 1;
        for(unsigned int d = 0; d < m_dimensions; d++) {
            radii[d] = .5 * boundary.m_radii[d];
            if((i / div) % 2 == 1) centers[d] = boundary.m_centers[d] - .5 * boundary.m_radii[d];
            else                   centers[d] = boundary.m_centers[d] + .5 * boundary.m_radii[d];
            div *= 2;
        }
        m_children[i] = std::make_unique<SPTree>(data, centers, radii);
    }

    // Move existing points to correct children
    for(unsigned int i = 0; i < m_pointIndices.size(); i++) {
        bool success = false;
        for(unsigned int j = 0; j < m_numberOfChildren; j++) {
            if(!success) success = m_children[j]->insert(m_pointIndices[i]);
        }
    }

    // Empty parent node
    m_pointIndices.empty();
    m_isLeaf = false;
}


// Build SPTree on dataset
void SPTree::fill(unsigned int N)
{
    for(unsigned int i = 0; i < N; i++) insert(i);
}


// Checks whether the specified tree is correct
bool SPTree::isCorrect()
{
    for(unsigned int n = 0; n < m_pointIndices.size(); n++) {
        const double* point = data[m_pointIndices[n]];
        if (!boundary.containsPoint(point)) return false;
    }
    if(!m_isLeaf) {
        bool correct = true;
        for(int i = 0; i < m_numberOfChildren; i++) correct = correct && m_children[i]->isCorrect();
        return correct;
    }
    else return true;
}



// Build a list of all indices in SPTree
void SPTree::getAllIndices(unsigned int* indices)
{
    getAllIndices(indices, 0);
}


// Build a list of all indices in SPTree
unsigned int SPTree::getAllIndices(unsigned int* indices, unsigned int loc)
{

    // Gather indices in current quadrant
    for(unsigned int i = 0; i < m_pointIndices.size(); i++) indices[loc + i] = m_pointIndices[i];
    loc += m_pointIndices.size();

    // Gather indices in children
    if(!m_isLeaf) {
        for(int i = 0; i < m_numberOfChildren; i++) loc = m_children[i]->getAllIndices(indices, loc);
    }
    return loc;
}


unsigned int SPTree::getDepth() {
    if(m_isLeaf) return 1;
    int depth = 0;
    for(unsigned int i = 0; i < m_numberOfChildren; i++) depth = fmax(depth, m_children[i]->getDepth());
    return 1 + depth;
}


// Compute non-edge forces using Barnes-Hut algorithm
void SPTree::computeNonEdgeForces(unsigned int point_index, double theta, double neg_f[], double* sum_Q)
{

    // Make sure that we spend no time on empty nodes or self-interactions
    if(m_cumulativeSize == 0 || (m_isLeaf && m_pointIndices.size() == 1 && m_pointIndices[0] == point_index)) return;

    // Compute distance between point and center-of-mass
    double D = .0;
    for(unsigned int d = 0; d < m_dimensions; d++) buff[d] = data[point_index][d] - m_centerOfMass[d];
    for(unsigned int d = 0; d < m_dimensions; d++) D += buff[d] * buff[d];

    // Check whether we can use this node as a "summary"
    double max_width = 0.0;
    double cur_width;
    for(unsigned int d = 0; d < m_dimensions; d++) {
        cur_width = boundary.m_radii[d];
        max_width = (max_width > cur_width) ? max_width : cur_width;
    }
    if(m_isLeaf || max_width / sqrt(D) < theta) {

        // Compute and add t-SNE force between point and current node
        D = 1.0 / (1.0 + D);
        double mult = m_cumulativeSize * D;
        *sum_Q += mult;
        mult *= D;
        for(unsigned int d = 0; d < m_dimensions; d++) neg_f[d] += mult * buff[d];
    }
    else {

        // Recursively apply Barnes-Hut to children
        for(unsigned int i = 0; i < m_numberOfChildren; i++) m_children[i]->computeNonEdgeForces(point_index, theta, neg_f, sum_Q);
    }
}


// Computes edge forces
void SPTree::computeEdgeForces(unsigned int* row_P, unsigned int* col_P, double* val_P, int N, double* pos_f)
{

    // Loop over all edges in the graph
    unsigned int ind1 = 0;
    unsigned int ind2 = 0;
    double D;
    for(unsigned int n = 0; n < N; n++) {
        for(unsigned int i = row_P[n]; i < row_P[n + 1]; i++) {

            // Compute pairwise distance and Q-value
            D = 1.0;
            ind2 = col_P[i];
            for(unsigned int d = 0; d < m_dimensions; d++) buff[d] = data[ind1][d] - data[ind2][d];
            for(unsigned int d = 0; d < m_dimensions; d++) D += buff[d] * buff[d];
            D = val_P[i] / D;

            // Sum positive force
            for(unsigned int d = 0; d < m_dimensions; d++) pos_f[ind1 * m_dimensions + d] += D * buff[d];
        }
        ind1++;
    }
}


// Print out tree
void SPTree::print()
{
    if(m_cumulativeSize == 0) {
        printf("Empty node\n");
        return;
    }

    if(m_isLeaf) {
        printf("Leaf node; data = [");
        for(int i = 0; i < m_pointIndices.size(); i++) {
            const double* point = data[m_pointIndices[i]];
            for(int d = 0; d < m_dimensions; d++) printf("%f, ", point[d]);
            printf(" (index = %d)", m_pointIndices[i]);
            if(i < m_pointIndices.size() - 1) printf("\n");
            else printf("]\n");
        }
    }
    else {
        printf("Intersection node with center-of-mass = [");
        for(int d = 0; d < m_dimensions; d++) printf("%f, ", m_centerOfMass[d]);
        printf("]; children are:\n");
        for(int i = 0; i < m_numberOfChildren; i++) m_children[i]->print();
    }
}
