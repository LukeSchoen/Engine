#include "okex.h"

int64_t FetchCurrentBalance(Okex *okex)
{
  return okex->m_okex.FutureMyBalance();
}

int64_t FetchCurrentPrice(Okex *okex, int64_t *CurrentPriceTime)
{
  return okex->m_okex.FuturePrice(string("btc_usd"), std::string("this_week"));
}

int64_t FetchOrderStatus(Okex *okex, int64_t *avgPrice)
{
  return okex->m_okex.FutureMyOrders(avgPrice);
}

void ThreadedUpdate(Okex *okex)
{
  static Trade lastTrade;
  // Fetch price
  okex->currentPrice = FetchCurrentPrice(okex, &lastTrade.dateTime);
  // Log price
  time_t lastPrice = lastTrade.price;
  //if (okex->currentPrice != lastPrice)
  {
    lastTrade.price = okex->currentPrice;
    okex->m_tradeHistory.push_back(lastTrade);
    while (okex->m_tradeHistory.size() > 256)
      okex->m_tradeHistory.erase(okex->m_tradeHistory.begin());
  }

  // Balance
  //okex->currentBalance = FetchCurrentBalance(okex);
  //if (okex->started) Sleep(500);

  // Active Orders
  //int64_t avgPrice;
  //okex->activeOrders = FetchOrderStatus(okex, &avgPrice);
  //okex->activeOrderAvgPrice = avgPrice;
  //if (okex->started) Sleep(500);

  okex->started = true;
  okex->ready = true;
}

Okex::Okex(std::string apiKey, std::string secretKey, bool futures)
  : m_okex(apiKey.c_str(), secretKey.c_str(), futures)
{

}

void Okex::Update()
{
  if (!ready)
    return;
  ready = false;
  std::thread(ThreadedUpdate, this).detach();
}

std::string Okex::CloseOrders() // Take market and exit
{
  if (activeOrders > 0) return m_okex.FutureTrade(CloseLong, activeOrders, 0, x10leverage, this_week);
  if (activeOrders < 0) return m_okex.FutureTrade(CloseShort, 0 - activeOrders, 0, x10leverage, this_week);
}

std::string Okex::OpenLong()
{
  startingBalance = currentBalance; return m_okex.FutureTrade(LongContract, 1, 0, x10leverage, this_week);
}

std::string Okex::OpenShort()
{
  startingBalance = currentBalance; return m_okex.FutureTrade(ShortContract, 1, 0, x10leverage, this_week);
}

// std::string Trade::DateTime()
// {
//   return std::string(ctime(&dateTime));
// }
