#include <gmock/gmock.h>
#include <omp.h>
#include "../../bhtsne/source/SpacePartitioningTreeTemplate.h"

using namespace bhtsne;

class SpacePartitioningTreeTest : public testing::Test
{
public:
};

TEST_F(SpacePartitioningTreeTest, OpenMPComputeNonEdgeForces)
{

    const auto result = Vector2D<double>{ {
        { 0,56,19,80,58 },
        { 47,35,89,82,74 },
        { 17,85,71,51,30 },
        { 1,9,36,14,16 },
        { 98,44,11,0,0 },
        { 37,53,57,60,60 },
        { 16,66,45,35,5 },
        { 60,78,80,51,30 },
        { 87,72,95,92,53 },
        { 14,46,23,86,20 }
    } };

    const unsigned int dataSize = result.height();
    constexpr unsigned int outputDimensions = 5;
    ASSERT_EQ(outputDimensions, result.width());
    const double gradientAccuracy = 0.2;

    auto tree = SpacePartitioningTree<outputDimensions>(result);
    auto negativeForces = Vector2D<double>(dataSize, outputDimensions, 0.0);
    auto omp_negativeForces = Vector2D<double>(dataSize, outputDimensions, 0.0);

    double sumQ = 0.0;
    double omp_sumQ = 0.0;

    // serial
    // TODO rename to negativeForces everywhere
    for (unsigned int n = 0; n < dataSize; ++n) {
        tree.computeNonEdgeForces(n, gradientAccuracy, negativeForces[n], sumQ);
    }

    // parallel
    int executedInParallel = 0;
    // omp version on windows (2.0) does only support signed loop variables, should be unsigned
    #pragma omp parallel for reduction(+:omp_sumQ) reduction(+:executedInParallel)
    for (int n = 0; n < dataSize; ++n)
    {
        executedInParallel = omp_in_parallel() ? 1 : 0;
        tree.computeNonEdgeForces(n, gradientAccuracy, omp_negativeForces[n], omp_sumQ);
    }
    ASSERT_NE(executedInParallel, 0);

    ASSERT_DOUBLE_EQ(sumQ, omp_sumQ);

    for (unsigned int i = 0; i < negativeForces.height(); ++i)
    {
        for (unsigned int j = 0; j < negativeForces.width(); ++j)
        {
            ASSERT_DOUBLE_EQ(negativeForces[i][j], omp_negativeForces[i][j]);
        }
    }
}
