#include "LogicMonday.h"

bool _GetBit(void *pData, int64_t bitOffset)
{
  uint8_t b = *(((char*)pData) + (bitOffset >> 3));
  return (b >> (bitOffset & 7)) & 1;
}

void LogicMonday::Train(std::vector<InsightExample> *examples)
{
  pExamples = examples;
  std::vector<uint32_t> queryBitIndices;
  queryLength = (*pExamples)[0].input.data.size();
  resultLength = (*pExamples)[0].output.data.size();
  nodes.clear();
  nodes.shrink_to_fit();
  entryNodeIndices.resize(resultLength);
  entryNodeIndices.shrink_to_fit();
  m_scores = new InsightScore[queryLength];
  m_nodeMap = new std::unordered_map<InsightNode, uint32_t, InsightNodeHasher>();
  m_totalExamples = (*pExamples).size() * resultLength;
  m_examplesDone = 0;
  m_lastProgressUpdate = 0;
  queryBitIndices.resize(queryLength);
  for (uint32_t i = 0; i < queryLength; i++)
    queryBitIndices[i] = i;

  for (uint32_t i = 0; i < resultLength; i++)
    entryNodeIndices[i] = CreateInsightRecursive(0, pExamples->size(), queryBitIndices, i);

  delete[] m_scores;
  m_scores = nullptr;
  delete m_nodeMap;
  m_nodeMap = nullptr;
}

void LogicMonday::Query(BitBuffer &input, BitBuffer &output)
{
  for (uint32_t i = 0; i < resultLength; i++)
    output.data[i] = QueryRecursive(input, entryNodeIndices[i]);
}

bool LogicMonday::IsLeaf(int64_t exampleStart, int64_t exampleCount, uint32_t resultIndex)
{
  bool resultValue = (*pExamples)[exampleStart].output.data[resultIndex];
  uint32_t resultLength = (*pExamples)[exampleStart].output.data.size();
  for (uint32_t i = 1; i < exampleCount; i++)
    if (resultValue != (*pExamples)[exampleStart + i].output.data[resultIndex])
      return false;
  return true;
}

uint32_t LogicMonday::GetBestQueryBitID(int64_t exampleStart, int64_t exampleCount, std::vector<uint32_t>& queryBitIndices, uint32_t resultIndex)
{
  for (uint32_t i = 0; i < queryBitIndices.size(); i++)
    m_scores[i].Clear();

  for (uint32_t i = 0; i < exampleCount; i++)
  {
    InsightExample *pExample = &((*pExamples)[exampleStart + i]);
    for (uint32_t j = 0; j < queryBitIndices.size(); j++)
    {
      uint32_t queryBitIndex = queryBitIndices[j];
      uint32_t countIndex = uint32_t((*pExamples)[exampleStart + i].input.data[queryBitIndex]);
      countIndex |= (uint32_t((*pExamples)[exampleStart + i].output.data[resultIndex]) << 1);
      m_scores[j].cases[countIndex]++;
    }
  }

  double bestCorrelation = -FLT_MAX;
  uint32_t bestQueryBitID = 0;
  for (uint32_t i = 0; i < queryBitIndices.size(); i++)
  {
    double correlationStrength = m_scores[i].CorrelationStrength();
    if (correlationStrength > bestCorrelation)
    {
      bestCorrelation = correlationStrength;
      bestQueryBitID = i;
    }
  }

  return bestQueryBitID;
}

void LogicMonday::SortExamplesByQueryBitValue(int64_t exampleStart, int64_t exampleCount, uint32_t queryBitIndex, uint32_t *pTrueCount, uint32_t *pFalseCount)
{
  uint32_t nextFalse = 0;
  uint32_t nextTrue = exampleCount - 1;
  InsightExample temp;
  while (true)
  {
    while (!(*pExamples)[exampleStart + nextFalse].input.data[queryBitIndex])
      ++nextFalse;
    while ((*pExamples)[exampleStart + nextTrue].input.data[queryBitIndex])
      --nextTrue;
    if (nextFalse < nextTrue)
    {
      // This copy is bad!
      temp = (*pExamples)[exampleStart + nextFalse];
      (*pExamples)[exampleStart + nextFalse] = (*pExamples)[exampleStart + nextTrue];
      (*pExamples)[exampleStart + nextTrue] = temp;
    }
    else
    {
      break;
    }
  }

  *pFalseCount = nextFalse;
  *pTrueCount = exampleCount - nextFalse;
}

uint32_t LogicMonday::CreateInsightRecursive(int64_t exampleStart, int64_t exampleCount, std::vector<uint32_t>& queryBitIndices, uint32_t resultIndex)
{
  InsightNode newNode;
  if (IsLeaf(exampleStart, exampleCount, resultIndex))
  {
    uint32_t result = (*pExamples)[exampleStart].output.data[resultIndex];
    newNode.queryBitIndex = 0;
    newNode.childNodeIndices[0] = result;
    newNode.childNodeIndices[1] = result;
  }
  else
  {
    uint32_t trueCount = 0;
    uint32_t falseCount = 0;
    uint32_t bestQueryBitID = GetBestQueryBitID(exampleStart, exampleCount, queryBitIndices, resultIndex);
    uint32_t bestQueryBitIndex = queryBitIndices[bestQueryBitID];
    queryBitIndices[bestQueryBitID] = queryBitIndices.back();
    queryBitIndices.pop_back();
    SortExamplesByQueryBitValue(exampleStart, exampleCount, bestQueryBitIndex, &trueCount, &falseCount);
    newNode.queryBitIndex = bestQueryBitIndex;

    newNode.childNodeIndices[0] = CreateInsightRecursive(exampleStart, falseCount, queryBitIndices, resultIndex);
    newNode.childNodeIndices[1] = CreateInsightRecursive(exampleStart + falseCount, trueCount, queryBitIndices, resultIndex);
    queryBitIndices.push_back(bestQueryBitIndex);
    if (newNode.childNodeIndices[0] == newNode.childNodeIndices[1])
    {
      return newNode.childNodeIndices[0];
    }
  }

  if (m_nodeMap->count(newNode) > 0)
  {
    return m_nodeMap->at(newNode);
  }
  else
  {
    AddNode(newNode);
    uint32_t result = nodes.size() - 1;
    m_nodeMap->emplace(newNode, result);
    return result;
  }
}

bool LogicMonday::QueryRecursive(BitBuffer &query, uint32_t nodeIndex)
{
  InsightNode &node = nodes[nodeIndex];
  if (node.IsLeaf())
    return node.Result();
  nodeIndex = node.QueryChildIndex(query);
  return QueryRecursive(query, nodeIndex);
}

void LogicMonday::AddNode(InsightNode node)
{
  nodes.push_back(node);
}

BitBuffer::BitBuffer(int64_t dataLength, void *pData /*= nullptr*/)
{
  data.resize(dataLength);
  if (pData)
    for (int64_t d = 0; d < dataLength; d++)
      data[d] = _GetBit(pData, d);
}

bool InsightNode::operator==(const InsightNode &other) const
{
  return (queryBitIndex == other.queryBitIndex
    && childNodeIndices[0] == other.childNodeIndices[0]
    && childNodeIndices[1] == other.childNodeIndices[1]);
}

bool InsightNode::IsLeaf()
{
  return (childNodeIndices[0] == childNodeIndices[1] && childNodeIndices[0] <= 1);
}

bool InsightNode::Result()
{
  return childNodeIndices[0] > 0;
}

uint32_t InsightNode::QueryChildIndex(BitBuffer & query)
{
  return childNodeIndices[uint32_t(query.data[queryBitIndex])];
}

std::size_t InsightNodeHasher::operator()(const InsightNode& k) const
{
  using std::size_t;
  using std::hash;
  return ((hash<uint32_t>()(k.queryBitIndex)
    ^ (hash<uint32_t>()(k.childNodeIndices[0]) << 1)) >> 1)
    ^ (hash<uint32_t>()(k.childNodeIndices[1]) << 1);
}

double InsightScore::CorrelationStrength()
{
  return abs((int64_t(cases[0] * cases[3]) - int64_t(cases[1] * cases[2])) / sqrt(double((cases[0] + cases[1]) * (cases[2] + cases[3]) * (cases[0] + cases[2]) * (cases[1] + cases[3]))));
}

void InsightScore::Clear()
{
  memset(cases, 0, sizeof(cases));
}
