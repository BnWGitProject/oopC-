#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <locale>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif

// Исключение для ошибок доступа
class AccessDeniedException : public std::runtime_error {
public:
    AccessDeniedException(const std::string& msg) : std::runtime_error(msg) {}
};

// Исключение для ошибок ввода данных
class InvalidInputException : public std::runtime_error {
public:
    InvalidInputException(const std::string& msg) : std::runtime_error(msg) {}
};

// Базовый класс пользователя
class User {
protected:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel)
        : name(name), id(id), accessLevel(accessLevel) {
        if (name.empty()) {
            throw InvalidInputException("Имя пользователя не может быть пустым");
        }
        if (accessLevel < 0 || accessLevel > 3) {
            throw InvalidInputException("Уровень доступа должен быть от 0 до 3");
        }
    }

    virtual ~User() = default;

    // Геттеры
    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    // Виртуальный метод для вывода информации
    virtual void displayInfo() const {
        std::cout << "Имя: " << name
            << ", ID: " << id
            << ", Уровень доступа: " << accessLevel;
    }

    // Виртуальный метод для сериализации
    virtual std::string serialize() const {
        return "User," + name + "," + std::to_string(id) + "," + std::to_string(accessLevel);
    }

    // Виртуальный метод для десериализации
    virtual void deserialize(const std::string& data) {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);

        name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        id = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        accessLevel = std::stoi(data.substr(pos3 + 1));
    }
};

// Класс студента
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& name, int id, const std::string& group)
        : User(name, id, 1), group(group) {
        if (group.empty()) {
            throw InvalidInputException("Группа не может быть пустой");
        }
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Группа: " << group << " (Студент)" << std::endl;
    }

    std::string serialize() const override {
        return "Student," + name + "," + std::to_string(id) + "," + std::to_string(accessLevel) + "," + group;
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);

        name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        id = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        accessLevel = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        group = data.substr(pos4 + 1);
    }
};

// Класс преподавателя
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, const std::string& department)
        : User(name, id, 2), department(department) {
        if (department.empty()) {
            throw InvalidInputException("Кафедра не может быть пустой");
        }
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Кафедра: " << department << " (Преподаватель)" << std::endl;
    }

    std::string serialize() const override {
        return "Teacher," + name + "," + std::to_string(id) + "," + std::to_string(accessLevel) + "," + department;
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);

        name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        id = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        accessLevel = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        department = data.substr(pos4 + 1);
    }
};

// Класс администратора
class Administrator : public User {
private:
    std::string position;

public:
    Administrator(const std::string& name, int id, const std::string& position)
        : User(name, id, 3), position(position) {
        if (position.empty()) {
            throw InvalidInputException("Должность не может быть пустой");
        }
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Должность: " << position << " (Администратор)" << std::endl;
    }

    std::string serialize() const override {
        return "Administrator," + name + "," + std::to_string(id) + "," + std::to_string(accessLevel) + "," + position;
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);

        name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        id = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
        accessLevel = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
        position = data.substr(pos4 + 1);
    }
};

// Класс ресурса
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int requiredAccessLevel)
        : name(name), requiredAccessLevel(requiredAccessLevel) {
        if (name.empty()) {
            throw InvalidInputException("Название ресурса не может быть пустым");
        }
        if (requiredAccessLevel < 0 || requiredAccessLevel > 3) {
            throw InvalidInputException("Требуемый уровень доступа должен быть от 0 до 3");
        }
    }

    std::string getName() const { return name; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }

    void displayInfo() const {
        std::cout << "Ресурс: " << name
            << ", Требуемый уровень доступа: " << requiredAccessLevel << std::endl;
    }

    std::string serialize() const {
        return name + "," + std::to_string(requiredAccessLevel);
    }

    void deserialize(const std::string& data) {
        size_t pos = data.find(',');
        name = data.substr(0, pos);
        requiredAccessLevel = std::stoi(data.substr(pos + 1));
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }
};

// Шаблонный класс системы контроля доступа
template<typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    std::vector<T> resources;

public:
    void addUser(std::unique_ptr<User> user) {
        users.push_back(std::move(user));
    }

    void addResource(const T& resource) {
        resources.push_back(resource);
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void displayAllResources() const {
        for (const auto& resource : resources) {
            resource.displayInfo();
        }
    }

    void checkUserAccess(int userId, const std::string& resourceName) const {
        auto userIt = std::find_if(users.begin(), users.end(),
            [userId](const auto& user) { return user->getId() == userId; });

        auto resourceIt = std::find_if(resources.begin(), resources.end(),
            [resourceName](const auto& resource) { return resource.getName() == resourceName; });

        if (userIt == users.end()) {
            throw std::runtime_error("Пользователь с ID " + std::to_string(userId) + " не найден");
        }

        if (resourceIt == resources.end()) {
            throw std::runtime_error("Ресурс с именем " + resourceName + " не найден");
        }

        if (resourceIt->checkAccess(**userIt)) {
            std::cout << "Доступ разрешен: " << (*userIt)->getName()
                << " может использовать " << resourceName << std::endl;
        }
        else {
            throw AccessDeniedException("Доступ запрещен: у " + (*userIt)->getName() +
                " недостаточный уровень доступа к " + resourceName);
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Не удалось открыть файл для записи");
        }

        // Сохраняем пользователей
        file << "[Users]\n";
        for (const auto& user : users) {
            file << user->serialize() << "\n";
        }

        // Сохраняем ресурсы
        file << "[Resources]\n";
        for (const auto& resource : resources) {
            file << resource.serialize() << "\n";
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Не удалось открыть файл для чтения");
        }

        users.clear();
        resources.clear();

        std::string line;
        bool readingUsers = false;
        bool readingResources = false;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            if (line == "[Users]") {
                readingUsers = true;
                readingResources = false;
                continue;
            }
            else if (line == "[Resources]") {
                readingUsers = false;
                readingResources = true;
                continue;
            }

            if (readingUsers) {
                size_t pos = line.find(',');
                std::string type = line.substr(0, pos);

                if (type == "Student") {
                    auto student = std::make_unique<Student>("", 0, "");
                    student->deserialize(line);
                    users.push_back(std::move(student));
                }
                else if (type == "Teacher") {
                    auto teacher = std::make_unique<Teacher>("", 0, "");
                    teacher->deserialize(line);
                    users.push_back(std::move(teacher));
                }
                else if (type == "Administrator") {
                    auto admin = std::make_unique<Administrator>("", 0, "");
                    admin->deserialize(line);
                    users.push_back(std::move(admin));
                }
                else if (type == "User") {
                    auto user = std::make_unique<User>("", 0, 0);
                    user->deserialize(line);
                    users.push_back(std::move(user));
                }
            }
            else if (readingResources) {
                Resource resource("", 0);
                resource.deserialize(line);
                resources.push_back(resource);
            }
        }
    }

    void printDataAsFileFormat() const {
        std::cout << "[Users]\n";
        for (const auto& user : users) {
            std::cout << user->serialize() << "\n";
        }
        std::cout << "[Resources]\n";
        for (const auto& resource : resources) {
            std::cout << resource.serialize() << "\n";
        }
    }
};

int main() {
#ifdef _WIN32
    // Чтобы русские символы корректно отображались в консоли Windows
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    setlocale(LC_ALL, ""); // Локаль системы

    try {
        AccessControlSystem<Resource> acs;

        // Добавляем пользователей
        acs.addUser(std::make_unique<Student>("Иван Иванов", 101, "ИТ-01"));
        acs.addUser(std::make_unique<Teacher>("Елена Петрова", 102, "Математика"));
        acs.addUser(std::make_unique<Administrator>("Сергей Смирнов", 103, "Администратор"));

        // Добавляем ресурсы
        acs.addResource(Resource("Библиотека", 1));
        acs.addResource(Resource("Лаборатория", 2));
        acs.addResource(Resource("Сервер", 3));

        acs.printDataAsFileFormat();

        // Проверка доступа
        acs.checkUserAccess(101, "Библиотека"); // Доступ есть
        acs.checkUserAccess(101, "Сервер");     // Доступа нет, выбросит исключение

    }
    catch (const AccessDeniedException& ex) {
        std::cerr << "Ошибка доступа: " << ex.what() << std::endl;
    }
    catch (const InvalidInputException& ex) {
        std::cerr << "Ошибка ввода: " << ex.what() << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
    }

    return 0;
}
