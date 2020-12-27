#include "MatchingEngine.h"


using namespace matchingengine;

void testInsertAndPrint() {
    MatchingEngine matchingEngine;
    std::shared_ptr<Order> newOrder;
    
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 1000, 10, "orderId_1");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 1000, 10, "orderId_2");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 1000, 10, "orderId_3");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 1001, 10, "orderId_4");
    matchingEngine.insertOrder(newOrder);

    matchingEngine.print();

    std::cout << "\n--- inspect ---\n";
    matchingEngine.inspect();
}


void testIsPriceCross() {
    MatchingEngine matchingEngine;
    std::shared_ptr<Order> order_1 = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 1200, 10, "A");
    std::shared_ptr<Order> order_2 = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 1100, 10, "B");
    std::cout << "isPriceCross = " << matchingEngine.isPriceCross(order_1, order_2);
}


void testSell() {
    MatchingEngine matchingEngine;
    std::shared_ptr<Order> newOrder;

    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 10, 10, "orderId_1");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 9, 10, "orderId_2");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 5, 10, "orderId_3");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 2, 10, "orderId_4");
    matchingEngine.insertOrder(newOrder);

    matchingEngine.print();

    std::cout << "\n---- sell ----\n";

    matchingEngine.processOrder(OrderType::GFD, OrderSide::SELL, 1, 100, "orderSell");

    matchingEngine.print();
    matchingEngine.inspect();
}

void testBuy() {
    MatchingEngine matchingEngine;
    std::shared_ptr<Order> newOrder;

    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 60, 10, "orderId_1");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 50, 10, "orderId_2");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 40, 10, "orderId_3");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 30, 10, "orderId_4");
    matchingEngine.insertOrder(newOrder);

    matchingEngine.print();

    std::cout << "\n---- buy ----\n";

    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 40, 15, "orderSell");

    matchingEngine.print();
    matchingEngine.inspect();
}

void testSellAndBuy() {
    MatchingEngine matchingEngine;

    std::cout << "\n---- eg.1. ----\n";
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 10, "order1");

    matchingEngine.print();
    matchingEngine.inspect();
    matchingEngine.purgeEngine();

    std::cout << "\n---- eg.2. ----\n";
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 10, "order1");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 20, "order2");

    matchingEngine.print();
    matchingEngine.inspect();
    matchingEngine.purgeEngine();

    std::cout << "\n---- eg.3. ----\n";
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 10, "order1");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1001, 20, "order2");

    matchingEngine.print();
    matchingEngine.inspect();
    matchingEngine.purgeEngine();

    std::cout << "\n---- eg.4. ----\n";
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 10, "order1");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::SELL, 900, 20, "order2");

    matchingEngine.print();
    matchingEngine.inspect();
    matchingEngine.purgeEngine();

    std::cout << "\n---- eg.5. ----\n";
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 10, "ORDER1");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1010, 10, "ORDER2");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::SELL, 1000, 15, "ORDER3");

    matchingEngine.inspect();
    matchingEngine.purgeEngine();
}

void testCancelOrder() {
    MatchingEngine matchingEngine;
    std::shared_ptr<Order> newOrder;
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 1000, 10, "orderId_1");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 1000, 10, "orderId_2");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::SELL, 1000, 10, "orderId_3");
    matchingEngine.insertOrder(newOrder);
    newOrder = std::make_shared<Order>(OrderType::GFD, OrderSide::BUY, 1001, 10, "orderId_4");
    matchingEngine.insertOrder(newOrder);

    std::cout << "\n---- cancel orderId_3 ----\n";
    matchingEngine.cancelOrder("orderId_3");
    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n---- cancel orderId_1 ----\n";
    matchingEngine.cancelOrder("orderId_1");
    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n---- cancel orderId_2 ----\n";
    matchingEngine.cancelOrder("orderId_2");
    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n---- cancel orderId_xxx ----\n";
    matchingEngine.cancelOrder("orderId_xxx");
    matchingEngine.print();
    matchingEngine.inspect();
}

void testModifyOrder() {
    MatchingEngine matchingEngine;
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 10, "orderId_1");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1000, 20, "orderId_2");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1001, 30, "orderId_3");
    matchingEngine.processOrder(OrderType::GFD, OrderSide::BUY, 1002, 40, "orderId_4");

    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n--- modify ---\n";
    matchingEngine.modifyOrder("orderId_1", OrderSide::BUY, 1000, 15);

    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n--- modify ---\n";
    matchingEngine.modifyOrder("orderId_1", OrderSide::BUY, 1001, 25);

    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n--- modify ---\n";
    matchingEngine.modifyOrder("orderId_1", OrderSide::SELL, 1001, 25);
    matchingEngine.modifyOrder("orderId_3", OrderSide::SELL, 1001, 45);

    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n--- modify ---\n";
    matchingEngine.modifyOrder("orderId_3", OrderSide::SELL, 1002, 22);
    matchingEngine.modifyOrder("orderId_1", OrderSide::SELL, 1004, 33);
    
    matchingEngine.print();
    matchingEngine.inspect();

    std::cout << "\n--- modify ---\n";
    matchingEngine.modifyOrder("orderId_3", OrderSide::BUY, 1002, 22);
    matchingEngine.modifyOrder("orderId_1", OrderSide::SELL, 1004, 33);

    matchingEngine.print();
    matchingEngine.inspect();
}

int main()
{
    std::cout << "Begin Test!\n\n";

    //testInsertAndPrint();
    //testIsPriceCross();
    //testSell();
    //testBuy();
    //testSellAndBuy();
    //testCancelOrder();
    //testModifyOrder();

    std::cout << "\n\nEnd of Test!\n\n";
}


