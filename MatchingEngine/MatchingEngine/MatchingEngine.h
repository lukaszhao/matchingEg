#pragma once
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <unordered_map>
#include <functional>

#include <memory>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace matchingengine {

enum class OrderType { IOC, GFD };

enum class OrderSide { BUY, SELL };

using Price = int;
using Quantity = int;
using OrderId = std::string;

class Order {
public:
    OrderType m_orderType;
    OrderSide m_orderSide;
    Price     m_price;
    Quantity  m_quantity;
    OrderId   m_orderId;

    Order(OrderType orderType,
        OrderSide orderSide,
        Price price,
        Quantity quantity,
        const OrderId& orderId) :
        m_orderType(orderType),
        m_orderSide(orderSide),
        m_price(price),
        m_quantity(quantity),
        m_orderId(orderId) {}

    /// <summary>
    /// returns a string that prints out OrderType
    /// </summary>
    static std::string OrderTypeToString(OrderType orderType);

    /// <summary>
    /// returns a string that prints out OrderSide
    /// </summary>
    static std::string OrderSideToString(OrderSide orderSide);

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};




class MatchingEngine {
private:
    using OrdersList = std::list<std::shared_ptr<Order> >;
    using PriceMap = std::map<Price, OrdersList, std::greater<Price> >;

    std::unordered_map<OrderId, std::shared_ptr<Order> > m_orderIdToOrder;

    PriceMap m_priceMapBuy;
    PriceMap m_priceMapSell;

    /// <summary>
    /// Insert order into a price map, no validation
    /// </summary>
    void insertIntoPriceMap(PriceMap& priceMap, std::shared_ptr<Order> order);

    /// <summary>
    /// print price and quanty summary of a price map
    /// </summary>
    void printPriceQuantitySummary(const PriceMap& priceMap, std::ostream& os) const;

    /// <summary>
    /// trade new order (sell) against all queued buy orders in m_priceMapBuy, this function
    /// updated new order and m_priceMapBuy, but doesn't insert new order into matching engine
    /// </summary>
    void tradeSellOrder(std::shared_ptr<Order> newOrder);

    /// <summary>
    /// trade new order (buy) against all queued sell orders in m_priceMapSell, this function
    /// updated new order and m_priceMapSell, but doesn't insert new order into matching engine
    /// </summary>
    void tradeBuyOrder(std::shared_ptr<Order> newOrder);

    /// <summary>
    /// erase an order from a price map
    /// </summary>
    void eraseOrderFromPriceMap(PriceMap& priceMap, Price price, const OrderId& orderId);


public:

    /// <summary>
    /// execute message PRINT
    /// </summary>
    void print() const;

    /// <summary>
    /// inspect engine data, for testing purpose
    /// </summary>
    void inspect() const;

    /// <summary>
    /// function returns true if price, quantity, and orderId are valid, o.w. false
    /// </summary>
    bool isValidOrder(Price price, Quantity quantity, const OrderId& orderId) const;

    /// <summary>
    /// insert order, this function doesn't validate order
    /// </summary>
    void insertOrder(std::shared_ptr<Order> newOrder);

    /// <summary>
    /// function returns true if buy and sell orders are price cross, meaning
    /// buy price is equal or higher than sell price
    /// </summary>
    bool isPriceCross(std::shared_ptr<Order> buyOrder,
        std::shared_ptr<Order> sellOrder) const;

    /// <summary>
    /// print trade event to ostream
    /// </summary>
    void printTradeEvent(std::shared_ptr<Order> olderOrder,
        std::shared_ptr<Order> newOrder,
        Quantity tradeQuantity,
        std::ostream& os) const;

    void processOrder(OrderType orderType,
        OrderSide orderSide,
        Price price,
        Quantity quantity,
        const OrderId& orderId);

    /// <summary>
    /// purge mathing engine, delete all queued orders
    /// </summary>
    void purgeEngine();

    /// <summary>
    /// cancel order, remove order from engine; if orderId doesn't exist, no op
    /// </summary>
    void cancelOrder(const OrderId& orderId);

    /// <summary>
    /// modify order; if orderId doesn't exist, no op; if order type is IOC, no op
    /// </summary>
    void modifyOrder(const OrderId& orderId,
        OrderSide newOrderSide,
        Price newPrice,
        Quantity newQuantity);
};

} // namespace matchingengine