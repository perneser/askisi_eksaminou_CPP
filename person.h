#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <cstring>
#include <list>

// #include "course.h"
class Course;

class Person
{
private:
    char* AM;               //Αριθμός μητρώου
    std::string fullname;        //Ονοματεπώνυμο
    std::string gender;          //Φύλο
    std::list <const Course *> courses;   // Λίστα μαθημάτων που παρακολουθεί ο φοιτητής/διδάσκει ο καθηγητής

    public:
    
    // Constructors
    Person(const char* _AM, std::string fullname);
    Person(const char* _AM, std::string fullname, std::string gender);

    // Copy constructor
    Person(const Person &other);

    // Virtual destructor (για την κληρονομικότητα)
    virtual ~Person();

    //Getters
    const char* getAM()const;
    std::string getFullname()const;
    std::string getGender()const;
    std::list<const Course *> getCourses()const;

    //Setters
    void setAM(const char* _AM);
    void setFullname(std::string _fullname);
    void setGender(std::string _gender);
    void setCourses(std::list<const Course *> _courses);

    //Αλλαγή φύλου με το μοναδιαίο τελεστή '-'
    void operator-();

    bool isFemale()const;
    bool isMale()const;
    bool isStudent()const;
    bool isProfessor()const;

    // Προσθήκη μαθήματος στην λίστα μαθημάτων που παρακολουθεί ο φοιτητής/ διδάσκει ο καθηγητής
    void addCourse(const Course &course);

    // Αφαίρεση μαθήματος από την λίστα μαθημάτων που παρακολουθεί ο φοιτητής/ διδάσκει ο καθηγητής
    void removeCourse(const Course &course);

    // Εμφάνιση των χαρακτηριστικών του ατόμου
    virtual void print(std::ostream &os)const;
    // Εμφάνιση μαθημάτων που παρακολουθεί ο φοιτητής/ διδάσκει ο καθηγητής
    virtual void printCourses()const;
    // Μετατροπή των στοιχείων του ατόμου σε string
    std::string toString()const;
};

// Δηλώνεται εδώ για να μην χρειαστεί να χρησιμοποιηθεί η λέξη κλειδί friend
std::ostream& operator<<(std::ostream& os, const Person& p);

#endif









