#ifndef Pre_h__
#define Pre_h__

#include <vector>

enum PreInstruction : int
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
  int a;
  int b;

  void Generate(int programLength)
  {
    instructions.clear();
    for (int i = 1; i < programLength; i++)
      instructions.push_back((PreInstruction)(rand() % INTNUM));
  }

  std::vector<int> Execute(int runLengthDepth)
  {
    a = 0;
    b = 0;
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


class Pre
{
public:

  static std::vector<int> Predict(const std::vector<int> &examples, int predictionDepth = 1, int botBrainSize = 10)
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
    while(true)
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

        getchar();

        return prediction;
      }
    }
  }
};



class MultiPre
{

};


#endif // Pre_h__
