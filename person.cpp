#include "person.h"
#include "student.h"
#include "professor.h"
#include <cstring>
#include <iostream>
#include "course.h"
#include <list>
#include "myException.h"

 
std::ostream& operator<<(std::ostream& os, const Person& p) {
    p.print(os);
    return os;
}

// Constructors

Person::Person(const char* _AM, std::string _fullname)
              :                    fullname(_fullname)
{
    AM = new char[strlen(_AM) + 1];
    strcpy (AM, _AM);
    gender = "Άγνωστο";
}

Person::Person(const char* _AM, std::string _fullname, std::string _gender)
              :                    fullname(_fullname),     gender(_gender)
{
    AM = new char[strlen(_AM) + 1];
    strcpy (AM, _AM);
}


// Copy constructor
Person::Person(const Person &other)
{
    AM = new char[strlen(other.AM)];
    strcpy (AM, other.AM);
    fullname = other.fullname;
    gender = other.gender;
}

// Destructor
Person::~Person()
{
    delete [] AM;
}

// Getters 

const char* Person::getAM() const 
{
    return AM;
}

std::string Person::getFullname() const
{
    return fullname;
}

std::string Person::getGender() const
{
    return gender;
}

std::list<const Course *> Person::getCourses()const
{
    return courses;
}


//Setters

void Person::setAM(const char* _AM) {
    delete[] AM;
    AM = new char[strlen(_AM) + 1];
    strcpy(AM, _AM);
}

void Person::setFullname(std::string _fullname)
{
    fullname = _fullname;
}

void Person::setGender(std::string _gender)
{
    gender = _gender;
}

void Person::setCourses(std::list<const Course *> _courses)
{
    courses = _courses;
}

//Αλλαγή φύλου με το μοναδιαίο τελεστή '-'
void Person::operator-() {
    if (gender == "Άντρας") {
        gender = "Γυναίκα";
    } 
    else if (gender == "Male") {
        gender = "Female";
    } 
    else if (gender == "Γυναίκα") {
        gender = "Άντρας";
    } 
    else if (gender == "Female") {
        gender = "Male";
    }
}

bool Person::isFemale() const
{
    if ( (gender == "Γυναίκα") || (gender == "Female") ) 
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Person::isMale() const
{
    if ( (gender == "Άντρας") || (gender == "Male") )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Person::isStudent() const
{
    if (dynamic_cast<const Student*>(this) != nullptr)
    {
        return 1;
    } // else
    return 0;
}

bool Person::isProfessor() const
{
    if (dynamic_cast<const Professor*>(this) != nullptr)
    {
        return 1;
    }  // else
    return 0;
}


void Person::addCourse(const Course &in_course)
{
    try {
            for (const Course *course : courses)
            {
                if (course->getID() == in_course.getID())
                {
                    throw MyException("Σφάλμα: το μάθημα " + in_course.getDescription()
                        +" είναι ήδη δηλωμένο για "
                        + ( (this->isMale())? "τον " : "την ") // Φυλετική διάκριση
                        + this->getFullname() + '\n' );
                }
            }
            courses.push_back(&in_course);
    }
    catch (MyException e)
    {
        std::cerr << e.getErrorMessage();
    }
    catch (...)
    {
        std::cerr << "Σφάλμα κατά την δήλωση μαθήματος" << std::endl;
    }
}

// Αφαίρεση μαθήματος από την λίστα μαθημάτων που παρακολουθεί ο φοιτητής/ διδάσκει ο καθηγητής
void Person::removeCourse(const Course &in_course)
{
    std::list<const Course *>::iterator it;
    for (it = courses.begin(); it != courses.end(); ++it)
    {
        if (in_course.getID() == (*it)->getID())
        {
            courses.erase(it);
            break;  // Έξοδος από το loop για να αποφύγουμε βλάβη στην μνήμη
        }
    }    
}

// Εμφάνιση των χαρακτηριστικών του ατόμου
void Person::print(std::ostream &os) const
{
    int nameSize = strlen(this->getFullname().c_str());
    
    os << "AM: " << this->getAM() 
    << " | Ονοματεπώνυμο: " << this->getFullname() << " (" << nameSize << ")"
    << " | Φύλο: " << this->getGender();
}

//Εκτύπωση μαθημάτων που παρακολουθεί ο φοιτητής/ διδάσκει ο καθηγητής
void Person::printCourses() const
{
    std::list<const Course*>courses = this->getCourses();
    for (const Course* course : courses)
    {
        std::cout << course->getDescription() << std::endl;
    }
}

// Μετατροπή των στοιχείων του ατόμου σε string
std::string Person::toString()const
{
    std::string personStr;
    personStr += std::string (this->getAM()) + ',';
    personStr += (this->getFullname()) + ',';
    personStr += (this->getGender());
    
    return personStr;
}