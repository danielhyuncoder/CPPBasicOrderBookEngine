#include <iostream> 
#include <fstream>
#include "../include/OrderBook.hpp" 
#include "../include/json.hpp"
using namespace MultiLevelOrderBook;

using json = nlohmann::json;

int main() {
  std::ifstream inFile("../data/config.json");
  if (!inFile) {
      std::cerr << "Could not open the config file!\n";
      return 1;
  }
  json j;
  inFile >> j;
  inFile.close();
  int level = j["Level"];
  OrderBook<long double, long double, long double, long double, long double> orderbook(level==1?Level::Level1 : (level==2?Level::Level2 : Level::Level3)); // Could define custom datatypes
  std::ifstream orderFile("../data/orders.json");
  if (!orderFile) {
      std::cerr << "Could not open the orders file!\n";
      return 1;
  }
  json oJ;
  orderFile >> oJ;
  orderFile.close();
  for (auto& obj : oJ) {
     std::string function = obj["FUNCTION"];
       if (function == "MARKET") {
         orderbook.MarketOrder(obj["DIRECTION"]=="BUY"?OrderDirection::BUY : OrderDirection::SELL, obj["QTY"], obj["LOG"]=="OUT"?LogType::OUT : LogType::NONE);
       } else if (function == "LIMIT"){
         orderbook.LimitOrder(obj["DIRECTION"]=="BUY"?OrderDirection::BUY : OrderDirection::SELL, obj["QTY"], obj["UID"], obj["PRICE"], obj["TIMESTAMP"], obj["LOG"]=="OUT"?LogType::OUT : LogType::NONE);
       } else if (function == "MODIFY"){
         std::string modType = obj["MODIFICATION"];
        if (modType=="KILL"){
          orderbook.ModifyOrder(obj["ID"], obj["LOG"]=="OUT"?LogType::OUT : LogType::NONE);
        } else {
          orderbook.ModifyOrder(obj["ID"], modType=="QUANTITY"?ModificationType::QUANTITY : ModificationType::PRICE, obj["VALUE"], obj["LOG"]=="OUT"?LogType::OUT : LogType::NONE);
        }
       } else {

              long long orderRange=obj["RANGE"];
               orderbook.DisplayBook(orderRange);
       }

  }
};