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
    FRIEND_TEST(TsneDeepTest, RandomSeed);
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
    const static std::vector<unsigned int> s_testValuesInt;
    const static std::vector<double> s_testValuesDouble;
    const static std::vector<std::string> s_testValuesString;
    const static std::vector<std::vector<double>> s_testDataSet;
};

const std::vector<unsigned int> TsneDeepTest::s_testValuesInt = std::vector<unsigned int>{ 1, 0, 42, 1337 };
const std::vector<double> TsneDeepTest::s_testValuesDouble = std::vector<double>{ 1.0, 0.0, 4.2, 13.37 };
const std::vector<std::string> TsneDeepTest::s_testValuesString = std::vector<std::string>{ "true", "false", "fourty-two", "leet" };
const std::vector<std::vector<double>> TsneDeepTest::s_testDataSet = std::vector<std::vector<double>>{ { 1.0, 2.0, 3.0 },{ 4.0, 5.0, 6.0 } };

TEST_F(TsneDeepTest, Constructor)
{
    auto tsne = bhtsne::TSNE();
    EXPECT_EQ(50.0, tsne.perplexity());
    EXPECT_EQ(0.2, tsne.gradientAccuracy());
    EXPECT_EQ(1000, tsne.iterations());
    EXPECT_EQ(2, tsne.outputDimensions());
    EXPECT_EQ(0, tsne.inputDimensions());
    EXPECT_EQ(0, tsne.dataSize());
    EXPECT_LT(0, tsne.randomSeed());
    EXPECT_EQ("result", tsne.outputFile());
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

TEST_F(TsneDeepTest, RandomSeed)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.m_seed = number;
        EXPECT_EQ(number, m_tsne.randomSeed());
    }
}

TEST_F(TsneDeepTest, SetRandomSeed)
{
    for(const auto & number : s_testValuesInt)
    {
        m_tsne.setRandomSeed(number);
        ASSERT_EQ(number, m_tsne.m_seed);
    }
}

TEST_F(TsneDeepTest, Perplexity)
{
    for (const auto & number : s_testValuesDouble)
    {
        m_tsne.m_perplexity = number;
        EXPECT_EQ(number, m_tsne.perplexity());
    }
}

TEST_F(TsneDeepTest, SetPerplexity)
{
    for (const auto & number : s_testValuesDouble)
    {
        m_tsne.setPerplexity(number);
        EXPECT_EQ(number, m_tsne.m_perplexity);
    }
}

TEST_F(TsneDeepTest, GradientAccuracy)
{
    for (const auto & number : s_testValuesDouble)
    {
        m_tsne.m_gradientAccuracy = number;
        EXPECT_EQ(number, m_tsne.gradientAccuracy());
    }
}

TEST_F(TsneDeepTest, SetGradientAccuracy)
{
    for (const auto & number : s_testValuesDouble)
    {
        m_tsne.setGradientAccuracy(number);
        EXPECT_EQ(number, m_tsne.m_gradientAccuracy);
    }
}

TEST_F(TsneDeepTest, Iterations)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.m_iterations = number;
        EXPECT_EQ(number, m_tsne.iterations());
    }
}

TEST_F(TsneDeepTest, SetIterations)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.setIterations(number);
        EXPECT_EQ(number, m_tsne.m_iterations);
    }
}

TEST_F(TsneDeepTest, OutputDimensions)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.m_outputDimensions = number;
        EXPECT_EQ(number, m_tsne.outputDimensions());
    }
}

TEST_F(TsneDeepTest, SetOutputDimensions)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.setOutputDimensions(number);
        EXPECT_EQ(number, m_tsne.m_outputDimensions);
    }
}

TEST_F(TsneDeepTest, InputDimensions)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.m_inputDimensions = number;
        EXPECT_EQ(number, m_tsne.inputDimensions());
    }
}

TEST_F(TsneDeepTest, DataSize)
{
    for (const auto & number : s_testValuesInt)
    {
        m_tsne.m_dataSize = number;
        EXPECT_EQ(number, m_tsne.dataSize());
    }
}

TEST_F(TsneDeepTest, OutputFile)
{
    for (const auto & text : s_testValuesString)
    {
        m_tsne.m_outputFile = text;
        EXPECT_EQ(text, m_tsne.outputFile());
    }
}

TEST_F(TsneDeepTest, SetOutputFile)
{
    for (const auto & number : s_testValuesString)
    {
        m_tsne.setOutputFile(number);
        EXPECT_EQ(number, m_tsne.m_outputFile);
    }
}

TEST_F(TsneDeepTest, LoadFromStream)
{
    std::ostringstream out;
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            out << value << ',';
        }
        out.seekp(-1, std::ios_base::cur);
        out << std::endl;
    }

    std::istringstream in(out.str());
    EXPECT_TRUE(m_tsne.loadFromStream(in));
    EXPECT_EQ(s_testDataSet.size(), m_tsne.dataSize());
    EXPECT_EQ(s_testDataSet[0].size(), m_tsne.inputDimensions());

    auto it = m_tsne.m_data.begin();
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            EXPECT_EQ(value, *(it++));
        }
    }
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
    auto cinBuf = std::cin.rdbuf();
    std::stringstream out;

    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            out << value << ',';
        }
        out.seekp(-1, std::ios_base::cur);
        out << std::endl;
    }
    out.seekp(0);

    std::cin.rdbuf(out.rdbuf());

    EXPECT_TRUE(m_tsne.loadCin());
    EXPECT_EQ(s_testDataSet.size(), m_tsne.dataSize());
    EXPECT_EQ(s_testDataSet[0].size(), m_tsne.inputDimensions());

    auto it = m_tsne.m_data.begin();
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            EXPECT_EQ(value, *(it++));
        }
    }

    std::cin.rdbuf(cinBuf);
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
