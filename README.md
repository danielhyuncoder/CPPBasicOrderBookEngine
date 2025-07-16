# MultiLevelOrderBook

A high-performance, dependency-free C++ order book engine implementation designed for handling both limit and market orders, order modifications, and level-based order book data display (Level 1, 2, 3). It is completely self-contained except for using the lightweight [json.hpp](https://github.com/nlohmann/json) library for any optional JSON functionality.

---

## Features

* **Templated Order Book** supporting flexible data types
* **Priority Queues** to maintain sorted bid/ask orderbooks
* **Limit and Market Order Execution**
* **Order Modification** (cancel/modify quantity or price)
* **Order Matching Engine**
* **Level 1/2/3 Views** of Bid/Ask sides
* **Order Book Imbalance (OBI) Metric**
* **Verbose Logging for Level 3 Insights**

---

## Project Structure

* `Order`: Generic struct holding order data including price, ID, UID, timestamp, status, etc.
* `ModificationOrder`: Holds instruction to either kill or modify an existing order.
* `SORT_BID` / `SORT_ASK`: Sorting logic for priority queues.
* `OrderBook`: The main class managing order lifecycle (add, match, modify, cancel).
* `ControlEnums.hpp`: Should contain enums like `OrderType`, `OrderStatus`, `ModificationType`, `Level`, `OrderDirection`, `LogType` and associated string mappings.

---

## Functional Overview

### Order Lifecycle:

#### `LimitOrder(...)`

Adds a new order to the bid or ask side depending on direction. Order is logged and potentially matched immediately.

#### `MarketOrder(...)`

Consumes available liquidity from the opposite side of the book.

#### `ModifyOrder(id, type, value)`

Modifies an existing order:

* `KILL`: Cancels it
* `QUANTITY`: Changes quantity
* `PRICE`: Moves to a new price level

#### `DisplayBid(...)`, `DisplayAsk(...)`, `DisplayBook(...)`

Prints current state of bid and/or ask sides according to configured `Level` (1, 2, or 3).

#### `OBI()`

Returns the order book imbalance metric:

```
OBI = (TotalBuyQty - TotalSellQty) / (TotalBuyQty + TotalSellQty)
```

---

## Requirements

* **C++17 or later**
* **No third-party dependencies required**

---

## How to Use

1. **Include the engine in your project:**

   * `#include "OrderBook.hpp"`
   * `#include "ControlEnums.hpp"`

2. **Instantiate the order book:**

```cpp
MultiLevelOrderBook::OrderBook<double, long long, int, int, double> book(MultiLevelOrderBook::Level::Level3);
```

3. **Place Orders:**

```cpp
book.LimitOrder(OrderDirection::BUY, 100.0, 1, 50.5, 10001, LogType::OUT);
book.MarketOrder(OrderDirection::SELL, 50.0, LogType::OUT);
```

4. **Modify Orders:**

```cpp
book.ModifyOrder(orderId, ModificationType::QUANTITY, newQty, LogType::OUT);
book.ModifyOrder(orderId, LogType::OUT); // KILL
```

5. **View Order Book:**

```cpp
book.DisplayBook(10);  // Display top 10 from both sides
```

---

## Logging

Logging verbosity is controlled via the `LogType` enum:

* `LogType::OUT`: Verbose logging to `stdout`
* `LogType::NONE`: Silent mode

Order types and statuses are printed using `OrderTypeLog`, `OrderStatusLog`, and `OrderDirectionLog` string arrays from `ControlEnums.hpp`.

---

## Supported Order Book Levels

* **Level 1:** Best bid/ask only
* **Level 2:** All visible price levels (aggregated)
* **Level 3:** Full depth with order IDs and timestamps

---

## Sample Enum Definitions (ControlEnums.hpp)

```cpp
enum class OrderType { LIMIT, MARKET };
enum class OrderStatus { OPEN, PARTIAL, FILLED };
enum class OrderDirection { BUY, SELL };
enum class ModificationType { KILL, QUANTITY, PRICE };
enum class Level { Level1, Level2, Level3 };
enum class LogType { NONE, OUT };

const std::string OrderTypeLog[] = {"LIMIT", "MARKET"};
const std::string OrderStatusLog[] = {"OPEN", "PARTIAL", "FILLED"};
const std::string OrderDirectionLog[] = {"BUY", "SELL"};
```

---

## License

MIT License. Use freely with attribution.



