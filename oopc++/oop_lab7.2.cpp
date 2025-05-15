#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <random>

using namespace std;

class Character {
private:
    string name;
    int health;
    int attack;
    int defense;
    mutex mtx;

public:
    Character(string name, int health, int attack, int defense)
        : name(name), health(health), attack(attack), defense(defense) {}

    void takeDamage(int damage) {
        lock_guard<mutex> lock(mtx);
        int actualDamage = max(1, damage - defense);
        health -= actualDamage;
        if (health < 0) health = 0;
    }

    bool isAlive() {
        lock_guard<mutex> lock(mtx);
        return health > 0;
    }

    int getAttack() {
        lock_guard<mutex> lock(mtx);
        return attack;
    }

    void displayInfo() {
        lock_guard<mutex> lock(mtx);
        cout << name << " [HP: " << health << ", ATK: " << attack << ", DEF: " << defense << "]\n";
    }
};

class Monster {
private:
    string name;
    int health;
    int attack;
    int defense;
    mutex mtx;

public:
    Monster(string name, int health, int attack, int defense)
        : name(name), health(health), attack(attack), defense(defense) {}

    void takeDamage(int damage) {
        lock_guard<mutex> lock(mtx);
        int actualDamage = max(1, damage - defense);
        health -= actualDamage;
        if (health < 0) health = 0;
    }

    bool isAlive() {
        lock_guard<mutex> lock(mtx);
        return health > 0;
    }

    int getAttack() {
        lock_guard<mutex> lock(mtx);
        return attack;
    }

    void displayInfo() {
        lock_guard<mutex> lock(mtx);
        cout << name << " [HP: " << health << ", ATK: " << attack << ", DEF: " << defense << "]\n";
    }
};

vector<Monster> monsters;
mutex monstersMutex;

void generateMonsters() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> healthDist(30, 80);
    uniform_int_distribution<> attackDist(5, 20);
    uniform_int_distribution<> defenseDist(0, 10);
    vector<string> names = { "Goblin", "Orc", "Troll", "Skeleton", "Zombie" };

    while (true) {
        this_thread::sleep_for(chrono::seconds(3));

        uniform_int_distribution<> nameDist(0, names.size() - 1);
        string name = names[nameDist(gen)];
        int health = healthDist(gen);
        int attack = attackDist(gen);
        int defense = defenseDist(gen);

        {
            lock_guard<mutex> lock(monstersMutex);
            monsters.emplace_back(name, health, attack, defense);
            cout << "New monster appeared: ";
            monsters.back().displayInfo();
        }
    }
}

void battle(Character& hero, Monster& monster) {
    while (hero.isAlive() && monster.isAlive()) {
        // Герой атакует
        monster.takeDamage(hero.getAttack());
        if (!monster.isAlive()) {
            cout << "Monster defeated!\n";
            break;
        }

        // Монстр атакует
        hero.takeDamage(monster.getAttack());
        if (!hero.isAlive()) {
            cout << "Hero defeated!\n";
            break;
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}

// Поток для обработки боев
void battleThread(Character& hero) {
    while (hero.isAlive()) {
        Monster* currentMonster = nullptr;

        {
            lock_guard<mutex> lock(monstersMutex);
            if (!monsters.empty()) {
                currentMonster = &monsters.back();
                monsters.pop_back();
                cout << "Starting battle with: ";
                currentMonster->displayInfo();
            }
        }

        if (currentMonster) {
            battle(hero, *currentMonster);
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main() {
    Character hero("Hero", 100, 20, 10);

    thread generator(generateMonsters);
    generator.detach();

    thread battle(battleThread, ref(hero));

    while (hero.isAlive()) {
        this_thread::sleep_for(chrono::seconds(1));

        cout << "\n=== Current State ===\n";
        hero.displayInfo();

        {
            lock_guard<mutex> lock(monstersMutex);
            cout << "Monsters in queue: " << monsters.size() << endl;
            for (auto& m : monsters) {
                m.displayInfo();
            }
        }
    }

    battle.join();
    cout << "Game over!\n";

    return 0;
}