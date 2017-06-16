#include "Insight.h"
#include "LogicMonday.h"

Insight::Insight(int64_t inputBits, int64_t outputBits)
{
  rawInputBits = inputBits;
  rawOutputBits = outputBits;
  inputBitCount = rawInputBits;
  outputBitCount = 1;

  if (rawOutputBits > 1)
    inputBitCount += (int64_t)ceil(log(rawOutputBits) / log(2));

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
  BitBuffer rawInputBuffer(rawInputBits, input);
  BitBuffer rawOutputBuffer(rawOutputBits, output);
  BitBuffer inputBuffer(inputBitCount);
  BitBuffer outputBuffer(outputBitCount);
  
  for (int64_t i = 0; i < rawInputBits; i++)
    inputBuffer.data[i] = rawInputBuffer.data[i];

  int extraBitCount = inputBitCount - rawInputBits;
  for (int64_t i = 0; i < rawOutputBits; i++)
  {

    BitBuffer temp(extraBitCount, &i);
    for (int64_t j = 0; j < extraBitCount; j++)
      inputBuffer.data[rawInputBits + j] = temp.data[j];

    outputBuffer.data[0] = rawOutputBuffer.data[i];
    examples->emplace_back(inputBuffer, outputBuffer);
  }

}

void Insight::Execute(void *input, void *output)
{
  BitBuffer rawInputBuffer(rawInputBits, input);
  BitBuffer inputBuffer(inputBitCount);
  BitBuffer outputBuffer(outputBitCount);
  char* dst = (char*)output;

  for (int64_t i = 0; i < rawInputBits; i++)
    inputBuffer.data[i] = rawInputBuffer.data[i];

  int extraBitCount = inputBitCount - rawInputBits;
  for (int64_t i = 0; i < rawOutputBits; i++)
  {

    BitBuffer temp(extraBitCount, &i);
    for (int64_t j = 0; j < extraBitCount; j++)
      inputBuffer.data[rawInputBits + j] = temp.data[j];

    cerebro->Query(inputBuffer, outputBuffer);
    
    int outputByte = i / 8;
    int outputBit = i % 8;
    if (outputBuffer.data[0])
      dst[outputByte] |= (1 << outputBit);
    else
      dst[outputByte] &= ~(1 << outputBit);
  }


}
