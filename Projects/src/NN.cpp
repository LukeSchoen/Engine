#include "nn.h"
#include "ImageFile.h"
//#include "NeuralNetwork.h"
#include "Reference.h"
#include "Vector2.h"
#include "Window.h"
#include <algorithm>
#include "Controls.h"
#include <vector>
#include "StreamFile.h"

double RandomDouble()
{
  return rand() / double(RAND_MAX);
}

int64_t Random(int64_t min, int64_t max)
{
  double r = RandomDouble();
  return (int64_t)(min * (1 - r) + max * r);
}

// void NeuralNetTest()
// {
// 	Window window("NN", false, 256, 256, false);
// 
// 	ImageFile image;
// 	uint32_t w, h;
// 	uint32_t *pImg = image.ReadImage("D:/UserFiles/Desktop/box.png", &w, &h);
// 	NeuralNetwork net({ 2, 1, 3 });
// 
// 	while (true)
// 	{
// 		for (int64_t i = 0; i < 10000; i++)
// 		{
// 			std::vector<int64_t> input = { (rand() % w), (rand() % h) };
// 			net.FeedForward({ ((double)input[0]) / w, ((double)input[1]) / h });
// 			uint32_t c = pImg[input[0] + input[1] * w];
// 			double r, g, b;
// 			r = c & 255;
// 			g = (c >> 8) & 255;
// 			b = (c >> 16) & 255;
// 			net.BackPropogate({ r / 256.0, g / 256.0, b / 256.0 });
// 		}
// 
// 		std::vector<double> results;
// 		for (int y = 0; y < window.height; y++)
// 			for (int x = 0; x < window.width; x++)
// 			{
// 				net.FeedForward({ ((double)x) / window.width, ((double)y) / window.height });
// 				net.GetResults(results);
// 				window.pixels[x + y * window.width] = uint32_t((int)std::max(std::min(results[0] * 256, 255.0), 0.0) + ((int)std::max(std::min(results[1] * 256, 255.0), 0.0)) * 256 + ((int)std::max(std::min(results[2] * 256, 255.0), 0.0)) * 256 * 256);
// 				//if(x < w && y < h) window.pixels[x + y * window.width] = pImg[x + y * w];
// 			}
// 		window.Swap();
// 		Controls::Update();
// 	}
// 
// 
// 
// }


struct TradeRecord
{
  int64_t timeStamp;
  float price;
  float volume;
};

void NeuralNetTest()
{
  ImageFile image;
  uint32_t w, h;
  uint32_t *pImg = image.ReadImage("D:/UserFiles/Desktop/sometrash.png", &w, &h);
  NeuralNetwork net(std::vector<uint32_t>{ 200, 128, 64, 32, 16, 8, 4, 2, 1 });

  StreamFileReader reader("D:/UserFiles/Desktop/block.x");

  int64_t size;
  void* data = reader.ReadToEnd(&size);

  int64_t count = size / 16;

  std::vector<TradeRecord> records;
  char* next = (char*)data;
  for (int i = 0; i < count; i++)
  {
    TradeRecord n;
    memcpy(&n.timeStamp, next, sizeof(n.timeStamp));
    next += sizeof(n.timeStamp);
    memcpy(&n.price, next, sizeof(n.price));
    next += sizeof(n.price);
    memcpy(&n.volume, next, sizeof(n.volume));
    next += sizeof(n.volume);
    records.push_back(n);
  }

  std::vector<TradeRecord> training;
  std::vector<TradeRecord> testing;
  std::vector<double> result;
  training.resize(records.size() / 2);
  testing.resize(records.size() - records.size() / 2);

  for (int i = 0; i < training.size(); i++)
    training[i] = records[i];
  for (int i = 0; i < testing.size(); i++)
    testing[i] = records[training.size() + i];


  std::vector<double> sinData;
  sinData.resize(1000000);
  for (int i = 0; i < 1000000; i++)
    sinData[i] = sin(i / float(100));

  std::vector<double> input;
  int id = 0;
  double rms = 0.005;
  double controlRMS = 0.005;
  while (true)
  {
    for (int j = 0; j < 50000; j++)
    {
      int64_t index = Random(100, training.size() - 50);
      input.resize(200);
      for (int i = 0; i < 100; i++)
      {
        input[i * 2 + 0] = training[index - 100 + i].price;
        input[i * 2 + 1] = training[index - 100 + i].volume;
      }
      net.FeedForward(input);
      double currentPrice = training[index - 1].price;
      double targetPrice = training[index + 50].price;
      double aD = targetPrice - currentPrice;
      double naD = aD / currentPrice;
      net.BackPropogate(std::vector<double>{naD});
    }
    printf("A: %.2f\n", net.GetAverageError());

    double newRMS = 0;
    double newControlRMS = 0;
    double smooth = 20;
      int64_t c = 0;
      for (int i = 0; i < 5000; i++)
      {
        int64_t index = Random(100, testing.size() - 50);
        input.resize(200);
        for (int i = 0; i < 100; i++)
        {
          input[i * 2 + 0] = testing[index - 100 + i].price;
          input[i * 2 + 1] = testing[index - 100 + i].volume;
        }
        net.FeedForward(input);
        net.GetResults(result);
        double pD = result[0] * testing[index - 1].price;
        double aD = testing[index + 50].price - testing[index - 1].price;
        double error = (pD - aD) / testing[index - 1].price;
        double controlError = aD / testing[index - 1].price;
        //double error = training[index + 50].price - result[0];
        newRMS += error * error;
        newControlRMS += controlError * controlError;
        c++;
      }
      newRMS /= c;
      newRMS = sqrt(newRMS);
      newControlRMS /= c;
      newControlRMS = sqrt(newControlRMS);
      rms = (rms * smooth + newRMS) / (smooth + 1);
      controlRMS = (controlRMS * smooth + newControlRMS) / (smooth + 1);
      printf("%.5f, %.5f\n", rms, controlRMS);
  }

    // StreamFileWriter writer((std::string("E:/Networks/stock") + std::to_string(id) + std::string(".net")).c_str());
    // for (int64_t i = 0; i < net.m_layers.size(); i++)
    // {
    //  for (int64_t j = 0; j < net.m_layers[i].size(); j++)
    //  {
    //   for (int64_t k = 0; k < net.m_layers[i][j].m_outputWeights.size(); k++)
    //   {
    //    writer.WriteBytes(&(net.m_layers[i][j].m_outputWeights[k]), sizeof(net.m_layers[i][j].m_outputWeights[k]));
    //   }
    //  }
    // }
    // id++;
    //}


    Window window("NN", false, w, h, false);

    int index = 0;
    while (true)
    {
      index++;
      for (int64_t i = 0; i < 10000; i++)
      {
        std::vector<int64_t> input = { (rand() % w), (rand() % h) };
        //nets[netID].FeedForward({ ((double)input[0]) / w, ((double)input[1]) / h });
        uint32_t c = pImg[input[0] + input[1] * w];

        double r, g, b;
        r = (c & 255) / 255.0;
        g = ((c >> 8) & 255) / 255.0;
        b = ((c >> 16) & 255) / 255.0;
        std::vector<double> res;
        //nets[l].FeedForward({ ((double)input[0]) / w, ((double)input[1]) / h });
        net.GetResults(res);
        r -= res[0];
        g -= res[1];
        b -= res[2];
        //c = std::min(std::max(res[0] * 255, 0.0), 255.0) + std::min(std::max(res[0] * 255, 0.0), 255.0) * 256 + std::min(std::max(res[0] * 255, 0.0), 255.0) * 256 * 265;

        net.FeedForward({ ((double)input[0]) / w, ((double)input[1]) / h });
        net.BackPropogate({ r, g, b });
      }

      std::vector<double> results;
      for (int y = 0; y < window.height; y++)
      {
        for (int x = 0; x < window.width; x++)
        {
          double r, g, b;
          r = g = b = 0;
          net.FeedForward({ ((double)x) / window.width, ((double)y) / window.height });
          net.GetResults(results);
          r += results[0];
          g += results[1];
          b += results[2];
          //if(x < w && y < h) window.pixels[x + y * window.width] = pImg[x + y * w];
          window.pixels[x + y * window.width] = uint32_t((int)std::max(std::min(r * 256, 255.0), 0.0) * 256 * 256 + ((int)std::max(std::min(g * 256, 255.0), 0.0)) * 256 + ((int)std::max(std::min(b * 256, 255.0), 0.0)));
        }
      }
      window.Swap();
      if (!Controls::Update()) exit(0);
    }
  }