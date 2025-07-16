# 🧾 Orderbook Function Types & JSON Payload Format

This system processes trading instructions in the form of a JSON array. Each object must define a `FUNCTION` field to describe what type of operation to perform.

---

## 📌 Supported FUNCTION Types

- `MARKET`
- `LIMIT`
- `MODIFY`  
  - `KILL`  
  - `QUANTITY`  
  - `PRICE`
- `DISPLAY` (fallback if `FUNCTION` is not recognized)

---

## 1️⃣ MARKET ORDER

Executes a market order at the best available price.

### Required Fields

| Field     | Type     | Description                     |
|-----------|----------|---------------------------------|
| FUNCTION  | string   | Must be `"MARKET"`              |
| DIRECTION | string   | `"BUY"` or `"SELL"`             |
| QTY       | integer  | Quantity to buy or sell         |
| LOG       | string   | `"OUT"` or `"NONE"`             |

---

## 2️⃣ LIMIT ORDER

Places a limit order into the order book at a given price.

### Required Fields

| Field      | Type     | Description                           |
|------------|----------|---------------------------------------|
| FUNCTION   | string   | Must be `"LIMIT"`                     |
| DIRECTION  | string   | `"BUY"` or `"SELL"`                   |
| QTY        | integer  | Quantity to buy or sell               |
| UID        | integer  | Unique numeric identifier of order    |
| PRICE      | float    | Limit price                           |
| TIMESTAMP  | integer  | Event timestamp (e.g., Unix time)     |
| LOG        | string   | `"OUT"` or `"NONE"`                   |

---

## 3️⃣ MODIFY ORDER

Modifies or cancels an existing order based on `MODIFICATION` type.

---

### 🔹 A. KILL

Cancels an order by ID.

#### Required Fields

| Field        | Type     | Description                         |
|--------------|----------|-------------------------------------|
| FUNCTION     | string   | Must be `"MODIFY"`                  |
| MODIFICATION | string   | Must be `"KILL"`                    |
| ID           | integer  | Order ID to cancel                  |
| LOG          | string   | `"OUT"` or `"NONE"`                 |

---

### 🔹 B. QUANTITY / PRICE

Modifies an order’s quantity or price.

#### Required Fields

| Field        | Type     | Description                         |
|--------------|----------|-------------------------------------|
| FUNCTION     | string   | Must be `"MODIFY"`                  |
| MODIFICATION | string   | `"QUANTITY"` or `"PRICE"`           |
| ID           | integer  | Order ID to modify                  |
| VALUE        | number   | New quantity or price               |
| LOG          | string   | `"OUT"` or `"NONE"`                 |

---

## 4️⃣ DISPLAY ORDER BOOK

Displays the order book to the console with a specified depth.

### Required Fields

| Field   | Type     | Description                         |
|---------|----------|-------------------------------------|
| RANGE   | integer  | Number of order from both the bid/ask to display         |

---

## 📝 Notes

- All fields are case-sensitive.
- `UID` and `ID` must be integers.
- `LOG` is required for all functions.
- If `FUNCTION` is not `"MARKET"`, `"LIMIT"`, or `"MODIFY"`, the object is treated as a `DISPLAY` command.




