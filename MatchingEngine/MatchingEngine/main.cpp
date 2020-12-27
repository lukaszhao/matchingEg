#include "MatchingEngine.h"
#include "MessageProcessor.h"

using namespace matchingengine;

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


void testTokenizer() {
    std::string msg = "A B2 C3 D^ E#";
    auto result = MessageProcessor::tokenizeMessage(msg);
    std::cout << "tokens = " << stringifyVector(result);
}

void testListenToMessage() {
    MessageProcessor messageProcessor(std::make_shared<MatchingEngine>());
    messageProcessor.listenToMessage(std::cin);
}


int main()
{
    std::cout << "Begin Test!\n\n";

    //testTokenizer();
    testListenToMessage();

    std::cout << "\n\nEnd of Test!\n\n";
}


