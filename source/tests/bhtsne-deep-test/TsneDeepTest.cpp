#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <bhtsne/TSNE.h>
#include <bhtsne/SparseMatrix.h>

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
    FRIEND_TEST(TsneDeepTest, SaveCSV);
    FRIEND_TEST(TsneDeepTest, SaveLegacy);
    FRIEND_TEST(TsneDeepTest, SaveSVG);
    FRIEND_TEST(TsneDeepTest, ZeroMean);
    FRIEND_TEST(TsneDeepTest, Normalize);
    FRIEND_TEST(TsneDeepTest, ComputeGaussianPerplexityExact);
    FRIEND_TEST(TsneDeepTest, ComputeSquaredEuclideanDistance);
    FRIEND_TEST(TsneDeepTest, ComputeGaussianPerplexity);
};

class BinaryWriter
{
public:
    BinaryWriter() = default;
    explicit BinaryWriter(std::ofstream * f)
        : fileStream(f)
    {
    }

    template <typename T>
    auto& operator<<(const T & value)
    {
        fileStream->write(reinterpret_cast<const char *>(&value), sizeof(value));
        return *this;
    }

private:
    std::ofstream * fileStream;
};

class TsneDeepTest : public testing::Test
{
protected:
    TsneDeepTest()
        : m_tsne(PublicTSNE())
        , m_tempFile(std::tmpnam(nullptr))
    {
    }

    PublicTSNE m_tsne;
    std::string m_tempFile;
    std::ofstream m_fileStream;
    BinaryWriter m_writer;
    const static std::vector<unsigned int> s_testValuesInt;
    const static std::vector<double> s_testValuesDouble;
    const static std::vector<std::string> s_testValuesString;
    const static std::vector<std::vector<double>> s_testDataSet;

    auto createTempfile()
    {
        m_fileStream.open(m_tempFile, std::ios::out | std::ios::binary | std::ios::trunc);
        m_writer = BinaryWriter(&m_fileStream);
    }

    auto removeTempfile()
    {
        if (m_fileStream.is_open())
        {
            m_fileStream.close();
        }
        EXPECT_EQ(0, remove(m_tempFile.c_str()));
    }
};

const std::vector<unsigned int> TsneDeepTest::s_testValuesInt = std::vector<unsigned int>{ 1, 0, 42, 1337 };
const std::vector<double> TsneDeepTest::s_testValuesDouble = std::vector<double>{ 1.0, 0.0, 4.2, 13.37 };
const std::vector<std::string> TsneDeepTest::s_testValuesString = std::vector<std::string>{ "true", "false", "fourty-two", "leet" };
const std::vector<std::vector<double>> TsneDeepTest::s_testDataSet = std::vector<std::vector<double>>{
	{ 1.0, 2.0, 3.0 },
	{ 4.0, 5.0, 6.0 },
	{ 7.0, 8.0, 9.0 },
	{ 10.0, 11.0, 12.0 },
	{ 13.0, 14.0, 15.0 },
	{ 16.0, 17.0, 18.0 },
	{ 19.0, 20.0, 21.0 } };

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
		if (number >= 2.0)
		{
			EXPECT_EQ(number, m_tsne.m_perplexity);
		}
		else 
		{
			EXPECT_EQ(2.0, m_tsne.m_perplexity);
		}
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
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto inputDimensions = static_cast<int>(s_testDataSet[0].size());
    auto gradientAccuracy = 0.5;
    auto perplexity = 25.0;
    auto outputDimensions = 1;
    auto iterations = 100;
    auto randomSeed = 42;

    createTempfile();
    m_writer << dataSize << inputDimensions << gradientAccuracy << perplexity << outputDimensions << iterations;
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            m_writer << value;
        }
    }
    m_writer << randomSeed;
    m_fileStream.flush();
    m_fileStream.close();

    EXPECT_TRUE(m_tsne.loadLegacy(m_tempFile));
    EXPECT_EQ(dataSize, m_tsne.m_dataSize);
    EXPECT_EQ(inputDimensions, m_tsne.m_inputDimensions);
    EXPECT_EQ(gradientAccuracy, m_tsne.m_gradientAccuracy);
    EXPECT_EQ(perplexity, m_tsne.m_perplexity);
    EXPECT_EQ(outputDimensions, m_tsne.m_outputDimensions);
    EXPECT_EQ(iterations, m_tsne.m_iterations);
    EXPECT_EQ(randomSeed, m_tsne.m_seed);

    auto it = m_tsne.m_data.begin();
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            EXPECT_EQ(value, *(it++));
        }
    }

    removeTempfile();
}

TEST_F(TsneDeepTest, LoadCSV)
{
    createTempfile();
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            m_fileStream << value << ',';
        }
        m_fileStream.seekp(-1, std::ios_base::cur);
        m_fileStream << std::endl;
    }

    m_fileStream.flush();
    m_fileStream.close();

    EXPECT_TRUE(m_tsne.loadCSV(m_tempFile));
    EXPECT_EQ(s_testDataSet.size(), m_tsne.m_dataSize);
    EXPECT_EQ(s_testDataSet[0].size(), m_tsne.m_inputDimensions);
    
    auto it = m_tsne.m_data.begin();
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            EXPECT_EQ(value, *(it++));
        }
    }

    removeTempfile();
}

TEST_F(TsneDeepTest, LoadTSNE)
{
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto inputDimensions = static_cast<int>(s_testDataSet[0].size());

    createTempfile();
    m_writer << dataSize << inputDimensions;
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            m_writer << value;
        }
    }
    m_fileStream.flush();
    m_fileStream.close();

    EXPECT_TRUE(m_tsne.loadTSNE(m_tempFile));
    EXPECT_EQ(dataSize, m_tsne.m_dataSize);
    EXPECT_EQ(inputDimensions, m_tsne.m_inputDimensions);

    auto it = m_tsne.m_data.begin();
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            EXPECT_EQ(value, *(it++));
        }
    }

    removeTempfile();
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
    m_tsne.m_dataSize = 3;
    m_tsne.m_perplexity = 1;
    EXPECT_THROW(m_tsne.run(), std::invalid_argument);

    m_tsne.m_data = s_testDataSet;
    m_tsne.m_dataSize = s_testDataSet.size();
    m_tsne.m_inputDimensions = s_testDataSet[0].size();
    m_tsne.m_perplexity = 2;
    m_tsne.m_outputDimensions = 1;
    m_tsne.m_seed = 1;

    m_tsne.m_gradientAccuracy = 0;
    auto expected = std::vector<double>{ -72.9579, 237.73738, 80.6992, -236.04152, -166.73192, 5.9935555, 151.30116 };
    EXPECT_NO_THROW(m_tsne.run());
    auto it = m_tsne.m_result.begin();
    auto itExp = expected.begin();
    while (it != m_tsne.m_result.end())
    {
        EXPECT_FLOAT_EQ(*(itExp++), *(it++));
    }

    m_tsne.m_gradientAccuracy = 0.1;
    expected = std::vector<double>{ 6.74018e-05, -5.00873e-06, -2.8833609e-05, -6.8209149e-05, -6.69799e-05, 3.64486e-05, 6.5181e-05 };
    EXPECT_NO_THROW(m_tsne.run());
    it = m_tsne.m_result.begin();
    itExp = expected.begin();
    while (it != m_tsne.m_result.end())
    {
        EXPECT_FLOAT_EQ(*(itExp++), *(it++));
    }
}

TEST_F(TsneDeepTest, RunApproximation)
{
    m_tsne.m_data = s_testDataSet;
    m_tsne.m_dataSize = s_testDataSet.size();
    m_tsne.m_inputDimensions = s_testDataSet[0].size();
    m_tsne.m_perplexity = 2;
    m_tsne.m_outputDimensions = 1;
    m_tsne.m_seed = 1;
    m_tsne.m_gradientAccuracy = 0.1;

    m_tsne.m_gen.seed(m_tsne.m_seed);
    m_tsne.m_result.initialize(m_tsne.m_dataSize, m_tsne.m_outputDimensions);

    auto expected = std::vector<double>{ 6.74018e-05, -5.00873e-06, -2.8833609e-05, -6.8209149e-05, -6.69799e-05, 3.64486e-05, 6.5181e-05 };
    EXPECT_NO_THROW(m_tsne.runApproximation());
    auto it = m_tsne.m_result.begin();
    auto itExp = expected.begin();
    while (it != m_tsne.m_result.end())
    {
        EXPECT_FLOAT_EQ(*(itExp++), *(it++));
    }
}

TEST_F(TsneDeepTest, RunExact)
{
    m_tsne.m_data = s_testDataSet;
    m_tsne.m_dataSize = s_testDataSet.size();
    m_tsne.m_inputDimensions = s_testDataSet[0].size();
    m_tsne.m_perplexity = 2;
    m_tsne.m_outputDimensions = 1;
    m_tsne.m_seed = 1;
    m_tsne.m_gradientAccuracy = 0;

    m_tsne.m_gen.seed(m_tsne.m_seed);
    m_tsne.m_result.initialize(m_tsne.m_dataSize, m_tsne.m_outputDimensions);

    auto expected = std::vector<double>{ -72.9579, 237.73738, 80.6992, -236.04152, -166.73192, 5.9935555, 151.30116 };
    EXPECT_NO_THROW(m_tsne.runExact());
    auto it = m_tsne.m_result.begin();
    auto itExp = expected.begin();
    while (it != m_tsne.m_result.end())
    {
        EXPECT_FLOAT_EQ(*(itExp++), *(it++));
    }
}

TEST_F(TsneDeepTest, SaveToStream)
{
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto outputDimensions = static_cast<int>(s_testDataSet[0].size());

    m_tsne.m_dataSize = dataSize;
    m_tsne.m_outputDimensions = outputDimensions;
    m_tsne.m_result = bhtsne::Vector2D<double>(s_testDataSet);

    std::ostringstream result;
    EXPECT_NO_THROW(m_tsne.saveToStream(result));
    auto in = std::istringstream(result.str());

    double d;
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            in >> d;
            EXPECT_EQ(value, d);
            in.seekg(1, std::ios_base::cur);
        }
    }
}

TEST_F(TsneDeepTest, SaveToCout)
{
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto outputDimensions = static_cast<int>(s_testDataSet[0].size());

    m_tsne.m_dataSize = dataSize;
    m_tsne.m_outputDimensions = outputDimensions;
    m_tsne.m_result = bhtsne::Vector2D<double>(s_testDataSet);

    std::ostringstream result;
    auto coutBuf = std::cout.rdbuf();
    std::cout.rdbuf(result.rdbuf());
    EXPECT_NO_THROW(m_tsne.saveToCout());
    auto in = std::istringstream(result.str());
    std::cout.rdbuf(coutBuf);

    double d;
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            in >> d;
            EXPECT_EQ(value, d);
            in.seekg(1, std::ios_base::cur);
        }
    }
}

TEST_F(TsneDeepTest, SaveCSV)
{
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto outputDimensions = static_cast<int>(s_testDataSet[0].size());

    m_tsne.m_dataSize = dataSize;
    m_tsne.m_outputDimensions = outputDimensions;
    m_tsne.m_result = bhtsne::Vector2D<double>(s_testDataSet);

    m_tsne.setOutputFile(m_tempFile);
    EXPECT_NO_THROW(m_tsne.saveCSV());

    std::ifstream result;
    EXPECT_NO_THROW(result.open(m_tempFile + ".csv",
                                static_cast<std::ios_base::openmode>(std::ios_base::in | std::ios_base::beg)));
    EXPECT_TRUE(result.is_open());

    double d;
    for (auto sample : s_testDataSet)
    {
        for (auto value : sample)
        {
            result >> d;
            EXPECT_EQ(value, d);
            result.seekg(1, std::ios_base::cur);
        }
    }

    result.close();
    EXPECT_EQ(0, remove((m_tempFile + ".csv").c_str()));
}

TEST_F(TsneDeepTest, SaveLegacy)
{
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto outputDimensions = static_cast<int>(s_testDataSet[0].size());

    m_tsne.m_dataSize = dataSize;
    m_tsne.m_outputDimensions = outputDimensions;
    m_tsne.m_result = bhtsne::Vector2D<double>(s_testDataSet);

    m_tsne.setOutputFile(m_tempFile);
    EXPECT_NO_THROW(m_tsne.saveLegacy());

    std::ifstream result;
    EXPECT_NO_THROW(result.open(m_tempFile + ".dat", std::ios::in | std::ios::binary));
    EXPECT_TRUE(result.is_open());

    int readDataSize;
    int readOutputDimensions;
    auto readData = bhtsne::Vector2D<double>(dataSize, outputDimensions);
    auto readLandmarks = std::vector<int>(dataSize);
    auto readCosts = std::vector<double>(dataSize);
    result.read(reinterpret_cast<char*>(&readDataSize), sizeof(dataSize));
    result.read(reinterpret_cast<char*>(&readOutputDimensions), sizeof(outputDimensions));
    result.read(reinterpret_cast<char*>(readData[0]), readData.size() * sizeof(double));
    result.read(reinterpret_cast<char*>(readLandmarks.data()), readLandmarks.size() * sizeof(int));
    result.read(reinterpret_cast<char*>(readCosts.data()), readCosts.size() * sizeof(double));
    EXPECT_EQ(dataSize, readDataSize);
    EXPECT_EQ(outputDimensions, readOutputDimensions);

    for (auto i = 0; i < dataSize; ++i)
    {
        for (auto j = 0; j < outputDimensions; ++j)
        {
            EXPECT_EQ(s_testDataSet[i][j], readData[i][j]);
        }
        EXPECT_EQ(i, readLandmarks[i]);
        EXPECT_EQ(0, readCosts[i]);
    }

    result.close();
    remove((m_tempFile + ".dat").c_str());
}

TEST_F(TsneDeepTest, SaveSVG)
{
    auto dataSize = static_cast<int>(s_testDataSet.size());
    auto outputDimensions = static_cast<int>(s_testDataSet[0].size());

    m_tsne.m_dataSize = dataSize;
    m_tsne.m_outputDimensions = outputDimensions;
    m_tsne.m_result = bhtsne::Vector2D<double>(s_testDataSet);

    m_tsne.setOutputFile(m_tempFile);
    EXPECT_NO_THROW(m_tsne.saveSVG());

    std::ifstream result;
    EXPECT_NO_THROW(result.open(m_tempFile + ".svg",
                                static_cast<std::ios_base::openmode>(std::ios_base::in | std::ios_base::beg)));
    EXPECT_TRUE(result.is_open());

    // ignore xml header
    result.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    EXPECT_FALSE(result.eof());
    // ignore svg header
    result.ignore(std::numeric_limits<std::streamsize>::max(), '>');
    EXPECT_FALSE(result.eof());

    double d;
    std::string circle = "circle ";
    std::string cx = "cx='";
    std::string close = "' ";
    std::string cy = "cy='";

    auto checkFunc = [&result](std::string tag)
    {
        std::string s;
        s.resize(tag.length());
        result.read(&s[0], s.length());
        EXPECT_FALSE(result.eof());
        EXPECT_FALSE(result.fail());
        EXPECT_EQ(tag, s);
    };

    for (auto sample : s_testDataSet)
    {
        // ignore until circle opening tag
        result.ignore(std::numeric_limits<std::streamsize>::max(), '<');
        EXPECT_FALSE(result.eof());

        checkFunc(circle);
        checkFunc(cx);

        result >> d;
        EXPECT_EQ(sample[0], d);

        checkFunc(close);
        checkFunc(cy);

        result >> d;
        EXPECT_EQ(sample[1], d);

        checkFunc(close);
    }

    result.close();
    EXPECT_EQ(0, remove((m_tempFile + ".svg").c_str()));
}

TEST_F(TsneDeepTest, ZeroMean)
{
	auto testSet = bhtsne::Vector2D<double>(s_testDataSet);
	auto expectedResults = std::vector<std::vector<double>>
	{
		{ -9.0, -9.0, -9.0 },
		{ -6.0, -6.0, -6.0 },
		{ -3.0, -3.0, -3.0 },
		{  0.0,  0.0,  0.0 },
		{  3.0,  3.0,  3.0 },
		{  6.0,  6.0,  6.0 },
		{  9.0,  9.0,  9.0 }
	};

	m_tsne.zeroMean(testSet);

	auto it = testSet.begin();
	for (auto sample : expectedResults)
	{
		for (auto value : sample)
		{
			EXPECT_DOUBLE_EQ(value, *(it++));
		}
	}
}

TEST_F(TsneDeepTest, Normalize)
{
	auto testSet = bhtsne::Vector2D<double>(s_testDataSet);
	auto expectedResults = std::vector<std::vector<double>>(7);
	for (auto i = size_t(0); i < 7; ++i)
	{
		auto sample = std::vector<double>(3);
		for (auto j = size_t(0); j < 3; ++j)
		{
			sample[j] = (i * 3 + j + 1) / 21.0;
		}
		expectedResults[i] = sample;
	}

	m_tsne.normalize(testSet);

	auto it = testSet.begin();
	for (auto sample : expectedResults)
	{
		for (auto value : sample)
		{
			EXPECT_DOUBLE_EQ(value, *(it++));
		}
	}
}

TEST_F(TsneDeepTest, ComputeGaussianPerplexityExact)
{
    m_tsne.m_data = s_testDataSet;
    m_tsne.m_dataSize = s_testDataSet.size();

    auto result = m_tsne.computeGaussianPerplexityExact();

    EXPECT_EQ(s_testDataSet.size(), result.width());
    EXPECT_EQ(s_testDataSet.size(), result.height());

    for(auto i = 0; i < result.width(); i++)
        for(auto j = 0; j < result.height(); j++)
            EXPECT_FLOAT_EQ(i == j ? 0.0 : 1.0 / (result.width() - 1.0), result[i][j]);
}

TEST_F(TsneDeepTest, ComputeSquaredEuclideanDistance)
{
    auto expectedDists = std::vector<int>{ 0, 27, 108, 243, 432, 675, 972, 27, 0, 27, 108, 243, 432, 675, 108, 27, 0, 27, 108, 243, 432, 243, 108, 27, 0, 27, 108, 243, 432, 243, 108, 27, 0, 27, 108, 675, 432, 243, 108, 27, 0, 27, 972, 675, 432, 243, 108, 27, 0 };

    auto result = m_tsne.computeSquaredEuclideanDistance(s_testDataSet);

    auto exp = expectedDists.begin();
    for (auto entry : result)
    {
        EXPECT_DOUBLE_EQ(*(exp++), entry);
    }
}

TEST_F(TsneDeepTest, ComputeGaussianPerplexity)
{
	auto similarities = bhtsne::SparseMatrix();

	auto expectedValues = std::vector<double>{ 0.6169480414, 0.1151944809, 0.01670360337, 0.001118768075, 3.461157475e-05, 9.891998642e-07, 0.6169480414, 0.5, 3.3198386e-36, 7.786851871e-95, 6.452155618e-177, 3.461157475e-05, 0.1151944809, 0.5, 0.5, 3.3198386e-36, 7.786851871e-95, 0.001118768075, 0.01670360337, 3.3198386e-36, 0.5, 0.5, 3.3198386e-36, 0.01670360337, 0.001118768075, 7.786851871e-95, 3.3198386e-36, 0.5, 0.5, 0.1151944809, 3.461157475e-05, 6.452155618e-177, 7.786851871e-95, 3.3198386e-36, 0.5, 0.6169480414, 9.891998642e-07, 3.461157475e-05, 0.001118768075, 0.01670360337, 0.1151944809, 0.6169480414 };
	auto expectedColumns = std::vector<unsigned int>{ 1, 2, 3, 4, 5, 6, 0, 2, 3, 4, 5, 6, 0, 1, 3, 4, 5, 6, 0, 1, 2, 4, 5, 6, 0, 1, 2, 3, 5, 6, 0, 1, 2, 3, 4, 6, 0, 1, 2, 3, 4, 5 };
	auto expectedRows = std::vector<unsigned int>{ 0, 6, 12, 18, 24, 30, 36, 42 };

	m_tsne.m_data = bhtsne::Vector2D<double>(s_testDataSet);
	m_tsne.m_dataSize = m_tsne.m_data.height();
	m_tsne.m_inputDimensions = m_tsne.m_data.width();
	m_tsne.m_perplexity = 2.0;

	m_tsne.computeGaussianPerplexity(similarities);
	m_tsne.symmetrizeMatrix(similarities);

	EXPECT_EQ(expectedValues.size(), similarities.values.size());
	EXPECT_EQ(expectedColumns.size(), similarities.columns.size());
	EXPECT_EQ(expectedRows.size(), similarities.rows.size());

	auto expectedValue = expectedValues.begin();
	for (auto val : similarities.values)
	{
		EXPECT_FLOAT_EQ(*(expectedValue++), val);
	}

	auto expectedColumn = expectedColumns.begin();
	for (auto col : similarities.columns)
	{
		EXPECT_FLOAT_EQ(*(expectedColumn++), col);
	}

	auto expectedRow = expectedRows.begin();
	for (auto row : similarities.rows)
	{
		EXPECT_FLOAT_EQ(*(expectedRow++), row);
	}
}
