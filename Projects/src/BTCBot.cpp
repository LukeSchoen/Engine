#include "Window.h"
#include "Controls.h"
#include "softText.h"
#include "SoftButton.h"
#include "SoftGraph.h"
#include "Okex.h"
#include "StreamFile.h"
#include "Pre.h"
#include <string>
#include <stdio.h>

void BTCBot()
{
  if (false)
  {
    // Load Prices
    std::vector<int> priceInCents;
    int64_t fileLen;
    StreamFileReader reader("C:/temp/btc_history.x", &fileLen);
    std::string file((char*)reader.ReadToEnd(), fileLen);
    const char *ptr = file.c_str();
    while (true)
    {
      int val = atoi(ptr);
      priceInCents.push_back(val);
      while (*ptr != '\n' && *ptr != NULL) ptr++;
      if(*ptr == NULL) break;
      ptr++;
    }


    priceInCents.pop_back();

    float usd = 100;
    float btc = 0;
    int lastPrice = priceInCents[0];
    int trades = 0;
    bool buyMode = true;
    int goods = 0;
    int bads = 0;
    int gppdsThreshold = 15;
    for (auto item : priceInCents)
    {
      float percentChange = item != lastPrice ? percentChange = ((item / (lastPrice + 0.0f))-1) * 100.0f : 0.0f;

      if (buyMode)
      {
        if (percentChange < 0)
        {
          goods++;
          bads = 0;
        }
        else
        {
          bads++;
          goods = 0;
        }

        if (goods > gppdsThreshold)
        {
          btc = usd / (item / 100);
          usd = 0;
          buyMode = 1 - buyMode;
          lastPrice = item;
          trades++;
        }

      }
      else if(!buyMode )//&& percentChange > threshold)
      {
        if (percentChange > 0)
        {
          goods++;
          bads = 0;
        }
        else
        {
          bads++;
          goods = 0;
        }


        if (goods > gppdsThreshold)
        {
          usd = btc * (item / 100);
          btc = 0;
          buyMode = 1 - buyMode;
          lastPrice = item;
          trades++;
        }

      }
    }


//     float usd = 100;
//     float btc = 0;
//     float threshold = 5;
//     int lastPrice = priceInCents[0];
//     int trades = 0;
//     bool buyMode = true;
//     for (auto item : priceInCents)
//     {
//       float percentChange = 0;
//       if (item != lastPrice)
//         percentChange = ((item / (lastPrice + 0.0f)) - 1) * 100.0f;
// 
//       if (buyMode)//&& percentChange < 0 - threshold)
//       {
//         btc = usd / (item / 100);
//         usd = 0;
//         buyMode = 1 - buyMode;
//         lastPrice = item;
//         trades++;
//       }
//       else if (!buyMode)//&& percentChange > threshold)
//       {
//         usd = btc * (item / 100);
//         btc = 0;
//         buyMode = 1 - buyMode;
//         lastPrice = item;
//         trades++;
//       }
//     }

    printf("%d trades\n", trades);
    printf("%f usd\n", std::max(usd, btc * 0.01f * priceInCents[priceInCents.size() - 1]));

    //// Normalize Inputs
    //int start = priceInCents[0]; for (auto & item : priceInCents) item -= start; // offset from start
    //for (auto & item : priceInCents) item /= 500; // rescale
    //for (int64_t i = 0; i < priceInCents.size() - 1; i++) if (priceInCents[i] == priceInCents[i + 1]) priceInCents.erase(priceInCents.begin() + i--); // Remove duplicates

    //std::vector<int> predictedCents;
    //for (int64_t p = 0; p < 256; p++)
    //{
    //  // Create some inputs
    //  std::vector<int> input; for (int i = 0; i < 3; i++) input.push_back(priceInCents[i + p]);
    //  auto ret = Pre::SimplePredict(input, std::max((int)input.size(), 8), 32);
    //  predictedCents.push_back(ret[0] + input[0]);
    //}
  
    //priceInCents;
    //predictedCents;

    getchar();
    exit(0);
  }



  Window window("BTCBOT", false, 1200, 400);
  SoftText text(&window);

  std::string apiKey = "28d20390-2427-40b0-8b9d-5567831194cd";
  std::string secretKey = "7678A929B5050DBE82325B34238365D7"; // swap
  Okex okex(apiKey, secretKey, true);

  SoftObject a("UP", &window, 40, 115, 128, 32, 0x999999, 0x112211);

  SoftButton longButton("UP", &window, 40, 115, 128, 32, 0x999999, 0x112211);
  SoftButton shortButton("DN", &window, 40, 230, 128, 32, 0x999999, 0x221111);
  SoftButton exitButton("EXIT", &window, 180, 250, 128, 32, 0x999999, 0x111122);

  SoftGraph priceGraph("", &window, 300, 16, 400, 365, 0xffffff, 0x222222);
  SoftGraph predictionGraph("", &window, 300 + priceGraph.m_width + 20, 16, 400, 365, 0xffffff, 0x222222);

  int64_t startingBalance = 0;

  int64_t okexUpdateStep = 0;
  int64_t okexUpdateTime = 50;

  while (Controls::Update())
  {
    window.Clear();
    bool newData = okexUpdateStep++ == 0;
    if (okexUpdateStep >= okexUpdateTime) okexUpdateStep = 0;

    if (newData) okex.Update();

    // Prices, balances and orders
    if (okex.started)
    {
      text.DrawText("Price", 0xFFDF00, 60, 25, 1);
      text.DrawPriceUSD(okex.currentPrice, 0xFFDF00, 0x884800, 30, 45, 2);

      text.DrawText("Balance", 0x00FF00, 60, 320, 1);
      text.DrawPriceBTC(okex.currentBalance, 0x00FF00, 0x008800, 30, 340, 2);
      if (okex.activeOrders != 0)
      {
        if (exitButton.Update()) okex.CloseOrders();
        text.DrawText("Active Order Open!", 0x990000, 30, 180, 2);

        text.DrawText("Type:", 0x444444, 32, 210, 1);
        if (okex.activeOrders > 0)
          text.DrawText("Long", 0x005500, 80, 210, 1);
        else
          text.DrawText("Short", 0x550000, 80, 210, 1);
        text.DrawText("Price:", 0x444444, 140, 210, 1);
        text.DrawPriceUSD(okex.activeOrderAvgPrice, 0xFFDF00, 0x884800, 200, 210, 1);
        //ext.DrawPriceBTC(okex.currentBalance - okex.startingBalance, 0x00E000, 0x009000, 350, 210, 1);

        text.DrawText("Amount:", 0x444444, 32, 230, 1);
        text.DrawNumber(abs(okex.activeOrders), 0x999999, 100, 230, 1);

        text.DrawText("Change:", 0x444444, 140, 230, 1);
        text.DrawPriceUSD(okex.currentPrice - okex.activeOrderAvgPrice, 0xFFDF00, 0x884800, 200, 230, 1);
      }
      else
      {
        if (shortButton.Update()) okex.OpenShort();
        if (longButton.Update()) okex.OpenLong();
        text.DrawText("No Orders", 0x1E90FF, 30, 180, 2);
      }

      // Draw Price Graph
      static std::vector<int64_t> priceList;
      static int64_t itemNum = 0;
      priceList.clear();
      okex.m_lock.lock();
      for(auto & item : okex.m_tradeHistory)
        priceList.push_back(item.price);
      okex.m_lock.unlock();
      priceGraph.SetValueList(&priceList);
      priceGraph.Update();

      static std::vector<int64_t> predictions64;
      // Predict Price
      if (newData)
      {
        predictions64.clear();

        // Load Prices
        std::vector<int> priceInCents;
        for (auto & item : priceList) priceInCents.emplace_back(item);

        // Normalize Inputs
        for (auto & item : priceInCents) item /= 400; // rescale
        for (int64_t i = 0; i < priceInCents.size() - 1; i++) if (priceInCents[i] == priceInCents[i + 1]) priceInCents.erase(priceInCents.begin() + i--); // Remove duplicates

        if (priceInCents.size() > 5) priceInCents.erase(priceInCents.begin(), priceInCents.begin() + priceInCents.size() - 5);

        int start = priceInCents[0]; for (auto & item : priceInCents) item -= start; // offset from start

        printf("--------\n");
        for (auto & item : priceInCents) printf("%d\n", item);

        auto predictions = Pre::PredictSequence(priceInCents, 5, 1000, 32);
        for (auto item : predictions) predictions64.emplace_back(item);
        predictionGraph.SetValueList(&predictions64);
      }
      predictionGraph.Update();

      // Print Price
      /*static bool started = false;
      static StreamFileWriter stream("c:/temp/btc_history.x");
      static time_t prevTime = clock();*/

      //if (clock() - prevTime > 1000)
      //{
      //  prevTime = clock();
      //  int currPrice = (int)floor(okex.currentPrice);
      //  printf("%d\n", currPrice);
      //  static char buffer[64];
      //  sprintf(buffer, "%d\n", currPrice);
      //  stream.WriteBytes(buffer, strlen(buffer));
      //  stream.FlushStream();
      //}

    }
    else
    {
      text.DrawText("Retrieving data...", 0x1E90FF, 30, 30, 1);
    }

    window.Swap();
    Sleep(5);
  }
}
