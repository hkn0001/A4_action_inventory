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
    cool_text("I am beginning to learn ruby and was following the instructions of one video to access the desktop through the -ls command in the terminal. Now, I would like to get out of the desktop in the terminal and don't know how. I know, I know I am a total noob... but can someone please help. Thanks. BTW the command I wrote was:");
}
