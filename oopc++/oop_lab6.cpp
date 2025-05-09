#include <iostream>
#include <vector>
#include <stdexcept> 

template <typename T>
class Queue {
private:
    std::vector<T> elements;  

public:
    void push(const T& item) {
        elements.push_back(item);
    }

    T pop() {
        if (elements.empty()) {
            throw std::out_of_range("Queue is empty! Cannot pop.");
        }
        T firstElement = elements.front();  
        elements.erase(elements.begin());   
        return firstElement;
    }

    bool isEmpty() const {
        return elements.empty();
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "Queue is empty.\n";
            return;
        }
        std::cout << "Queue: ";
        for (const auto& elem : elements) {
            std::cout << elem << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    try {
        Queue<std::string> stringQueue;
        stringQueue.push("Apple");
        stringQueue.push("Banana");
        stringQueue.display();

        std::cout << "Popped: " << stringQueue.pop() << "\n";
        std::cout << "Popped: " << stringQueue.pop() << "\n";
        stringQueue.display();

        std::cout << "Popped: " << stringQueue.pop() << "\n";

    }
    catch (const std::out_of_range& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
    }

    try {
        Queue<int> intQueue;
        intQueue.push(100);
        intQueue.display();

        std::cout << "Popped: " << intQueue.pop() << "\n";
        intQueue.display();

        std::cout << "Popped: " << intQueue.pop() << "\n";

    }
    catch (const std::out_of_range& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
    }

    return 0;
}