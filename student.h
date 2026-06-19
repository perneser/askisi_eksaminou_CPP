#ifndef STUDENT_H
#define STUDENT_H

#include "person.h"
#include <string>

class Course;

class Student : public Person {
    private:
        unsigned int semester;  //Τρέχον εξάμηνο
        

    public:
        //Constructor με ΑΜ και Ονοματεπώνυμο
        Student(const char* _AM, std::string _fullname);

        //Constructor με όλα τα χαρακτηριστικά
        Student(const char* _AM, std::string _fullname, std::string _gender, unsigned int _semester);
        
        //Copy Constructor
        Student(const Student& other);

        //Destructor
        ~Student();

        //Getters
        unsigned int getSemester() const;

        //Setters
        void setSemester(unsigned int _semester);


        //Αύξηση εξαμήνου (++a)
        Student& operator++();

        //Αύξηση εξαμήνου (a++)
        Student operator++(int);

        //Τροποποίηση εξαμήνου με +=
        void operator+=(unsigned int val);

        //Τροποποίηση εξαμήνου με -=
        void operator-=(unsigned int val);

        // Εμφάνιση των χαρακτηριστικών του φοιτητή 
        void print(std::ostream &os) const override; 
        // Μετατροπή των στοιχείων του φοιτητή σε string
        std::string toString()const;
};
#endif