/******************************************************************************
*                              Курс программирования                          *
*******************************************************************************
* Project Type  : Win32 Console Application                                   *
* Project Name  : Курсовая работа по спискам                                  *
* File Name     : Student.h                                                   *
* Language      : c++20(latest), MSVS ver 143 and above                       *
* Programmers   : Парфенюк Дмитрий Александрович                              *
* Modified by   :                                                             *
* Created       : 01.11.2021                                                  *
* Last Revision : 10.11.2021                                                  *
*******************************************************************************
* Comments	    : Заголовок структуры "Студент"                               *
******************************************************************************/
#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <format>

struct Date {
    int year{0};
    int month{0};
    int day{0};
};

struct Address {
    int index{0};
    std::string city;
    std::string street;
    int building{0};
    int apartment{0};
};

struct Student {
    std::string firstName; // Имя
    std::string surname; // Фамилия
    std::string fatherName; // Отчество

    int groupNum{0}; // Номер группы

    std::string sex; // ПОЛ
    Date birthDate; // Дата рождения
    Address address; // Адрес рождения

    //Оператор равенства перегружен для удобства поиска
    bool operator==(Student& b) {
        return (firstName.compare(b.firstName) == 0) &&
            (surname.compare(b.surname) == 0) &&
            (fatherName.compare(b.fatherName) == 0) &&
            (groupNum == b.groupNum) &&
            (sex.compare(b.sex) == 0) &&
            (birthDate.day == b.birthDate.day) &&
            (birthDate.month == b.birthDate.month) &&
            (birthDate.year == b.birthDate.year) &&
            (address.apartment == b.address.apartment) &&
            (address.building == b.address.building) &&
            (address.city == b.address.city) &&
            (address.index == b.address.index) &&
            (address.street == b.address.street);
    }
};
/*
Сериализация информации о студенте в символьную строку
Последовательность информации:
Фамилия, Имя, Отчество, номер группы,
пол, день рождения, месяц, год, индекс, город, улица,
дом, квартира.
*/
std::string stringifyStudent(Student* student, std::string format = "{} {} {} {} {} {} {} {} {} {} {} {} {}\n");

// Десериализация информации о студенте из строки.
Student* getStudentFromString(std::string&);
/******************************************************************************
*                       End of file Student.h                                 *
******************************************************************************/