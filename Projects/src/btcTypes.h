#include <vector>
#ifndef btcTypes_h__
#define btcTypes_h__

struct Order
{
  Order(float _price, float _volume) : price(_price), volume(_volume) {}
  float price;
  float volume;
};

typedef std::vector<Order> orderList;

#endif // btcTypes_h__
