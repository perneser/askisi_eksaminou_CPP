#include "registry.h"
#include "terminalUi.h"
#include "myException.h"
#include <iostream>

// Προσθήκες που έγιναν στον κώδικα της άσκησης 3, (πέρα από ό,τι ζητάει η εξαμηνιαία άσκηση):

// ● διόρθωση και αναζήτηση δεδομένων στους καταλόγους του φοιτητολογίου
// - void Registry::updatePerson(const char* keyMemberAM, const Person &in_person);
// - Course& Registry::findCourseByID(std::string keyCourseID);
// ● Όταν ένα μάθημα διαγράφεται από το φοιτητολόγιο,
//   αφαιρείται και από τις λίστες μαθημάτων τα οποία
//   παρακολουθεί ένας φοιτητής / διδάσκει ένας καθηγητής 
// ● Όταν ένας καθηγητής διαγράφεται από το φοιτητολόγιο,
//   αφαιρείται και από τα μαθήματα για τα οποία ήταν ο υπεύθυνος καθηγητής
// ● Αποθήκευση και ανάκτηση του συνόλου της πληροφορίας σε / από csv αρχεία. 


int main() 
{
    Registry services;;

    // Προσπαθούμε να φορτώσουμε δεδομένα. 
    try 
    {
        services.loadFromCsv("courses.csv", "professors.csv", "students.csv");
    } 
    catch (...) 
    {
        // Αν τα αρχεία δεν υπάρχουν (π.χ. πρώτη εκτέλεση), το πρόγραμμα δεν κρασάρει.
        // Απλά προχωράει έχοντας τις λίστες της Γραμματείας εντελώς άδειες.
    }

    // Ξεκινάμε κανονικά το περιβάλλον του τερματικού
    TerminalUI app(services);
    app.run();

    return 0; 
}



// main για την άσκηση 3, με τις νέες προσθήκες που έγιναν
// στον κώδικα της άσκησης 3, πέρα από ό,τι ζητάει η εξαμηνιαία άσκηση
// (για την χρήση της, απλά βάλετε σε σχόλια την προηγούμενη main και ξεσχολιάστε αυτήν την main)

/*
#include "student.h"
#include "professor.h"
#include "course.h"
#include "registry.h"
#include "myException.h"

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {

    
    Registry services; //Δημιουργία φοιτητολογίου

    // Δημιουργία δυναμικού πίνακα δεικτών σε αντικείμενα Student
    // (χρήση δεικτών επειδή η κλάση δεν έχει default constructor)
    Student** students = new Student*[3];

    //Εισαγωγή δεδομένων (Ελληνικοί και Λατινικοί χαρακτήρες)
    students[0] = new Student("21390123", "Νίκος Νικολόπουλος", "Άντρας", 3);
    students[1] = new Student("22390345", "Alex Smith"); //(Εξάμηνο 1 εξ ορισμού)
    students[2] = new Student(*students[0]); //Χρήση copy constructor
    students[2]->setFullname("Γεωργία Γεωργίου");
    students[2]->setAM("233390456");
    -*(students[2]); //Αλλαγή φύλου σε γυναίκα

    cout << "==============================================================" << endl;
    cout << "Πίνακας φοιτητών:" << std::endl;
    for (int i = 0; i < 3; i++) {
        cout << *students[i] << std::endl;
    }
    cout << "==============================================================" << endl;


    // // Επίδειξη λειτουργιών της άσκησης 2
    // cout << "\nΕπίδειξη Λειτουργιών:" << std::endl;
    
    // // Test ++ (Prefix)
    // ++(*students[1]);
    // cout << "Alex Smith μετά από ++: " << *students[1] << std::endl;

    // // Test +=
    // *students[0] += 2;
    // cout << "Νίκος Νικολόπουλος μετά από += 2: " << *students[0] << std::endl;

    // // Test --
    // *students[0] -= 1;
    // cout << "Νίκος Νικολόπουλος μετά από -= 1: " << *students[0] << std::endl;

 

    // Δημιουργία δυναμικού πίνακα δεικτών σε αντικείμενα Professor
    // (χρήση δεικτών επειδή η κλάση δεν έχει default constructor)
    Professor** professors = new Professor*[3];
    
    // Εισαγωγή δεδομένων
    professors[0] = new Professor ("1232000", "Ελευθέριος Αυστηρός", "Άντρας", "Προγραμματιστής");
    professors[1] = new Professor ("3453000", "Κωνσταντίνα Καλούλα", "Γυναίκα", "Μαθηματικός");
    professors[2] = new Professor ("5674000", "Ιωάννης Δίκαιος", "Άντρας", "Φυσικός");


    for (int i = 0; i < 3 ; i++)
    {
        services.addPerson(*students[i]);
    }

    for (int i = 0; i < 3 ; i++)
    {
        services.addPerson(*professors[i]);
    }

    cout << "==============================================================" << endl;
    cout << "Πίνακας καθηγητών:" << std::endl;
    for (int i = 0; i < 3; i++) {
        cout << *professors[i] << std::endl;
    }
    cout << "==============================================================" << endl;
    // Δημιουργία μαθημάτων - Σύνθεση / Composition
    services.createCourse(Course("ICE-2000", "Εφαρμοσμένα Μαθηματικά", 4, professors[1]));
    services.createCourse(Course ("ICE-3000", "Αντικειμενοστρεφής Προγρραμματισμός", 2, professors[0]));
    services.createCourse(Course ("ICE-4000", "Φυσική", 6, professors[2]));
    services.updateCourse("ICE-2000", "ICE-1000", "Εφαρμοσμένα Μαθηματικά", 4, professors[1]);

    vector<Course> courses = services.getCourses();

    cout << "==============================================================" << endl;
    cout << "Πίνακας μαθημάτων:" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        cout << courses[i] << std::endl;
    }
        cout << "==============================================================" << endl;
        // Δήλωση μαθημάτων που παρακολουθούν οι φοιτητές
        students[0]->addCourse(courses[1]);
        students[0]->addCourse(courses[2]);
        students[0]->addCourse(courses[2]);


        students[1]->addCourse(courses[2]);
        students[1]->addCourse(courses[2]);

        students[2]->addCourse(courses[0]);
        students[2]->addCourse(courses[1]);

    cout << "==============================================================" << endl;
    // Εμφάνιση όλων των μαθημάτων που παρακολουθεί ο κάθε φοιτητής
    for (int i = 0; i < 3; i++)
    {
        std::cout <<"Τα μαθήματα που παρακολουθεί " 
                  <<((students[i]->isFemale())? "η φοιτήτρια ": "ο φοιτητής " ) // Φυλετική διάκριση
                  << students[i]->getFullname() << ':' << std::endl;
        
        students[i]->printCourses();
        cout << "==============================================================" << endl;

    }

    // Δήλωση μαθημάτων που διδάσκουν οι καθηγητές
    professors[0]->addCourse(courses[0]);
    professors[0]->addCourse(courses[1]);

    professors[1]->addCourse(courses[0]);
    professors[1]->addCourse(courses[2]);
    professors[1]->addCourse(courses[2]);

    professors[2]->addCourse(courses[1]);
    professors[2]->addCourse(courses[2]);
    professors[2]->addCourse(courses[2]);
    cout << "==============================================================" << endl;


    // Εμφάνιση όλων των μαθημάτων που παρακολουθεί ο κάθε φοιτητής
    for (int i = 0; i < 3; i++)
    {
        std::cout <<"Τα μαθήματα που διδάσκει "
                  << ((professors[i]->isFemale())? "η καθηγήτρια ": "ο καθηγητής " ) // Φυλετική διάκριση
                  << professors[i]->getFullname() << ':' << std::endl;
       
        professors[i]->printCourses();   
        cout << "==============================================================" << endl;
    }
    cout << "==============================================================" << endl;

    services.removeCourse(courses[2]);
            std::cout <<"Τα μαθήματα που διδάσκει "
                  << ((professors[2]->isFemale())? "η καθηγήτρια ": "ο καθηγητής " ) // Φυλετική διάκριση
                  << professors[2]->getFullname() 
                  << " μετά από την αφαίρεση του μαθήματος 'Φυσική':" << std::endl;
    professors[2]->printCourses();   
    cout << "==============================================================" << endl;


    // Επεξεργασία των στοιχείων του καθηγητή *professors[2]
    services.updatePerson("5674000", Professor("9999999", "Ιωάννης Άδικος", "Άντρας", "Αστρονόμος"));

    
    // services.removePerson(*professors[2]);   // Διαγραφή καθηγητή από το φοιτητολόγιο
    // services.removePerson(*students[2]);   // Διαγραφή φοιτητή από το φοιτητολόγιο

    cout << "Τα άτομα που βρίσκονται στο φοιτητολόγιο:" << endl;
    services.printPersons();
    
    //////////////////////
    /// services.removeCourse(courses[2]);       // Διαγραφή μαθήματος από το φοιτητολόγιο
    /// professors[1]->printCourses();
    cout << "Τα μαθήματα που βρίσκονται στο φοιτητολόγιο:" << endl;
    services.printCourses();
    cout << "==============================================================" << endl;
    
    // Αναζήτηση μαθήματος στο φοιτητολόγιο
    try 
    {
        cout << "Αποτέλεσμα αναζήτησης 'ICE-3000': " << services.findCourseByID("ICE-3000") << endl;
        cout << "Αποτέλεσμα αναζήτησης 'ICE-4000': " << services.findCourseByID("ICE-4000") << endl;
    }
    catch (MyException e)
    {
        cerr << e.getErrorMessage() << endl;
    }
    catch (...)
    {
        cerr << "Δεν βρέθηκε το μάθημα." << endl;
    }

    cout << "==============================================================" << endl;
    services.sendEmailToAllProfessors();
    cout << "==============================================================" << endl;
    services.sendEmailToAllStudents();
    cout << "==============================================================" << endl;


    try {
        services.saveCoursesToCsv("courses.csv");
        services.saveProfessorsToCsv("professors.csv");
        services.saveStudentsToCsv("students.csv");
    }
    catch (MyException e)
    {
        cerr << e.getErrorMessage() << endl;
    }
    catch (...)
    {
        cerr << "Η εγγραφή των μαθημάτων σε CSV αρχείο απέτυχε." << endl;
    }


    // Αποδέσμευση μνήμης
    for (int i = 0; i < 3; i++) {
        delete students[i];
    }
    delete[] students;

    
    for (int i = 0; i < 3; i++)
    {
        delete professors[i];
    }
    delete[] professors;
    


    // Το πρόγραμμα τρέχει μια φορά, δημιουργεί τα CSV αρχεία, και παρακάτω
    // γίνεται φόρτωση των δεδομένων από αυτά τα αρχεία, αφότου έχουν ήδη
    // διαγραφεί τα αντικείμενα που δημιουργήθηκαν προηγουμένως,
    // ώστε να μην υπάρχει σύγκρουση δεδομένων.

    // Δημιουργούμε το αντικείμενο του φοιτητολογίου (Registry)
    Registry csvRegistry; 

    // Προσπαθούμε να φορτώσουμε τα δεδομένα με ασφάλεια
    try 
    {
        
        cout << "==============================================================" << endl;
        cout << "Γίνεται φόρτωση δεδομένων από τα CSV..." << endl;
        
        // Καλούμε τη μέθοδο loadFromCsv για να φορτώσουμε τα δεδομένα από τα αρχεία CSV
        csvRegistry.loadFromCsv("courses.csv", "professors.csv", "students.csv");

        cout << "Το σύστημα του φοιτητολογίου είναι έτοιμο!" << endl;

        // Εκτύπωση όλων των δεδομένων που φόρτωσε το σύστημα
        csvRegistry.printAllData();

        cout << "==============================================================" << endl;

    } 
    catch (MyException &e) 
    {
        // Αν οποιαδήποτε get...FromCsv πετάξει το throw MyException, θα έρθει κατευθείαν εδώ
        cerr << "\nΣφάλμα: " << e.getErrorMessage() << endl;
    }
    catch (const std::exception &e) 
    {
        // Μια δικλείδα ασφαλείας για οποιοδήποτε άλλο standard error της C++
        cerr << "\nΆγνωστο σφάλμα: " << e.what() << endl;
    }

    
    return 0;
}
*/