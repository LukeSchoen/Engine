#include "string.h"
#include "okcoinapi.h"
#include <sstream>
#include <vector>
#include <utility>
#include <string>

string OKCoinApi::GetTicker(string &symbol)					//Get /api/v1/ticker	获取OKCoin行情
{
  Uri uri;
  urlprotocol.GetUrl(uri, HTTP_API_TYPE_TICKER);
  uri.AddParam("symbol", symbol);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::GetDepth(string &symbol, string &size, string &merge)				//Get /api/v1/depth		获取OKCoin市场深度
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_DEPTH);
  uri.AddParam("symbol", symbol);
  uri.AddParam("size", size);
  uri.AddParam("merge", merge);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::GetTrades(string &symbol, string &since)			//Get /api/v1/trades	获取OKCoin最近600交易信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_TRADES);
  uri.AddParam("symbol", symbol);
  uri.AddParam("since", since);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::GetKline(string &symbol, string &type, string &size, string &since)			//Get /api/v1/kline		获取比特币或莱特币的K线数据
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_KLINE);
  uri.AddParam("symbol", symbol);
  uri.AddParam("type", type);
  uri.AddParam("size", size);
  uri.AddParam("since", since);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoUserinfo()
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_USERINFO);
  uri.AddParam("api_key", m_api_key);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoTrade(string &symbol, string &type, string &price, string &amount)			//POST /api/v1/trade	下单交易
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_TRADE);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("type", type);
  uri.AddParam("price", price);
  uri.AddParam("amount", amount);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoTrade_History(string &symbol, string &since)			//POST /api/v1/trade_history				获取OKCoin历史交易信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_TRADE_HISTORY);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("since", since);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoBatch_Trade(string &symbol, string &type, string &orders_data)	//POST /api/v1/batch_trade批量下单
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_BATCH_TRADE);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("type", type);
  uri.AddParam("orders_data", orders_data);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoCancel_Order(string &symbol, string &order_id)			//POST /api/v1/cancel_order撤销订单
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_CANCEL_ORDER);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("order_id", order_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoOrder_Info(string &symbol, string &order_id)			//POST /api/v1/order_info获取用户的订单信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_ORDER_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("order_id", order_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoOrders_Info(string &type, string &symbol, string &order_id)		//POST /api/v1/orders_info批量获取用户订单
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_ORDERS_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("type", type);
  uri.AddParam("symbol", symbol);
  uri.AddParam("order_id", order_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoOrder_History(string &symbol, string &status, string &current_page, string &page_length)				//POST /api/v1/order_history				获取历史订单信息，只返回最近七天的信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_ORDER_HISTORY);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("status", status);
  uri.AddParam("current_page", current_page);
  uri.AddParam("page_length", page_length);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoWithdraw(string &symbol, string &chargefee, string &trade_pwd, string &withdraw_address, string &withdraw_amount)			//POST /api/v1/withdraw	提币BTC/LTC
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_WITHDRAW);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("chargefee", chargefee);
  uri.AddParam("trade_pwd", trade_pwd);
  uri.AddParam("withdraw_address", withdraw_address);
  uri.AddParam("withdraw_amount", withdraw_amount);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoCancel_Withdraw(string &symbol, string &withdraw_id)			//POST /api/v1/cancel_withdraw				取消提币BTC/LTC
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_CANCEL_WITHDRAW);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("withdraw_id", withdraw_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoOrder_Fee(string &symbol, string &order_id)			//POST /api/v1/order_fee查询手续费
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_ORDER_FEE);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("order_id", order_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoLend_Depth(string &symbol)				//POST /api/v1/lend_depth获取放款深度前10
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_LEND_DEPTH);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoBorrows_Info(string &symbol)				//POST /api/v1/borrows_info查询用户借款信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_BORROWS_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoBorrow_Money(string &symbol, string &days, string &amount, string &rate)			//POST /api/v1/borrow_money申请借款
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_BORROW_MONEY);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("days", days);
  uri.AddParam("amount", amount);
  uri.AddParam("rate", rate);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoCancel_Borrow(string &symbol, string &borrow_id)				//POST /api/v1/cancel_borrow				取消借款申请
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_CANCEL_BORROW);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("borrow_id", borrow_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoBorrow_Order_info(string &borrow_id)		//POST /api/v1/borrow_order_info			获取借款订单记录
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_BORROW_ORDER_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("borrow_id", borrow_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoRepayment(string &borrow_id)				//POST /api/v1/repayment用户还全款
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_REPAYMENT);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("borrow_id", borrow_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoUnrepayments_Info(string &symbol, string &current_page, string &page_length)	//POST /api/v1/unrepayments_info			未还款列表
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_UNREPAYMENTS_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("current_page", current_page);
  uri.AddParam("page_length", page_length);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoAccount_Records(string &symbol, string &type, string &current_page, string &page_length)				//POST /api/v1/account_records				获取用户提现/充值记录
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_ACCOUNT_RECORDS);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("type", type);
  uri.AddParam("current_page", current_page);
  uri.AddParam("page_length", page_length);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}



int64_t OKCoinApi::FuturePrice(string &symbol, string &contract_type, int64_t *CurrentPriceTime /*= nullptr*/)			//GET /api/v1/future_ticker				获取OKCoin期货行情
{
  while (true)
  {
    Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_TICKER);
    uri.AddParam("symbol", symbol);
    uri.AddParam("contract_type", contract_type);
    uri.Requset();
    std::string ret = uri.result;

    int lastPtr = ret.find("last");
    if (lastPtr == -1)
    {
      Sleep(100);
      printf("bad...\n");
      continue;
    }
    printf("good...\n");

    if (CurrentPriceTime)
    {
      lastPtr = ret.find("date ");  
      ret = ret.substr(lastPtr + 7);
      ret = ret.substr(0, ret.find("\""));
      *CurrentPriceTime = atoll(ret.c_str());
    }

    ret = ret.substr(lastPtr + 6);
    ret = ret.substr(0, ret.find(","));
    int dotPtr = ret.find(".");
    std::string dollars = ret.substr(0, dotPtr);
    std::string cents = ret.substr(dotPtr + 1);
    if (cents.length() == 1) cents += "0";
    ret = dollars + cents;
    return stoi(ret);
  }
}
string OKCoinApi::DoFuture_Depth(string &symbol, string &contract_type, string &size, string &merge)	//GET /api/v1/future_depth				获取OKCoin期货深度信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_DEPTH);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("size", size);
  uri.AddParam("merge", merge);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Trades(string &symbol, string &contract_type)			//GET /api/v1/future_trades				获取OKCoin期货交易记录信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_TRADES);

  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("api_key", symbol);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("sign", sign);
  //   uri.AddParam("price", price);
  //   uri.AddParam("amount", amount);
  uri.AddParam("type", "1"); // 1: open long position    2 : open short position    3 : liquidate long position    4 : liquidate short position
  uri.AddParam("match_price", "1"); // 0: open order at price field 1: Ignore price field and just open long now
  uri.AddParam("lever_rate", "10"); // 10 or 20
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Index(string &symbol)				//GET /api/v1/future_index				获取OKCoin期货指数信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_INDEX);
  uri.AddParam("symbol", symbol);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoExchange_Rate()		//GET /api/v1/exchange_rate				获取美元人民币汇率
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_EXCHANGE_RATE);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Estimated_Price(string &symbol)	//GET /api/v1/future_estimated_price	获取交割预估价
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_ESTIMATED_PRICE);
  uri.AddParam("symbol", symbol);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Kline(string &symbol, string &type, string &contract_type, string &size, string &since)	//GET /api/v1/future_kline				获取期货合约的K线数据
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_KLINE);
  uri.AddParam("symbol", symbol);
  uri.AddParam("type", type);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("size", size);
  uri.AddParam("since", since);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Hold_amount(string &symbol, string &contract_type)		//GET /api/v1/future_hold_amount		获取当前可用合约总持仓量
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_HOLD_AMOUNT);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Userinfo()			//POST /api/v1/future_userinfo			获取OKCoin期货账户信息 （全仓）
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_USERINFO);
  uri.AddParam("api_key", m_api_key);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Position(string &symbol, string &contract_type)			//POST /api/v1/future_position			获取用户持仓获取OKCoin期货账户信息 （全仓）
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_POSITION);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Trade(string &symbol, string &contract_type, string &price, string &amount, string &type, bool match_price, string &lever_rate)
{
  std::vector<std::pair<char*, std::string>> params;
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_TRADE);
  uri.AddParam("api_key", m_api_key);
  params.push_back({ "api_key", m_api_key });

  uri.AddParam("symbol", symbol);
  params.push_back({ "symbol", symbol });

  uri.AddParam("contract_type", contract_type);
  params.push_back({ "contract_type", contract_type });

  uri.AddParam("price", price);
  params.push_back({ "price", price });

  uri.AddParam("amount", amount);
  params.push_back({ "amount", amount });

  uri.AddParam("type",  type);
  params.push_back({ "type", type });

  uri.AddParam("match_price", match_price ? "1" : "0");
  params.push_back({ "match_price", match_price ? "1" : "0" });

  uri.AddParam("lever_rate", lever_rate);
  params.push_back({ "lever_rate", lever_rate });

  string sign = uri.GetSign(m_secret_key);

  uri.AddParam("secret_key", m_secret_key);
  params.push_back({ "secret_key", m_secret_key });

  uri.AddParam("sign", sign);
  params.push_back({ "sign", sign });

  auto s = uri.GetParamSet();

  uri.Requset();

  return uri.result;
}
string OKCoinApi::DoFuture_Trades_history(string &symbol, string &date, string &since)			//POST /api/v1/future_trades_history	获取OKCoin期货交易历史
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_TRADES_HISTORY);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("date", date);
  uri.AddParam("since", since);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Batch_trade(string &symbol, string &contract_type, string &orders_data, string &lever_rate)	//POST /api/v1/future_batch_trade		批量下单
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_BATCH_TRADE);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("orders_data", orders_data);
  uri.AddParam("lever_rate", lever_rate);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Cancel(string &symbol, string &order_id, string &contract_type)				//POST /api/v1/future_cancel			取消期货订单
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_CANCEL);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("order_id", order_id);
  uri.AddParam("contract_type", contract_type);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Order_info(string &symbol, string &contract_type, string &status, string &order_id, string &current_page, string &page_length)	//POST /api/v1/future_order_info		获取期货订单信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_ORDER_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("status", status);
  uri.AddParam("order_id", order_id);
  uri.AddParam("current_page", current_page);
  uri.AddParam("page_length", page_length);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Orders_info(string &symbol, string &contract_type, string &order_id)			//POST /api/v1/future_orders_info		批量获取期货订单信息
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_ORDERS_INFO);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("order_id", order_id);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}

string OKCoinApi::DoFuture_Position_4fix(string &symbol, string &contract_type, string &type)			//POST /api/v1/future_position_4fix		逐仓用户持仓查询
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_POSITION_4FIX);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("type", type);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
string OKCoinApi::DoFuture_Explosive(string &symbol, string &contract_type, string &status, string &current_page, string &page_length)		//POST /api/v1/future_explosive			获取期货爆仓单
{
  Uri uri; urlprotocol.GetUrl(uri, HTTP_API_TYPE_FUTURE_EXPLOSIVE);
  uri.AddParam("api_key", m_api_key);
  uri.AddParam("symbol", symbol);
  uri.AddParam("contract_type", contract_type);
  uri.AddParam("status", status);
  uri.AddParam("current_page", current_page);
  uri.AddParam("page_length", page_length);
  string sign = uri.GetSign(m_secret_key);
  uri.AddParam("secret_key", m_secret_key);
  uri.AddParam("sign", sign);
  uri.Requset();
  return uri.result;
}
