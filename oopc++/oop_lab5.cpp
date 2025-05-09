#include <iostream>
#include <vector>
#include <memory>

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
            throw std::out_of_range("Queue is empty!");
        }
        T firstElement = elements.front(); 
        elements.erase(elements.begin());   
        return firstElement;
    }

    bool isEmpty() const {
        return elements.empty();
    }

    void display() const {
        std::cout << "Queue: ";
        for (const auto& elem : elements) {
            std::cout << elem << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    Queue<std::string> stringQueue;
    stringQueue.push("Apple");
    stringQueue.push("Banana");
    stringQueue.push("Cherry");

    std::cout << "String Queue:\n";
    stringQueue.display();

    std::cout << "Popped: " << stringQueue.pop() << "\n";
    stringQueue.display();

    Queue<int> intQueue;
    intQueue.push(10);
    intQueue.push(20);
    intQueue.push(30);

    std::cout << "\nInteger Queue:\n";
    intQueue.display();

    std::cout << "Popped: " << intQueue.pop() << "\n";
    intQueue.display();

    return 0;
}