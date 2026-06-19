#include "course.h"
#include "professor.h"
#include <string>
#include "registry.h"
#include <iostream>

std::ostream& operator<<(std::ostream &os, const Course &c)
{
    c.print(os);
    return os;
}


Course::Course(std::string _ID, std::string _description, unsigned int _semester)
            :          ID(_ID), description(_description),    semester(_semester), mainProfessor(nullptr)
{

}

Course::Course(std::string _ID, std::string _description, unsigned int _semester, Professor * _mainProfessor)
            :          ID(_ID), description(_description),    semester(_semester), mainProfessor(_mainProfessor)
    
{
}

Course::~Course()
{
    // Δεν είμαστε ιδιοκτήτες του mainProfessor.
    // Ο δείκτης απλώς δείχνει σε έναν Professor
    // που διαχειρίζεται άλλο μέρος του προγράμματος.
}

std::string Course::getID() const
{
    return ID;
}

std::string Course::getDescription() const
{
    return description;
}

unsigned int Course::getSemester() const
{
    return semester;
}

Professor* Course::getMainProfessor() const
{
    return mainProfessor;
}

void Course::setID(std::string _ID)
{
    ID = _ID;
}

void Course::setDescription(std::string _description)
{
    description = _description;
}

void Course::setSemester(unsigned int _semester)
{
    semester = _semester;
}

void Course::setMainProfessor(Professor *_mainProfessor)
{
    mainProfessor = _mainProfessor;
}


void Course::print(std::ostream &os) const
{
    os << "Κωδικός: " << this->getID()
       << " | Περιγραφή: " << this->getDescription()
       << " | Εξάμηνο διδασκαλίας: " << this->getSemester()
       << " | Υπεύθυνος μαθήματος: ";

    if (this->getMainProfessor() != nullptr)
    {
        os << this->getMainProfessor()->getFullname();
    }
    else
    {
        os << "Κανένας";
    }
}

// Μετατροπή των στοιχείων του μαθήματος σε string
std::string Course::toString() const
{
    std::string courseStr;
    courseStr += this->getID() + ',';
    courseStr += this->getDescription() + ',';
    courseStr += std::to_string(this->getSemester()) + ',';
    Professor *mainProfessor = this->getMainProfessor();
    if (mainProfessor != nullptr)
    {
        courseStr += std::string(mainProfessor->getAM());
    }
    else
    {
        courseStr += "Κανένας";
    }
    // courseStr += '\n';
    return courseStr;
}