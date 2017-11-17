#include "Window.h"
#include "Controls.h"
#include "okex.h"
#include "softText.h"
#include "softButton.h"

void BTCBot()
{
  Window window("BTCBOT", false, 800, 400);
  SoftText text(&window);


  std::string apiKey = "28d20390-2427-40b0-8b9d-5567831194cd";
  std::string secretKey = "7678A929B5050DBE82325B34238365D7";-swap
  Okex okex(apiKey, secretKey, true);

  //okex.StopLong();
  //std::string ret = okex.OpenLong();
  //ret = ret;
  //exit(0);

  softButton longButton(&window, 40, 115, 128, 32, 0x999999, 0x112211, "UP");
  softButton shortButton(&window, 40, 230, 128, 32, 0x999999, 0x221111, "DN");
  softButton exitButton(&window, 180, 250, 128, 32, 0x999999, 0x111122, "EXIT");

  while (Controls::Update())
  {
    window.Clear();
    okex.Update();

    if (okex.started)
    {
      text.DrawText("Price", 0xFFDF00, 60, 25, 1);
      text.DrawPriceUSD(okex.currentPrice, 0xFFDF00, 0x884800, 30, 45, 2);

      text.DrawText("Balance", 0x008000, 60, 320, 1);
      text.DrawPriceBTC(okex.currentBalance, 0x008000, 0x004000, 30, 340, 2);

      if (okex.activeOrders != 0)
      {
        if (exitButton.Update()) okex.CloseOrders();
        text.DrawText("Active Order Open!", 0x990000, 30, 180, 2);
        text.DrawText("Type:", 0x444444, 32, 210, 1);
        if(okex.activeOrders > 0)
          text.DrawText("Long", 0x005500, 80, 210, 1);
        else
          text.DrawText("Short", 0x550000, 80, 210, 1);
        text.DrawText("Price:", 0x444444, 140, 210, 1);
        text.DrawPriceUSD(okex.activeOrderAvgPrice, 0xFFDF00, 0x884800, 200, 210, 1);

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
    }
    else
    {
      text.DrawText("Retrieving data...", 0x1E90FF, 30, 30, 1);
    }

    window.Swap();
    Sleep(50);
  }

}
