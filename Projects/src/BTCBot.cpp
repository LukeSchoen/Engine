#include "Window.h"
#include "Threads.h"
#include "Controls.h"
#include "FrameRate.h"
#include <vector>
#include "Assets.h"
#include "StreamFile.h"
#include <direct.h>
#include "btcTypes.h"
#include "btcFunctions.h"
#include <mutex>
#include "ImageFile.h"
#include <string>

std::mutex updateThread;

// Thread synchronizers
static volatile bool running = true;
static volatile bool streaming = true;
std::vector<Order> buys;
std::vector<Order> sells;

static int updateExchangeData(void *ptr)
{
  while (running)
  {
    Sleep(250);
    updateThread.lock();
    UpdateOrders();
    buys = GetBuyOrders();
    sells = GetSellOrders();
    updateThread.unlock();
  }
  streaming = false;
  return 0;
}


void BTCBot()
{
  chdir(ASSETDIR "/BTCBot/"); 
  uint32_t x, y;
  Text text(ImageFile::ReadImage("font.bmp", &x, &y));

  Window window("OKCOIN", false, 1500, 900, false);

  SDL_CreateThread(updateExchangeData, "streamer", nullptr);

  Graphic ordersGraphBox(&window, 256, window.height, window.width - 256, 0);


  float openPrice = 0;


  while (Controls::Update())
  {
    if (buys.size() > 0)
    {
      if (updateThread.try_lock())
      {
        window.Clear(0, 190, 255);
        float highestBuy = buys[0].price;
        float lowestSell = sells.back().price;

        float ameragePrice = (lowestSell + highestBuy) / 2.0;
        float margin = lowestSell - highestBuy;


        DrawOrdersGraph(buys, sells, ordersGraphBox);
        text.DrawText(&ordersGraphBox, DollarToText(lowestSell).c_str(), 0x004400, 8, 8, 3);
        text.DrawText(&ordersGraphBox, DollarToText(highestBuy).c_str(), 0x440000, 8, ordersGraphBox.height - 8 - 8 * 3, 3);

        updateThread.unlock();

      }
      window.Swap();
    }
  }

  running = false; while (streaming) Sleep(10); // Wait for streamer to end before exiting
}
