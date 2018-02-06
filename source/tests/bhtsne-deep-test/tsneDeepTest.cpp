#include <gtest/gtest.h>

#include <bhtsne/tsne.h>

class PublicTSNE : public bhtsne::TSNE
{
    FRIEND_TEST(TsneDeepTest, Constructor);
    FRIEND_TEST(TsneDeepTest, ComputeGradient);
    FRIEND_TEST(TsneDeepTest, ComputeGradientExact);
    FRIEND_TEST(TsneDeepTest, EvaluateError);
    FRIEND_TEST(TsneDeepTest, EvaluateErrorExact);
    FRIEND_TEST(TsneDeepTest, SymmetrizeMatrix);
    FRIEND_TEST(TsneDeepTest, GaussNumber);
    FRIEND_TEST(TsneDeepTest, SetRandomSeed);
    FRIEND_TEST(TsneDeepTest, Perplexity);
    FRIEND_TEST(TsneDeepTest, SetPerplexity);
    FRIEND_TEST(TsneDeepTest, GradientAccuracy);
    FRIEND_TEST(TsneDeepTest, SetGradientAccuracy);
    FRIEND_TEST(TsneDeepTest, Iterations);
    FRIEND_TEST(TsneDeepTest, SetIterations);
    FRIEND_TEST(TsneDeepTest, OutputDimensions);
    FRIEND_TEST(TsneDeepTest, SetOutputDimensions);
    FRIEND_TEST(TsneDeepTest, InputDimensions);
    FRIEND_TEST(TsneDeepTest, DataSize);
    FRIEND_TEST(TsneDeepTest, OutputFile);
    FRIEND_TEST(TsneDeepTest, SetOutputFile);
    FRIEND_TEST(TsneDeepTest, LoadFromStream);
    FRIEND_TEST(TsneDeepTest, LoadLegacy);
    FRIEND_TEST(TsneDeepTest, LoadCSV);
    FRIEND_TEST(TsneDeepTest, LoadTSNE);
    FRIEND_TEST(TsneDeepTest, LoadCin);
    FRIEND_TEST(TsneDeepTest, Run);
    FRIEND_TEST(TsneDeepTest, RunApproximation);
    FRIEND_TEST(TsneDeepTest, RunExact);
    FRIEND_TEST(TsneDeepTest, SaveToStream);
    FRIEND_TEST(TsneDeepTest, SaveToCout);
    FRIEND_TEST(TsneDeepTest, SaveToCout);
    FRIEND_TEST(TsneDeepTest, SaveLegacy);
    FRIEND_TEST(TsneDeepTest, SaveSVG);
    FRIEND_TEST(TsneDeepTest, ZeroMean);
    FRIEND_TEST(TsneDeepTest, Normalize);
    FRIEND_TEST(TsneDeepTest, ComputeGaussianPerplexityExact);
    FRIEND_TEST(TsneDeepTest, ComputeSquaredEuclideanDistance);
    FRIEND_TEST(TsneDeepTest, ComputeGaussianPerplexity);
};

class TsneDeepTest : public testing::Test
{
protected:
    TsneDeepTest()
        : m_tsne(PublicTSNE())
    {
    }

    PublicTSNE m_tsne;
};

TEST_F(TsneDeepTest, Constructor)
{
    FAIL();
}

TEST_F(TsneDeepTest, ComputeGradient)
{
    FAIL();
}

TEST_F(TsneDeepTest, ComputeGradientExact)
{
    FAIL();
}

TEST_F(TsneDeepTest, EvaluateError)
{
    FAIL();
}

TEST_F(TsneDeepTest, EvaluateErrorExact)
{
    FAIL();
}

TEST_F(TsneDeepTest, SymmetrizeMatrix)
{
    FAIL();
}

TEST_F(TsneDeepTest, GaussNumber)
{
    m_tsne.m_gen.seed(0);
    ASSERT_DOUBLE_EQ(1.1630780958763871, m_tsne.gaussNumber());
    m_tsne.m_gen.seed(1);
    ASSERT_DOUBLE_EQ(0.15606557998386178, m_tsne.gaussNumber());
}

TEST_F(TsneDeepTest, SetRandomSeed)
{
    m_tsne.setRandomSeed(0);
    ASSERT_EQ(2357136044, m_tsne.m_gen());
    m_tsne.setRandomSeed(1);
    ASSERT_EQ(1791095845, m_tsne.m_gen());
}

TEST_F(TsneDeepTest, Perplexity)
{
    m_tsne.m_perplexity = 0;
    ASSERT_EQ(0, m_tsne.perplexity());
    m_tsne.m_perplexity = 1;
    ASSERT_EQ(1, m_tsne.perplexity());
}

TEST_F(TsneDeepTest, SetPerplexity)
{
    FAIL();
}

TEST_F(TsneDeepTest, GradientAccuracy)
{
    FAIL();
}

TEST_F(TsneDeepTest, SetGradientAccuracy)
{
    FAIL();
}

TEST_F(TsneDeepTest, Iterations)
{
    FAIL();
}

TEST_F(TsneDeepTest, SetIterations)
{
    FAIL();
}

TEST_F(TsneDeepTest, OutputDimensions)
{
    FAIL();
}

TEST_F(TsneDeepTest, SetOutputDimensions)
{
    FAIL();
}

TEST_F(TsneDeepTest, InputDimensions)
{
    FAIL();
}

TEST_F(TsneDeepTest, DataSize)
{
    FAIL();
}

TEST_F(TsneDeepTest, OutputFile)
{
    FAIL();
}

TEST_F(TsneDeepTest, SetOutputFile)
{
    FAIL();
}

TEST_F(TsneDeepTest, LoadFromStream)
{
    FAIL();
}

TEST_F(TsneDeepTest, LoadLegacy)
{
    FAIL();
}

TEST_F(TsneDeepTest, LoadCSV)
{
    FAIL();
}

TEST_F(TsneDeepTest, LoadTSNE)
{
    FAIL();
}

TEST_F(TsneDeepTest, LoadCin)
{
    FAIL();
}

TEST_F(TsneDeepTest, Run)
{
    FAIL();
}

TEST_F(TsneDeepTest, RunApproximation)
{
    FAIL();
}

TEST_F(TsneDeepTest, RunExact)
{
    FAIL();
}

TEST_F(TsneDeepTest, SaveToStream)
{
    FAIL();
}

TEST_F(TsneDeepTest, SaveToCout)
{
    FAIL();
}

TEST_F(TsneDeepTest, SaveCSV)
{
    FAIL();
}

TEST_F(TsneDeepTest, SaveLegacy)
{
    FAIL();
}

TEST_F(TsneDeepTest, SaveSVG)
{
    FAIL();
}

TEST_F(TsneDeepTest, ZeroMean)
{
    FAIL();
}

TEST_F(TsneDeepTest, Normalize)
{
    FAIL();
}

TEST_F(TsneDeepTest, ComputeGaussianPerplexityExact)
{
    FAIL();
}

TEST_F(TsneDeepTest, ComputeSquaredEuclideanDistance)
{
    FAIL();
}

TEST_F(TsneDeepTest, ComputeGaussianPerplexity)
{
    FAIL();
}
