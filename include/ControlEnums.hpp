#pragma once
#include <iostream>
namespace MultiLevelOrderBook {
  // Enums
  enum class Level : int {
    Level1 = 0, Level2 = 1, Level3 = 2
  };
  enum class OrderDirection : int {
    SELL = 0, BUY = 1
  };
  enum class OrderType : int {
    LIMIT = 0, MARKET = 1
  };
  enum class OrderStatus : int {
    OPEN = 0, PARTIAL = 1
  };
  enum class ModificationType : int {
    QUANTITY = 0, PRICE = 1, KILL = 2
  };
  enum class LogType {
    NONE, OUT
  };
  const std::string LevelLog[3] = { "Level 1", "Level 2", "Level 3" };
  const std::string OrderDirectionLog[2] = { "SELL", "BUY" };
  const std::string OrderTypeLog[2] = { "LIMIT", "MARKET" };
  const std::string OrderStatusLog[2] = { "OPEN", "PARTIALLY FILLED" };
  const std::string ModificationTypeLog[3] = { "QUANTITY", "PRICE", "KILL" };
};