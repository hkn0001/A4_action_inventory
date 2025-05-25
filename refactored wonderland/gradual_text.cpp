#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void cool_text(const std::string& text) {
    auto tp = std::chrono::steady_clock::now();
    for(char letter : text) {
        std::cout << letter << std::flush;
        tp += std::chrono::milliseconds(50);
        std::this_thread::sleep_until(tp);
    }
}


int main() {
    cool_text("blah blah blah :??LIFds;ljs ;alsdkfghawo;rljgvhasfdl;kfjdsalkfjsda;lkfjdsalfjsdalkfjlasdjflsdj a");
}
