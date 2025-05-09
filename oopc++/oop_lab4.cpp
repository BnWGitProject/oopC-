#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Inventory {
private:
    std::unique_ptr<std::string[]> items;  
    size_t capacity;                       
    size_t size;                           

    void resize() {
        size_t new_capacity = (capacity == 0) ? 1 : capacity * 2;
        auto new_items = std::make_unique<std::string[]>(new_capacity);

        for (size_t i = 0; i < size; ++i) {
            new_items[i] = items[i];
        }

        items = std::move(new_items);
        capacity = new_capacity;
    }

public:
    Inventory() : items(nullptr), capacity(0), size(0) {}

    void addItem(const std::string& item) {
        if (size >= capacity) {
            resize();
        }
        items[size++] = item;
    }

    void displayInventory() const {
        std::cout << "Inventory (" << size << "/" << capacity << "):\n";
        for (size_t i = 0; i < size; ++i) {
            std::cout << "- " << items[i] << "\n";
        }
    }
};

int main() {
    Inventory inv;

    inv.addItem("Sword");
    inv.addItem("Health Potion");
    inv.addItem("Shield");
    inv.addItem("Mana Potion");

    inv.displayInventory();

    return 0;
}