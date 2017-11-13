#include "Window.h"
#include "Controls.h"
#include "okex.h"
#include "softText.h"

void BTCBot()
{
  Window window("BTCBOT", false);
  SoftText text(&window);

  Okex okex;

  while (Controls::Update())
  {

    window.Clear();
    okex.Update();

    text.DrawPrice(okex.currentPrice, 0xFFDF00, 50, 50, 2);

    window.Swap();
  }

}
