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
  std::string DateTime();
};

class Okex
{
public:
  Okex(std::string apiKey, std::string secretKey, bool futures);

  void Update();

  std::string CloseOrders(); // Take market and exit

  std::string OpenLong();
  std::string OpenShort();

  volatile bool ready = true;
  volatile bool started = false;
  volatile int64_t currentBalance = 0; // BTC
  volatile int64_t currentPrice = 0; // BTC in USD

  volatile int64_t activeOrders = 0; // Whole number of contracts, positive is long, negative is short
  volatile int64_t activeOrderAvgPrice = 0; // USD

  volatile int64_t startingBalance = 0;

  OKCoinApi m_okex;

  std::vector<Trade> m_tradeHistory;

private:

};

#endif // Okex_h__
