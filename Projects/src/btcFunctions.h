#ifndef btcFunctions_h__
#define btcFunctions_h__
#include "btcTypes.h"
#include "StreamFile.h"


struct Graphic
{
  Graphic(Window *_window, int _width, int _height, int _xpos, int _ypos)
  {
    window = _window;
    xpos = _xpos;
    ypos = _ypos;
    width = _width;
    height = _height;
  }

  void SetPixel(int x, int y, uint32_t c) const
  {
    window->pixels[(x + xpos) + (y + ypos) * window->width] = c;
  }

  uint32_t GetPixel(int x, int y) const
  {
    return window->pixels[(x + xpos) + (y + ypos) * window->width];
  }

  Window *window;
  int xpos, ypos;
  int width, height;
};



struct Text
{
  Text(uint32_t *_img)
  {
    img = _img;
  }

  void DrawText(Graphic *g, const char *text, uint32_t col, int xpos, int ypos, int size = 1)
  {
    int strl = strlen(text);
    for (int c = 0; c < strl; c++)
    {
      char m = (text[c] - 33);
      int ix = m % 32;
      int iy = m / 32;
      if (text[c] != ' ')
        for (int y = 0; y < 8; y++)
          for (int x = 0; x < 8; x++)
            if (img[(ix * 8 + x) + (iy * 8 + y) * 256] == 0xFFFFFFFF)
              for (int sy = 0; sy < size; sy++)
                for (int sx = 0; sx < size; sx++)
                  g->SetPixel(xpos + (x) * size + sx, ypos + (y) * size + sy, col);
      if (text[c] == '\n')
        ypos += 8 * size;
      xpos += 8 * size;
    }
  }

  uint32_t *img;
};


std::string DollarToText(float f)
{
  static char t[1024];
  sprintf(t, "$%0.2f", f);
  return std::string(t);
}


orderList OpenOrderList(const char *filePath)
{
  std::vector<Order> ret;
  StreamFileReader stream(filePath);
  int orderCount;
  stream.ReadBytes(&orderCount, sizeof(orderCount));
  for (int i = 0; i < orderCount; i++)
  {
    float price, volume;
    stream.ReadBytes(&price, sizeof(price));
    stream.ReadBytes(&volume, sizeof(volume));
    ret.emplace_back(price, volume);
  }
  return ret;
}

void GetOrders()
{
  //const char *p = ASSETDIR "/BTCBot/OKCOIN MARKET";
  const char *p = "OKCOIN.exe MARKET";
  system(p);
}

void UpdateOrders()
{
  GetOrders();
}

orderList GetBuyOrders()
{
  return OpenOrderList("MarketBuy.dat");
}

orderList GetSellOrders()
{
  return OpenOrderList("MarketSell.dat");
}

void DrawOrdersGraph(const orderList &buys, const orderList &sells, const Graphic &screen)
{
  float HighestBuy = buys[0].price;
  float LowestBuy = buys.back().price;

  float HighestSell = sells[0].price;
  float LowestSell = sells.back().price;

  float ameragePrice = (LowestSell + HighestBuy) / 2.0;
  float spread = LowestSell - HighestBuy;

  float shownArea = abs(ameragePrice - LowestBuy);
  shownArea = Max(shownArea, abs(ameragePrice - HighestSell));

  float totalBuyVolume = 0;
  float totalSellVolume = 0;
  for (auto buy : buys) totalBuyVolume += buy.volume;
  for (auto sell : sells) totalSellVolume += sell.volume;
  float highestTotalVolume = Max(totalBuyVolume, totalSellVolume);

  float minNormalPrice = LowestBuy;
  float maxNormalPrice = HighestSell;

  float xf = 0;
  for (int i = sells.size() - 1; i >= 1; i--)
  {
    auto sell = sells[i];
    xf += sell.volume / highestTotalVolume;
    for (int x = 0; x < screen.width * xf; x++)
    {
      float yf = (sell.price - minNormalPrice) / (maxNormalPrice - minNormalPrice);
      int y = yf * (screen.height - 1);

      float nyf = (sells[i - 1].price - minNormalPrice) / (maxNormalPrice - minNormalPrice);
      int ny = nyf * (screen.height - 1);

      for (int yi = y; yi < ny; yi++)
      {
        screen.SetPixel(x, yi, 0xFF0000);
      }
    }
  }

  xf = 0;
  for (int i = 0; i < buys.size() - 1; i++)
  {
    auto buy = buys[i];
    xf += buy.volume / highestTotalVolume;
    for (int x = 0; x < screen.width * xf; x++)
    {
      float yf = (buy.price - minNormalPrice) / (maxNormalPrice - minNormalPrice);
      int y = yf * (screen.height - 1);

      float nyf = (buys[i + 1].price - minNormalPrice) / (maxNormalPrice - minNormalPrice);
      int ny = nyf * (screen.height - 1);

      for (int yi = ny; yi < y; yi++)
      {
        screen.SetPixel(x, yi, 0x00FF00);
      }
    }
  }
}

#endif // btcFunctions_h__
