#include "student.h"
#include <string>
#include <cstring>
#include <iostream>
#include "course.h"


std::ostream& operator<<(std::ostream& os, const Student& s)
{
    s.print(os);
    return os;
}

//Constructor με ΑΜ και Ονοματεπώνυμο
Student::Student(const char* _AM, std::string _fullname)
                : Person (_AM, _fullname)
{
    semester = 1;
}

//Constructor με όλα τα χαρακτηριστικά
Student::Student(const char* _AM, std::string _fullname, std::string _gender, unsigned int _semester)
                : Person (_AM, _fullname, _gender), semester(_semester)
{
}

//Copy Constructor

Student::Student(const Student& other):Person(other)
{
    semester = other.semester;
}


//Destructor
Student::~Student()
{
}

//Getters

unsigned int Student::getSemester() const
{
    return semester;
}

//Setters


void Student::setSemester(unsigned int _semester) 
{
    semester = _semester;
}

//Αύξηση εξαμήνου (++a)
Student& Student::operator++() {
    this->semester++;
    return *this;
}

//Αύξηση εξαμήνου (a++)
Student Student::operator++(int) {
    Student temp = *this;
    this->semester++;
    return temp;
}

//Τροποποίηση εξαμήνου με +=
void Student::operator+=(unsigned int val) {
    this->semester += val;
}

//Τροποποίηση εξαμήνου με -=
void Student::operator-=(unsigned int val) {
    this->semester -= val;
}

// Εμφάνιση των χαρακτηριστικών του φοιτητή 
void Student::print(std::ostream &os) const
{
    Person::print(os);
    os <<  " | Εξάμηνο: " << this->getSemester();
}

// Μετατροπή των στοιχείων του φοιτητή σε string
std::string Student::toString()const
{
    std::string personStr = Person::toString() + ',';
    std::string studentStr = personStr;
    studentStr += std::to_string(this->getSemester()) + ',';

    for (const Course* coursePtr : this->getCourses())
    {
        studentStr += (coursePtr->getID()) + ',';
    }

    if (!studentStr.empty() && studentStr.back() == ',') {
        studentStr.pop_back(); // Αφαιρεί το τελευταίο κόμμα
    }

    // studentStr += '\n';
    return studentStr;
}
