#include <iostream>
#include "registry.h" // Κάνουμε include τη Γραμματεία
#include "myException.h"
#include "student.h"
#include "professor.h"
#include "course.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;
int main() 
    Registry csvRegistry; 
    try 
        cout << "==============================================================" << endl;
        cout << "Γίνεται φόρτωση δεδομένων από τα CSV..." << endl;
        csvRegistry.loadAllData("courses.csv", "professors.csv", "students.csv");
        cout << "Το Σύστημα είναι έτοιμο!" << endl;
        csvRegistry.printAllData();
        cout << "==============================================================" << endl;
    catch (MyException &e) 
        cerr << "\nΣφάλμα: " << e.getErrorMessage() << endl;
    catch (const std::exception &e) 
        cerr << "\nΆγνωστο σφάλμα: " << e.what() << endl;
    return 0;
