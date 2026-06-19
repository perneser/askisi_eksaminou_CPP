#ifndef PROFESSOR_H
#define PROFESSOR_H

#include "person.h"
#include <string>

class Professor : public Person {
    private:
        std::string specialty;
    public:
        // Constructors
        Professor (const char* _AM, std::string _fullname, std::string _gender, std::string specialty);
        // Destructor
       ~Professor();

        // Getter
        std::string getSpecialty() const;

        // Setter
        void setSpecialty(std::string _specialty);

        // Εμφάνιση των χαρακτηριστικών του καθηγητή
        void print(std::ostream& os) const override;
        // Μετατροπή των στοιχείων του καθηγητή σε string
        std::string toString()const;
};
#endif