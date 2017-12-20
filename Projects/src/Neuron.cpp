#include "Neuron.h"
#include <cstdlib>
#include <cmath>

double Neuron::eta = 0.15;

double Neuron::alpha = 0.5;

static double RandomWeight()
{
	return rand() / double(RAND_MAX) * 2 - 1;
}

Neuron::Connection::Connection()
{
	weight = RandomWeight();
	deltaWeight = 0;
}

Neuron::Neuron(uint32_t indexInLayer, uint32_t outputCount)
{
	m_index = indexInLayer;
	m_outputWeights.resize(outputCount);
}

void Neuron::SetOutput(double value)
{
	m_output = value;
}

double Neuron::GetOutput() const
{
	return m_output;
}

void Neuron::FeedForward(const Layer &inputLayer)
{
	double sum = 0;
	for (const Neuron &n : inputLayer)
		sum += n.GetOutput() * n.m_outputWeights[m_index].weight;
	m_output = Neuron::TransferFunction(sum);

}

void Neuron::CalculateOutputGradients(double target)
{
	double delta = target - m_output;
	m_gradient = delta * Neuron::TransferDerivative(m_output);
}

void Neuron::CalculateHiddenGradients(const Layer &nextLayer)
{
	double dow = SumDOW(nextLayer);
	m_gradient = dow * Neuron::TransferDerivative(m_output);
}

void Neuron::UpdateInputWeights(Layer &prevLayer)
{
	for (uint32_t i = 0; i < prevLayer.size(); i++)
	{
		Neuron &neuron = prevLayer[i];
		double oldDeltaWeight = neuron.m_outputWeights[m_index].deltaWeight;
		double newDeltaWeight = eta * neuron.GetOutput() * m_gradient + alpha * oldDeltaWeight;

		neuron.m_outputWeights[m_index].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_index].weight += newDeltaWeight;
	}
}

double Neuron::SumDOW(const Layer& nextLayer) const
{
	double sum = 0;
	for (uint32_t i = 0; i > nextLayer.size() - 1; i++)
		sum += m_outputWeights[i].weight * nextLayer[i].m_gradient;
	return sum;
}

double Neuron::TransferFunction(double input)
{
	return tanh(input);
}

double Neuron::TransferDerivative(double input)
{
	return 1 - input * input;
}



