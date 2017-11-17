#ifndef __OKCOINAPI_H__
#define __OKCOINAPI_H__

#include <string>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include "curlprotocol.h"

using namespace std;

enum FutureContractType
{
  LongContract = 0,
  ShortContract = 1,
  CloseLong = 2,
  CloseShort = 3,
};

enum FutureContractMarket
{
  this_week = 0,
  next_week = 1,
  quarter = 2,
};

enum FutureContractLeverage
{
  x10leverage = 0,
  x20leverage = 1,
};

class OKCoinApi
{
public:
  OKCoinApi(string api_key, string secret_key, bool futures)
    : m_api_key(api_key)
    , m_secret_key(secret_key)
  {
    urlprotocol.InitApi(futures ? HTTP_SERVER_TYPE_COM : HTTP_SERVER_TYPE_CN);
    //Uri uri;
    //m_signature = uri.GetSign(m_secret_key);
  };

  void SetKey(string api_key, string secret_key);

  CUrlProtocol urlprotocol;

  string m_api_key;
  string m_secret_key;
  string m_signature;

  const char* FutureContractMarketNames[3] = { "this_week", "next_week", "quarter" };

  std::string FutureTrade(FutureContractType orderType = LongContract, int contractCount = 1 /*100usd each*/, int priceInUSDCents = 0 /*0=best price*/, FutureContractLeverage leverage = x20leverage, FutureContractMarket market = this_week)
  {
    std::string ret;
    while (true)
    {
      Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_TRADE);
      std::vector<std::pair<char*, std::string>> params;
      uri.AddParam("api_key", m_api_key);
      params.push_back({ "api_key", m_api_key });

      uri.AddParam("symbol", "btc_usd");
      params.push_back({ "symbol", "btc_usd" });

      uri.AddParam("contract_type", FutureContractMarketNames[market]);
      params.push_back({ "contract_type", FutureContractMarketNames[market]});

      static char buf[64];
      itoa(priceInUSDCents / 100, buf, 10);
      std::string price = buf;
      //price += ".";
      //itoa(priceInUSDCents & 100, buf, 10);
      //price += buf;
      uri.AddParam("price", price);
      params.push_back({ "price", price });

      itoa(contractCount, buf, 10);
      std::string amt = buf;
      uri.AddParam("amount", amt);
      params.push_back({ "amount", amt });

      int ot = 0;
      if (orderType == LongContract) ot = 1;
      if (orderType == ShortContract) ot = 2;
      if (orderType == CloseLong) ot = 3;
      if (orderType == CloseShort) ot = 4;

      uri.AddParam("type", std::to_string(ot));
      params.push_back({ "type", std::to_string(ot) });

      uri.AddParam("match_price", priceInUSDCents == 0 ? "1" : "0");
      params.push_back({ "match_price", priceInUSDCents == 0 ? "1" : "0" });

      uri.AddParam("lever_rate", leverage == x20leverage ? "20" : "10");
      params.push_back({ "lever_rate", leverage == x20leverage ? "20" : "10" });

      m_signature = uri.GetSign(m_secret_key);
      uri.AddParam("secret_key", m_secret_key);
      params.push_back({ "secret_key", m_secret_key });

      uri.AddParam("sign", m_signature);
      params.push_back({ "sign", m_signature });

      auto s = uri.GetParamSet();
      uri.Requset();
      ret = uri.result;
      if (ret.find("order_id") == -1)
      {
        printf("bad..\n");
        Sleep(100);
        continue;
      }
      printf("good..\n");
      break;
    }
    return ret;
  }

  int64_t FuturePrice(string &symbol, string &contract_type);

  int64_t FutureMyOrders(int64_t *startPrice = nullptr, FutureContractMarket market = this_week)
  {
    while (true)
    {
      Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_POSITION_4FIX);
      uri.AddParam("api_key", m_api_key);
      uri.AddParam("symbol", "btc_usd");
      uri.AddParam("contract_type", FutureContractMarketNames[market]);
      uri.AddParam("type", "1");
      string sign = uri.GetSign(m_secret_key);
      uri.AddParam("secret_key", m_secret_key);
      uri.AddParam("sign", sign);
      uri.Requset();
      std::string ret = uri.result;
      int retPos = ret.find("holding");
      if (retPos == -1)
      {
        printf("bad..\n");
        Sleep(100);
        continue;
      }
      printf("good...\n");

      int buyLoc = ret.find("buy_amount");
      int selLoc = ret.find("sell_amount");

      // Check longs
      if (buyLoc >= 0)
      {
        std::string p = ret.substr(buyLoc + 12);
        p = p.substr(0, p.find(","));
        int longContracts = atoi(p.c_str());
        if (longContracts)
        {
          int avgLoc = ret.find("buy_price_avg");
          std::string a = ret.substr(avgLoc + 15);
          a = a.substr(0, a.find(","));
          if (startPrice) *startPrice = atoi(a.c_str()) * 100;
          return longContracts;
        }
      }
      
      // Check shorts
      if (selLoc >= 0)
      {
        std::string p = ret.substr(selLoc + 13);
        p = p.substr(0, p.find(","));
        int shortContracts = atoi(p.c_str());
        if (shortContracts)
        {
          int avgLoc = ret.find("sell_price_avg");;
          std::string a = ret.substr(avgLoc + 16);
          a = a.substr(0, a.find(","));
          if (startPrice) *startPrice = atoi(a.c_str()) * 100;
          return -shortContracts;
        }
      }
      if (startPrice) *startPrice = 0;
      return 0;
    }

  }

  int64_t FutureMyBalance()
  {
    while (true)
    {
      Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_USERINFO_4FIX);
      uri.AddParam("api_key", m_api_key);
      string sign = uri.GetSign(m_secret_key);
      uri.AddParam("secret_key", m_secret_key);
      uri.AddParam("sign", sign);
      uri.Requset();
      std::string ret = uri.result;
      int retLoc = ret.find("rights");
      if (retLoc == -1)
      {
        printf("bad..\n");
        Sleep(100);
        continue;
      }
      ret = ret.substr(retLoc + 8);
      ret = ret.substr(0, ret.find(","));
      printf("good...\n");

      std::string left = ret.substr(0, ret.find("."));
      std::string right = ret.substr(ret.find(".") + 1);
      int64_t bitCoins = atoi(left.c_str());
      int64_t satoshi = atoi(right.c_str());
      return bitCoins * 100000000 + satoshi;
    }
  }


  // Spot Trading
  string GetTicker(string &symbol);
  string GetDepth(string &symbol, string &size, string &merge);
  string GetTrades(string &symbol, string &since);
  string GetKline(string &symbol, string &type, string &size, string &since);
  string DoUserinfo();
  string DoTrade(string &symbol, string &type, string &price, string &amount);
  string DoTrade_History(string &symbol, string &since);
  string DoBatch_Trade(string &symbol, string &type, string &orders_data);
  string DoCancel_Order(string &symbol, string &order_id);
  string DoOrder_Info(string &symbol, string &order_id);
  string DoOrders_Info(string &type, string &symbol, string &order_id);
  string DoOrder_History(string &symbol, string &status, string &current_page, string &page_length);
  string DoWithdraw(string &symbol, string &chargefee, string &trade_pwd, string &withdraw_address, string &withdraw_amount);
  string DoCancel_Withdraw(string &symbol, string &withdraw_id);
  string DoOrder_Fee(string &symbol, string &order_id);
  string DoLend_Depth(string &symbol);
  string DoBorrows_Info(string &symbol);
  string DoBorrow_Money(string &symbol, string &days, string &amount, string &rate);
  string DoCancel_Borrow(string &symbol, string &borrow_id);
  string DoBorrow_Order_info(string &borrow_id);
  string DoRepayment(string &borrow_id);
  string DoUnrepayments_Info(string &symbol, string &current_page, string &page_length);
  string DoAccount_Records(string &symbol, string &type, string &current_page, string &page_length);

  // Futures Market
  string DoFuture_Depth(string &symbol, string &contract_type, string &size, string &merge);
  string DoFuture_Trades(string &symbol, string &contract_type);
  string DoFuture_Index(string &symbol);
  string DoExchange_Rate();
  string DoFuture_Estimated_Price(string &symbol);
  string DoFuture_Kline(string &symbol, string &type, string &contract_type, string &size, string &since);
  string DoFuture_Hold_amount(string &symbol, string &contract_type);
  string DoFuture_Userinfo();
  string DoFuture_Position(string &symbol, string &contract_type);
  string DoFuture_Trade(string &symbol, string &contract_type, string &price, string &amount, string &type, bool match_price, string &lever_rate);
  string DoFuture_Trades_history(string &symbol, string &date, string &since);
  string DoFuture_Batch_trade(string &symbol, string &contract_type, string &orders_data, string &lever_rate);
  string DoFuture_Cancel(string &symbol, string &order_id, string &contract_type);
  string DoFuture_Order_info(string &symbol, string &contract_type, string &status, string &order_id, string &current_page, string &page_length);
  string DoFuture_Orders_info(string &symbol, string &contract_type, string &order_id);
  string DoFuture_Position_4fix(string &symbol, string &contract_type, string &type);
  string DoFuture_Explosive(string &symbol, string &contract_type, string &status, string &current_page, string &page_length);
};

#endif /* __OKCOINAPI_H__ */
