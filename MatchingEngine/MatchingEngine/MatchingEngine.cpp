#include "MatchingEngine.h"

namespace matchingengine {


void MatchingEngine::insertIntoPriceMap(PriceMap& priceMap, std::shared_ptr<Order> order)
{
    if (priceMap.find(order->m_price) != priceMap.end()) {
        // this price already exists
        priceMap[order->m_price].push_back(order);
    }
    else {
        // new price
        priceMap.emplace(order->m_price, OrdersList{ order });
    }
}


std::string Order::OrderTypeToString(OrderType orderType)
{
    switch (orderType) {
    case OrderType::GFD:
        return "GFD";
    case OrderType::IOC:
        return "IOC";
    default:
        return "UNEXPECTED_ORDER_TYPE";
    }
}


std::string Order::OrderSideToString(OrderSide orderSide)
{
    switch (orderSide) {
    case OrderSide::BUY:
        return "BUY";
    case OrderSide::SELL:
        return "SELL";
    default:
        return "UNEXPECTED_ORDER_SIDE";
    }
}


void MatchingEngine::printPriceQuantitySummary(const PriceMap& priceMap, std::ostream& os) const
{
    for (const auto& priceEntry : priceMap) {
        const auto& orders = priceEntry.second;
        Quantity quantitySum = std::accumulate(orders.cbegin(),
            orders.cend(),
            0,
            [](Quantity sum, std::shared_ptr<Order> order) { return sum + order->m_quantity; });
        if (quantitySum > 0) {
            os << priceEntry.first << " " << quantitySum << "\n";
        }
    }
}


void MatchingEngine::print() const
{
    std::cout << "SELL:\n";
    printPriceQuantitySummary(m_priceMapSell, std::cout);
    std::cout << "BUY:\n";
    printPriceQuantitySummary(m_priceMapBuy, std::cout);
}


bool MatchingEngine::isValidOrder(Price price, Quantity quantity, const OrderId& orderId) const
{
    if (price <= 0 || quantity <= 0) {
        return false;
    }

    if (m_orderIdToOrder.find(orderId) != m_orderIdToOrder.cend()) {
        return false;
    }

    return true;
}


void MatchingEngine::insertOrder(std::shared_ptr<Order> newOrder)
{
    m_orderIdToOrder[newOrder->m_orderId] = newOrder;
    switch (newOrder->m_orderSide) {
    case OrderSide::BUY: {
        insertIntoPriceMap(m_priceMapBuy, newOrder);
    } break;
    case OrderSide::SELL: {
        insertIntoPriceMap(m_priceMapSell, newOrder);
    } break;
    }
}


bool MatchingEngine::isPriceCross(std::shared_ptr<Order> buyOrder,
    std::shared_ptr<Order> sellOrder) const
{
    if (buyOrder->m_orderSide != OrderSide::BUY || sellOrder->m_orderSide != OrderSide::SELL) {
        throw std::runtime_error("Wrong order sides");
    }
    return buyOrder->m_price >= sellOrder->m_price;
}


void MatchingEngine::printTradeEvent(std::shared_ptr<Order> olderOrder,
    std::shared_ptr<Order> newOrder,
    Quantity tradeQuantity,
    std::ostream& os) const
{
    os << "TRADE " << olderOrder->m_orderId
        << " " << olderOrder->m_price
        << " " << tradeQuantity
        << " " << newOrder->m_orderId
        << " " << newOrder->m_price
        << " " << tradeQuantity
        << "\n";
}


void MatchingEngine::tradeSellOrder(std::shared_ptr<Order> newOrder)
{
    if (newOrder->m_orderSide != OrderSide::SELL) {
        throw std::runtime_error("Wrong order side");
    }

    // for Sell order, just need to compare with queued buy orders from high price to low price
    for (auto buyOrdersPerPrice = m_priceMapBuy.begin(); buyOrdersPerPrice != m_priceMapBuy.end();) {
        for (auto buyOrder = buyOrdersPerPrice->second.begin(); buyOrder != buyOrdersPerPrice->second.end();) {

            if (newOrder->m_quantity <= 0) {
                // trade is done
                break;
            }

            if (isPriceCross(*buyOrder, newOrder)) {
                // matched
                Quantity tradeQuantity = std::min(newOrder->m_quantity, (*buyOrder)->m_quantity);
                printTradeEvent(*buyOrder, newOrder, tradeQuantity, std::cout);
                // update remaining quantities 
                (*buyOrder)->m_quantity -= tradeQuantity;
                newOrder->m_quantity -= tradeQuantity;

                if ((*buyOrder)->m_quantity <= 0) {
                    // remove buyOrder
                    m_orderIdToOrder.erase((*buyOrder)->m_orderId);
                    buyOrder = buyOrdersPerPrice->second.erase(buyOrder);
                }
                else {
                    ++buyOrder;
                }
            }
            else {
                // there no more buy order price equal or higher than newOrder (sell) price
                break;
            }
        }

        if (buyOrdersPerPrice->second.empty()) {
            // all queued buy orders at this price are traded
            m_priceMapBuy.erase(buyOrdersPerPrice++);
        }
        else {
            ++buyOrdersPerPrice;
        }
    }
}


void MatchingEngine::tradeBuyOrder(std::shared_ptr<Order> newOrder)
{
    if (newOrder->m_orderSide != OrderSide::BUY) {
        throw std::runtime_error("Wrong order side");
    }

    // for Buy orders, need to compare with queued sell orders from the first price that's equal or smaller than sell price
    for (auto sellOrdersPerPrice = m_priceMapSell.lower_bound(newOrder->m_price);
        sellOrdersPerPrice != m_priceMapSell.end();) {
        for (auto sellOrder = sellOrdersPerPrice->second.begin(); sellOrder != sellOrdersPerPrice->second.end();) {

            if (newOrder->m_quantity <= 0) {
                // trade is done
                break;
            }

            if (isPriceCross(newOrder, *sellOrder)) {
                // matched
                Quantity tradeQuantity = std::min(newOrder->m_quantity, (*sellOrder)->m_quantity);
                printTradeEvent(*sellOrder, newOrder, tradeQuantity, std::cout);
                // update remaining quantities
                (*sellOrder)->m_quantity -= tradeQuantity;
                newOrder->m_quantity -= tradeQuantity;

                if ((*sellOrder)->m_quantity <= 0) {
                    // remove sell order
                    m_orderIdToOrder.erase((*sellOrder)->m_orderId);
                    sellOrder = sellOrdersPerPrice->second.erase(sellOrder);
                }
                else {
                    ++sellOrder;
                }
            }
        }

        if (sellOrdersPerPrice->second.empty()) {
            // all queued sell orders at this price are traded
            m_priceMapSell.erase(sellOrdersPerPrice++);
        }
        else {
            ++sellOrdersPerPrice;
        }
    }
}


void MatchingEngine::processOrder(OrderType orderType,
    OrderSide orderSide,
    Price price,
    Quantity quantity,
    const OrderId& orderId)
{   
    // validate order
    if (!isValidOrder(price, quantity, orderId)) {
        return;
    }

    // create a new order
    std::shared_ptr<Order> newOrder = std::make_shared<Order>(orderType,
        orderSide,
        price,
        quantity,
        orderId);

    switch (orderSide) {
    case OrderSide::SELL:
        tradeSellOrder(newOrder);
        break;
    case OrderSide::BUY:
        tradeBuyOrder(newOrder);
        break;
    default:
        throw std::runtime_error("Unsupported order side!");
    }

    if (newOrder->m_quantity > 0 && newOrder->m_orderType == OrderType::GFD) {
        // new order has non-traded quantity and is of type GFD, need to queue it
        insertOrder(newOrder);
    }
}


void MatchingEngine::purgeEngine()
{
    m_orderIdToOrder.clear();
    m_priceMapBuy.clear();
    m_priceMapSell.clear();
}


void MatchingEngine::eraseOrderFromPriceMap(PriceMap& priceMap,
    Price price,
    const OrderId& orderId)
{
    auto ordersPerPrice = priceMap.find(price);
    if (ordersPerPrice != priceMap.end()) {
        auto orderItr = std::find_if(ordersPerPrice->second.begin(),
            ordersPerPrice->second.end(),
            [orderId](std::shared_ptr<Order> order) {
                return order->m_orderId == orderId;
            });
        if (orderItr != ordersPerPrice->second.end()) {
            ordersPerPrice->second.erase(orderItr);
        }

        if (ordersPerPrice->second.empty()) {
            priceMap.erase(price);
        }
    }
}


void MatchingEngine::cancelOrder(const OrderId& orderId)
{
    if (m_orderIdToOrder.find(orderId) == m_orderIdToOrder.cend()) {
        // order Id doesn't exist, no op
        return;
    }

    Price price = m_orderIdToOrder[orderId]->m_price;
    OrderSide orderSide = m_orderIdToOrder[orderId]->m_orderSide;
    switch (orderSide) {
    case OrderSide::BUY:
        eraseOrderFromPriceMap(m_priceMapBuy, price, orderId);
        break;
    case OrderSide::SELL:
        eraseOrderFromPriceMap(m_priceMapSell, price, orderId);
        break;
    default:
        throw std::runtime_error("Unsupported order side!");
    }

    m_orderIdToOrder.erase(orderId);
}


void MatchingEngine::modifyOrder(const OrderId& orderId,
    OrderSide newOrderSide,
    Price newPrice,
    Quantity newQuantity)
{
    if (m_orderIdToOrder.find(orderId) == m_orderIdToOrder.cend()) {
        // order doesn't exist, no op
        return;
    }

    auto& order = m_orderIdToOrder[orderId];

    if (order->m_orderType == OrderType::IOC) {
        // cannot modify IOC order, no op
        return;
    }

    OrderSide oldOrderSide = order->m_orderSide;
    Price oldPrice = order->m_price;
    

    order->m_price = newPrice;
    order->m_quantity = newQuantity;
    order->m_orderSide = newOrderSide;

    switch(oldOrderSide) {
    case OrderSide::BUY:
        eraseOrderFromPriceMap(m_priceMapBuy, oldPrice, orderId);
        break;
    case OrderSide::SELL:
        eraseOrderFromPriceMap(m_priceMapSell, oldPrice, orderId);
        break;
    default:
        throw std::runtime_error("Unsupported order side!");
    }

    switch (newOrderSide) {
    case OrderSide::BUY:
        insertIntoPriceMap(m_priceMapBuy, order);
        break;
    case OrderSide::SELL:
        insertIntoPriceMap(m_priceMapSell, order);
        break;
    default:
        throw std::runtime_error("Unsupported order side!");
    }
    
}





} // namespace matchingengine