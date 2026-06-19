#include "professor.h"
#include <string>
#include <iostream>
#include "course.h"


// Εμφάνιση των χαρακτηριστικών του καθηγητή
void Professor::print(std::ostream& os) const
{
    Person::print(os);
    os << " | Ειδικότητα: " << this->getSpecialty();
}

std::ostream& operator<<(std::ostream &os, const Professor &p)
{
    p.print(os);
    return os;
}


// Constructors
Professor::Professor (const char* _AM, std::string _fullname, std::string _gender, std::string _specialty)
                    : Person(_AM,_fullname, _gender),                                specialty(_specialty)
{
}

// Destructor
Professor::~Professor()
{
}

// Getter
std::string Professor::getSpecialty() const
{
    return specialty;
}

// Setter
void Professor::setSpecialty(std::string _specialty)
{
    specialty = _specialty;
}


// Μετατροπή των στοιχείων του καθηγητή σε string
std::string Professor::toString()const
{
    std::string personStr = Person::toString() += ',';
    std::string professorStr = personStr;
    professorStr += (this->getSpecialty()) += ',';;

    for (const Course* coursePtr : this->getCourses())
    {
        professorStr += (coursePtr->getID()) += ',';
    }

    if (!professorStr.empty() && professorStr.back() == ',') {
        professorStr.pop_back(); // Αφαιρεί το τελευταίο κόμμα
    }

    // professorStr += '\n';
    return professorStr;
}