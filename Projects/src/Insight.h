#include <vector>
#ifndef Insight_h__
#define Insight_h__

class InsightExample;
class LogicMonday;

class Insight
{
public:
  Insight(int64_t inputBits, int64_t outputBits);

  ~Insight();

  void Train();

  void AddExample(void *input, void *output);

  void Execute(void *input, void *output);

private:

  int64_t inputBitCount, outputBitCount;
  std::vector<InsightExample> *examples = nullptr;
  LogicMonday *cerebro = nullptr;
};

#endif // Insight_h__
