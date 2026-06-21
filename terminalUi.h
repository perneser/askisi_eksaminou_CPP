#ifndef TERMINALUI_H
#define TERMINALUI_H

#include "registry.h"
#include <ncursesw/ncurses.h>
#include <string>
#include <vector>

class TerminalUI {
private:
    Registry& registry; // Αναφορά στη Γραμματεία (για να βλέπουμε τα δεδομένα)
    int max_y, max_x;   // Διαστάσεις του παραθύρου του τερματικού

    // Ιδιωτικές βοηθητικές μέθοδοι του μενού
    void drawMainMenu(int highlight);
    void showMessage(const std::string& msg);

public:
    // Constructor: Παίρνει το ήδη φορτωμένο φοιτητολόγιο
    TerminalUI(Registry& reg);
    
    // Destructor: Κλείνει το ncurses με ασφάλεια
    ~TerminalUI();

    void viewPersons();
    void viewCourses();
    void searchPerson();
    void searchCourse();
    void addPerson();
    void updatePerson();
    void addCourse();
    void updateCourse();
    void deletePerson();
    void deleteCourse();
    void assignCourseToPerson();
    void removeCourseFromPerson();
    void assignProfessorToCourse();

    // Η κεντρική μέθοδος που τρέχει το πρόγραμμα
    void run();
};

#endif