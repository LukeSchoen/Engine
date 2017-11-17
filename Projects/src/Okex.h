#ifndef Okex_h__
#define Okex_h_

#include <thread>
#include <time.h>
#include <string>
#include "net\http\curl\okcoinapi.h"

class Okex;
void ThreadedUpdate(Okex * okex);

class Trade
{
public:
  int64_t price;
  time_t dateTime;
  std::string DateTime() { return ctime(&dateTime); }
};

class Okex
{
public:
  Okex(std::string apiKey, std::string secretKey, bool futures)
    : m_okex(apiKey, secretKey, futures) {}

  void Update()
  {
    if (!ready)
      return;
    ready = false;
    std::thread(ThreadedUpdate, this).detach();
  }

  std::string CloseOrders() // Take market and exit
  {
    if (activeOrders > 0) return m_okex.FutureTrade(CloseLong, activeOrders, 0, x10leverage, this_week);
    if (activeOrders < 0) return m_okex.FutureTrade(CloseShort, 0 - activeOrders, 0, x10leverage, this_week);
  }

  std::string OpenLong() { return m_okex.FutureTrade(LongContract, 1, 0, x10leverage, this_week); }
  std::string OpenShort() { return m_okex.FutureTrade(ShortContract, 1, 0, x10leverage, this_week); }

  volatile bool ready = true;
  volatile bool started = false;
  volatile int64_t currentBalance = 0; // BTC
  volatile int64_t currentPrice = 0; // BTC in USD

  volatile int64_t activeOrders = 0; // Whole number of contracts, positive is long, negative is short
  volatile int64_t activeOrderAvgPrice = 0; // USD

  OKCoinApi m_okex;

private:

};

int64_t FetchCurrentBalance(Okex *okex)
{
  return okex->m_okex.FutureMyBalance();
}

int64_t FetchCurrentPrice(Okex *okex)
{
  return okex->m_okex.FuturePrice(string("btc_usd"), std::string("this_week"));
}

int64_t FetchOrderStatus(Okex *okex, int64_t *avgPrice)
{
  return okex->m_okex.FutureMyOrders(avgPrice);
}

void ThreadedUpdate(Okex * okex)
{
  okex->currentPrice = FetchCurrentPrice(okex);
  if (okex->started) Sleep(500);

  okex->currentBalance = FetchCurrentBalance(okex);
  if (okex->started) Sleep(500);

  int64_t avgPrice;
  okex->activeOrders = FetchOrderStatus(okex, &avgPrice);
  okex->activeOrderAvgPrice = avgPrice;
  if (okex->started) Sleep(500);

  okex->started = true;
  okex->ready = true;
}

#endif // Okex_h__
