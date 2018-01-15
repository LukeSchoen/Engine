#ifndef NeuralNetwork_h__
#define NeuralNetwork_h__

#include <vector>
#include <cstdint>
#include "Neuron.h"
using namespace std;

class NeuralNetwork
{
public:
	typedef vector<Neuron> Layer;
	NeuralNetwork(const vector<uint32_t> &topology);
	void FeedForward(const vector<double> &inputs);
	void BackPropogate(const vector<double> &outputs);
	void GetResults(vector<double> &results) const;
	double GetAverageError() const;
private:
	vector<Layer> m_layers;
	double m_error = 0;
	double m_recentAverageError = 0;
	double m_recentAverageSmoothingFactor = 1000;
};
#endif // NeuralNetwork_h__