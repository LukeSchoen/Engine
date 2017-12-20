#include "NeuralNetwork.h"


double RandomDouble()
{
	return rand() / double(RAND_MAX);
}

bool RandomBool()
{
	return RandomDouble() < 0.5;
}

int main()
{
	NeuralNetwork net({ 2, 5, 5, 1 });
	std::vector<double> result;
	int64_t i = 0;
	while (true)
	{
		bool a = RandomBool();
		bool b = RandomBool();
		bool expected = a ^ b;
		net.FeedForward({ double(a), double(b) });
		net.BackPropogate({ double(expected) });
		if (i % 100000 == 0)
		{
			printf("%.5f\n", net.GetAverageError());
		}
		++i;
	}
    return 0;
}

