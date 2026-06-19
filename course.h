#ifndef COURSE_H
#define COURSE_H

#include <string>
// #include "professor.h"
class Professor;

class Course {
    private:
        std::string ID;     // Κωδικός
        std::string description;  // Περιγραφή
        unsigned int semester;  // Εξάμηνο διδασκαλίας
        Professor *mainProfessor; // Υπεύθυνος μαθήματος
    public:
        // Constructors
        Course(std::string _ID, std::string _description, unsigned int _semester);
        Course(std::string _ID, std::string _description, unsigned int _semester, Professor * _mainProfessor);

        // Destructor
        ~Course();

        // Getters
        std::string getID() const;
        std::string getDescription() const;
        unsigned int getSemester() const;
        Professor* getMainProfessor() const;
    
        // Setters
        void setID(std::string _ID);
        void setDescription(std::string _description);
        void setSemester(unsigned int _semester);
        void setMainProfessor(Professor *_mainProfessor);

        // Εμφάνιση χαρακτηριστικών του μαθήματος
        void print(std::ostream &os)const;
        // Μετατροπή των στοιχείων του μαθήματος σε string
        std::string toString()const;
};

std::ostream& operator<<(std::ostream &os, const Course &course);
#endif