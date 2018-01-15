#ifndef Neuron_h__
#define Neuron_h__

#include <cstdint>
#include <vector>

using namespace std;

class Neuron
{
public:
	typedef vector<Neuron> Layer;
	struct Connection
	{
		double weight;
		double deltaWeight;
		Connection();
	};

	Neuron(uint32_t indexInLayer, uint32_t outputCount);
	void SetOutput(double value);
	double GetOutput() const;
	void FeedForward(const Layer &inputLayer);

	void CalculateOutputGradients(double target);
	void CalculateHiddenGradients(const Layer &nextLayer);
	void UpdateInputWeights(Layer &prevLayer);
	double SumDOW(const Layer& nextLayer) const;

private:



	static double TransferFunction(double input);
	static double TransferDerivative(double input);
	static double eta;
	static double alpha;
	uint32_t m_index;
	double m_output = 1;
	double m_gradient;
	vector<Connection> m_outputWeights;
};
#endif // Neuron_h__