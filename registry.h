#ifndef REGISTRY_H
#define REGISTRY_H

#include <list>
#include <vector>
#include <string>

class Person;
class Course;
class Student;
class Professor;


class Registry
{
    private:
        // Ενιαίος κατάλογος των μελών του τμήματος (φοιτητές, καθηγητές) 
        std::vector<Person*> persons;

        // Κατάλογος των μαθημάτων
        std::vector<Course> courses;    // Composition
    public:
        // Getters
        std::vector<Person*> getPersons()const;

        // Getters για προηγούμενη υλοποίηση του προγράμματος
        std::vector<const Student*> getStudents()const;
        std::vector<const Professor*> getProfessors()const;


        std::vector<Course> getCourses()const;
    
        // Εισαγωγή φοιτητή/καθηγητή στο φοιτητολόγιο
        void addPerson(Person &person);
        // Δημιουργια μαθήματος
        void createCourse(const Course &course);

        // Διαγραφή φοιτητή/καθηγητή από τον κατάλογο
        void removePerson(const Person &person);
        // Διαγραφή μαθηματος από τον κατάλογο
        void removeCourse(const Course &course);

        //Διόρθωση στοιχείων φοιτητή/καθηγητή του φοιτητολογίου
        void updatePerson(const char* keyMemberAM, const Person &in_person);
        
        // Διόρθωση μαθήματος στο φοιτητολόγιο
  		void updateCourse(std::string keyCourseID,
			std::string _ID,
			std::string _name,
			unsigned int _semester,
			Professor *_professorPtr
		);

        // Αναζήτηση μαθήματος στο φοιτητολόγιο με βάση τον κωδικό του
        Course& findCourseByID(std::string keyCourseID);

        // Εμφάνιση ατόμων εγγεγραμένων στο φοιτητολόγιο
        void printPersons()const;
        
        // Εμφάνιση μαθημάτων που υπάρχουν στο φοιτητολόγιο
        void printCourses()const;

        // Εμφάνιση όλων των στοιχείων του συστήματος
        void printAllData() const;

        // Αποστολή e-mail σε όλους τους εκπαιδευτικούς
        void sendEmailToAllProfessors()const;
        // Αποστολή e-mail σε όλους τους φοιτητές
        void sendEmailToAllStudents()const;

        // Βοηθητικές Μέθοδοι (finders)
        Course* findCourse(std::string in_ID);
        // const overload για να μπορεί να καλείται από const αντικείμενα
        const Course* findCourse(std::string in_ID) const;
        Person* findPerson(std::string in_AM) const;

        // Αποθήκευση και ανάκτηση δεδομένων από αρχεία CSV
        void saveProfessorsToCsv(const std::string &filename)const;
        std::vector<Professor*> getProfessorsFromCsv(const std::string &filename)const;

        void saveStudentsToCsv(const std::string &filename)const;
        std::vector<Student*> getStudentsFromCsv(const std::string &filename)const;

        void saveCoursesToCsv(const std::string &filename)const;
        std::vector<Course> getCoursesFromCsv(const std::string &filename)const;

        // Μέθοδος Σύνδεσης (Linker)
        void linkMainProfessors(const std::string &filename);

        // Η Κεντρική Μέθοδος (Orchestrator)
        void loadAllData(std::string coursesCsvFile, std::string professorsCsvFile, std::string studentsCsvFile);

    };

#endif