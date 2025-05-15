#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <ctime>
#include <map>
#include <cstdlib> // Для rand() и srand()

// Объявление класса Character перед Monster, так как Monster использует Character
class Character;

template<typename T>
class Logger {
private:
    std::ofstream logFile;
public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile) {
            throw std::runtime_error("Cannot open log file!");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const T& message) {
        time_t now = time(0);
        char* dt = ctime(&now);
        logFile << dt << ": " << message << std::endl;
        std::cout << message << std::endl;
    }
};

class Item {
protected:
    std::string name;
    std::string description;
public:
    Item(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    virtual ~Item() = default;

    virtual void use() = 0;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
};

class Weapon : public Item {
private:
    int attackBonus;
public:
    Weapon(const std::string& n, const std::string& desc, int atk)
        : Item(n, desc), attackBonus(atk) {}

    void use() override {
        std::cout << "Equipped " << name << " (+" << attackBonus << " attack)" << std::endl;
    }

    int getAttackBonus() const { return attackBonus; }

    std::string serialize() const override {
        return "Weapon," + name + "," + description + "," + std::to_string(attackBonus);
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);

        name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        description = data.substr(pos2 + 1, pos3 - pos2 - 1);
        attackBonus = std::stoi(data.substr(pos3 + 1));
    }
};

class Potion : public Item {
private:
    int healAmount;
public:
    Potion(const std::string& n, const std::string& desc, int heal)
        : Item(n, desc), healAmount(heal) {}

    void use() override {
        std::cout << "Drank " << name << " (+" << healAmount << " HP)" << std::endl;
    }

    int getHealAmount() const { return healAmount; }

    std::string serialize() const override {
        return "Potion," + name + "," + description + "," + std::to_string(healAmount);
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);

        name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        description = data.substr(pos2 + 1, pos3 - pos2 - 1);
        healAmount = std::stoi(data.substr(pos3 + 1));
    }
};

class Inventory {
private:
    std::vector<std::unique_ptr<Item>> items;
public:
    void addItem(std::unique_ptr<Item> item) {
        items.push_back(std::move(item));
    }

    void removeItem(int index) {
        if (index >= 0 && index < static_cast<int>(items.size())) {
            items.erase(items.begin() + index);
        }
    }

    void display() const {
        if (items.empty()) {
            std::cout << "Inventory is empty." << std::endl;
            return;
        }

        std::cout << "Inventory:" << std::endl;
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << i + 1 << ". " << items[i]->getName()
                << " - " << items[i]->getDescription() << std::endl;
        }
    }

    Item* getItem(int index) {
        if (index >= 0 && index < static_cast<int>(items.size())) {
            return items[index].get();
        }
        return nullptr;
    }

    size_t size() const { return items.size(); }

    std::string serialize() const {
        std::string result;
        for (const auto& item : items) {
            result += item->serialize() + "\n";
        }
        return result;
    }

    void deserialize(const std::string& data) {
        items.clear();
        std::istringstream iss(data);
        std::string line;

        while (std::getline(iss, line)) {
            if (line.empty()) continue;

            size_t pos = line.find(',');
            std::string type = line.substr(0, pos);

            if (type == "Weapon") {
                auto weapon = std::make_unique<Weapon>("", "", 0);
                weapon->deserialize(line);
                items.push_back(std::move(weapon));
            }
            else if (type == "Potion") {
                auto potion = std::make_unique<Potion>("", "", 0);
                potion->deserialize(line);
                items.push_back(std::move(potion));
            }
        }
    }
};

class Character; // Предварительное объявление

class Monster {
protected:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
public:
    Monster(const std::string& n, int h, int a, int d)
        : name(n), health(h), maxHealth(h), attack(a), defense(d) {}
    virtual ~Monster() = default;

    virtual void attackTarget(Character& target);
    virtual void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    bool isAlive() const { return health > 0; }

    virtual void displayInfo() const {
        std::cout << name << " [HP: " << health << "/" << maxHealth
            << ", ATK: " << attack << ", DEF: " << defense << "]";
    }

    virtual std::string serialize() const {
        return name + "," + std::to_string(health) + "," + std::to_string(maxHealth) + ","
            + std::to_string(attack) + "," + std::to_string(defense);
    }

    virtual void deserialize(const std::string& data) {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);

        name = data.substr(0, pos1);
        health = std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
        maxHealth = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        attack = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        defense = std::stoi(data.substr(pos4 + 1));
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

class Character {
private:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int level;
    int experience;
    Inventory inventory;

    void levelUp() {
        level++;
        maxHealth += 10;
        health = maxHealth;
        attack += 2;
        defense += 1;
        experience = 0;
        std::cout << name << " leveled up to level " << level << "!" << std::endl;
    }
public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), maxHealth(h), attack(a), defense(d), level(1), experience(0) {}

    void attackTarget(Monster& target) {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!" << std::endl;
        }
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) {
            health = 0;
            throw std::runtime_error(name + " has been defeated!");
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP!" << std::endl;
    }

    size_t getInventorySize() const {
        return inventory.size();
    }

    void gainExperience(int exp) {
        experience += exp;
        std::cout << name << " gained " << exp << " experience points!" << std::endl;
        if (experience >= 100) {
            levelUp();
        }
    }

    void addToInventory(std::unique_ptr<Item> item) {
        inventory.addItem(std::move(item));
    }

    void useItem(int index) {
        Item* item = inventory.getItem(index);
        if (item) {
            item->use();

            if (auto potion = dynamic_cast<Potion*>(item)) {
                heal(potion->getHealAmount());
            }
            else if (auto weapon = dynamic_cast<Weapon*>(item)) {
                attack += weapon->getAttackBonus();
            }

            inventory.removeItem(index);
        }
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health << "/" << maxHealth
            << ", ATK: " << attack << ", DEF: " << defense
            << ", LVL: " << level << ", EXP: " << experience << "/100" << std::endl;
    }

    void displayInventory() const {
        inventory.display();
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }

    std::string serialize() const {
        return name + "," + std::to_string(health) + "," + std::to_string(maxHealth) + ","
            + std::to_string(attack) + "," + std::to_string(defense) + ","
            + std::to_string(level) + "," + std::to_string(experience) + "\n"
            + inventory.serialize();
    }

    void deserialize(const std::string& data) {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);
        size_t pos5 = data.find(',', pos4 + 1);
        size_t pos6 = data.find(',', pos5 + 1);
        size_t pos7 = data.find('\n', pos6 + 1);

        name = data.substr(0, pos1);
        health = std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
        maxHealth = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        attack = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        defense = std::stoi(data.substr(pos4 + 1, pos5 - pos4 - 1));
        level = std::stoi(data.substr(pos5 + 1, pos6 - pos5 - 1));
        experience = std::stoi(data.substr(pos6 + 1, pos7 - pos6 - 1));

        if (pos7 != std::string::npos) {
            inventory.deserialize(data.substr(pos7 + 1));
        }
    }
};

void Monster::attackTarget(Character& target) {
    int damage = attack - target.getDefense();
    if (damage > 0) {
        target.takeDamage(damage);
        std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << std::endl;
    }
    else {
        std::cout << name << " attacks " << target.getName() << ", but it has no effect!" << std::endl;
    }
}

class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 8, 2) {}

    void displayInfo() const override {
        std::cout << "Goblin [HP: " << health << "/" << maxHealth
            << ", ATK: " << attack << ", DEF: " << defense << "]";
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 40, 10, 5) {}

    void attackTarget(Character& target) override {
        // Скелет имеет 20% шанс нанести двойной урон
        int damage = attack - target.getDefense();
        if (damage > 0) {
            if (rand() % 5 == 0) { // 20% chance
                damage *= 2;
                std::cout << "Critical hit! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!" << std::endl;
        }
    }

    void displayInfo() const override {
        std::cout << "Skeleton [HP: " << health << "/" << maxHealth
            << ", ATK: " << attack << ", DEF: " << defense << "]";
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 20, 10) {}

    void takeDamage(int damage) override {
        // Дракон имеет 30% шанс уклониться от атаки
        if (rand() % 10 < 3) { // 30% chance
            std::cout << name << " dodged the attack!" << std::endl;
            return;
        }
        Monster::takeDamage(damage);
    }

    void displayInfo() const override {
        std::cout << "Dragon [HP: " << health << "/" << maxHealth
            << ", ATK: " << attack << ", DEF: " << defense << "]";
    }
};

class Game {
private:
    Character player;
    Logger<std::string> logger;
    std::vector<std::unique_ptr<Monster>> monsters;

    void generateMonsters() {
        monsters.clear();
        int count = 3 + rand() % 3; // 3-5 монстров

        for (int i = 0; i < count; ++i) {
            int type = rand() % 3;
            switch (type) {
            case 0: monsters.push_back(std::make_unique<Goblin>()); break;
            case 1: monsters.push_back(std::make_unique<Skeleton>()); break;
            case 2: monsters.push_back(std::make_unique<Dragon>()); break;
            }
        }
    }

    void battle() {
        while (!monsters.empty() && player.getHealth() > 0) {
            // Вывод информации о состоянии
            std::cout << "\n=== Battle ===" << std::endl;
            player.displayInfo();
            for (size_t i = 0; i < monsters.size(); ++i) {
                std::cout << i + 1 << ". ";
                monsters[i]->displayInfo();
                std::cout << std::endl;
            }

            // Ход игрока
            std::cout << "\n1. Attack\n2. Use item\nChoose action: ";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                std::cout << "Choose target (1-" << monsters.size() << "): ";
                int target;
                std::cin >> target;

                if (target > 0 && target <= static_cast<int>(monsters.size())) {
                    try {
                        player.attackTarget(*monsters[target - 1]);
                        logger.log(player.getName() + " attacked " + monsters[target - 1]->getName());

                        // Проверяем, жив ли монстр
                        if (!monsters[target - 1]->isAlive()) {
                            int exp = 20 + monsters[target - 1]->getAttack() * 2;
                            player.gainExperience(exp);
                            logger.log("Defeated " + monsters[target - 1]->getName() + " and gained " + std::to_string(exp) + " EXP");
                            monsters.erase(monsters.begin() + target - 1);
                        }
                    }
                    catch (const std::exception& e) {
                        logger.log(e.what());
                        return;
                    }
                }
                else {
                    std::cout << "Invalid target!" << std::endl;
                }
            }
            else if (choice == 2) {
                player.displayInventory();
                if (player.getInventorySize() > 0) {
                    std::cout << "Choose item (1-" << player.getInventorySize() << "): ";
                    int item;
                    std::cin >> item;
                    if (item > 0 && item <= static_cast<int>(player.getInventorySize())) {
                        player.useItem(item - 1);
                    }
                    else {
                        std::cout << "Invalid item choice!" << std::endl;
                    }
                }
            }

            // Ход монстров
            for (size_t i = 0; i < monsters.size(); ++i) {
                try {
                    monsters[i]->attackTarget(player);
                    logger.log(monsters[i]->getName() + " attacked " + player.getName());
                }
                catch (const std::exception& e) {
                    logger.log(e.what());
                    return;
                }
            }
        }

        if (player.getHealth() > 0) {
            logger.log("Victory! All monsters defeated!");

            // Награда за победу
            int rewardType = rand() % 2;
            if (rewardType == 0) {
                auto weapon = std::make_unique<Weapon>("Steel Sword", "A sharp sword", 5);
                player.addToInventory(std::move(weapon));
                logger.log("Received Steel Sword as reward!");
            }
            else {
                auto potion = std::make_unique<Potion>("Health Potion", "Restores 30 HP", 30);
                player.addToInventory(std::move(potion));
                logger.log("Received Health Potion as reward!");
            }
        }
    }

public:
    Game(const std::string& playerName)
        : player(playerName, 100, 10, 5), logger("game_log.txt") {
        srand(static_cast<unsigned>(time(0)));
    }

    void start() {
        logger.log("Game started for " + player.getName());

        while (player.getHealth() > 0) {
            std::cout << "\n=== Main Menu ===" << std::endl;
            std::cout << "1. Start battle\n2. Show inventory\n3. Save game\n4. Load game\n5. Exit\nChoose option: ";

            int choice;
            std::cin >> choice;

            switch (choice) {
            case 1:
                generateMonsters();
                battle();
                break;
            case 2:
                player.displayInventory();
                if (player.getInventorySize() > 0) {
                    std::cout << "Use item? (1-Yes, 2-No): ";
                    int use;
                    std::cin >> use;
                    if (use == 1) {
                        std::cout << "Choose item (1-" << player.getInventorySize() << "): ";
                        int item;
                        std::cin >> item;
                        if (item > 0 && item <= static_cast<int>(player.getInventorySize())) {
                            player.useItem(item - 1);
                        }
                        else {
                            std::cout << "Invalid item choice!" << std::endl;
                        }
                    }
                }
                break;
            case 3:
                saveGame();
                break;
            case 4:
                loadGame();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
            }
        }

        logger.log("Game over for " + player.getName());
    }

    void saveGame() {
        std::ofstream file("save.txt");
        if (!file) {
            logger.log("Error: Could not save game!");
            return;
        }

        file << player.serialize();
        logger.log("Game saved successfully");
    }

    void loadGame() {
        std::ifstream file("save.txt");
        if (!file) {
            logger.log("Error: Could not load game!");
            return;
        }

        std::string data((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        player.deserialize(data);
        logger.log("Game loaded successfully");
    }
};

int main() {
    std::cout << "Enter your name: ";
    std::string name;
    std::getline(std::cin, name);

    Game game(name);
    game.start();

    return 0;
}