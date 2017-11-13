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
  void Update()
  {
    if (!ready)
      return;
    ready = false;
    std::thread(ThreadedUpdate, this).detach();
  }

  volatile bool ready = true;
  volatile int64_t currentPrice = 0;

private:

};


int64_t FetchCurrentPrice()
{
  int lastPtr = -1;
  std::string ret;
  while (lastPtr == -1)
  {
    std::string cn_apiKey = "";
    std::string cn_secretKey = "";
    OKCoinApiCom futures(cn_apiKey, cn_secretKey);
    ret = futures.DoFuture_Ticker(string("btc_usd"), std::string("this_week"));
    lastPtr = ret.find("last");
    if (lastPtr == -1)
    {
      Sleep(100);
      printf("Bad...\n");
    }
  }
  printf("Good...\n");
  ret = ret.substr(lastPtr + 6);
  ret = ret.substr(0, ret.find(","));
  if(ret.find(".") > 0)
    ret.erase(std::remove_copy(ret.begin(), ret.end(), ret.begin(), '.'), ret.end());
  return stoi(ret);
}

void ThreadedUpdate(Okex * okex)
{
  okex->currentPrice = FetchCurrentPrice();
  okex->ready = true;
}

#endif // Okex_h__
