# CPPBasicOrderBookEngine

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

## How to Use (From 

1. **Edit Orders.json**

   * Add different orders as required. Refer to PayloadGuide.mb for documentation.

2. **Run main.cpp using g++**

* g++ src/main.cpp -o main.exe
* Run src/main.exe

2. **Build docker image and run container**
* Compile a docker image based on the root project directory.
* Run a container based on the image.



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



## License

MIT License. Use freely with attribution.



