#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>

class Entity {
public:
    virtual ~Entity() = default;
    virtual std::string getName() const = 0;
    virtual int getHealth() const = 0;
    virtual int getLevel() const = 0;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;
};

class Player : public Entity {
private:
    std::string name;
    int health;
    int level;

public:
    Player(const std::string& name, int health, int level)
        : name(name), health(health), level(level) {}

    std::string getName() const override { return name; }
    int getHealth() const override { return health; }
    int getLevel() const override { return level; }

    std::string serialize() const override {
        return name + "," + std::to_string(health) + "," + std::to_string(level);
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);

        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            throw std::runtime_error("Invalid data format");
        }

        name = data.substr(0, pos1);
        health = std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
        level = std::stoi(data.substr(pos2 + 1));
    }
};

template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            std::cout << "Name: " << entity->getName()
                << ", Health: " << entity->getHealth()
                << ", Level: " << entity->getLevel() << std::endl;
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing.");
        }

        for (const auto& entity : entities) {
            file << entity->serialize() << "\n";
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for reading.");
        }

        entities.clear();
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto player = new Player("", 0, 0);
            player->deserialize(line);
            entities.push_back(player);
        }
    }

    ~GameManager() {
        for (auto entity : entities) {
            delete entity;
        }
    }
};

int main() {
    GameManager<Entity*> manager;
    manager.addEntity(new Player("Hero", 100, 1));
    manager.addEntity(new Player("Mage", 80, 2));
    manager.addEntity(new Player("Warrior", 120, 3));

    try {
        manager.saveToFile("game_save.txt");
        std::cout << "Game saved successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving game: " << e.what() << std::endl;
    }

    GameManager<Entity*> loadedManager;
    try {
        loadedManager.loadFromFile("game_save.txt");
        std::cout << "Game loaded successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
    }

    loadedManager.displayAll();

    return 0;
}