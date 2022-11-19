/******************************************************************************
*                              Курс программирования                          *
*******************************************************************************
* Project Type  : Win32 Console Application                                   *
* Project Name  : Курсовая работа по спискам                                  *
* File Name     : List.cpp                                                    *
* Language      : c++20(latest), MSVS ver 143 and above                       *
* Programmers   : Парфенюк Дмитрий Александрович                              *
* Modified by   :                                                             *
* Created       : 01.11.2021                                                  *
* Last Revision : 10.11.2021                                                  *
*******************************************************************************
* Comments	    : Реализация класса "Список"                                  *
******************************************************************************/
#include "List.h"
#include <format>
#include <iomanip>

int List::getLength() {
    // Просто возвращаем значение поля
    return m_length;
}

void List::loadFromFile(std::string& fileName) {
    std::ifstream in(fileName);
    // Пока можем получать строки из файла
    for (std::string line; std::getline(in, line); ) {
        // Десереализуем строки и добавляем студентов в список
        push(getStudentFromString(line));
    }
    in.close();
}

void  List::saveToFile(std::string& fileName) {
    std::ofstream out;
    // Перед записью очищаем файл
    out.open(fileName, std::ofstream::out | std::ofstream::trunc);

    Node* cur = m_head;
    // Пока мы не достигли конца списка
    while (cur != nullptr) {
        // Сериализуем и записываем в файл данные
        out << stringifyStudent(cur->data);
        cur = cur->next;
    }

    out.close();
}

static std::string table_header() {
    std::stringstream ss("");
    ss << std::setfill('=');
    ss << '|';
    ss << std::setw(13) << '|';
    ss << std::setw(13) << '|';
    ss << std::setw(15) << '|';
    ss << std::setw(14) << '|';
    ss << std::setw(10) << '|';
    ss << std::setw(15) << '|';
    ss << std::setw(74) << '|' << '\n';

    ss << '|';
    ss << std::setfill(' ');
    ss << std::setw(13) << "Фамилия" << '|';
    ss << std::setw(6) << "Имя" << '|';
    ss << std::setw(13) << "Отчество" << '|';
    ss << std::setw(11) << "Номер группы" << '|';
    ss << std::setw(9) << "Пол" << '|';
    ss << std::setw(9) << "Дата рождения" << '|';
    ss << std::setw(10) << "Адрес проживания" << '|' << '\n';

    ss << std::setfill('=');
    ss << '|';
    ss << std::setw(13) << '|';
    ss << std::setw(13) << '|';
    ss << std::setw(15) << '|';
    ss << std::setw(14) << '|';
    ss << std::setw(10) << '|';
    ss << std::setw(15) << '|';
    ss << std::setw(74) << '|' << '\n';

    return ss.str();
}

std::string table_footer() {
    std::stringstream ss("");
    ss << std::setfill('=');
    ss << '|';
    ss << std::setw(13) << '|';
    ss << std::setw(13) << '|';
    ss << std::setw(15) << '|';
    ss << std::setw(14) << '|';
    ss << std::setw(10) << '|';
    ss << std::setw(15) << '|';
    ss << std::setw(74) << '|' << '\n';

    return ss.str();
}

std::string table_elem(Student* data) {
    std::stringstream ss("");
    ss << std::setfill(' ');

    ss << std::setw(12) << data->surname << '|';
    ss << std::setw(12) << data->firstName << '|';
    ss << std::setw(14) << data->fatherName << '|';
    ss << std::setw(13) << data->groupNum << '|';
    ss << std::setw(9) << data->sex << "|  ";
    ss << data->birthDate.day;
    ss << '.' << data->birthDate.month;
    ss << '.' << data->birthDate.year << "  |";
    ss << std::setw(6) << data->address.index << ", r. ";
    ss << std::setw(14) << data->address.city << " ул. ";
    ss << std::setw(26) << data->address.street << " дом ";
    ss << std::setw(3) << data->address.building << " кв. ";
    ss << std::setw(4) << data->address.apartment << '|' << std::endl;
    return ss.str();
}

void List::consolePrint() {
    auto cur = m_head;
    // Шапка таблицы
    std::cout << table_header();
    // Пока не достигли конца списка
    while (cur != nullptr) {
        // Выводим форматированную информацию
        std::cout << table_elem(cur->data);
        cur = cur->next;
    }
    // Конец таблицы
    std::cout << table_footer();
}

int List::searchIndex(dataType* data) {
    // Поиск реализован в виде стандартного линейного поиска
    int found = -1;
    auto current = m_head;
    int i = 0;
    while (current != nullptr) {
        if ((*(current->data)) == *data) {
            found = i;
            break;
        }
        i++;
        current = current->next;
    }
    return found;
}

Node* List::getNodeAt(int index) {
    auto curNode = m_head;
    // Index раз переходим к следующему элементу после m_head
    for (int i = 0; i < index; i++) {
        // Если не смогли сделать переход - возможная ошибка - возвращаем пустой указатель
        if (curNode->next == nullptr) {
            return nullptr;
        }
        curNode = curNode->next;
    }
    return curNode;
}

List::~List() {
    // Удаление каждого элемента списка и информации внутри него
    while (m_head != nullptr) {
        auto next = m_head->next;
        delete m_head->data;
        delete m_head;
        m_head = next;
    }
}

dataType* List::getAt(int index) {
    // Получаем элемент списка
    auto node = getNodeAt(index);
    // Возвращаем nullptr если не удалось получить элемент
    if (node == nullptr) {
        return nullptr;
    }
    // Иначе возвращаем информацию в элементе
    return node->data;
}

List* List::copy() {
    // Копирование реализовано за счет свойств метода filter
    return this->filter([](auto a) {return true; });
}

void List::push(dataType* entry) {
    // Стандартное добавление в список
    Node* newHead(new Node);
    newHead->next = m_head;
    newHead->data = entry;
    m_head = newHead;
    m_length++;
}

void List::appendAfter(dataType* entry, int index) {
    // Создание элемента списка
    Node* newNode(new Node);
    newNode->data = entry;
    // Получаем элемент после которого нужно осуществить вставку
    auto curNode = getNodeAt(index);
    // Если не удалось получить элемент
    if (curNode == nullptr) {
        // При отсутствии элементов не считаем ошибкой
        if (m_length == 0) {
            return;
        }
        // Иначе критическая ошибка
        std::cerr << std::format("invalid operation: tried to append after {}, but the length is {}!\n", index, m_length);
        exit(1);
    }
    // Now curnode is list[index]
    // Вставка элемента
    auto nextNode = curNode->next;
    curNode->next = newNode;
    newNode->next = nextNode;
    m_length++;
}

dataType* List::pop() {
    // Если элементов не было ничего не возвращаем пустой указатель
    if (m_head == nullptr) {
        return nullptr;
    }
    // Иначе выдаем информацию первого элемента списка
    Node* node = m_head;
    m_head = m_head->next;
    dataType* data = node->data;
    // И удаляем элемент
    delete node;
    m_length--;

    return data;
}

void List::removeAt(int index) {
    // Получаем элемент перед удаляемым
    auto curNode = getNodeAt(index - 1);
    // Если получили первый элемент
    // (При пустом списке код идет по этому пути, т.к. m_head == nullptr)
    if (curNode == m_head) {
        // То у нас уже есть инструменты для работы с ним
        auto a = pop();
        delete a;
        return;
    }

    // Если хотим удалить пустой указатель - критическая ошибка
    if (curNode->next == nullptr) {
        std::cerr << std::format("invalid operation: tried to remove at index {}, but the length is {}!\n", index, m_length);
        exit(1);
    }
    // Now curnode is list[index - 1]
    // Извлекаем элементы до и после удаляемого
    auto toRemove = curNode->next;
    auto afterRemove = toRemove->next;
    // Удаляем элемент и его информацию
    dataType* data = toRemove->data;
    delete toRemove;
    delete data;
    // Соединяем оставшиеся элементы
    curNode->next = afterRemove;
    m_length--;
    return;
}

List& List::sort(SortComparator comp) {
    // Стандартная сортировка пузырьком
    // Сортируются указатели на информацию для избежания лишних вставок
    for (int i = 0; i < m_length - 1; i++) {
        // Используются for и getNodeAt вместо while для большего контроля над кодом, хотя это повышает сложность алгоритма
        auto toSort = getNodeAt(i);
        auto cur = toSort->next;
        // Цикл по оставшимся элементам после toSort
        while (cur != nullptr) {
            // Если cur->data и toSort->data удовлетворяют предикату
            if (comp(cur->data, toSort->data)) {
                // То меняем их местами
                std::swap(cur->data, toSort->data);
            }
            cur = cur->next;
        }
    }
    // Возвращает самого себя для возможной сцепки нескольких функций
    return *this;
}

List* List::filter(Predicate pred) {
    // Создаем новый список для заполнения
    List* filtered(new List);
    auto cur = m_head;
    // Если список пуст, возвращается пустой список
    if (m_head == nullptr) {
        return filtered;
    }
    while (cur != nullptr) {
        // Если информация элемента списка удовлетворяет предикату
        if (pred(cur->data)) {
            // Добавляем ее копию в новый список
            filtered->push(new dataType(*(cur->data)));
        }
        cur = cur->next;
    }
    // Возвращаем отфильтрованый список (связей с исходным нет)
    return filtered;
}
/******************************************************************************
*                       End of file List.cpp                                  *
******************************************************************************/