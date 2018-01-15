#include "NeuralNetwork.h"
#include "Neuron.h"
#include <cassert>

NeuralNetwork::NeuralNetwork(const vector<unsigned int> &topology)
{
	m_layers.resize(topology.size());
	for (uint32_t i = 0; i < topology.size() - 1; i++)
		for(uint32_t j = 0; j <= topology[i]; j++)
			m_layers[i].emplace_back(j, topology[i + 1]); // +1 for bias neuron

	//Output layer
	for (uint32_t j = 0; j <= topology.back(); j++)
		m_layers.back().emplace_back(j, 0);
}

void NeuralNetwork::FeedForward(const vector<double> &inputs)
{
	assert(inputs.size() == m_layers[0].size() - 1);
	for (int i = 0; i < inputs.size(); i++)
		m_layers[0][i].SetOutput(inputs[i]);

	for (uint32_t i = 1; i < m_layers.size(); i++)
		for (uint32_t j = 0; j < m_layers[i].size() - 1; j++)
			m_layers[i][j].FeedForward(m_layers[i - 1]);
}

void NeuralNetwork::BackPropogate(const vector<double> &target)
{
	Layer& outputLayer = m_layers.back();
	m_error = 0;

	for (uint32_t i = 0; i < outputLayer.size() - 1; i++)
	{
		double delta = target[i] - outputLayer[i].GetOutput();
		m_error += delta * delta;
	}
	m_error /= outputLayer.size() - 1;
	m_error = sqrt(m_error);

	m_recentAverageError = (m_recentAverageError * m_recentAverageSmoothingFactor + m_error) / (m_recentAverageSmoothingFactor + 1);

	//Calculate output gradients
	for (uint32_t i = 0; i < outputLayer.size() - 1; i++)
		outputLayer[i].CalculateOutputGradients(target[i]);

	//Calculate gradient on hidden layers
	for (int64_t i = m_layers.size() - 2; i >= 0; --i)
	{
		Layer &hiddenLayer = m_layers[i];
		Layer &nextLayer = m_layers[i + 1];

		for (int j = 0; j < hiddenLayer.size(); j++)
			hiddenLayer[j].CalculateHiddenGradients(nextLayer);
	}

	//Update weights
	for (int64_t i = m_layers.size() - 1; i > 0; --i)
	{
		Layer &layer = m_layers[i];
		Layer &previousLayer = m_layers[i - 1];
		for (uint32_t j = 0; j < layer.size() - 1; j++)
			layer[j].UpdateInputWeights(previousLayer);
	}
}

void NeuralNetwork::GetResults(vector<double> &results) const
{
	results.clear();
	for (int i = 0; i < m_layers.back().size(); i++)
		results.push_back(m_layers.back()[i].GetOutput());
}

double NeuralNetwork::GetAverageError() const
{
	return m_recentAverageError;
}
