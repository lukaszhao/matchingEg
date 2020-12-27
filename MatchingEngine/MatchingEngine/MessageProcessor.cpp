#include "MessageProcessor.h"


namespace matchingengine {


template<typename T>
std::string stringifyVector(const std::vector<T>& vec) {
    std::stringstream ss;
    ss << "[ size = " << vec.size();
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << " [" << i << "] " << vec[i];
    }
    ss << " ]";
    return ss.str();
}

std::vector<std::string> MessageProcessor::tokenizeMessage(const std::string& msg)
{
    std::stringstream ss(msg);
    std::vector<std::string> tokens;
    
    std::string token;

    while (getline(ss, token, ' ')) {
        tokens.push_back(std::move(token));
    }

    return tokens;
}


bool MessageProcessor::getOrderSideFromToken(const std::string& token,
    OrderSide& orderSide)
{
    if (token == "BUY") {
        orderSide = OrderSide::BUY;
        return true;
    }

    if (token == "SELL") {
        orderSide = OrderSide::SELL;
        return true;
    }

    return false;
}


bool MessageProcessor::getOrderTypeFromToken(const std::string& token,
    OrderType& orderType)
{
    if (token == "IOC") {
        orderType = OrderType::IOC;
        return true;
    }
    if (token == "GFD") {
        orderType = OrderType::GFD;
        return true;
    }
    return false;
}


bool MessageProcessor::getPriceFromToken(const std::string& token,
    Price& price)
{
    try {
        price = std::stoi(token);
    }
    catch (...) {
        return false;
    }
    return true;
}


bool MessageProcessor::getQuantityFromToken(const std::string& token,
    Quantity& quantity)
{
    try {
        quantity = std::stoi(token);
    }
    catch (...) {
        return false;
    }
    return true;
}


bool MessageProcessor::getOrderIdFromToken(std::string& token,
    OrderId& orderId)
{
    if (token.empty()) {
        return false;
    }
    orderId.swap(token);
    return true;
}


void MessageProcessor::listenToMessage(std::istream& is) const
{
    std::string line;
    while (true) {
        getline(is, line, '\n');
        
        std::vector<std::string> tokens = tokenizeMessage(line);

        if (!tokens.empty()) {
            
            // BUY or SELL
            if (tokens[0] == "BUY" || tokens[0] == "SELL") {
                // expect 5 tokens
                if (tokens.size() != 5) {
                    continue;
                }

                OrderSide orderSide;
                bool success = getOrderSideFromToken(tokens[0], orderSide);
                if (!success) {
                    continue;
                }
                OrderType orderType;
                success = getOrderTypeFromToken(tokens[1], orderType);
                if (!success) {
                    continue;
                }
                Price price;
                success = getPriceFromToken(tokens[2], price);
                if (!success) {
                    continue;
                }
                Quantity quantity;
                success = getQuantityFromToken(tokens[3], quantity);
                if (!success) {
                    continue;
                }
                OrderId orderId;
                success = getOrderIdFromToken(tokens[4], orderId);
                if (!success) {
                    continue;
                }

                m_matchingEngineI->processOrder(orderType, orderSide, price, quantity, orderId);
            }

            // CANCEL
            if (tokens[0] == "CANCEL") {
                // expect 2 tokens
                if (tokens.size() != 2) {
                    continue;
                }

                OrderId orderId;
                bool success = getOrderIdFromToken(tokens[1], orderId);
                if (!success) {
                    continue;
                }

                m_matchingEngineI->cancelOrder(orderId);
            }

            // MODIFY
            if (tokens[0] == "MODIFY") {
                // expect 5 tokens
                if (tokens.size() != 5) {
                    continue;
                }

                OrderId orderId;
                bool success = getOrderIdFromToken(tokens[1], orderId);
                if (!success) {
                    continue;
                }
                OrderSide newOrderSide;
                success = getOrderSideFromToken(tokens[2], newOrderSide);
                if (!success) {
                    continue;
                }
                Price newPrice;
                success = getPriceFromToken(tokens[3], newPrice);
                if (!success) {
                    continue;
                }
                Quantity newQuantity;
                success = getQuantityFromToken(tokens[4], newQuantity);
                if (!success) {
                    continue;
                }
                
                m_matchingEngineI->modifyOrder(orderId, newOrderSide, newPrice, newQuantity);
            }

            // PRINT
            if (tokens[0] == "PRINT") {
                m_matchingEngineI->print();
            }
        }
    }
}

} // namespace matchingengine