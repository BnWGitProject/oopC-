#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

class Entity {
protected:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), maxHealth(h), attack(a), defense(d) {}

    virtual void attackTarget(Entity& target) {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    virtual void heal(int amount) {
        health = (health + amount > maxHealth) ? maxHealth : health + amount;
        std::cout << name << " heals for " << amount << " HP. Current HP: " << health << "\n";
    }

    virtual void displayInfo() const {
        std::cout << "Entity: " << name << ", HP: " << health << "/" << maxHealth
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // Методы для доступа к protected полям
    int getDefense() const { return defense; }
    std::string getName() const { return name; }
    void takeDamage(int damage) { health -= damage; }

    virtual ~Entity() {}
};

class Character : public Entity {
public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    void attackTarget(Entity& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            // Критический удар с шансом 20%
            if (rand() % 100 < 20) {
                damage *= 2;
                std::cout << "Critical hit! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    void heal(int amount) override {
        int actualHeal = amount * 1.5; // Персонаж лечится на 50% эффективнее
        health = (health + actualHeal > maxHealth) ? maxHealth : health + actualHeal;
        std::cout << name << " uses healing potion! ";
        std::cout << "Heals for " << actualHeal << " HP. Current HP: " << health << "/" << maxHealth << "\n";
    }

    void displayInfo() const override {
        std::cout << "Character: " << name << ", HP: " << health << "/" << maxHealth
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Monster : public Entity {
public:
    Monster(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    void attackTarget(Entity& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            // Ядовитая атака с шансом 30%
            if (rand() % 100 < 30) {
                damage += 5;
                std::cout << "Poisonous attack! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    void displayInfo() const override {
        std::cout << "Monster: " << name << ", HP: " << health << "/" << maxHealth
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Boss : public Monster {
    std::string specialAbility;
public:
    Boss(const std::string& n, int h, int a, int d, const std::string& ability)
        : Monster(n, h, a, d), specialAbility(ability) {}

    void attackTarget(Entity& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            // Огненный удар с шансом 40%
            if (rand() % 100 < 40) {
                damage += 10;
                std::cout << specialAbility << "! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    void displayInfo() const override {
        std::cout << "BOSS: " << name << ", HP: " << health << "/" << maxHealth
            << ", Attack: " << attack << ", Defense: " << defense
            << ", Ability: " << specialAbility << std::endl;
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел

    Character hero("Hero", 100, 20, 10);
    Monster goblin("Goblin", 50, 15, 5);
    Boss dragon("Dragon King", 200, 30, 20, "Fire Breath");

    Entity* entities[] = { &hero, &goblin, &dragon };

    std::cout << "=== Initial Status ===\n";
    for (auto& entity : entities) {
        entity->displayInfo();
    }

    std::cout << "\n=== Battle ===\n";
    hero.attackTarget(goblin);
    goblin.attackTarget(hero);
    dragon.attackTarget(hero);

    std::cout << "\n=== Healing ===\n";
    hero.heal(30);

    std::cout << "\n=== After Battle ===\n";
    for (auto& entity : entities) {
        entity->displayInfo();
    }

    return 0;
}