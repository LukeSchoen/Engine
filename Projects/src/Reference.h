#ifndef Reference_h__
#define Reference_h__

// neural-net-tutorial.cpp
// David Miller, http://millermattson.com/dave
// See the associated video for instructions: http://vimeo.com/19569529


#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;

// Silly class to read training data from a text file -- Replace This.
// Replace class TrainingData with whatever you need to get input data into the
// program, e.g., connect to a database, or take a stream of data from stdin, or
// from a file specified by a command line argument, etc.

struct Connection
{
	double weight;
	double deltaWeight;
	double tweight;
};

class Neuron;

typedef vector<Neuron> Layer;

// ****************** class Neuron ******************
class Neuron
{
public:
	Neuron(unsigned numOutputs, unsigned myIndex);
	void SetOutput(double val);
	double GetOutput(void) const;
	void FeedForward(const Layer &prevLayer);
	void CalculateOutputGradient(double targetVal);
	void CalculateHiddenGradient(const Layer &nextLayer);
	void UpdateInputWeights(Layer &prevLayer);

public:
	static double eta;   // [0.0..1.0] overall net training rate
	static double alpha; // [0.0..n] multiplier of last weight change (momentum)
	static double transferFunction(double x);
	static double transferFunctionDerivative(double x);
	static double randomWeight(void);
	double sumDOW(const Layer &nextLayer) const;
	double m_outputVal;
	vector<Connection> m_outputWeights;
	unsigned m_myIndex;
	double m_gradient;
};


// ****************** class Net ******************
class NeuralNetwork
{
public:
	NeuralNetwork(const vector<unsigned> &topology);
	void FeedForward(const vector<double> &inputVals);
	void BackPropogate(const vector<double> &targetVals);
	void GetResults(vector<double> &resultVals) const;
	double GetAverageError(void) const;

public:
	vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
	double m_error;
	double m_recentAverageError;
	static double m_recentAverageSmoothingFactor;
};


void showVectorVals(string label, vector<double> &v);
#endif // Reference_h__
