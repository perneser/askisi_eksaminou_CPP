#include "terminalUi.h"
#include <locale.h>
#include <cstring>
#include <cstdlib>
#include "myException.h"
#include "person.h"
#include "professor.h"
#include "registry.h"
#include "student.h"
#include "course.h"

bool TerminalUI::getStringInput(const std::string& prompt, char* buffer, int max_len, int y, int x)
{
    std::string input;
    curs_set(1); 
    noecho();    
    
    while (true) 
    {
        move(y, x);
        clrtoeol();
        mvprintw(y, x, "%s%s", prompt.c_str(), input.c_str());
        
        int c = getch();
        
        if (c == 27) 
        { 
            curs_set(0);
            return false;
        } 
        else if (c == '\n' || c == 10) 
        { 
            strncpy(buffer, input.c_str(), max_len);
            buffer[max_len] = '\0';
            curs_set(0);
            return true;
        } 
        else if (c == KEY_BACKSPACE || c == 127 || c == '\b') 
        {
            if (!input.empty()) 
            {
                // hack για τα ελληνικά (utf-8 2 bytes)
                do {
                    input.pop_back();
                } while (!input.empty() && (input.back() & 0xC0) == 0x80);
            }
        } 
        else if (input.length() < (size_t)max_len && c >= 32 && c <= 255) 
        {
            input.push_back((char)c);
        }
    }
}

TerminalUI::TerminalUI(Registry& reg) : registry(reg) 
{
    setlocale(LC_ALL, ""); 
    setenv("ESCDELAY", "25", 1); // fix για να μην κολλάει το ESC
    
    initscr();             
    clear();
    noecho();              
    cbreak();              
    curs_set(0);           
    keypad(stdscr, TRUE);  
    
    getmaxyx(stdscr, max_y, max_x); 
}

TerminalUI::~TerminalUI() 
{
    endwin();
}

void TerminalUI::showMessage(const std::string& msg) 
{
    clear();
    mvprintw(max_y / 2, (max_x - msg.length()) / 2, "%s", msg.c_str());
    mvprintw((max_y / 2) + 2, (max_x - 30) / 2, "Πατήστε οποιοδήποτε πλήκτρο...");
    refresh();
    getch();
}

void TerminalUI::drawMainMenu(int highlight) 
{
    clear();
    
    std::string title = "--- ΣΥΣΤΗΜΑ ΦΟΙΤΗΤΟΛΟΓΙΟΥ ---";
    mvprintw(2, (max_x - title.length()) / 2, "%s", title.c_str());

    std::vector<std::string> choices = {
        "1. Προβολή Μελών του Τμήματος (Φοιτητές/τριες, Καθηγητές/τριες)",
        "2. Προβολή Μαθημάτων",
        "3. Αναζήτηση Ατόμου (βάσει Α.Μ.)",
        "4. Αναζήτηση Μαθήματος (βάσει Κωδικού)",
        "5. Προσθήκη Νέου Ατόμου",
        "6. Τροποποίηση Στοιχείων Υπάρχοντος Ατόμου",
        "7. Προσθήκη Νέου Μαθήματος",
        "8. Τροποποίηση Στοιχείων Υπάρχοντος Μαθήματος",
        "9. Διαγραφή Ατόμου από το Φοιτητολόγιο",
        "10. Διαγραφή Μαθήματος",
        "11. Δήλωση Μαθήματος (σε Φοιτητή/τρια ή Καθηγητή/τρια)",
        "12. Αφαίρεση Μαθήματος από Φοιτητή/τρια ή Καθηγητή/τρια",
        "13. Ορισμός Υπεύθυνου Καθηγητή σε Μάθημα",
        "14. Email στο σύνολο των Φοιτητών/τριών",
        "15. Email στο σύνολο των Καθηγητών/τριών",
        "16. Αποθήκευση και Έξοδος"
    };
    
    int start_y = 5;
    for (int i = 0; i < (int)choices.size(); i++) 
    {
        if (highlight == i) 
        {
            attron(A_REVERSE); 
            mvprintw(start_y + i, 5, "%s", choices[i].c_str());
            attroff(A_REVERSE);
        } 
        else 
        {
            mvprintw(start_y + i, 5, "%s", choices[i].c_str());
        }
    }
    refresh();
}

void TerminalUI::viewPersons() 
{
    clear(); 
    mvprintw(1, 2, "--- ΛΙΣΤΑ ΦΟΙΤΗΤΟΛΟΓΙΟΥ ---");
    
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        mvprintw(3, 2, "Δεν βρέθηκαν άτομα στο σύστημα.");
    } 
    else 
    {
        int line = 3; 
        for (const Person* p : persons) 
        {
            if (line >= max_y - 2) 
            { 
                mvprintw(line, 2, "...(η λίστα συνεχίζεται, πατήστε ένα πλήκτρο)...");
                break;
            }
            
            std::string role;
            if (p->isStudent()) {
                role = p->isFemale() ? "Φοιτήτρια" : "Φοιτητής";
            } else if (p->isProfessor()) {
                role = p->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
            } else {
                role = "Άγνωστος Ρόλος";
            }
            
            mvprintw(line, 2, "[%s] ΑΜ: %s | Όνομα: %s | Φύλο: %s", 
                     role.c_str(), p->getAM(), p->getFullname().c_str(), p->getGender().c_str());
            line++;
        }
    }
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh(); 
    getch();   
}

void TerminalUI::viewCourses() 
{
    clear(); 
    mvprintw(1, 2, "--- ΛΙΣΤΑ ΜΑΘΗΜΑΤΩΝ ---");
    
    std::vector<Course> courses = registry.getCourses();
    
    if (courses.empty()) 
    {
        mvprintw(3, 2, "Δεν βρέθηκαν μαθήματα στο σύστημα.");
    } 
    else 
    {
        int line = 3;
        for (const Course& c : courses) 
        {
            if (line >= max_y - 2) 
            {
                mvprintw(line, 2, "...(η λίστα συνεχίζεται)...");
                break;
            }
            
            std::string profName = "Κανένας";
            if (c.getMainProfessor() != nullptr) 
            {
                profName = c.getMainProfessor()->getFullname();
            }
            
            mvprintw(line, 2, "Κωδικός: %s | Περιγραφή: %s | Εξάμηνο: %d | Υπεύθυνος: %s", 
                     c.getID().c_str(), c.getDescription().c_str(), c.getSemester(), profName.c_str());
            line++;
        }
    }
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή...");
    refresh();
    getch();
}

void TerminalUI::searchPerson() 
{
    clear();
    mvprintw(1, 2, "--- ΑΝΑΖΗΤΗΣΗ ΑΤΟΜΟΥ ---");
    mvprintw(2, 2, "(ESC για ακύρωση)");

    char am_input[50];
    if (!getStringInput("Αριθμός Μητρώου: ", am_input, 49, 4, 2)) return;
    
    Person* p = registry.findPerson(std::string(am_input));
    
    if (p != nullptr) 
    {
        mvprintw(6, 2, "ΒΡΕΘΗΚΕ: %s", p->getFullname().c_str());
        mvprintw(7, 2, "Φύλο: %s", p->getGender().c_str());
        
        std::string role;
        if (p->isStudent()) {
            role = p->isFemale() ? "Φοιτήτρια" : "Φοιτητής";
        } else if (p->isProfessor()) {
            role = p->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
        } else {
            role = "Άγνωστος";
        }
        mvprintw(8, 2, "Ιδιότητα: %s", role.c_str());
        
        mvprintw(10, 2, "[ Μαθήματα ]");
        int line = 11;
        std::list<const Course*> courses = p->getCourses();
        
        if (courses.empty()) 
        {
            mvprintw(line, 4, "- Κανένα μάθημα");
        } 
        else 
        {
            for (const Course* c : courses) 
            {
                mvprintw(line++, 4, "- %s (%s)", c->getDescription().c_str(), c->getID().c_str());
            }
        }
    } 
    else 
    {
        mvprintw(6, 2, "[!] Δεν βρέθηκε άτομο με ΑΜ: %s", am_input);
    }
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο...");
    refresh();
    getch();
}

void TerminalUI::searchCourse() 
{
    clear();
    mvprintw(1, 2, "--- ΑΝΑΖΗΤΗΣΗ ΜΑΘΗΜΑΤΟΣ ---");
    mvprintw(2, 2, "(ESC για ακύρωση)");
    
    char id_input[50];
    if (!getStringInput("Κωδικός Μαθήματος: ", id_input, 49, 4, 2)) return;
    
    Course* c = registry.findCourse(std::string(id_input));
    
    if (c != nullptr) 
    {
        mvprintw(6, 2, "ΒΡΕΘΗΚΕ: %s", c->getDescription().c_str());
        mvprintw(7, 2, "Κωδικός: %s", c->getID().c_str());
        mvprintw(8, 2, "Εξάμηνο: %d", c->getSemester());
        
        std::string profName = "Κανένας";
        if (c->getMainProfessor() != nullptr) 
        {
            profName = c->getMainProfessor()->getFullname();
        }
        mvprintw(9, 2, "Υπεύθυνος: %s", profName.c_str());
    } 
    else 
    {
        mvprintw(6, 2, "[!] Δεν βρέθηκε μάθημα με κωδικό: %s", id_input);
    }
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο...");
    refresh();
    getch();
}

void TerminalUI::updatePerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν άτομα για τροποποίηση.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0; 
    int choice = -1;
    bool selecting = true;

    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΕΠΙΛΟΓΗ ΑΤΟΜΟΥ ΠΡΟΣ ΤΡΟΠΟΠΟΙΗΣΗ ---");
        mvprintw(2, 2, "Βελάκια + Enter (ESC για ακύρωση)");

        int lines = max_y - 6; 
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + lines) offset = highlight - lines + 1;

        for (int i = 0; i < lines && (i + offset) < (int)persons.size(); i++) 
        {
            int actual_idx = i + offset;
            if (actual_idx == highlight) attron(A_REVERSE);
            
            std::string role;
            if (persons[actual_idx]->isStudent()) {
                role = persons[actual_idx]->isFemale() ? "Φοιτήτρια" : "Φοιτητής";
            } else {
                role = persons[actual_idx]->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
            }
            
            mvprintw(4 + i, 4, "[%s] ΑΜ: %s | %s", role.c_str(), persons[actual_idx]->getAM(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: highlight = (highlight == 0) ? persons.size() - 1 : highlight - 1; break;
            case KEY_DOWN: highlight = (highlight == (int)persons.size() - 1) ? 0 : highlight + 1; break;
            case 10: choice = highlight; selecting = false; break;
        }
    }

    Person* p = persons[choice];
    std::string am_input = p->getAM(); 
    
    clear();
    mvprintw(1, 2, "--- ΤΡΟΠΟΠΟΙΗΣΗ ΣΤΟΙΧΕΙΩΝ ---");
    mvprintw(4, 2, "Επιλέχθηκε: %s (%s)", p->getFullname().c_str(), am_input.c_str());
    
    mvprintw(6, 2, "Τρέχον Όνομα: %s", p->getFullname().c_str());
    char name_input[100];
    if (!getStringInput("Νέο (Κενό για διατήρηση): ", name_input, 99, 7, 2)) return;
    std::string new_name = (strlen(name_input) > 0) ? name_input : p->getFullname();
    
    mvprintw(8, 2, "Τρέχον Φύλο: %s", p->getGender().c_str());
    char gender_input[50];
    if (!getStringInput("Νέο (Κενό για διατήρηση): ", gender_input, 49, 9, 2)) return;
    std::string new_gender = (strlen(gender_input) > 0) ? gender_input : p->getGender();

    if (p->isStudent()) 
    {
        Student* s = dynamic_cast<Student*>(p);
        mvprintw(10, 2, "Τρέχον Εξάμηνο: %d", s->getSemester());
        char sem_input[10];
        if (!getStringInput("Νέο (Κενό για διατήρηση): ", sem_input, 9, 11, 2)) return;
        
        int new_sem = s->getSemester();
        if (strlen(sem_input) > 0) {
            new_sem = atoi(sem_input);
        }

        Student tempStudent(am_input.c_str(), new_name, new_gender, new_sem);
        tempStudent.setCourses(p->getCourses()); 
        registry.updatePerson(am_input.c_str(), tempStudent);
    }
    else if (p->isProfessor()) 
    {
        Professor* prof = dynamic_cast<Professor*>(p);
        mvprintw(10, 2, "Τρέχουσα Ειδικότητα: %s", prof->getSpecialty().c_str());
        char spec_input[100];
        if (!getStringInput("Νέα (Κενό για διατήρηση): ", spec_input, 99, 11, 2)) return;
        std::string new_spec = (strlen(spec_input) > 0) ? spec_input : prof->getSpecialty();

        Professor tempProf(am_input.c_str(), new_name, new_gender, new_spec);
        tempProf.setCourses(p->getCourses()); 
        registry.updatePerson(am_input.c_str(), tempProf);
    }
    
    mvprintw(13, 2, "[+] Έγινε η τροποποίηση!");
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο...");
    refresh();
    getch();
}

void TerminalUI::addPerson() 
{
    clear();
    mvprintw(1, 2, "--- ΠΡΟΣΘΗΚΗ ΑΤΟΜΟΥ ---");
    
    char type_input[10];
    if (!getStringInput("Ιδιότητα (1=Φοιτητής, 2=Καθηγητής): ", type_input, 9, 4, 2)) return;
    
    if (type_input[0] != '1' && type_input[0] != '2') 
    {
        mvprintw(6, 2, "[!] Λάθος επιλογή.");
        getch();
        return;
    }
    
    char am_input[50];
    if (!getStringInput("ΑΜ: ", am_input, 49, 5, 2)) return;
    
    if (registry.findPerson(std::string(am_input)) != nullptr) 
    {
        mvprintw(7, 2, "[!] Υπάρχει ήδη!");
        getch();
        return;
    }
    
    char name_input[100];
    if (!getStringInput("Ονοματεπώνυμο: ", name_input, 99, 6, 2)) return;
    
    char gender_input[50];
    if (!getStringInput("Φύλο (Άνδρας/Γυναίκα): ", gender_input, 49, 7, 2)) return;
    std::string gender_str(gender_input);
    
    if (type_input[0] == '1') 
    {
        char sem_input[10];
        if (!getStringInput("Εξάμηνο: ", sem_input, 9, 8, 2)) return;
        int sem = atoi(sem_input);
        if(sem <= 0) sem = 1;
        
        Student* newStudent = new Student(am_input, name_input, gender_str, sem);
        registry.addPerson(*newStudent);
    }
    else 
    {
        char spec_input[100];
        if (!getStringInput("Ειδικότητα: ", spec_input, 99, 8, 2)) return;
        
        Professor* newProf = new Professor(am_input, name_input, gender_str, spec_input);
        registry.addPerson(*newProf);
    }
    
    mvprintw(10, 2, "[+] Αποθηκεύτηκε!");
    refresh();
    getch();
}

void TerminalUI::addCourse() 
{
    clear();
    mvprintw(1, 2, "--- ΠΡΟΣΘΗΚΗ ΜΑΘΗΜΑΤΟΣ ---");
    
    char id_input[50];
    if (!getStringInput("Κωδικός: ", id_input, 49, 4, 2)) return;
    
    if (registry.findCourse(std::string(id_input)) != nullptr) 
    {
        mvprintw(6, 2, "[!] Υπάρχει ήδη!");
        getch();
        return;
    }
    
    char desc_input[100];
    if (!getStringInput("Περιγραφή: ", desc_input, 99, 5, 2)) return;
    
    char sem_input[10];
    if (!getStringInput("Εξάμηνο: ", sem_input, 9, 6, 2)) return;
    int sem = atoi(sem_input);
    if(sem <= 0) sem = 1;
    
    char prof_input[50];
    if (!getStringInput("ΑΜ Υπεύθυνου (Κενό για κανέναν): ", prof_input, 49, 7, 2)) return;
    
    Professor* mainProf = nullptr;
    if (strlen(prof_input) > 0) 
    {
        Person* p = registry.findPerson(std::string(prof_input));
        if (p != nullptr && p->isProfessor()) 
        {
            mainProf = dynamic_cast<Professor*>(p);
            std::string article = mainProf->isFemale() ? "την" : "τον";
            mvprintw(8, 2, "Συνδέθηκε με %s %s", article.c_str(), mainProf->getFullname().c_str());
        } 
        else 
        {
            mvprintw(8, 2, "[!] Δεν βρέθηκε καθηγητής, αποθήκευση χωρίς υπεύθυνο.");
        }
    }
    
    Course newCourse(id_input, desc_input, sem, mainProf);
    registry.createCourse(newCourse);
    
    mvprintw(10, 2, "[+] Προστέθηκε!");
    refresh();
    getch();
}

void TerminalUI::updateCourse() 
{
    std::vector<Course> courses = registry.getCourses();
    
    if (courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Άδεια λίστα μαθημάτων.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0;
    int choice = -1;
    bool selecting = true;

    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΤΡΟΠΟΠΟΙΗΣΗ ΜΑΘΗΜΑΤΟΣ ---");

        int lines = max_y - 6;
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + lines) offset = highlight - lines + 1;

        for (int i = 0; i < lines && (i + offset) < (int)courses.size(); i++) 
        {
            int actual_idx = i + offset;
            if (actual_idx == highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s (Εξάμηνο %d)", 
                     courses[actual_idx].getID().c_str(), 
                     courses[actual_idx].getDescription().c_str(), 
                     courses[actual_idx].getSemester());
            
            if (actual_idx == highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: highlight = (highlight == 0) ? courses.size() - 1 : highlight - 1; break;
            case KEY_DOWN: highlight = (highlight == (int)courses.size() - 1) ? 0 : highlight + 1; break;
            case 10: choice = highlight; selecting = false; break;
        }
    }

    Course* c = registry.findCourse(courses[choice].getID());
    std::string id_input = c->getID(); 
    
    clear();
    mvprintw(1, 2, "--- ΑΛΛΑΓΗ ΣΤΟΙΧΕΙΩΝ ---");
    mvprintw(4, 2, "Μάθημα: %s (%s)", c->getDescription().c_str(), id_input.c_str());
    
    mvprintw(6, 2, "Τρέχων Κωδικός: %s", c->getID().c_str());
    char new_id_input[50];
    if (!getStringInput("Νέος (Κενό για διατήρηση): ", new_id_input, 49, 7, 2)) return;
    std::string new_id = (strlen(new_id_input) > 0) ? new_id_input : c->getID();
    
    mvprintw(8, 2, "Τρέχουσα Περιγραφή: %s", c->getDescription().c_str());
    char desc_input[100];
    if (!getStringInput("Νέα (Κενό για διατήρηση): ", desc_input, 99, 9, 2)) return;
    std::string new_desc = (strlen(desc_input) > 0) ? desc_input : c->getDescription();

    mvprintw(10, 2, "Τρέχον Εξάμηνο: %d", c->getSemester());
    char sem_input[10];
    if (!getStringInput("Νέο (Κενό για διατήρηση): ", sem_input, 9, 11, 2)) return;
    int new_sem = c->getSemester();
    if (strlen(sem_input) > 0) {
        new_sem = atoi(sem_input);
    }

    std::string profName = "Κανένας";
    if (c->getMainProfessor() != nullptr) {
        profName = c->getMainProfessor()->getFullname();
    }
    
    mvprintw(12, 2, "Υπεύθυνος: %s", profName.c_str());
    char prof_input[50];
    if (!getStringInput("Νέος ΑΜ (Κενό=διατήρηση, '-'=Διαγραφή): ", prof_input, 49, 13, 2)) return;
    
    Professor* newProf = c->getMainProfessor(); 
    if (strcmp(prof_input, "-") == 0) 
    {
        newProf = nullptr; 
        mvprintw(14, 2, "Ο υπεύθυνος αφαιρέθηκε.");
    } 
    else if (strlen(prof_input) > 0) 
    {
        Person* p = registry.findPerson(std::string(prof_input));
        if (p != nullptr && p->isProfessor()) 
        {
            newProf = dynamic_cast<Professor*>(p);
            mvprintw(14, 2, "Νέος: %s", newProf->getFullname().c_str());
        } 
        else 
        {
            mvprintw(14, 2, "[!] Δεν βρέθηκε.");
        }
    }

    registry.updateCourse(id_input, new_id, new_desc, new_sem, newProf);
    
    mvprintw(16, 2, "[+] Έγινε!");
    refresh();
    getch();
}

void TerminalUI::deletePerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν άτομα.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0; 
    int choice = -1;
    bool selecting = true;

    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΔΙΑΓΡΑΦΗ ΑΤΟΜΟΥ ---");

        int lines = max_y - 6; 
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + lines) offset = highlight - lines + 1;

        for (int i = 0; i < lines && (i + offset) < (int)persons.size(); i++) 
        {
            int actual_idx = i + offset;
            if (actual_idx == highlight) attron(A_REVERSE);
            
            std::string role;
            if (persons[actual_idx]->isStudent()) {
                role = persons[actual_idx]->isFemale() ? "Φοιτήτρια" : "Φοιτητής";
            } else {
                role = persons[actual_idx]->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
            }
            
            mvprintw(4 + i, 4, "[%s] %s (%s)", role.c_str(), persons[actual_idx]->getFullname().c_str(), persons[actual_idx]->getAM());
            
            if (actual_idx == highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: highlight = (highlight == 0) ? persons.size() - 1 : highlight - 1; break;
            case KEY_DOWN: highlight = (highlight == (int)persons.size() - 1) ? 0 : highlight + 1; break;
            case 10: choice = highlight; selecting = false; break;
        }
    }

    Person* p = persons[choice];
    
    clear();
    mvprintw(1, 2, "--- ΕΠΙΒΕΒΑΙΩΣΗ ---");
    mvprintw(3, 2, "Θα διαγραφεί: %s (%s)", p->getFullname().c_str(), p->getAM());
    mvprintw(5, 2, "Σίγουρα; (y/n): ");
    
    int confirm = getch();
    
    if (confirm == 'y' || confirm == 'Y') 
    {
        registry.removePerson(*p); 
        mvprintw(7, 2, "[+] Διαγράφηκε.");
    } 
    else 
    {
        mvprintw(7, 2, "[-] Ακυρώθηκε.");
    }

    refresh();
    getch();
}

void TerminalUI::deleteCourse() 
{
    std::vector<Course> courses = registry.getCourses();
    
    if (courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν μαθήματα.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0;
    int choice = -1;
    bool selecting = true;

    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΔΙΑΓΡΑΦΗ ΜΑΘΗΜΑΤΟΣ ---");

        int lines = max_y - 6;
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + lines) offset = highlight - lines + 1;

        for (int i = 0; i < lines && (i + offset) < (int)courses.size(); i++) 
        {
            int actual_idx = i + offset;
            if (actual_idx == highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses[actual_idx].getID().c_str(), courses[actual_idx].getDescription().c_str());
            
            if (actual_idx == highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: highlight = (highlight == 0) ? courses.size() - 1 : highlight - 1; break;
            case KEY_DOWN: highlight = (highlight == (int)courses.size() - 1) ? 0 : highlight + 1; break;
            case 10: choice = highlight; selecting = false; break;
        }
    }

    Course selectedCourse = courses[choice];
    
    clear();
    mvprintw(1, 2, "--- ΕΠΙΒΕΒΑΙΩΣΗ ---");
    mvprintw(3, 2, "Θα διαγραφεί: %s (%s)", selectedCourse.getDescription().c_str(), selectedCourse.getID().c_str());
    mvprintw(5, 2, "Σίγουρα; (y/n): ");
    
    int confirm = getch();
    
    if (confirm == 'y' || confirm == 'Y') 
    {
        registry.removeCourse(selectedCourse); 
        mvprintw(7, 2, "[+] Διαγράφηκε.");
    } 
    else 
    {
        mvprintw(7, 2, "[-] Ακυρώθηκε.");
    }

    refresh();
    getch();
}

void TerminalUI::assignCourseToPerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    std::vector<Course> courses = registry.getCourses();
    
    if (persons.empty() || courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 60) / 2, "Χρειάζεται 1 άτομο και 1 μάθημα.");
        refresh();
        getch();
        return;
    }

    int p_highlight = 0, p_offset = 0, p_choice = -1;
    bool p_selecting = true;

    while (p_selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΔΗΛΩΣΗ (Επιλογή Ατόμου) ---");

        int lines = max_y - 6; 
        if (p_highlight < p_offset) p_offset = p_highlight;
        if (p_highlight >= p_offset + lines) p_offset = p_highlight - lines + 1;

        for (int i = 0; i < lines && (i + p_offset) < (int)persons.size(); i++) 
        {
            int actual_idx = i + p_offset;
            if (actual_idx == p_highlight) attron(A_REVERSE);
            
            std::string role;
            if (persons[actual_idx]->isStudent()) {
                role = persons[actual_idx]->isFemale() ? "Φοιτήτρια" : "Φοιτητής";
            } else {
                role = persons[actual_idx]->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
            }
            
            mvprintw(4 + i, 4, "[%s] %s", role.c_str(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == p_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: p_highlight = (p_highlight == 0) ? persons.size() - 1 : p_highlight - 1; break;
            case KEY_DOWN: p_highlight = (p_highlight == (int)persons.size() - 1) ? 0 : p_highlight + 1; break;
            case 10: p_choice = p_highlight; p_selecting = false; break;
        }
    }

    Person* selectedPerson = persons[p_choice];

    int c_highlight = 0, c_offset = 0, c_choice = -1;
    bool c_selecting = true;

    while (c_selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΔΗΛΩΣΗ (Επιλογή Μαθήματος) ---");
        mvprintw(2, 2, "Άτομο: %s", selectedPerson->getFullname().c_str());

        int lines = max_y - 6;
        if (c_highlight < c_offset) c_offset = c_highlight;
        if (c_highlight >= c_offset + lines) c_offset = c_highlight - lines + 1;

        for (int i = 0; i < lines && (i + c_offset) < (int)courses.size(); i++) 
        {
            int actual_idx = i + c_offset;
            if (actual_idx == c_highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses[actual_idx].getID().c_str(), courses[actual_idx].getDescription().c_str());
            
            if (actual_idx == c_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: c_highlight = (c_highlight == 0) ? courses.size() - 1 : c_highlight - 1; break;
            case KEY_DOWN: c_highlight = (c_highlight == (int)courses.size() - 1) ? 0 : c_highlight + 1; break;
            case 10: c_choice = c_highlight; c_selecting = false; break;
        }
    }

    clear();
    Course* selectedCourse = registry.findCourse(courses[c_choice].getID());
    
    bool already_has_it = false;
    std::list<const Course*> current_courses = selectedPerson->getCourses();
    
    for (const Course* c : current_courses) 
    {
        if (c->getID() == selectedCourse->getID()) 
        {
            already_has_it = true;
            break;
        }
    }

    if (already_has_it) 
    {
        mvprintw(3, 2, "[!] Το έχει ήδη δηλώσει!");
    } 
    else 
    {
        current_courses.push_back(selectedCourse);
        selectedPerson->setCourses(current_courses);
        mvprintw(4, 2, "[+] Έγινε η δήλωση!");
    }

    refresh();
    getch();
}

void TerminalUI::removeCourseFromPerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν άτομα.");
        refresh();
        getch();
        return;
    }

    int p_highlight = 0, p_offset = 0, p_choice = -1;
    bool p_selecting = true;

    while (p_selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΑΦΑΙΡΕΣΗ (Επιλογή Ατόμου) ---");

        int lines = max_y - 6; 
        if (p_highlight < p_offset) p_offset = p_highlight;
        if (p_highlight >= p_offset + lines) p_offset = p_highlight - lines + 1;

        for (int i = 0; i < lines && (i + p_offset) < (int)persons.size(); i++) 
        {
            int actual_idx = i + p_offset;
            if (actual_idx == p_highlight) attron(A_REVERSE);
            
            std::string role;
            if (persons[actual_idx]->isStudent()) {
                role = persons[actual_idx]->isFemale() ? "Φοιτήτρια" : "Φοιτητής";
            } else {
                role = persons[actual_idx]->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
            }
            
            mvprintw(4 + i, 4, "[%s] %s", role.c_str(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == p_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: p_highlight = (p_highlight == 0) ? persons.size() - 1 : p_highlight - 1; break;
            case KEY_DOWN: p_highlight = (p_highlight == (int)persons.size() - 1) ? 0 : p_highlight + 1; break;
            case 10: p_choice = p_highlight; p_selecting = false; break;
        }
    }

    Person* selectedPerson = persons[p_choice];
    std::list<const Course*> current_courses = selectedPerson->getCourses();

    if (current_courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 55) / 2, "Δεν έχει κανένα μάθημα.");
        refresh();
        getch();
        return;
    }

    std::vector<const Course*> courses_vector(current_courses.begin(), current_courses.end());

    int c_highlight = 0, c_offset = 0, c_choice = -1;
    bool c_selecting = true;

    while (c_selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΑΦΑΙΡΕΣΗ (Επιλογή Μαθήματος) ---");
        mvprintw(2, 2, "Άτομο: %s", selectedPerson->getFullname().c_str());

        int lines = max_y - 6;
        if (c_highlight < c_offset) c_offset = c_highlight;
        if (c_highlight >= c_offset + lines) c_offset = c_highlight - lines + 1;

        for (int i = 0; i < lines && (i + c_offset) < (int)courses_vector.size(); i++) 
        {
            int actual_idx = i + c_offset;
            if (actual_idx == c_highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses_vector[actual_idx]->getID().c_str(), courses_vector[actual_idx]->getDescription().c_str());
            
            if (actual_idx == c_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: c_highlight = (c_highlight == 0) ? courses_vector.size() - 1 : c_highlight - 1; break;
            case KEY_DOWN: c_highlight = (c_highlight == (int)courses_vector.size() - 1) ? 0 : c_highlight + 1; break;
            case 10: c_choice = c_highlight; c_selecting = false; break;
        }
    }

    const Course* courseToRemove = courses_vector[c_choice];

    clear();
    current_courses.remove(courseToRemove);
    selectedPerson->setCourses(current_courses);

    mvprintw(3, 2, "[+] Το μάθημα αφαιρέθηκε!");
    refresh();
    getch();
}

void TerminalUI::assignProfessorToCourse() 
{
    std::vector<Course> courses = registry.getCourses();
    std::vector<Person*> persons = registry.getPersons();
    
    std::vector<Professor*> professors;
    for (Person* p : persons) 
    {
        if (p->isProfessor()) 
        {
            professors.push_back(dynamic_cast<Professor*>(p));
        }
    }

    if (courses.empty() || professors.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 65) / 2, "Χρειάζεται 1 μάθημα και 1 καθηγητής.");
        refresh();
        getch();
        return;
    }

    int c_highlight = 0, c_offset = 0, c_choice = -1;
    bool c_selecting = true;

    while (c_selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΟΡΙΣΜΟΣ ΥΠΕΥΘΥΝΟΥ (Επιλογή Μαθήματος) ---");

        int lines = max_y - 6; 
        if (c_highlight < c_offset) c_offset = c_highlight;
        if (c_highlight >= c_offset + lines) c_offset = c_highlight - lines + 1;

        for (int i = 0; i < lines && (i + c_offset) < (int)courses.size(); i++) 
        {
            int actual_idx = i + c_offset;
            if (actual_idx == c_highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses[actual_idx].getID().c_str(), courses[actual_idx].getDescription().c_str());
            
            if (actual_idx == c_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: c_highlight = (c_highlight == 0) ? courses.size() - 1 : c_highlight - 1; break;
            case KEY_DOWN: c_highlight = (c_highlight == (int)courses.size() - 1) ? 0 : c_highlight + 1; break;
            case 10: c_choice = c_highlight; c_selecting = false; break;
        }
    }

    Course* selectedCourse = registry.findCourse(courses[c_choice].getID());

    int p_highlight = 0, p_offset = 0, p_choice = -1;
    bool p_selecting = true;

    while (p_selecting) 
    {
        clear();
        mvprintw(1, 2, "--- ΟΡΙΣΜΟΣ ΥΠΕΥΘΥΝΟΥ (Επιλογή Καθηγητή) ---");
        mvprintw(2, 2, "Μάθημα: %s", selectedCourse->getDescription().c_str());

        int lines = max_y - 6;
        if (p_highlight < p_offset) p_offset = p_highlight;
        if (p_highlight >= p_offset + lines) p_offset = p_highlight - lines + 1;

        for (int i = 0; i < lines && (i + p_offset) < (int)professors.size(); i++) 
        {
            int actual_idx = i + p_offset;
            if (actual_idx == p_highlight) attron(A_REVERSE);
            
            std::string prof_role = professors[actual_idx]->isFemale() ? "Καθηγήτρια" : "Καθηγητής";
            
            mvprintw(4 + i, 4, "[%s] %s | Ειδ.: %s", 
                     prof_role.c_str(),
                     professors[actual_idx]->getFullname().c_str(),
                     professors[actual_idx]->getSpecialty().c_str());
            
            if (actual_idx == p_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case 27: return; 
            case KEY_UP: p_highlight = (p_highlight == 0) ? professors.size() - 1 : p_highlight - 1; break;
            case KEY_DOWN: p_highlight = (p_highlight == (int)professors.size() - 1) ? 0 : p_highlight + 1; break;
            case 10: p_choice = p_highlight; p_selecting = false; break;
        }
    }

    Professor* selectedProf = professors[p_choice];

    clear();
    selectedCourse->setMainProfessor(selectedProf);

    bool already_teaching = false;
    std::list<const Course*> current_courses = selectedProf->getCourses();
    for (const Course* c : current_courses) 
    {
        if (c->getID() == selectedCourse->getID()) 
        {
            already_teaching = true;
            break;
        }
    }
    
    if (!already_teaching) 
    {
        current_courses.push_back(selectedCourse);
        selectedProf->setCourses(current_courses);
    }

    std::string article = selectedProf->isFemale() ? "Η" : "Ο";
    std::string resp = selectedProf->isFemale() ? "Υπεύθυνη" : "Υπεύθυνος";
    
    mvprintw(3, 2, "[+] %s %s ορίστηκε %s!", article.c_str(), selectedProf->getFullname().c_str(), resp.c_str());

    refresh();
    getch();
}

void TerminalUI::sendEmailToStudents() {
    clear();
    mvprintw(1, 2, "--- ΑΠΟΣΤΟΛΗ E-MAIL ΣΕ ΦΟΙΤΗΤΕΣ ---");
    char subject[100];
    if (!getStringInput("Θέμα e-mail: ", subject, 99, 3, 2)) return;

    clear();
    mvprintw(1, 2, "--- LOG ΑΠΟΣΤΟΛΗΣ (Φοιτητές) ---");
    int line = 3;
    int count = 0;
    
    for (Person* p : registry.getPersons()) {
        if (p->isStudent()) {
            mvprintw(line++, 2, "[Στάλθηκε] Προς: %s (ΑΜ: %s)", p->getFullname().c_str(), p->getAM());
            count++;
        }
    }
    
    mvprintw(line + 1, 2, "Ολοκληρώθηκε! Στάλθηκαν %d μηνύματα.", count);
    mvprintw(max_y - 1, 2, "Πατήστε ένα πλήκτρο...");
    refresh(); getch();
}

void TerminalUI::sendEmailToProfessors() {
    clear();
    mvprintw(1, 2, "--- ΑΠΟΣΤΟΛΗ E-MAIL ΣΕ ΚΑΘΗΓΗΤΕΣ ---");
    char subject[100];
    if (!getStringInput("Θέμα e-mail: ", subject, 99, 3, 2)) return;

    clear();
    mvprintw(1, 2, "--- LOG ΑΠΟΣΤΟΛΗΣ (Καθηγητές) ---");
    int line = 3;
    int count = 0;
    
    for (Person* p : registry.getPersons()) {
        if (p->isProfessor()) {
            mvprintw(line++, 2, "[Στάλθηκε] Προς: %s (ΑΜ: %s)", p->getFullname().c_str(), p->getAM());
            count++;
        }
    }
    
    mvprintw(line + 1, 2, "Ολοκληρώθηκε! Στάλθηκαν %d μηνύματα.", count);
    mvprintw(max_y - 1, 2, "Πατήστε ένα πλήκτρο...");
    refresh(); getch();
}

void TerminalUI::run() 
{
    int choice = 0;
    int highlight = 0;
    const int num_choices = 16; 

    bool isRunning = true;

    while (isRunning) 
    {
        drawMainMenu(highlight);
        
        int c = getch(); 
        switch (c) 
        {
            case KEY_UP:
                highlight = (highlight == 0) ? (num_choices - 1) : (highlight - 1);
                break;
            case KEY_DOWN:
                highlight = (highlight == num_choices - 1) ? 0 : (highlight + 1);
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }

        if (c == 10) 
        {
            if (choice == 0) {
                viewPersons();
            } else if (choice == 1) {
                viewCourses();
            } else if (choice == 2) {
                searchPerson();
            } else if (choice == 3) {
                searchCourse();
            } else if (choice == 4) {
                addPerson();
            } else if (choice == 5) {
                updatePerson();
            } else if (choice == 6) {
                addCourse();
            } else if (choice == 7) {
                updateCourse();
            } else if (choice == 8) {
                deletePerson();
            } else if (choice == 9) {
                deleteCourse();
            } else if (choice == 10) {
                assignCourseToPerson();
            } else if (choice == 11) {
                removeCourseFromPerson();
            } else if (choice == 12) {
                assignProfessorToCourse();
            } else if (choice == 13) {
                sendEmailToStudents();
            } else if (choice == 14){
                sendEmailToProfessors();
            } else if (choice == 15) {
                try {
                    registry.saveCoursesToCsv("courses.csv");
                    registry.saveProfessorsToCsv("professors.csv");
                    registry.saveStudentsToCsv("students.csv");
                    showMessage("Τα δεδομένα αποθηκεύτηκαν επιτυχώς!");
                } catch (MyException& e) {
                    showMessage("Σφάλμα: " + e.getErrorMessage());
                }
                isRunning = false; 
            }
        }
    }
}