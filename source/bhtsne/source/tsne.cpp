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


#include <bhtsne/tsne.h>


#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cassert>

#include <vector>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <stdexcept>
#include <algorithm>

#include <bhtsne/sptree.h>
#include <bhtsne/vptree.h>

#include <bhtsne/bhtsne-version.h> // includes BHTSNE_VERSION macro


using namespace bhtsne;


TSNE::TSNE()
    : m_perplexity(50.0)
    , m_gradientAccuracy(0.2)
    , m_iterations(1000)
    , m_outputDimensions(2)
    , m_inputDimensions(0)
    , m_dataSize(0)
    , m_outputFile("result")
    , m_gen(static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count()))
{
}

// Compute gradient of the t-SNE cost function (using Barnes-Hut algorithm) (approximately)
Vector2D<double> TSNE::computeGradient(unsigned int *inp_row_P, unsigned int *inp_col_P, double *inp_val_P)
{

    // Construct space-partitioning tree on current map
    auto tree = SPTree(m_outputDimensions, m_result[0], m_dataSize);

    // Compute all terms required for t-SNE gradient
    auto pos_f =Vector2D<double>(m_dataSize, m_outputDimensions, 0.0);
    tree.computeEdgeForces(inp_row_P, inp_col_P, inp_val_P, m_dataSize, pos_f[0]);

    double sum_Q = 0.0;
    auto neg_f = Vector2D<double>(m_dataSize, m_outputDimensions, 0.0);
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        tree.computeNonEdgeForces(n, m_gradientAccuracy, neg_f[n], &sum_Q);
    }

    auto res = Vector2D<double>(m_dataSize, m_outputDimensions);
    // Compute final t-SNE gradient
    for (unsigned int i = 0; i < m_dataSize; ++i)
        for (unsigned int j = 0; j < m_outputDimensions; ++j)
            res[i][j] = pos_f[i][j] - (neg_f[i][j] / sum_Q);
    return res;
}

// Compute gradient of the t-SNE cost function (exact)
Vector2D<double> TSNE::computeGradientExact(const Vector2D<double> & Perplexity)
{
    auto gradients = Vector2D<double>(m_dataSize, m_outputDimensions, 0.0);

    // Compute the squared Euclidean distance matrix
    auto distances = computeSquaredEuclideanDistance(m_result);
    assert(distances.height() == m_dataSize);
    assert(distances.width() == m_dataSize);

    // Compute Q-matrix and normalization sum
    auto Q = Vector2D<double>(m_dataSize, m_dataSize);
    double sum_Q = .0;
    //TODO compute only half of matrix
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
    	for (unsigned int m = 0; m < m_dataSize; ++m)
        {
            if (n != m)
            {
                auto matrixIndex = n*m_dataSize + m;
                Q[n][m] = 1.0 / (1.0 + distances[n][m]);
                sum_Q += Q[n][m];
            }
        }
    }

	// Perform the computation of the gradient
	for (unsigned int n = 0; n < m_dataSize; ++n)
    {
    	for (unsigned int m = 0; m < m_dataSize; ++m)
        {
            if (n != m)
            {
                double mult = (Perplexity[n][m] - (Q[n][m] / sum_Q)) * Q[n][m];
                for (unsigned int d = 0; d < m_outputDimensions; ++d)
                {
                    gradients[n][d] += (m_result[n][d] - m_result[m][d]) * mult;
                }
            }
		}
	}

    return gradients;
}


// Evaluate t-SNE cost function (exactly)
double TSNE::evaluateErrorExact(const Vector2D<double> & Perplexity)
{
    assert(Perplexity.height() == m_dataSize);
    assert(Perplexity.width() == m_dataSize);

    // Compute the squared Euclidean distance matrix
    auto Q = Vector2D<double>(m_dataSize, m_dataSize, std::numeric_limits<double>::min());
    auto distances = computeSquaredEuclideanDistance(m_result);
    assert(distances.height() == m_result.height());
    assert(distances.width() == m_result.height());

    // Compute Q-matrix and normalization sum
    //TODO init to 0 (or evaluate consequences)
    double sum_Q = std::numeric_limits<double>::min();
    //TODO only compute half of matrix
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
    	for (unsigned int m = 0; m < m_dataSize; ++m)
        {
            if (n != m)
            {
                Q[n][m] = 1.0 / (1.0 + distances[n][m]);
                sum_Q += Q[n][m];
            }
        }
    }

    //TODO use vector normalization method
    for(auto & each : Q)
    {
        each /= sum_Q;
    }

    // Sum t-SNE error
    //TODO remove numeric limits, coz Q is init like this
    double error = 0.0;
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        for (unsigned int m = 0; m < m_dataSize; ++m)
        {
            error += Perplexity[n][m] * log((Perplexity[n][m] + std::numeric_limits<float>::min())
                / (Q[n][m] + std::numeric_limits<float>::min()));
        }
    }

	return error;
}

// Evaluate t-SNE cost function (approximately)
double TSNE::evaluateError(unsigned int * row_P, unsigned int * col_P, double * val_P)
{
    // Get estimate of normalization term
    auto tree = SPTree(m_outputDimensions, m_result[0], m_dataSize);
    auto buff = std::vector<double>(m_outputDimensions, 0.0);
    double sum_Q = 0.0;
    for (unsigned int i = 0; i < m_dataSize; ++i)
    {
        tree.computeNonEdgeForces(i, m_gradientAccuracy, buff.data(), &sum_Q);
    }

    // Loop over all edges to compute t-SNE error
    double error = 0.0;
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        int ind1 = n * m_outputDimensions;
        for (unsigned int i = row_P[n]; i < row_P[n + 1]; ++i)
        {
            double Q = 0.0;
            int ind2 = col_P[i] * m_outputDimensions;
            for (unsigned int d = 0; d < m_outputDimensions; d++)
            {
                buff[d] = m_result[ind1][d] - m_result[ind2][d];
                Q += buff[d] * buff[d];
            }

            Q = (1.0 / (1.0 + Q)) / sum_Q;
            error += val_P[i] * log((val_P[i] + std::numeric_limits<float>::min())
                                    / (Q + std::numeric_limits<float>::min()));
        }
    }

    //TODO: write tests for evaluate error
    return error;
}

// Symmetrizes a sparse matrix
void TSNE::symmetrizeMatrix(std::vector<unsigned int> & row_P, std::vector<unsigned int> & col_P, std::vector<double> & val_P)
{
    // Count number of elements and row counts of symmetric matrix
    auto row_counts = std::vector<unsigned int>(m_dataSize, 0);
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        for (unsigned int i = row_P[n]; i < row_P[n + 1]; ++i)
        {
            // Check whether element (col_P[i], n) is present
            auto first = col_P.begin() + row_P[col_P[i]];
            auto last = col_P.begin() + row_P[col_P[i] + 1];

            if (std::find(first, last, n) == last)
            {
                row_counts[col_P[i]]++;
            }
            row_counts[n]++;
        }
    }
    unsigned no_elem = std::accumulate(row_counts.begin(), row_counts.begin() + m_dataSize, 0u);

    // Allocate memory for symmetrized matrix
    // TODO reuse the memory in row,col and val!!
    auto sym_row_P = std::vector<unsigned int>(m_dataSize + 1);
    auto sym_col_P = std::vector<unsigned int>(no_elem);
    auto sym_val_P = std::vector<double>(no_elem);

    // Construct new row indices for symmetric matrix
    sym_row_P[0] = 0;
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        sym_row_P[n + 1] = sym_row_P[n] + row_counts[n];
    }

    // Fill the result matrix
    auto offset = std::vector<int>(m_dataSize, 0);
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        for (unsigned int i = row_P[n]; i < row_P[n + 1]; ++i)
        { // considering element(n, col_P[i])

            // Check whether element (col_P[i], n) is present
            bool present = false;
            for (unsigned int m = row_P[col_P[i]]; m < row_P[col_P[i] + 1]; ++m)
            {
                if (col_P[m] == n)
                {
                    present = true;
                    if (n <= col_P[i])
                    { // make sure we do not add elements twice
                        sym_col_P[sym_row_P[n]        + offset[n]]        = col_P[i];
                        sym_col_P[sym_row_P[col_P[i]] + offset[col_P[i]]] = n;
                        sym_val_P[sym_row_P[n]        + offset[n]]        = val_P[i] + val_P[m];
                        sym_val_P[sym_row_P[col_P[i]] + offset[col_P[i]]] = val_P[i] + val_P[m];
                    }
                }
            }

            // If (col_P[i], n) is not present, there is no addition involved
            if (!present)
            {
                sym_col_P[sym_row_P[n]        + offset[n]]        = col_P[i];
                sym_col_P[sym_row_P[col_P[i]] + offset[col_P[i]]] = n;
                sym_val_P[sym_row_P[n]        + offset[n]]        = val_P[i];
                sym_val_P[sym_row_P[col_P[i]] + offset[col_P[i]]] = val_P[i];
            }

            // Update offsets
            if (!present || n <= col_P[i])
            {
                offset[n]++;
                if (col_P[i] != n)
                {
                    offset[col_P[i]]++;
                }
            }
        }
    }

    // Divide the result by two
    for (auto & each : sym_val_P)
    {
        each /= 2.0;
    }

    // Return symmetrized matrices
    row_P = std::move(sym_row_P);
    col_P = std::move(sym_col_P);
    val_P = std::move(sym_val_P);
}

// with mean zero and standard deviation one
double bhtsne::TSNE::gaussNumber()
{
    // Knuth, Art of Computer Programming vol v2, Section 3.4.1, Algorithm P (p.117)

    double S, V1, V2;
    // executed 1.27 times on average
    do
    {
        // V1, V2 uniformly distributed between -1 and +l.
        V1 = 2.0 * static_cast<double>(m_gen()) / m_gen.max() - 1.0;
        V2 = 2.0 * static_cast<double>(m_gen()) / m_gen.max() - 1.0;
        S = V1*V1 + V2*V2;
    } while (S >= 1);

    auto X1 = V1 * std::sqrt(-2 * std::log(S) / S);
    //same can be done for X2

    return X1;
}

void TSNE::setRandomSeed(unsigned long seed)
{
    std::cout << "Using random seed: " << seed << std::endl;
    m_gen.seed(seed);
}

double TSNE::perplexity() const
{
	return m_perplexity;
}

void TSNE::setPerplexity(double perplexity)
{
	m_perplexity = perplexity;
    if (m_perplexity < 2.0)
    {
        std::cerr << "perplexity has to be at least 2.0, setting perplexity to 2.0" << std::endl;
        m_perplexity = 2.0;
    }
}

double TSNE::gradientAccuracy() const
{
	return m_gradientAccuracy;
}

void TSNE::setGradientAccuracy(double accuracy)
{
	m_gradientAccuracy = accuracy;
}

unsigned int TSNE::iterations() const
{
	return m_iterations;
}

void TSNE::setIterations(unsigned int iterations)
{
	m_iterations = iterations;
}

unsigned int TSNE::outputDimensions() const
{
	return m_outputDimensions;
}

void TSNE::setOutputDimensions(unsigned int dimensions)
{
	m_outputDimensions = dimensions;
}

unsigned int TSNE::inputDimensions() const
{
	return m_inputDimensions;
}

unsigned int TSNE::dataSize() const
{
	return m_dataSize;
}

//TODO remove?
void TSNE::setDataSize(unsigned int value)
{
	m_dataSize = value;
}

std::string TSNE::outputFile() const
{
	return m_outputFile;
}

void TSNE::setOutputFile(const std::string& file)
{
	m_outputFile = file;
}


//load methods--------------------------------------------------------------------------------------

bool bhtsne::TSNE::loadFromStream(std::istream & stream)
{
    auto separator = ',';

    //read data points
    auto line = std::string();
    bool first = true;
    while (std::getline(stream, line))
    {
        std::istringstream iss(line);
        auto element = std::string();

        auto point = std::vector<double>();
        point.reserve(m_inputDimensions);

        //read values of data point
        while (std::getline(iss, element, separator))
        {
            point.push_back(std::stod(element));
        }

        //set dimensionality
        if (first)
        {
            first = false;
            m_inputDimensions = point.size();
            m_data.initialize(0, point.size());
        }

        //fail if inconsistent dimensionality
        if (m_inputDimensions != point.size() || m_inputDimensions == 0)
        {
            m_inputDimensions = 0;
            m_data.initialize(0, 0);
            return false;
        }

        m_data.appendRow(point);
    }

    if (m_data.height() == 0 || m_data.width() == 0)
    {
        return false;
    }

    m_dataSize = m_data.height();

    return true;
}

bool TSNE::loadLegacy(const std::string & file)
{
    std::ifstream f(file, std::ios::binary);
	if (!f.is_open())
    {
        std::cerr << "Could not open " << file << std::endl;
        return false;
    }

    //read params
	f.read(reinterpret_cast<char*>(&m_dataSize), sizeof(m_dataSize));
	f.read(reinterpret_cast<char*>(&m_inputDimensions), sizeof(m_inputDimensions));
	f.read(reinterpret_cast<char*>(&m_gradientAccuracy), sizeof(m_gradientAccuracy));
	f.read(reinterpret_cast<char*>(&m_perplexity), sizeof(m_perplexity));
	f.read(reinterpret_cast<char*>(&m_outputDimensions), sizeof(m_outputDimensions));
	f.read(reinterpret_cast<char*>(&m_iterations), sizeof(m_iterations));

    //read data
    m_data.initialize(m_dataSize, m_inputDimensions);
	f.read(reinterpret_cast<char*>(m_data[0]), m_dataSize * sizeof(double) * m_inputDimensions);

    //read seed
	if (!f.eof())
    {
        int seed;
		f.read(reinterpret_cast<char*>(&seed), sizeof(seed));
        setRandomSeed(seed);
	}

	return true;
}

bool TSNE::loadCSV(const std::string & file)
{
    std::ifstream f(file);
	if (!f.is_open())
    {
        std::cerr << "Could not open " << file << std::endl;
        return false;
    }

    return loadFromStream(f);
}

bool TSNE::loadTSNE(const std::string & file)
{
    std::ifstream f(file, std::ios::binary);
	if (!f.is_open())
    {
        std::cerr << "Could not open " << file << std::endl;
        return false;
    }

	f.read(reinterpret_cast<char*>(&m_dataSize), sizeof(m_dataSize));
	f.read(reinterpret_cast<char*>(&m_inputDimensions), sizeof(m_inputDimensions));

    //read data
    m_data.initialize(m_dataSize, m_inputDimensions);
    f.read(reinterpret_cast<char*>(m_data[0]), m_dataSize * sizeof(double) * m_inputDimensions);

	return true;
}

bool TSNE::loadCin()
{
    return loadFromStream(std::cin);
}


//run method---------------------------------------------------------------------------------------
void TSNE::run()
{
    if (m_dataSize - 1 < 3 * m_perplexity)
    {
        auto message = "perplexity (perplexity=" + std::to_string(m_perplexity) +
            ") has to be smaller than a third of the dataSize (dataSize=" + std::to_string(m_dataSize) + ")";
        std::cerr << message << std::endl;
        throw std::invalid_argument(message);
    }

    std::cout << "Using:"
        << "\ndata size " << m_dataSize
        << "\nin dimensions " << m_inputDimensions
        << "\nout dimensions " << m_outputDimensions
        << "\nperplexity " << m_perplexity
        << "\ngradient accuracy " << m_gradientAccuracy
        << std::endl;

    m_result.initialize(m_dataSize, m_outputDimensions);
    if (m_gradientAccuracy == 0.0)
    {
        runExact();
    }
    else
    {
        runApproximation();
    }
}


void TSNE::runApproximation()
{
	// Normalize input data to prevent numerical problems
	std::cout << "Computing input similarities..." << std::endl;
    zeroMean(m_data);
    normalize(m_data);

    // Compute input similarities for exact t-SNE
    // init sparse matrix P, TODO: create and use class SparseMatrix
    auto row_P = std::vector<unsigned int>();
    auto col_P = std::vector<unsigned int>();
    auto val_P = std::vector<double>();

	// Compute asymmetric pairwise input similarities
	computeGaussianPerplexity(row_P, col_P, val_P);

	// Symmetrize input similarities
	symmetrizeMatrix(row_P, col_P, val_P);

	//normalize val_P so that sum of all val = 1
	double sum_P = std::accumulate(val_P.begin(), val_P.end(), 0.0);
	for (auto & each : val_P)
    {
        each /= sum_P;
    	// Lie about the P-values
        each *= 12.0;
    }

	// Initialize solution (randomly)
    for (auto & each : m_result)
    {
        each = gaussNumber() * 0.0001;
    }

    //TODO: documentation for all these magic numbers
    unsigned stop_lying_iteration = 250;
    unsigned momentum_switch_iteration = 250;

    // Set learning parameters
    double momentum = 0.5;
    double final_momentum = 0.8;
    double eta = 200.0;

    auto uY = Vector2D<double>(m_dataSize, m_outputDimensions);
    auto gains = Vector2D<double>(m_dataSize, m_outputDimensions, 1.0);

	// Perform main training loop
    std::cout << " Input similarities computed. Learning embedding..." << std::endl;
	for (unsigned int iteration = 0; iteration < m_iterations; ++iteration)
    {
		// Compute approximate gradient
        auto gradients = computeGradient(row_P.data(), col_P.data(), val_P.data());

		// Update gains
        for (int i = 0; i < m_dataSize; ++i)
        {
            for (int j = 0; j < m_outputDimensions; ++j)
            {
                if (sign(gradients[i][j]) != sign(uY[i][j]))
                {
                    gains[i][j] += 0.2;
                }
                else
                {
                    gains[i][j] *= 0.8;
                }
                gains[i][j] = std::max(0.1, gains[i][j]);
            }
        }

		// Perform gradient update (with momentum and gains)
        for (int i = 0; i < m_dataSize; ++i)
        {
            for (int j = 0; j < m_outputDimensions; ++j)
            {
                uY[i][j] = momentum * uY[i][j] - eta * gains[i][j] * gradients[i][j];
                m_result[i][j] += uY[i][j];
            }
        }

		// Make solution zero-mean
		zeroMean(m_result);

		// Stop lying about the P-values after a while, and switch momentum
		if (iteration == stop_lying_iteration)
        {
			for (auto & each : val_P)
            {
                each /= 12.0;
            }
		}
		if (iteration == momentum_switch_iteration)
        {
            momentum = final_momentum;
        }

		// Print out progress
		if (iteration % 50 == 0 || iteration == m_iterations)
        {
			// doing approximate computation here!
			double error = evaluateError(row_P.data(), col_P.data(), val_P.data());
			std::cout << "Iteration " << (iteration + 1) << ": error is " << error << std::endl;
		}
	}
}


void TSNE::runExact()
{
    unsigned stop_lying_iteration = 250;
    unsigned momentum_switch_iteration = 250;

    // Set learning parameters
    double momentum = 0.5;
    double final_momentum = 0.8;
    double eta = 200.0;

    // Normalize input data (to prevent numerical problems)
    std::cout << "Computing input similarities..." << std::endl;
    zeroMean(m_data);
    normalize(m_data);

    // Compute input similarities for exact t-SNE
    auto P = computeGaussianPerplexityExact();
    assert(P.width() == m_dataSize);
    assert(P.height() == m_dataSize);

    // Symmetrize input similarities
    std::cout << "Symmetrizing..." << std::endl;
    for (unsigned int n = 0; n < m_dataSize; ++n)
    {
        for (unsigned int m = n + 1; m < m_dataSize; ++m)
        {
            P[n][m] += P[m][n];
            P[m][n] = P[n][m];
        }
    }

    double sum_P = std::accumulate(P.begin(), P.end(), 0.0);
    for (auto & each : P)
    {
        each /= sum_P;
    }

    // Lie about the P-values
    for (auto & each : P)
    {
        each *= 12.0;
    }

    // Initialize solution (randomly)
    for (auto & each : m_result)
    {
        each = gaussNumber() * 0.0001;
    }

    // Perform main training loop
    std::cout << "Input similarities computed. Learning embedding..." << std::endl;

    auto uY    = Vector2D<double>(m_dataSize, m_outputDimensions, 0.0);
    auto gains = Vector2D<double>(m_dataSize, m_outputDimensions, 1.0);

    for (unsigned int iteration = 0; iteration < m_iterations; ++iteration)
    {
        // Compute exact gradient
        auto gradients = computeGradientExact(P);
        assert(gradients.height() == m_dataSize);
        assert(gradients.width() == m_outputDimensions);

        // Update gains
        for (size_t i = 0; i < m_dataSize; ++i)
            for (size_t j = 0; j < m_outputDimensions; ++j)
                gains[i][j] = (sign(gradients[i][j]) != sign(uY[i][j])) ? (gains[i][j] + .2) : (gains[i][j] * .8);

        for (auto & each : gains)
        {
            each = std::max(each, 0.1);
        }

        // Perform gradient update (with momentum and gains)
        for (size_t i = 0; i < m_dataSize; ++i)
            for (size_t j = 0; j < m_outputDimensions; ++j)
                uY[i][j] = momentum * uY[i][j] - eta * gains[i][j] * gradients[i][j];

        for (size_t i = 0; i < m_dataSize; ++i)
            for (size_t j = 0; j < m_outputDimensions; ++j)
                m_result[i][j] += uY[i][j];

        // Make solution zero-mean
        zeroMean(m_result);

        // Stop lying about the P-values after a while, and switch momentum
        if (iteration == stop_lying_iteration)
        {
            for (auto & each : P)
            {
                each /= 12.0;
            }
        }

        if (iteration == momentum_switch_iteration)
        {
            momentum = final_momentum;
        }

        // Print out progress
        if (iteration % 50 == 0 || iteration == m_iterations)
        {
            double C = evaluateErrorExact(P);
            std::cout << "Iteration " << (iteration + 1) << ": error is " << C << std::endl;
        }
    }
}



//save methods--------------------------------------------------------------------------------------

void TSNE::saveToStream(std::ostream & stream)
{
	for (size_t i = 0; i < m_dataSize; ++i)
	{
		for (size_t j = 0; j < m_outputDimensions; ++j)
		{
			stream << m_result[i][j];
			if (j < m_outputDimensions - 1)
			{
				stream << ',';
			}
		}
		stream << '\n';
	}

    stream.flush();
}

void TSNE::saveToCout()
{
    saveToStream(std::cout);
}

void TSNE::saveCSV()
{
    std::ofstream csv_fstream(m_outputFile + ".csv");
	if (!csv_fstream.is_open())
	{
		std::cerr << "can't open " << m_outputFile << ".csv" << std::endl;
        return;
	}

    saveToStream(csv_fstream);
}

void TSNE::saveLegacy()
{
    std::ofstream f(m_outputFile + ".dat", std::ios::binary);
	if (!f.is_open())
    {
		std::cerr << "can't open " << m_outputFile << ".dat" << std::endl;
		return;
	}

	auto landmarks = std::vector<int>(m_dataSize);
    std::iota(landmarks.begin(), landmarks.end(), 0);

	auto costs = std::vector<double>(m_dataSize, 0.0);

	f.write(reinterpret_cast<char*>(&m_dataSize), sizeof(m_dataSize));
	f.write(reinterpret_cast<char*>(&m_outputDimensions), sizeof(m_outputDimensions));
	f.write(reinterpret_cast<char*>(m_result[0]), m_dataSize * m_outputDimensions * sizeof(double));
	f.write(reinterpret_cast<char*>(landmarks.data()), landmarks.size() * sizeof(int));
	f.write(reinterpret_cast<char*>(costs.data()), costs.size() * sizeof(double));

	std::cout << "Wrote the " << m_dataSize << " x " << m_outputDimensions
        << " data matrix successfully!" << std::endl;
}

void TSNE::saveSVG()
{
    assert(m_result.size() > 0);
    double extreme = *std::max_element(m_result.begin(), m_result.end());

    double radius = 0.5;
    double halfWidth = extreme + radius;

    //TODO: allow setting a labelFile, e.g. by command line option
    auto labelFile = std::string();
    auto labels = std::vector<uint8_t>();
    auto usingLabels = false;
	if (!labelFile.empty())
	{
		usingLabels = true;
        std::ifstream labelInput(labelFile, std::ios::in | std::ios::binary);
		if (!labelInput.is_open())
		{
            std::cerr << "Could not open " << labelFile << std::endl;
			return;
		}

		uint32_t labelCount;
		labelInput.read(reinterpret_cast<char*>(&labelCount), sizeof(labelCount));
		std::cout << "Labels file contains " << labelCount << " labels." << std::endl;
		if (labelCount < m_dataSize)
		{
			std::cerr << "Not enough labels for result" << std::endl;
			return;
		}

		labelCount = std::min(labelCount, m_dataSize);
		labels.resize(labelCount);
		labelInput.read(reinterpret_cast<char*>(labels.data()), labels.size());
	}

	uint8_t maxLabel = 0;
	for (auto label : labels)
    {
        maxLabel = std::max(label, maxLabel);
    }
	auto colors = std::vector<std::string>();
	for (auto i = 0; i <= maxLabel; ++i)
    {
        colors.push_back("hsl(" + std::to_string(360.0 * i / (maxLabel / 2 + 1)) + ", 100%, " + (i % 2 == 0 ? "25" : "60") + "%)");
    }

    std::ofstream f(m_outputFile + ".svg", std::ios::out | std::ios::trunc);
	if (!f.is_open())
    {
		std::cerr << "can't open " << m_outputFile << ".svg" << std::endl;
		return;
	}

	f << "<?xml version='1.0' encoding='UTF-8' ?>\n";
	f << "<svg xmlns='http://www.w3.org/2000/svg' version='1.1' width='600' height='600' viewBox='" << -halfWidth << " " << -halfWidth << " " << 2 * halfWidth << " " << 2 * halfWidth << "'>\n";

	auto color = std::string("black");
	for (unsigned int i = 0; i < m_dataSize; ++i)
	{
		if (usingLabels)
        {
            color = labels[i] < colors.size() ? colors[labels[i]] : "black";
        }

		f << "<circle "
			<< "cx='" << m_result[i][0] << "' "
			<< "cy='" << m_result[i][1] << "' "
			<< "fill='" << color << "' "
			<< "r='" << radius << "' "
			<< "stroke='none' opacity='0.5'/>\n";
	}
	f << "</svg>\n";
}

//make the mean of all data points equal 0 for each dimension -> zero mean
void TSNE::zeroMean(Vector2D<double> & points)
{
    const auto dimensions = points.width();
    const auto size = points.height();

    for (size_t d = 0; d < dimensions; d++)
    {
        auto mean = 0.0;
        for (auto i = 0u; i < size; ++i)
        {
            mean += points[i][d];
        }
        mean /= size;
        for (auto i = 0u; i < size; ++i)
        {
            points[i][d] -= mean;
        }
    }
}

void TSNE::normalize(Vector2D<double>& vec)
{
    assert(vec.size() > 0);
    double max_X = *std::max_element(vec.begin(), vec.end());
    for (auto & each : vec)
    {
        each /= max_X;
    }    
}

Vector2D<double> TSNE::computeGaussianPerplexityExact()
{
	// Compute the squared Euclidean distance matrix
	auto distances = computeSquaredEuclideanDistance(m_data);
    auto P = Vector2D<double>(m_dataSize, m_dataSize);

	// Compute the Gaussian kernel row by row
	for (int n = 0; n < m_dataSize; ++n)
    {
		// Initialize some variables
		double beta = 1.0;
		double min_beta = std::numeric_limits<double>::lowest();
		double max_beta = std::numeric_limits<double>::max();
		double tolerance_threshold = 1e-5;
        double sum_P = 0.0;

		// Iterate until we found a good perplexity
		for (unsigned iteration = 0; iteration < 200u; iteration++)
        {

			// Compute Gaussian kernel row
			for (int m = 0; m < m_dataSize; ++m)
            {
                P[n][m] = exp(-beta * distances[n][m]);
            }
			P[n][n] = std::numeric_limits<double>::min();

			// Compute entropy of current row
            sum_P = std::numeric_limits<double>::min();
			for (int m = 0; m < m_dataSize; ++m)
            {
                sum_P += P[n][m];
            }

			double H = 0.0; //TODO what is H?
			for (unsigned m = 0; m < m_dataSize; m++)
            {
                H += beta * (distances[n][m] * P[n][m]);
            }
			H = (H / sum_P) + log(sum_P);

			// Evaluate whether the entropy is within the tolerance level
			double Hdiff = H - log(m_perplexity);
			if (std::abs(Hdiff) < tolerance_threshold)
            {
				break;
			}
			else
            {
				if (Hdiff > 0)
                {
					min_beta = beta;
					if (max_beta == std::numeric_limits<double>::max() || max_beta == std::numeric_limits<double>::lowest())
                    {
                        beta *= 2.0;
                    }
					else
                    {
                        beta = (beta + max_beta) / 2.0;
                    }
				}
				else
                {
					max_beta = beta;
					if (min_beta == std::numeric_limits<double>::lowest() || min_beta == std::numeric_limits<double>::max())
                    {
                        beta /= 2.0;
                    }
					else
                    {
                        beta = (beta + min_beta) / 2.0;
                    }
				}
			}
		}

		// Row normalize P
		for (int m = 0; m < m_dataSize; ++m)
        {
            P[n][m] /= sum_P;
        }
	}
    
    return P;
}

Vector2D<double> TSNE::computeSquaredEuclideanDistance(const Vector2D<double> & points)
{
    auto dimensions = points.width();
    auto number = points.height();

    auto distances = Vector2D<double>(number, number, 0.0);

    for (auto i = 0u; i < number; ++i)
    {
        for (auto j = i + 1; j < number; ++j)
        {
            double distance = 0.0;
            for (size_t d = 0; d < dimensions; ++d)
            {
                double diff = points[i][d] - points[j][d];
                distance += diff * diff;
            }

            distances[i][j] = distance;
            distances[j][i] = distance;
        }
    }

    return distances;
}

void TSNE::computeGaussianPerplexity(std::vector<unsigned int> & row_P, std::vector<unsigned int> & col_P, std::vector<double> & val_P)
{
    assert(m_data.height() == m_dataSize);
    assert(m_data.width() == m_inputDimensions);

	int K = static_cast<int>(3 * m_perplexity);

	// Allocate the memory we need
	row_P.resize(m_dataSize + 1);
    col_P.resize(m_dataSize * K);
    val_P.resize(m_dataSize * K, 0.0);

	auto cur_P = std::vector<double>(m_dataSize - 1);
	row_P[0] = 0;
	for (int n = 0; n < m_dataSize; ++n)
    {
        row_P[n + 1] = row_P[n] + (unsigned int)K;
    }

	// Build ball tree on data set
	auto tree = VpTree<DataPoint, euclidean_distance>();
	auto obj_X = std::vector<DataPoint>(m_dataSize, DataPoint(m_inputDimensions, -1, m_data[0]));
	for (int n = 0; n < m_dataSize; ++n)
    {
        obj_X[n] = DataPoint(m_inputDimensions, n, m_data[n]);
    }
	tree.create(obj_X);

	// Loop over all points to find nearest neighbors
	std::cout << "Building tree..." << std::endl;
	std::vector<DataPoint> indices;
	std::vector<double> distances;
	for (int n = 0; n < m_dataSize; ++n)
    {

		if (n % 10000 == 0)
        {
            std::cout << " - point " << n << " of " << m_dataSize << std::endl;
        }

		// Find nearest neighbors
		indices.clear();
		distances.clear();
		tree.search(obj_X[n], K + 1, &indices, &distances);

		// Initialize some variables for binary search
		double beta = 1.0;
		double min_beta = std::numeric_limits<double>::lowest();
		double max_beta = std::numeric_limits<double>::max();
		double tol = 1e-5;

		// Iterate until we found a good perplexity
		double sum_P = 0.0;
		for (unsigned iteration = 0; iteration < 200; iteration++)
        {
			// Compute Gaussian kernel row
			for (int m = 0; m < K; ++m)
            {
                cur_P[m] = exp(-beta * distances[m + 1] * distances[m + 1]);
            }

			// Compute entropy of current row
			sum_P = std::numeric_limits<double>::min();
			for (int m = 0; m < K; ++m)
            {
                sum_P += cur_P[m];
            }

			double H = 0.0;
			for (int m = 0; m < K; ++m)
            {
                H += beta * (distances[m + 1] * distances[m + 1] * cur_P[m]);
            }
			H = (H / sum_P) + log(sum_P);

			// Evaluate whether the entropy is within the tolerance level
			double Hdiff = H - log(m_perplexity);
			if (Hdiff < tol && -Hdiff < tol)
            {
				break;
			}
			else
            {
				if (Hdiff > 0)
                {
					min_beta = beta;
					if (max_beta == std::numeric_limits<double>::max() || max_beta == std::numeric_limits<double>::lowest())
                    {
                        beta *= 2.0;
                    }
					else
                    {
                        beta = (beta + max_beta) / 2.0;
                    }
				}
				else
                {
					max_beta = beta;
					if (min_beta == std::numeric_limits<double>::lowest() || min_beta == std::numeric_limits<double>::max())
                    {
                        beta /= 2.0;
                    }
					else
                    {
                        beta = (beta + min_beta) / 2.0;
                    }
				}
			}
		}

		// Row-normalize current row of P and store in matrix
		for (unsigned int m = 0; m < K; ++m)
        {
            cur_P[m] /= sum_P;
        }
		for (unsigned int m = 0; m < K; ++m)
        {
			col_P[row_P[n] + m] = static_cast<unsigned int>(indices[m + 1].index());
			val_P[row_P[n] + m] = cur_P[m];
		}
	}
}
