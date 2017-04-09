#include "Insight.h"
#include "LogicMonday.h"

Insight::Insight(int64_t inputBits, int64_t outputBits)
{
  inputBitCount = inputBits;
  outputBitCount = outputBits;
  cerebro = new LogicMonday;
  examples = new std::vector<InsightExample>;
}

Insight::~Insight()
{

  delete cerebro;
  delete examples;
}

void Insight::Train()
{
  cerebro->Train(examples);
}

void Insight::AddExample(void *input, void *output)
{
  examples->emplace_back(BitBuffer(inputBitCount, input), BitBuffer(outputBitCount, output));
}

void Insight::Execute(void *input, void *output)
{
  BitBuffer outBuffer = (outputBitCount);
  cerebro->Query(BitBuffer(inputBitCount, input), outBuffer);
  memcpy(output, &(outBuffer.data[0]), (size_t)ceil(outputBitCount / 8.0f));
}
