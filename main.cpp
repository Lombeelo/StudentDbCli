/******************************************************************************
*                              Курс программирования                          *
*******************************************************************************
* Project Type  : Win32 Console Application                                   *
* Project Name  : Курсовая работа по спискам                                  *
* File Name     : Kurs.cpp                                                    *
* Language      : c++20(latest), MSVS ver 143 and above                       *
* Programmers   : Парфенюк Дмитрий Александрович                              *
* Modified by   :                                                             *
* Created       : 01.11.2021                                                  *
* Last Revision : 22.11.2021                                                  *
*******************************************************************************
* Comments	    : Главный файл курсовой работы - база данных студентов,       *
*                 основанная на списках                                       *
******************************************************************************/
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <map>
#include <chrono>
#include <ctime>
#include <functional>
#include <thread>
#include <any>
#include "List.h"
#include "Student.h"

struct State {
    std::string input{""};
    std::string out{""};
    List* list = nullptr;
    Student* selection = nullptr;
};

// Предикат - булева функция одного аргумента.
// Используется функтор вместо указателя на функцию для возможности осуществления лямбда-захватов
using Predicate = std::function<bool(Student*)>;
// Компаратор - булева функция двух аргументов.
// Используется функтор вместо указателя на функцию для возможности осуществления лямбда-захватов
using SortComparator = std::function<bool(dataType*, dataType*)>;
// Функция операции - принимает текущее состояние программы в качестве аргумента
using Operation = std::function<void(std::stringstream&, State&)>;
// Таблица операций
using OperationsMap = std::map<std::string, Operation>;
// Шаблон локальных компараторов
template <typename T>
using Comparator = std::function<bool(T const&, T const&)>;
// Шаблон геттеров
template <typename T>
using FunctionForCompare = std::function<T(Student*)>;
// Шаблон таблицы атрибутов
template <typename T>
using AttrMap = std::map<std::string, FunctionForCompare<T>>;
// Шаблон таблицы локальных компараторов
template <typename T>
using CompMap = std::map<std::string, Comparator<T>>;

// Различные функции печати
void printStudentInfo(Student* stud) {
    if (stud == nullptr) {
        std::cout << "Не выбран студент для вывода информации" << std::endl;
    } else {
        std::cout << stringifyStudent(stud, "ИНФОРМАЦИЯ О ВЫБРАННОМ СТУДЕНТЕ:\n"
                                      "Фамилия:{}\nИмя:{}\nОтчество:{}\nНомер группы:{}\nПол:{}\n"
                                      "Дата рождения{:02d}.{:02d}.{}\nАддрес проживания:\n\t{}, гор. {}, ул. {}, дом {} кв.{}\n");
    }
}
void printWelcome() {
    std::cout << "Добро пожаловать в Программу редактирования базы данных студентов! Используйте \"help\" для просмотра списка комманд\n";
}
void printExit() {
    std::cout << "Большое спасибо за пользование этой программой" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Функции вывода текстов ошибок
void invalidArgumentException() {
    std::cout << "Ошибка распознавания аргумента: один или несколько аргументов не были распознаны. Используйте \"help <название аргумента>\" для вывода списка возможных аргументов.\n";
}
void noArgumentException() {
    std::cout << "Ошибка аргумента: предоставлено недостаточно аргументов. Используйте \"help\" для просмотра списка аргументов комманд.\n";
}
void operationException() {
    std::cout << "Ошибка распознавания комманды: комманда не распознана. Используйте \"help\" для просмотра списка доступных комманд.\n";
}
void notFoundException() {
    std::cout << "По указанному параметру не найдено ни одного студента.\n";
}
void invalidRemoveException() {
    std::cout << "Ошибка удаления: не выбран студент и не был получен индекс для удаления." << std::endl;
}
void noSelectionException() {
    std::cout << "Ошибка: не выбран студент." << std::endl;
}
void noFileOpenedException() {
    std::cout << "Ошибка: нет открытого файла." << std::endl;
}

//Таблица геттеров, возвращающих ссылки на числа
AttrMap<int&> intFuncs{
  {"year", [](Student* a)-> int& {return a->birthDate.year; }},
  {"month", [](Student* a)-> int& {return a->birthDate.month; }},
  {"day", [](Student* a)-> int& {return a->birthDate.day; }},
  {"index",  [](Student* a)-> int& {return a->address.index; }},
  {"building", [](Student* a)-> int& {return a->address.building; }},
  {"apartment", [](Student* a)-> int& {return a->address.apartment; }},
  {"group", [](Student* a)-> int& { return a->groupNum; }},
};

//Таблица геттеров, возвращающих ссылки на строки
AttrMap<std::string&> strFuncs{
  {"firstname",[](Student* a) ->std::string& {return a->firstName;  }},
  {"surname",[](Student* a)->std::string& {return a->surname; }},
  {"fathername",[](Student* a)->std::string& {return a->fatherName; }},
  {"city",[](Student* a)->std::string& {return  a->address.city; }},
  {"street",[](Student* a)->std::string& {return a->address.street; }},
  {"sex",[](Student* a)->std::string& {return a->sex; }},
};

//Таблица компараторов
template <typename T>
CompMap<T> comps{
  {"asc",[](T const& a, T const& b) { return a < b; }},
  {"desc",[](T const& a, T const& b) { return a > b; }},
  {"eq",[](T const& a, T const& b) { return a == b; }},
  {"<",[](T const& a, T const& b) { return a < b; }},
  {">",[](T const& a, T const& b) { return a > b; }},
  {"=",[](T const& a, T const& b) { return a == b; }},
};

//Получение "слова" из строкового потока
std::string getOperation(std::stringstream& ss) {
    std::string operation;
    operation.clear();
    ss >> operation;
    return operation;
}

// Создание соответствующего геттера
template <typename T>
auto getFunc(std::string& attribute, AttrMap<T> funcs) {
    FunctionForCompare<T> intFunc = nullptr;
    // Если геттер есть в полученной таблице
    if (funcs.contains(attribute)) {
        // Возвращаем его
        intFunc = funcs[attribute];
    }
    return intFunc;
}

// Создание компаратора
template <typename T>
Comparator<T> getComparator(std::string& comp) {
    Comparator<T> comparator = nullptr;
    // Если в таблице есть этот компаратор
    if (comps<T>.contains(comp)) {
        //То его и возвращаем
        comparator = comps<T>[comp];
    }
    return comparator;
}

// Получение числа из строки
int getIntValue(std::string& value) {
    std::stringstream ss(value);
    int num = -1;
    // Если конвертация не удалась, или число меньше нуля - ошибка
    if (!(ss >> num) || num < 0) {
        num = -1;
    }
    return num;
}

// Сборка предиката для фильтрации
Predicate getPredicate(std::string& attribute, std::string& comp, std::string& value) {
    // Предполагаем -1 как невозможное значение
    int intValue = -1;
    // Пытаемся получить целочисленную функцию-геттер и соответствующий ему компаратор
    auto intFunc = getFunc(attribute, intFuncs);
    auto comparator = getComparator<int>(comp);
    // Пытаемся конвертировать строку в число
    intValue = getIntValue(value);

    // Иначе собираем предикат используя целочисленный геттер, целочисленный компаратор и полученное число для сравнения
    if (intValue != -1 && intFunc && comparator) {
        return [intValue, intFunc, comparator](Student* a)-> bool {return comparator(intFunc(a), intValue); };
    }

    // Если геттер или компаратор для целочисленного поля не удалось получить или не удалась конвертация в число
    // Пытаемся получить строковую функцию-геттер и соответствующий ему компаратор
    auto strFunc = getFunc(attribute, strFuncs);
    auto strComparator = getComparator<std::string>(comp);
    // Иначе собираем предикат используя строковый геттер, строковый компаратор и значение для сравнения
    if (strFunc && strComparator) {
        // Возвращаем итоговый предикат
        return [value, strFunc, strComparator](Student* a)-> bool {return strComparator(strFunc(a), value); };
    }
    // Если геттер или компаратор для строкового поля не удалось получить
    // Не удалось собрать предикат - возвращаем ничего
    return nullptr;
}

// Сборка компаратора для сортировки
SortComparator getSortComparator(std::string& attribute, std::string& comp) {
    // Пытаемся получить целочисленную функцию-геттер и соответствующий ему компаратор
    auto intFunc = getFunc(attribute, intFuncs);
    auto comparator = getComparator<int>(comp);

    if (intFunc && comparator) {
        // Иначе собираем компаратор используя целочисленный геттер и целочисленный компаратор
        return [intFunc, comparator](Student* a, Student* b)->bool {return comparator(intFunc(a), intFunc(b)); };
    }

    // Если геттер или компаратор для целочисленного поля не удалось получить
    // Пытаемся получить строковую функцию-геттер и соответствующий ему компаратор
    auto strFunc = getFunc(attribute, strFuncs);
    auto strComparator = getComparator<std::string>(comp);
    // Если геттер или компаратор не удалось получить
    if (strFunc && strComparator) {
        // Иначе собираем компаратор используя строковый геттер и строковый компаратор
        return [strFunc, strComparator](Student* a, Student* b)->bool {return strComparator(strFunc(a), strFunc(b)); };
    }

    // Не удалось собрать компаратор - возвращаем ничего
    return nullptr;
}

// Реализация операций
// Операция открытия файла
void op_open(std::stringstream& ss, State& state) {
    // Получаем аргумент
    std::string filename = getOperation(ss);
    // Если его нет - ошибка
    if (filename.empty()) {
        noArgumentException();
        return;
    }

    // Записываем открываемый файл
    state.input = filename;
    // По умолчанию сохраняем в тот же файл
    state.out = state.input;
    // Очищаем список и выделение, если до этого был открыт файл
    if (state.list != nullptr) {
        delete state.list;
        state.selection = nullptr;
    }
    // Создание нового списка и загрузка данных из файла
    state.list = new List();
    state.list->loadFromFile(state.input);
}

// Операция сохранения
void op_save(std::stringstream& ss, State& state) {
    // Получаем аргумент
    std::string filename = getOperation(ss);
    // Если это удалось, то переназначаем файл вывода
    if (!filename.empty()) {
        state.out = filename;
    }
    // Если не было открыто файла - ошибка
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }
    // Сохраняем список
    state.list->saveToFile(state.out);
}

// Операция печати
void op_print(std::stringstream& ss, State& state) {
    std::string operation = getOperation(ss);
    // Если списка нет - ошибка
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }
    // Не создавался список печатей т.к. только один возможный аргумент
    if (!operation.compare("student")) {
        // Печатаем выбранного студента, если есть нужный аргумент
        printStudentInfo(state.selection);
    } else {
        // Иначе печатаем весь список
        state.list->consolePrint();
    }
}

// Операция фильтрации списка
void op_filter(std::stringstream& ss, State& state) {
    // Ошибка, если не открыт файл
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }

    // Получаем аргументы
    std::string attribute = getOperation(ss);
    std::string order = getOperation(ss);
    std::string value = getOperation(ss);
    // Ошибка, если их не достаточно
    if (attribute.empty() || order.empty() || value.empty()) {
        noArgumentException();
        return;
    }

    // Пытаемся получить предикат (А.Аttribute 'order' value)
    // order - операция сравнения
    auto intFunc = getPredicate(attribute, order, value);
    // Если не получилось - ошибка
    if (intFunc == nullptr) {
        invalidArgumentException();
        return;
    }
    // Создаем новый отфильтрованный список
    auto newList = state.list->filter(intFunc);
    // удаляем прошлый список и очищаем выделение
    delete state.list;
    state.selection = nullptr;
    state.list = newList;
}

// Операция сортировки списка
void op_sort(std::stringstream& ss, State& state) {
    // Если списка нет, то сортировать нечего
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }

    // Получаем аргументы
    std::string attribute = getOperation(ss);
    std::string order = getOperation(ss);
    if (attribute.empty() || order.empty()) {
        noArgumentException();
        return;
    }

    // Пытаемся получить компаратор типа (А.attribute 'order' В.attribute), где order - операция сравнения
    auto comp = getSortComparator(attribute, order);
    // Ошибка если не удалось получить компаратор - скорее всего неверный аргумент
    if (comp == nullptr) {
        invalidArgumentException();
        return;
    }
    // Сортируем список с помощью компаратора
    state.list->sort(comp);
}

template <typename T>
bool convertTo(const std::string& str, T& out) {
    std::istringstream ss(str);
    ss >> out;
    return (bool)ss;
}

template <class T>
void readUntilread(std::stringstream& ss, const char* message, std::function<bool(T)> predicate = nullptr) {
    std::string input;
    T result{};

    if (predicate == nullptr) {
        predicate = [](T) { return true; };
    }

    do {
        std::cout << message;
        std::getline(std::cin, input);
    } while (!(convertTo(input, result) && predicate(result)));
    ss << result << ' ';
}

// Функция для обеспечения пользовательского ввода данных о студенте
Student* createStudent() {
    std::cout << "Вы в меню создания новой записи. Будьте добры ввести все необходимые данные" << std::endl;
    std::stringstream ss("");
    std::string input{};
    int intInput{};
    // Далее все по одной схеме - попытка ввода очередного поля, пока она не увенчается успехом
    readUntilread<std::string>(ss, "Введите фамилию студента: ");
    readUntilread<std::string>(ss, "Введите Имя студента: ");
    readUntilread<std::string>(ss, "Введите Отчество студента: ");
    readUntilread<int>(ss, "Введите номер группы студента (число): ");
    readUntilread<std::string>(ss, "Введите пол студента: ");
    readUntilread<int>(ss, "Введите день рождения студента (от 1 до 31): ", [](int a) {return a >= 1 && a <= 31; });
    readUntilread<int>(ss, "Введите месяц рождения студента (от 1 до 12): ", [](int a) {return a >= 1 && a <= 12; });
    readUntilread<int>(ss, "Введите год рождения студента (от 1900 до 2020): ", [](int a) {return a >= 1900 && a <= 2002; });
    readUntilread<int>(ss, "Введите индекс студента (число): ");
    readUntilread<std::string>(ss, "Введите город студента: ");
    readUntilread<std::string>(ss, "Введите улицу студента: ");
    readUntilread<int>(ss, "Введите дом студента (число): ");
    readUntilread<int>(ss, "Введите квартиру студента (число): ");
    ss << std::endl;
    auto stringa = ss.str();

    return getStudentFromString(stringa);
}

// Операция добавления нового элемента в список
void op_add(std::stringstream& ss, State& state) {
    // Если списка нет - ошибка
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }
    // Создаем студента с помощью подпрограммы
    state.selection = createStudent();
    state.list->push(state.selection);
}

// Операция поиска по одному из полей
void op_find(std::stringstream& ss, State& state) {
    // Если ничего не считывалось, ошибка
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }

    // Считываем оставшиеся аргументы
    std::string attribute = getOperation(ss);
    std::string order = "=";
    std::string value = getOperation(ss);

    // Ошибка, если не достаточно аргументов
    if (attribute.empty() || value.empty()) {
        noArgumentException();
        return;
    }
    // Пытаемся получить предикат (Атрибут == значению)
    auto pred = getPredicate(attribute, order, value);

    // Ошибка, если этого не удалось сделать
    if (pred == nullptr) {
        invalidArgumentException();
        return;
    }

    // Иначе фильтруем список по полученному предикату
    List* findings = state.list->filter(pred);
    // Первый элемент этого списка - ключ поиска
    auto find = findings->pop();
    // Находим этот же элемент в изначальном списке и выбираем его
    state.selection = state.list->getAt(state.list->searchIndex(find));
    // Вспомогательный список больше не нужен
    delete findings;
}

// Операция изменения выбранного студента
void op_edit(std::stringstream& ss, State& state) {
    // Если ничего не выбрано, то ошибка
    if (state.selection == nullptr) {
        noSelectionException();
        return;
    }

    // Получаем аргументы комманды
    std::string attr = getOperation(ss);
    std::string value = getOperation(ss);
    int intValue = -1;

    // Если получили недостаточно аргументов, то ошибка
    if (attr.empty() || value.empty()) {
        noArgumentException();
        return;
    }

    // Пытаемся получить функцию-геттер строкового аргумента
    auto strFunc = getFunc(attr, strFuncs);
    // Если получилось, то ,используя его, меняем поле
    if (strFunc != nullptr) {
        strFunc(state.selection) = value;
    } else {
        // Иначе пытаемся получить функцию-геттер целочисленного аргумента
        auto intFunc = getFunc(attr, intFuncs);
        int intValue = getIntValue(value);
        // Если получилось, меняем поле
        if (intFunc != nullptr && intValue >= 0) {
            intFunc(state.selection) = intValue;
        } else {
            // Иначе неправильные аргументы
            invalidArgumentException();
            return;
        }
    }
}

// Операция удаления элемента из списка
void op_remove(std::stringstream& ss, State& state) {
    // Ошибка если нет файла
    if (state.input.empty()) {
        noFileOpenedException();
        return;
    }
    int toRemove = -1;
    // Если есть выбранный элемент, то удаляем его
    if (state.selection != nullptr) {
        toRemove = state.list->searchIndex(state.selection);
    }
    // Получаем индекс для удаления и, если не получилось и нет выделения, то выводим ошибку
    if (!(ss >> toRemove) && toRemove == -1) {
        invalidRemoveException();
        return;
    }
    // Удаляем вычисленный элемент
    state.list->removeAt(toRemove);
    // Очищаем выделение
    state.selection = nullptr;
}

#pragma region "help" helpers
// Вывод помощи
void printHelp() {
    std::cout << "Помощь по работе с базой данных:\n"
        "open <filename>: открывает файл filename в качестве базы данных. пример: open input.txt\n"
        "save {filename}: сохраняет файл в файл, который открывался, последнее место сохранения, или новое место сохранения (если указано в качестве аргумента). Пример 1: save Пример 2: save out.txt\n"
        "print: Вывод БД в текущем состоянии в консоль.\n"
        "print student: вывод в консоль информации о текущем студенте\n"
        "filter <attr> <order> <value>: Фильтрует БД по условию (Меняет открытую БД). Пример: filter apartment < 500 - в БД останутся только студенты с номером квартиры меньше 500\n"
        "sort <attr> <order>: сортирует БД по выбранным атрибуту и порядку (Меняет открытую БД). Пример: sort apartment > - отсортирует студентов по возрастанию номера квартиры\n"
        "add: Переход в меню добавления студента\n"
        "find <attr> <value>: Находит первый элемент, в котором attr = value и \"выбирает\" его для возможных последующих изменений. Пример: find firstname Анна\n"
        "edit <attr> <value>: Меняет значение attr выбранного студента на value. Пример: edit firstname Виталий\n"
        "remove {index}: удаляет или выбранного студента или, если указано, студента и индексом \"index\". Пример: remove 3\n"
        "help: Вывод этого окна\n"
        "help attr: Вывод всех доступных атрибутов\n";
}
// Вывод помощи по атрибутам
void printHelpAttr() {
    std::cout << "Доступные атрибуты для всех функций:\n"
        "year: год рождения\n"
        "month: месяц рождения\n"
        "day: день рождения\n"
        "index: почтовый индекс места рождения\n"
        "building: номер дома места рождения\n"
        "apartment: номер квартиры места рождения\n"
        "sex: пол студента\n"
        "Дополнительные атрибуты для поиска, сортировки и изменения:\n"
        "firstname: Имя\n"
        "surname: Фамилия\n"
        "fathername: Отчество\n"
        "city: город рождения\n"
        "street: улица места рождения\n";
}
// Вывод помощи по порядкам сортировки и фильрации
void printHelpOrder() {
    std::cout << "Доступные порядки сортировки/фильтрации:\n"
        "\"asc\": по возрастанию (предыдущий элемент меньше последующего)\n"
        "\"desc\": по убыванию (предыдущий элемент больше следующего)\n"
        "\"eq\": равенством (элементы равны заданному (не рекомендуется))\n";
}

using HelpMap = std::map<std::string, std::function<void()>>;
// Таблица функций помощи
HelpMap helpFuncs{
  {"attr", printHelpAttr},
  {"order", printHelpOrder},
};
#pragma endregion

// Вывод помощи
void op_help(std::stringstream& ss, State& state) {
    std::string operation = getOperation(ss);
    // Если такая разновидность помощи зарегестрирована, то выводим ее
    if (helpFuncs.contains(operation)) {
        helpFuncs[operation]();
        return;
    }
    // Иначе выводим стандартную помощь
    printHelp();
}

// Выход из программы
void op_exit(std::stringstream& ss, State& state) {
    printExit();
    // Очистка памяти
    delete state.list;
    exit(0);
}

// Таблица зарегистрированных комманд и их функций-обработчиков
OperationsMap operations{
  {"open",  op_open},
  {"save", op_save},
  {"print", op_print},
  {"filter", op_filter},
  {"sort", op_sort},
  {"add", op_add},
  {"find", op_find},
  {"edit", op_edit},
  {"remove", op_remove},
  {"help", op_help},
  {"exit", op_exit},
};

// Функция обработки комманды - меняет состояние программы исходя из содержимого потока ss
void processList(std::stringstream& ss, State& state) {
    // Получаем одно "слово" из комманды
    std::string operation = getOperation(ss);

    // Если такая комманда зарегистрирована в таблице
    if (operations.contains(operation)) {
        // То мы ее выполняем (Меняем с ее помощью состояние программы)
        operations[operation](ss, state);
    } else {
        // Иначе мы не распознали комманду
        std::cout << "Комманда не распознана: используйте комманду \"help\" для получения справки" << std::endl;
    }
}

int main() {
    //setlocale(0, "Russian");

    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    // Задаем исходное состояние программы
    State state{};

    // Буфер введеной комманды
    std::string command;
    printWelcome();
    while (true) {
        std::cout << "DBMANAGE> ";

        // Получение пользовательского ввода и его упаковка в поток
        std::getline(std::cin, command);
        std::stringstream ss(command);

        // Передаем состояние программы и поток с коммандой функции обработки
        processList(ss, state);
    }
}

/******************************************************************************
*                       End of file Kurs.cpp                                  *
******************************************************************************/