#ifndef LogicMonday_h__
#define LogicMonday_h__

#include <vector>
#include <cstdint>
#include <unordered_map>

struct BitBuffer
{
  BitBuffer(int64_t dataLength, void *pData = nullptr);
  BitBuffer() = default;
  std::vector<bool> data;
};

struct InsightExample
{
  InsightExample(BitBuffer inputBuffer, BitBuffer outputBuffer)
  {
    input = inputBuffer;
    output = outputBuffer;
  }
  InsightExample() = default;
  BitBuffer input;
  BitBuffer output;
};

struct InsightNode
{
  uint32_t queryBitIndex;
  uint32_t childNodeIndices[2] = {0, 0};

  bool IsLeaf();
  bool Result();
  uint32_t QueryChildIndex(BitBuffer & query);
  bool operator==(const InsightNode &other) const;
};

struct InsightNodeHasher
{
  std::size_t operator()(const InsightNode& k) const;
};

struct InsightScore
{
  uint64_t cases[4] = {0,0,0,0};
  double CorrelationStrength();
  void Clear();
};

class LogicMonday
{
public:
  void Train(std::vector<InsightExample> *examples);
  void Query(BitBuffer &input, BitBuffer &output);

private:

  bool IsLeaf(int64_t exampleStart, int64_t exampleCount, uint32_t resultIndex);

  uint32_t GetBestQueryBitID(int64_t exampleStart, int64_t exampleCount, std::vector<uint32_t>& queryBitIndices, uint32_t resultIndex);

  void SortExamplesByQueryBitValue(int64_t exampleStart, int64_t exampleCount, uint32_t queryBitIndex, uint32_t *pTrueCount, uint32_t *pFalseCount);

  uint32_t CreateInsightRecursive(int64_t exampleStart, int64_t exampleCount, std::vector<uint32_t>& queryBitIndices, uint32_t resultIndex);

  bool QueryRecursive(BitBuffer &query, uint32_t nodeIndex);

  void AddNode(InsightNode node);

  uint32_t queryLength;
  uint32_t resultLength;
  std::vector<InsightNode> nodes;
  std::vector<uint32_t> entryNodeIndices;

  std::vector<InsightExample> *pExamples;

  InsightScore *m_scores = nullptr;
  std::unordered_map<InsightNode, uint32_t, InsightNodeHasher> *m_nodeMap;
  uint32_t m_totalExamples;
  uint32_t m_examplesDone;
  float m_lastProgressUpdate;
};

#endif // LogicMonday_h__