#pragma once
#include <iostream>
#include <queue>
#include <utility>
#include<vector>
#include <map>
#include "ControlEnums.hpp"
#include <algorithm>
namespace MultiLevelOrderBook {

  template <typename OB_PRICE_TYPE, typename OB_TIMESTAMP_TYPE, typename OB_ID_TYPE, typename OB_UID_TYPE, typename OB_QUANTITY_TYPE>
  struct Order {
    OB_PRICE_TYPE price;
    OB_TIMESTAMP_TYPE timestamp;
    OB_ID_TYPE ID;
    OB_UID_TYPE UID;
    OrderType type;
    OrderStatus status;
    OB_QUANTITY_TYPE quantity;
  };
  struct ModificationOrder {
    ModificationType type;
    long double value;
  };



  template <typename OB_PRICE_TYPE, typename OB_TIMESTAMP_TYPE, typename OB_ID_TYPE, typename OB_UID_TYPE, typename OB_QUANTITY_TYPE>
  struct SORT_BID {
    bool operator()(const Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>& order1, const Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>& order2) {
      return order1.price == order2.price ? order1.timestamp > order2.timestamp : order1.price < order2.price;
    };
  };
  template <typename OB_PRICE_TYPE, typename OB_TIMESTAMP_TYPE, typename OB_ID_TYPE, typename OB_UID_TYPE, typename OB_QUANTITY_TYPE>
  struct SORT_ASK {
    bool operator()(const Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>& order1, const Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>& order2) {
      return order1.price == order2.price ? order1.timestamp > order2.timestamp : order1.price > order2.price;
    };
  };

  template <typename OB_PRICE_TYPE, typename OB_TIMESTAMP_TYPE, typename OB_ID_TYPE, typename OB_UID_TYPE, typename OB_QUANTITY_TYPE>
  class OrderBook {
  private:
    std::priority_queue<Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>, std::vector<Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>>, SORT_BID<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>> bidHeap;
    std::priority_queue<Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>, std::vector<Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>>, SORT_ASK<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>> askHeap;
    std::map<OB_ID_TYPE, ModificationOrder> modifyMp;
    std::map<OB_ID_TYPE, bool> currentOrders;
    OB_QUANTITY_TYPE buySide, sellSide;
    Level level;
    OB_ID_TYPE current_id = 0;
    void modifyAllPossibleOrders(LogType log) {
      while (!this->bidHeap.empty()) {
        Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> order = this->bidHeap.top();
        if (this->modifyMp.find(order.ID) != this->modifyMp.end()) {
          this->bidHeap.pop();
          ModificationOrder mOrder = this->modifyMp[order.ID];
          this->modifyMp.erase(order.ID);
          if (mOrder.type == ModificationType::KILL) {
            this->buySide -= order.quantity;
            if (log == LogType::OUT) {
              if (level != Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " FOR " << order.quantity << " UNITS KILLED" << std::endl;
              }
              else {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " WITH ID " << order.ID << " FOR " << order.quantity << " UNITS KILLED" << std::endl;
              }
            }
          }
          else if (mOrder.type == ModificationType::QUANTITY) {
            this->buySide -= order.quantity;
            this->buySide += mOrder.value;
            if (log == LogType::OUT) {

              if (level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " WITH ID " << order.ID << " FOR " << order.quantity << " UNITS MODIFIED TO " << mOrder.value << " UNITS" << std::endl;
              }

            }
            order.quantity = mOrder.value;
            order.status = OrderStatus::OPEN;
            this->bidHeap.push(order);
          }
          else {

            if (log == LogType::OUT) {
              if (level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " WITH ID " << order.ID << " FOR " << order.quantity << " UNITS MODIFIED TO EXECUTE @ " << mOrder.value << std::endl;
              }
            }
            order.price = mOrder.value;
            this->bidHeap.push(order);
          }
        }
        else {
          break;
        }
      }
      //ask heap
      while (!this->askHeap.empty()) {
        Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> order = this->askHeap.top();
        if (this->modifyMp.find(order.ID) != this->modifyMp.end()) {
          this->askHeap.pop();
          ModificationOrder mOrder = this->modifyMp[order.ID];
          this->modifyMp.erase(order.ID);
          if (mOrder.type == ModificationType::KILL) {
            this->sellSide -= order.quantity;
            if (log == LogType::OUT) {
              if (level != Level::Level3) {

                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " FOR " << order.quantity << " UNITS KILLED" << std::endl;
              }
              else {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " WITH ID " << order.ID << " FOR " << order.quantity << " UNITS KILLED" << std::endl;
              }
            }
          }
          else if (mOrder.type == ModificationType::QUANTITY) {

            this->sellSide -= order.quantity;
            this->sellSide += mOrder.value;
            if (log == LogType::OUT) {
              if (level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " WITH ID " << order.ID << " FOR " << order.quantity << " UNITS MODIFIED TO " << mOrder.value << " UNITS" << std::endl;
              }

            }
            order.quantity = mOrder.value;
            order.status = OrderStatus::OPEN;
            this->askHeap.push(order);
          }
          else {


            if (log == LogType::OUT) {
              if (level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(order.status)] << " " << OrderTypeLog[static_cast<int>(order.type)] << " ORDER @ " << order.price << " WITH ID " << order.ID << " FOR " << order.quantity << " UNITS MODIFIED TO EXECUTE @ " << mOrder.value << std::endl;
              }
            }
            order.price = mOrder.value;
            this->askHeap.push(order);
          }
        }
        else {
          break;
        }
      }
    }
    void FillAllPossibleOrders(LogType log) {
      this->modifyAllPossibleOrders(log);
      while (!this->bidHeap.empty() && !this->askHeap.empty()) {
        Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> bestBidOrder = this->bidHeap.top();
        Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> bestAskOrder = this->askHeap.top();
        if (bestBidOrder.price >= bestAskOrder.price) {

          OB_QUANTITY_TYPE diff_bid_ask = bestBidOrder.quantity - bestAskOrder.quantity;
          this->bidHeap.pop();
          this->askHeap.pop();
          OB_QUANTITY_TYPE quantity_bid = bestBidOrder.quantity;
          if (diff_bid_ask <= 0) { // Bid Order filled
            this->buySide -= quantity_bid;
            if (log == LogType::OUT) {
              if (this->level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestAskOrder.price << " WITH ID " << bestBidOrder.ID << " FOR " << bestBidOrder.quantity << " UNITS FULLY FILLED " << std::endl;
              }
              else {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestAskOrder.price << " FOR " << bestBidOrder.quantity << " UNITS FULLY FILLED " << std::endl;
              }
            }


          }
          else {
            this->buySide -= bestAskOrder.quantity;
            bestBidOrder.quantity -= bestAskOrder.quantity;
            bestBidOrder.status = OrderStatus::PARTIAL;
            if (log == LogType::OUT) {
              if (this->level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestAskOrder.price << " WITH ID " << bestBidOrder.ID << " FOR " << quantity_bid << " UNITS PARTIALLY FILLED FOR " << bestAskOrder.quantity << " UNITS" << std::endl;
              }
              else {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::BUY)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestAskOrder.price << " FOR " << quantity_bid << " PARTIALLY FILLED FOR " << bestAskOrder.quantity << " UNITS" << std::endl;
              }
            }
            this->bidHeap.push(bestBidOrder);
          }

          if (diff_bid_ask >= 0) { // Ask Order filled
            this->sellSide -= bestAskOrder.quantity;
            if (log == LogType::OUT) {
              if (this->level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " WITH ID " << bestAskOrder.ID << " FOR " << bestAskOrder.quantity << " UNITS FULLY FILLED " << std::endl;
              }
              else {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " FOR " << bestAskOrder.quantity << " UNITS FULLY FILLED " << std::endl;
              }
            }
          }
          else {
            bestAskOrder.status = OrderStatus::PARTIAL;
            this->sellSide -= quantity_bid;
            if (log == LogType::OUT) {
              if (this->level == Level::Level3) {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " WITH ID " << bestAskOrder.ID << " FOR " << bestAskOrder.quantity << " UNITS PARTIALLY FILLED FOR " << bestBidOrder.quantity << " UNITS" << std::endl;
              }
              else {
                std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " FOR " << bestAskOrder.quantity << " UNITS PARTIALLY FILLED FOR " << bestBidOrder.quantity << " UNITS" << std::endl;
              }
            }
            bestAskOrder.quantity -= quantity_bid;
            this->askHeap.push(bestAskOrder);
          }
        }
        else {
          break;
        }
        this->modifyAllPossibleOrders(log);
      }
    };
  public:
    OrderBook(Level level) {
      this->level = level;
    };
    OB_QUANTITY_TYPE OBI() {
      OB_QUANTITY_TYPE numerator = (this->buySide - this->sellSide);
      OB_QUANTITY_TYPE denominator = (this->buySide + this->sellSide);

      if (denominator == 0) return 0;
      return numerator / denominator;
    };
    // Adding Orders
    void LimitOrder(OrderDirection direction, OB_QUANTITY_TYPE qty, OB_UID_TYPE uid, OB_PRICE_TYPE price_level, OB_TIMESTAMP_TYPE timestamp, LogType log) {

      if (qty <= 0) {
        std::cerr << "QUANTITY OF LIMIT ORDER MUST BE GREATER THAN ZERO" << std::endl; return;
      }
      if (price_level <= 0) {
        std::cerr << "PRICE OF LIMIT ORDER MUST BE GREATER THAN ZERO" << std::endl; return;
      }
      Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> order{ price_level, timestamp, this->current_id++, uid, OrderType::LIMIT, OrderStatus::OPEN, qty };
      this->currentOrders[this->current_id - 1] = true;
      if (log == LogType::OUT) {
        if (this->level == Level::Level3){
          std::cout << OrderDirectionLog[static_cast<int>(direction)] << " LIMIT ORDER PLACED @ " << price_level << " WITH ID OF " << this->current_id-1 << std::endl;
        } else {
          std::cout << OrderDirectionLog[static_cast<int>(direction)] << " LIMIT ORDER PLACED @ " << price_level << std::endl;
        }
      }
      if (direction == OrderDirection::BUY) {
     
        this->buySide += qty;
        this->bidHeap.push(order);
      }
      else {
    
        this->sellSide += qty;
        this->askHeap.push(order);
      };
      this->FillAllPossibleOrders(log);

    };
    void MarketOrder(OrderDirection direction, OB_QUANTITY_TYPE qty, LogType log) {
      if (qty <= 0) {
        std::cerr << "QUANTITY OF MARKET ORDER MUST BE GREATER THAN ZERO" << std::endl; return;
      }
      
      OB_PRICE_TYPE price_t = 0.0;
      OB_QUANTITY_TYPE real_qty = qty;

      if (direction == OrderDirection::BUY) {
        if (this->askHeap.empty()) {
          std::cerr << "MARKET ORDER FAILED : NO ORDERS CURRENTLY" << std::endl; return;
        };
   
        if (qty > this->sellSide) {
          if (log == LogType::OUT) std::cout << "BUY MARKET ORDER FAILED : TOO LITTLE LIQUIDITY" << std::endl;
          return;
        }
        else {
          std::cout << "BUY MARKET ORDER PLACED" << std::endl;

          while (!askHeap.empty() && qty > 0) {
            Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> bestAskOrder = this->askHeap.top();
            price_t += (bestAskOrder.price * std::min(bestAskOrder.quantity, qty));
            OB_QUANTITY_TYPE askOrderQuantity = bestAskOrder.quantity;
            this->askHeap.pop();
            OB_QUANTITY_TYPE diff_bid_ask = qty - bestAskOrder.quantity;
            if (diff_bid_ask >= 0) { // Ask Order filled
              this->sellSide -= bestAskOrder.quantity;
              if (log == LogType::OUT) {
                if (this->level == Level::Level3) {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " WITH ID " << bestAskOrder.ID << " FOR " << bestAskOrder.quantity << " UNITS FULLY FILLED " << std::endl;
                }
                else {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " FOR " << bestAskOrder.quantity << " UNITS FULLY FILLED " << std::endl;
                }
              }
            }
            else {
              bestAskOrder.status = OrderStatus::PARTIAL;
              this->sellSide -= qty;
              if (log == LogType::OUT) {
                if (this->level == Level::Level3) {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " WITH ID " << bestAskOrder.ID << " FOR " << bestAskOrder.quantity << " UNITS PARTIALLY FILLED FOR " << qty << " UNITS" << std::endl;
                }
                else {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestAskOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestAskOrder.type)] << " ORDER @ " << bestAskOrder.price << " FOR " << bestAskOrder.quantity << " UNITS PARTIALLY FILLED FOR " << qty << " UNITS" << std::endl;
                }
              }
              bestAskOrder.quantity -= qty;
              this->askHeap.push(bestAskOrder);
            }
            qty -= askOrderQuantity;

          }
        }
      }
      else {
        if (this->bidHeap.empty()) {
          std::cerr << "MARKET ORDER FAILED : NO ORDERS CURRENTLY" << std::endl; return;
        };
        if (qty > this->buySide) {
          if (log == LogType::OUT) std::cout << "SELL MARKET ORDER FAILED : TOO LITTLE LIQUIDITY" << std::endl;
          return;
        }
        else {
          std::cout << "SELL MARKET ORDER PLACED" << std::endl;
          while (!bidHeap.empty() && qty > 0) {
            Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> bestBidOrder = this->bidHeap.top();
            price_t += (bestBidOrder.price * std::min(bestBidOrder.quantity, qty));
            OB_QUANTITY_TYPE bidOrderQuantity = bestBidOrder.quantity;
            this->bidHeap.pop();
            OB_QUANTITY_TYPE diff_bid_ask = qty - bestBidOrder.quantity;
            if (diff_bid_ask >= 0) { // Ask Order filled
              this->buySide -= bestBidOrder.quantity;
              if (log == LogType::OUT) {
                if (this->level == Level::Level3) {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestBidOrder.price << " WITH ID " << bestBidOrder.ID << " FOR " << bestBidOrder.quantity << " UNITS FULLY FILLED " << std::endl;
                }
                else {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestBidOrder.price << " FOR " << bestBidOrder.quantity << " UNITS FULLY FILLED " << std::endl;
                }
              }
            }
            else {
              bestBidOrder.status = OrderStatus::PARTIAL;
              this->buySide -= qty;
              if (log == LogType::OUT) {
                if (this->level == Level::Level3) {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestBidOrder.price << " WITH ID " << bestBidOrder.ID << " FOR " << bestBidOrder.quantity << " UNITS PARTIALLY FILLED FOR " << qty << " UNITS" << std::endl;
                }
                else {
                  std::cout << OrderDirectionLog[static_cast<int>(OrderDirection::SELL)] << " " << OrderStatusLog[static_cast<int>(bestBidOrder.status)] << " " << OrderTypeLog[static_cast<int>(bestBidOrder.type)] << " ORDER @ " << bestBidOrder.price << " FOR " << bestBidOrder.quantity << " UNITS PARTIALLY FILLED FOR " << qty << " UNITS" << std::endl;
                }
              }
              bestBidOrder.quantity -= qty;
              this->bidHeap.push(bestBidOrder);
            }
            qty -= bidOrderQuantity;

          }
        }
      }
      if (log == LogType::OUT) {
        if (this->level == Level::Level3) {
          std::cout << OrderDirectionLog[static_cast<int>(direction)] << " MARKET ORDER FILLED " << " @ AVG PRICE OF " << (price_t / ((OB_PRICE_TYPE)real_qty)) << " WITH ID " << this->current_id << " FOR " << real_qty << " UNITS FULLY FILLED " << std::endl;
        }
        else {
          std::cout << OrderDirectionLog[static_cast<int>(direction)] << " MARKET ORDER FILLED " << " @ AVG PRICE OF " << (price_t / ((OB_PRICE_TYPE)real_qty)) << " FOR " << real_qty << " UNITS FULLY FILLED " << std::endl;
        }
      }
      current_id++;
    };
    void DisplayBid(long long data_range) {
      std::cout << "\n| ALL BID OFFERS |" << std::endl;
      if (this->bidHeap.empty()) { std::cout << "NO BID ORDERS CURRENTLY\n" << std::endl; return; }
      if (this->level == Level::Level1) {
        std::cout << "CURRENT BEST BID: " << static_cast<int>(this->bidHeap.top().status) << " LIMIT ORDER @ " << this->bidHeap.top().price << " FOR " << this->bidHeap.top().quantity << " UNITS" << std::endl;
        return;
      }
      std::vector<Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>> savedOrders;
      while (!this->bidHeap.empty()&&data_range--) {
        if (this->level == Level::Level2) {
          std::cout << "-> | BUY " << OrderStatusLog[static_cast<int>(this->bidHeap.top().status)] << " LIMIT ORDER @ " << this->bidHeap.top().price << " FOR " << this->bidHeap.top().quantity << " UNITS" << std::endl;
        }
        else {
          std::cout << "-> | BUY " << OrderStatusLog[static_cast<int>(this->bidHeap.top().status)] << " LIMIT ORDER WITH ID OF " << this->bidHeap.top().ID << " @ " << this->bidHeap.top().price << " FOR " << this->bidHeap.top().quantity << " UNITS AT THE TIMESTAMP: " << this->bidHeap.top().timestamp << std::endl;
        }
        savedOrders.push_back(this->bidHeap.top());
        this->bidHeap.pop();
      }
      for (Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> order : savedOrders) {
        this->bidHeap.push(order);
      }

      std::cout << "\n" << std::endl;
    }
    void DisplayAsk(long long data_range) {
      std::cout << "\n| ALL ASK OFFERS |" << std::endl;
      if (this->askHeap.empty()) { std::cout << "NO ASK ORDERS CURRENTLY\n" << std::endl; return; }
      if (this->level == Level::Level1) {
        std::cout << "CURRENT BEST ASK: " << static_cast<int>(this->askHeap.top().status) << " LIMIT ORDER @ " << this->askHeap.top().price << " FOR " << this->askHeap.top().quantity << " UNITS" << std::endl;
        return;
      }
      std::vector<Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE>> savedOrders;
      while (!this->askHeap.empty() && data_range--) {
        if (this->level == Level::Level2) {
          std::cout << "-> | SELL " << OrderStatusLog[static_cast<int>(this->askHeap.top().status)] << " LIMIT ORDER @ " << this->askHeap.top().price << " FOR " << this->askHeap.top().quantity << " UNITS" << std::endl;
        }
        else {
          std::cout << "-> | SELL " << OrderStatusLog[static_cast<int>(this->askHeap.top().status)] << " LIMIT ORDER WITH ID OF " << this->askHeap.top().ID << " @ " << this->askHeap.top().price << " FOR " << this->askHeap.top().quantity << " UNITS AT THE TIMESTAMP: " << this->askHeap.top().timestamp << std::endl;
        }
        savedOrders.push_back(this->askHeap.top());
        this->askHeap.pop();
      }
      for (Order<OB_PRICE_TYPE, OB_TIMESTAMP_TYPE, OB_ID_TYPE, OB_UID_TYPE, OB_QUANTITY_TYPE> order : savedOrders) {
        this->askHeap.push(order);
      }

      std::cout << "\n" << std::endl;
    }
    void DisplayBook(long long data_range) {
      this->DisplayBid(data_range);
      this->DisplayAsk(data_range);
    };
    // Modification orders
    void ModifyOrder(OB_ID_TYPE orderId, ModificationType type, OB_PRICE_TYPE qty, LogType log) {
      if (!this->currentOrders[orderId]) {
        std::cerr << "MODIFICATION ORDER FAILED: ORDER WITH ID OF " << orderId << " DOESN'T EXIST" << std::endl; return;
      }
      ModificationOrder mOrder{ type, qty };
      this->modifyMp[orderId] = mOrder;
      FillAllPossibleOrders(log);
    };
    void ModifyOrder(OB_ID_TYPE orderId, LogType log) {
      if (!this->currentOrders[orderId]) {
        std::cerr << "MODIFICATION ORDER FAILED: ORDER WITH ID OF " << orderId << " DOESN'T EXIST" << std::endl; return;
      }
      ModificationOrder mOrder{ ModificationType::KILL, 0.0 };
      this->modifyMp[orderId] = mOrder;
      FillAllPossibleOrders(log);
    };

  };

};