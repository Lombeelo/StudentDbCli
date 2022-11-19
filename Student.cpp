/******************************************************************************
*                              ���� ����������������                          *
*******************************************************************************
* Project Type  : Win32 Console Application                                   *
* Project Name  : �������� ������ �� �������                                  *
* File Name     : Student.cpp                                                 *
* Language      : c++20(latest), MSVS ver 143 and above                       *
* Programmers   : �������� ������� �������������                              *
* Modified by   :                                                             *
* Created       : 01.11.2021                                                  *
* Last Revision : 10.11.2021                                                  *
*******************************************************************************
* Comments	    : ������� ��� ������ �� ����������                            *
******************************************************************************/
#include "Student.h"

Student* getStudentFromString(std::string& str) {
    // ��������� ����� ������������ ��� �������� ���������� ����������
    std::stringstream in(str);
    // ������� ��������� ����� new �.�. ������ ����������� �������� ����� delete
    Student* student = new Student();
    // �������������� ������
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
    // ���������� ��������� �� ���������� ��������
    return student;
}

std::string stringifyStudent(Student* student, std::string format) {
    std::stringstream out;
    // Vformat - ����� ������� c++20. ������� ����� ������ �� ������ ������� format �
    // ����������� ������ ����������, ������� ������������ �������������� � ������ ������� �� ������������ ��������
    out << std::vformat(format,
          std::make_format_args(student->surname, student->firstName, student->fatherName,
                                student->groupNum, student->sex,
                                student->birthDate.day, student->birthDate.month, student->birthDate.year,
                                student->address.index, student->address.city, student->address.street,
                                student->address.building, student->address.apartment));
    // ���������� ��������������� ������
    return out.str();
}
/******************************************************************************
*                      End of file Student.cpp                                *
******************************************************************************/