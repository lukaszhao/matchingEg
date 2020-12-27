#pragma once

#include "MatchingEngineI.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>


namespace matchingengine {

class MessageProcessor
{
private:
    std::shared_ptr<MatchingEngineI>  m_matchingEngineI;



public:
    /// <summary>
    /// ctor, inject dependency
    /// </summary>
    MessageProcessor(std::shared_ptr<MatchingEngineI> matchingEngineI) :
        m_matchingEngineI(matchingEngineI) {}

    /// <summary>
    /// parse message, return tokens
    /// </summary>
    static std::vector<std::string> tokenizeMessage(const std::string& msg);

    /// <summary>
    /// convert a token string to order side;
    /// function returns true if succeeds, o.w. false
    /// </summary>
    static bool getOrderSideFromToken(const std::string& token,
        OrderSide& orderSide);

    /// <summary>
    /// convert a token string to order type;
    /// function returns true if succeeds, o.w. false
    /// </summary>
    static bool getOrderTypeFromToken(const std::string& token,
        OrderType& orderType);

    /// <summary>
    /// convert a token string to price;
    /// function returns true if succeeds, o.w. false
    /// </summary>
    static bool getPriceFromToken(const std::string& token,
        Price& price);

    /// <summary>
    /// convert a token string to quantity;
    /// function returns true if succeeds, o.w. false
    /// </summary>
    static bool getQuantityFromToken(const std::string& token,
        Quantity& quantity);

    /// <summary>
    /// convert a token string to order ID;
    /// this function will steal token's resource;
    /// function returns true if succeeds, o.w. false
    /// </summary>
    static bool getOrderIdFromToken(std::string& token,
        OrderId& orderId);

    /// <summary>
    /// listen to message, and parse message
    /// </summary>
    void listenToMessage(std::istream& is) const;

};

} // namespace matchingengine

