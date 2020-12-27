#pragma once

#include <string>

namespace matchingengine {

enum class OrderType { IOC, GFD };

enum class OrderSide { BUY, SELL };

using Price = int;
using Quantity = int;
using OrderId = std::string;


/// <summary>
/// Interface class of class MatchingEngine,
/// for dependency injection and mocking in unit tests
/// </summary>
class MatchingEngineI
{
public:

    virtual ~MatchingEngineI() {}

    virtual void print() const = 0;

    virtual void processOrder(OrderType orderType,
        OrderSide orderSide,
        Price price,
        Quantity quantity,
        const OrderId& orderId) = 0;

    virtual void purgeEngine() = 0;

    virtual void cancelOrder(const OrderId& orderId) = 0;

    virtual void modifyOrder(const OrderId& orderId,
        OrderSide newOrderSide,
        Price newPrice,
        Quantity newQuantity) = 0;
};

} // namespace matchingengine

