/******************************************************************************
*                              Курс программирования                          *
*******************************************************************************
* Project Type  : Win32 Console Application                                   *
* Project Name  : Курсовая работа по спискам                                  *
* File Name     : Student.cpp                                                 *
* Language      : c++20(latest), MSVS ver 143 and above                       *
* Programmers   : Парфенюк Дмитрий Александрович                              *
* Modified by   :                                                             *
* Created       : 01.11.2021                                                  *
* Last Revision : 10.11.2021                                                  *
*******************************************************************************
* Comments	    : Функции для работы со студентами                            *
******************************************************************************/
#include "Student.h"

Student* getStudentFromString(std::string& str) {
    // Строковый поток используется для удобства разделения информации
    std::stringstream in(str);
    // Студент создается через new т.к. список гарантирует удаление через delete
    Student* student = new Student();
    // Десереализация строки
    in >> student->surname;
    in >> student->firstName;
    in >> student->fatherName;
    in >> student->groupNum;
    in >> student->sex;
    in >> student->birthDate.day;
    in >> student->birthDate.month;
    in >> student->birthDate.year;
    in >> student->address.index;
    in >> student->address.city;
    in >> student->address.street;
    in >> student->address.building;
    in >> student->address.apartment;
    // Возвращаем указатель на созданного студента
    return student;
}

std::string stringifyStudent(Student* student, std::string format) {
    std::stringstream out;
    // Vformat - новая функция c++20. Создает стоку исходя из строки формата format и
    // формального списка аргументов, которые впоследствии подставлсяются в строку формата по определенным правилам
    out << std::vformat(format,
          std::make_format_args(student->surname, student->firstName, student->fatherName,
                                student->groupNum, student->sex,
                                student->birthDate.day, student->birthDate.month, student->birthDate.year,
                                student->address.index, student->address.city, student->address.street,
                                student->address.building, student->address.apartment));
    // Возвращаем сгенерированную строку
    return out.str();
}
/******************************************************************************
*                      End of file Student.cpp                                *
******************************************************************************/