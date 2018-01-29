#ifndef Pre_h__
#define Pre_h__

#include <vector>

enum PreInstruction : uint8_t
{
  SWAPAB,
  COPYAB,
  AddBTA,
  SUBBFA,
  MULBTA,
  DIVBTA,
  SETAT0,
  SETAT1,
  SETAT2,
  SETAT3,
  INTNUM,
};

class PreProgram
{
public:
  std::vector<PreInstruction> instructions;
  void Generate(int programLength)
  {
    instructions.clear();
    for (int i = 1; i < programLength; i++)
      instructions.push_back((PreInstruction)(rand() % INTNUM));
  }
  std::vector<int> Execute(int runLengthDepth)
  {
    int a = 0;
    int b = 0;
    std::vector<int> ret;
    bool s = true;
    for (int l = 0; l < runLengthDepth; l++)
    {
      for (auto & i : instructions)
        switch (i)
        {
        case SWAPAB: std::swap(a, b); break;
        case COPYAB: b = a; break;
        case AddBTA: a += b; break;
        case SUBBFA: a -= b; break;
        case MULBTA: a *= b; break;
        case DIVBTA: a /= b > 0 ? b : 1; break;
        case SETAT0: a = s ? 0 : a; break;
        case SETAT1: a = s ? 1 : a; break;
        case SETAT2: a = s ? 2 : a; break;
        case SETAT3: a = s ? 3 : a; break;
        default: break;
        }
      s = false;
      ret.push_back(a);
    }
    return ret;
  }
  void Optimize(int DepthPrecision)
  {
    auto base = Execute(DepthPrecision);

    // Dead Code Elimination
    for (int64_t i = 0; i < instructions.size(); i++)
    {
      int instruction = instructions[i];
      instructions.erase(instructions.begin() + i);
      auto test = Execute(DepthPrecision);
      bool same = true;
      for (int p = 0; p < base.size(); p++)
        same &= base[p] == test[p];
      if (same)
        i--;
      else
        instructions.insert(instructions.begin() + i, (PreInstruction)instruction);
    }

  }
};

struct Prediction
{
  int value;
  float likelyhood;
};

struct PredictionMoment
{
  std::vector<Prediction> distribution;
};

struct PredictionDistribution
{
  std::vector<PredictionMoment> moments;
};

class Pre
{
public:
  static std::vector<int> SimplePredict(const std::vector<int> &examples, int predictionDepth = 1, int botPopulationSize = 100000, int botBrainSize = 10)
  {
    // Need
    // list of programs
    // Interpreter
    // Compare results to examples
    // Calculate statistical significance
    // Select best programs
    // Determine next predictions
    // std::vector<PreProgram> bots((size_t)botCount);

    // Generate Random Programs
    //for (auto & bot : bots) bot.Generate(botBrainSize);

    // Run each program examples.size() number of times
    //for (auto & bot : bots)

    PreProgram bot;
    int c = 0;
    int64_t maxSeachSize = botPopulationSize;
    int64_t seachSize = 0;
    while (seachSize++ < maxSeachSize)
    {
      c++;
      bot.Generate(rand() % botBrainSize + 1);
      std::vector<int> test = bot.Execute(examples.size());

      // input normalization
      int inputOffset = examples[0];

      bool same = true;
      for (int p = 0; p < examples.size(); p++)
        same &= examples[p] == test[p] + inputOffset;

      if (same)
      {
        bot.Optimize(examples.size());
        std::vector<int> prediction = bot.Execute(examples.size() + predictionDepth);
        prediction.erase(prediction.begin(), prediction.begin() + examples.size());

        // Print Result
        printf("Found At Program Number : %d\n\n", c);
        for (auto & r : examples) printf("%d ", r);
        printf("[ ");
        for (auto & r : prediction) printf("%d ", r + inputOffset);
        printf("]\n\n\nProgram Length : %d\n\n", (int)bot.instructions.size());
        for (auto & i : bot.instructions)
        {
          switch (i)
          {
          case SWAPAB: printf("Swap A <> B\n"); break;
          case COPYAB: printf("Copy A -> B\n"); break;
          case AddBTA: printf("Add A <- B\n"); break;
          case SUBBFA: printf("Sub A <- B\n"); break;
          case MULBTA: printf("Mul A <- B\n"); break;
          case DIVBTA: printf("Div A <- B\n"); break;
          case SETAT0: printf("Set A <- 0\n"); break;
          case SETAT1: printf("Set A <- 1\n"); break;
          case SETAT2: printf("Set A <- 2\n"); break;
          case SETAT3: printf("Set A <- 3\n"); break;
          default: break;
          }
        }
        return prediction;
      }
    }
    return{ 0, 0 };
  }

  static PredictionDistribution PredictDistribution(const std::vector<int> &examples, int predictionDepth = 1, int botPopulationSize = 100000, int botBrainSize = 10)
  {
    PredictionDistribution distribution;
    distribution.moments.resize(predictionDepth);

    PreProgram bot;

    for (int i = 0; i < botPopulationSize; i++)
    {
      bot.Generate(rand() % botBrainSize + 1); // Create random bot
      std::vector<int> botPattern = bot.Execute(examples.size() + predictionDepth); // Run random bot

      float corelation = 0.0f; // correlate examples with bot pattern
      for (int p = 1; p < examples.size(); p++) // (ignore first example as its normalised to be correct)
      {
        float example = examples[p];
        float prediction = botPattern[p];
        int diff = abs(prediction - example);
        if (diff == 0)
          corelation++;
        else
        corelation += 1.0 / (diff * diff * diff + 1);
      }
      for (int p = 0; p < predictionDepth; p++)
      {
        Prediction pre;
        pre.likelyhood = corelation;
        pre.value = botPattern[examples.size() + p];
        distribution.moments[p].distribution.push_back(pre); // Store predictions weighted by corelation
      }
    }

    // Normalize prediction quotients for each future moment
    for (auto & moment : distribution.moments)
    {
      float sumLikelyhood = 0.f;
      for (auto & prediction : moment.distribution) sumLikelyhood += prediction.likelyhood;
      for (auto & prediction : moment.distribution) prediction.likelyhood /= sumLikelyhood;
    }

    // Combine identical result predictions
    // --THIS IS A REALLY LAGGY CODE BLOCK--
    //for (auto & moment : distribution.moments)
    //{
    //  for (int i = 0; i < moment.distribution.size() - 1; i++)
    //  {
    //    int val = moment.distribution[i].value;
    //    for (int j = i + 1; j < moment.distribution.size(); j++)
    //    {
    //      if (moment.distribution[j].value == val)
    //      {
    //        moment.distribution[i].likelyhood += moment.distribution[j].likelyhood;
    //        moment.distribution.erase(moment.distribution.begin() + j--);
    //      }
    //    }
    //  }
    //}

//     // Drop junk predictions
//     for (auto & moment : distribution.moments)
//     {
//       float average = 0;
//       for (int i = 0; i < moment.distribution.size() - 1; i++)
//       {
//         average = moment.distribution[i].likelyhood;
//       }
//     }

    return distribution;
  };

  static std::vector<int> PredictSequence(const std::vector<int> &examples, int predictionDepth = 1, int botPopulationSize = 100000, int botBrainSize = 10)
  {
    auto distribution = PredictDistribution(examples, predictionDepth, botPopulationSize, botBrainSize);
    std::vector<int> prediction;
    for (auto & item : distribution.moments)
    {
      int bestValue = 0;
      float bestLikelyhood = 0;
      for (auto & prediction : item.distribution)
      {
        if (prediction.likelyhood > bestLikelyhood)
        {
          bestLikelyhood = prediction.likelyhood;
          bestValue = prediction.value;
        }
      }
      bestLikelyhood = bestLikelyhood;
      prediction.push_back(bestValue);
    }
    return prediction;
  }

};
#endif // Pre_h__
