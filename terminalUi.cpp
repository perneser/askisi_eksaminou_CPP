#include "terminalUi.h"
#include <locale.h>
#include "myException.h"
#include "person.h"
#include "professor.h"
#include "registry.h"
#include "student.h"
#include "course.h"

// Constructor: Αρχικοποίηση του ncurses
TerminalUI::TerminalUI(Registry& reg) : registry(reg) 
{
    setlocale(LC_ALL, ""); // Υποστήριξη Ελληνικών
    initscr();             // Έναρξη ncurses
    clear();
    noecho();              // Να μην τυπώνονται τα πλήκτρα που πατάει ο χρήστης
    cbreak();              // Άμεση ανάγνωση πλήκτρων (χωρίς Enter)
    curs_set(0);           // Απόκρυψη του κέρσορα που αναβοσβήνει
    keypad(stdscr, TRUE);  // Ενεργοποίηση ειδικών πλήκτρων (βελάκια, F1-F12)
    
    getmaxyx(stdscr, max_y, max_x); // Παίρνουμε τις διαστάσεις της οθόνης
}

// Destructor: Τερματισμός του ncurses
TerminalUI::~TerminalUI() 
{
    endwin();
}

// Βοηθητική μέθοδος για εμφάνιση μηνυμάτων
void TerminalUI::showMessage(const std::string& msg) 
{
    clear();
    mvprintw(max_y / 2, (max_x - msg.length()) / 2, "%s", msg.c_str());
    mvprintw((max_y / 2) + 2, (max_x - 30) / 2, "Πατήστε οποιοδήποτε πλήκτρο...");
    refresh();
    getch();
}

// Σχεδιασμός του Κεντρικού Μενού
void TerminalUI::drawMainMenu(int highlight) 
{
    clear();
    
    // Τίτλος
    std::string title = "=== ΣΥΣΤΗΜΑ ΦΟΙΤΗΤΟΛΟΓΙΟΥ ===";
    mvprintw(2, (max_x - title.length()) / 2, "%s", title.c_str());

// Επιλογές Μενού
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
        "12. Αφαίρεση Μαθήματος από Φοιτητή/τρια ή Καθηγητή/τρια", // <-- Η ΝΕΑ ΕΠΙΛΟΓΗ
        "13. Ορισμός Υπεύθυνου Καθηγητή σε Μάθημα",
        "14. Αποθήκευση και Έξοδος"
    };
    

    int start_y = 5;
    for (size_t i = 0; i < choices.size(); i++) 
    {
        if (highlight == i) 
        {
            attron(A_REVERSE); // Επισήμανση (highlight) της επιλεγμένης γραμμής
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

// Υλοποίηση της Προβολής Ατόμων
void TerminalUI::viewPersons() 
{
    clear(); // Καθαρίζουμε το μενού
    
    // Τυπώνουμε τον τίτλο στην κορυφή
    mvprintw(1, 2, "=== ΛΙΣΤΑ ΦΟΙΤΗΤΟΛΟΓΙΟΥ (ΑΤΟΜΑ) ===");
    
    // Τραβάμε τα δεδομένα από τη Γραμματεία
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        mvprintw(3, 2, "Δεν βρέθηκαν άτομα στο σύστημα.");
    } 
    else 
    {
        int line = 3; // Ξεκινάμε την εκτύπωση από την 3η γραμμή
        for (const Person* p : persons) 
        {
            // Προστασία: Αν φτάσουμε στο τέλος της οθόνης, σταματάμε να τυπώνουμε
            if (line >= max_y - 2) 
            { 
                mvprintw(line, 2, "...(η λίστα συνεχίζεται, πατήστε ένα πλήκτρο)...");
                break;
            }
            
            // Βρίσκουμε τον ρόλο του ατόμου
            std::string role;
            if (p->isStudent())
            {
                if (p->isFemale())
                    role = "Φοιτήτρια";
                else
                    role = "Φοιτητής";
            } else if (p->isProfessor())
            {
                if (p->isFemale())
                    role = "Καθηγήτρια";
                else
                    role = "Καθηγητής";
            } 
            else 
            {
                role = "Άγνωστος Ρόλος";
            }
            
            // Εκτύπωση των στοιχείων (χρησιμοποιούμε .c_str() επειδή η printw δέχεται C-strings)
            mvprintw(line, 2, "[%s] ΑΜ: %s | Όνομα: %s | Φύλο: %s", 
                     role.c_str(), p->getAM(), p->getFullname().c_str(), p->getGender().c_str());
            line++;
        }
    }
    
    // Μήνυμα εξόδου στο κάτω μέρος της οθόνης
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh(); // Εμφάνιση των αλλαγών
    getch();   // Αναμονή για πάτημα πλήκτρου
}

// Υλοποίηση της Προβολής Μαθημάτων
void TerminalUI::viewCourses() 
{
    clear(); 
    mvprintw(1, 2, "=== ΛΙΣΤΑ ΜΑΘΗΜΑΤΩΝ ===");
    
    std::vector<Course> courses = registry.getCourses();
    
    if (courses.empty()) 
    {
        mvprintw(3, 2, "Δεν βρέθηκαν μαθήματα στο σύστημα.");
    } 
    else 
    {
        int line = 3;
        for (const auto& c : courses) 
        {
            if (line >= max_y - 2) 
            {
                mvprintw(line, 2, "...(η λίστα συνεχίζεται, πατήστε ένα πλήκτρο)...");
                break;
            }
            
            // Ελέγχουμε αν υπάρχει υπεύθυνος καθηγητής για να πάρουμε το όνομά του
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
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση της Αναζήτησης Ατόμου
void TerminalUI::searchPerson() 
{
    clear();
    mvprintw(1, 2, "=== ΑΝΑΖΗΤΗΣΗ ΑΤΟΜΟΥ ΒΑΣΕΙ Α.Μ. ===");
    mvprintw(3, 2, "Πληκτρολογήστε τον Αριθμό Μητρώου: ");
    
    // ΠΡΟΣΟΧΗ: Ρυθμίσεις για να διαβάσουμε κείμενο
    echo();       // Ενεργοποιούμε την εμφάνιση πληκτρολόγησης
    curs_set(1);  // Εμφανίζουμε τον κέρσορα για να ξέρει πού γράφει
    
    char am_input[50];
    getnstr(am_input, 49); // Διαβάζει αυτό που γράφει ο χρήστης μέχρι να πατήσει Enter
    
    // Επαναφορά των ρυθμίσεων του μενού
    noecho();
    curs_set(0);
    
    // Ψάχνουμε το άτομο χρησιμοποιώντας τον κώδικα της Γραμματείας σου
    Person* p = registry.findPerson(std::string(am_input));
    
    if (p != nullptr) 
    {
        mvprintw(5, 2, "-> ΒΡΕΘΗΚΕ: %s", p->getFullname().c_str());
        mvprintw(6, 2, "-> Φύλο: %s", p->getGender().c_str());
        
        std::string role;
        if (p->isStudent())
        {
            if (p->isFemale())
                role = "Φοιτήτρια";
            else
                role = "Φοιτητής";
        } 
        else if (p->isProfessor())
        {
            if (p->isFemale())
                role = "Καθηγήτρια";
            else
                role = "Καθηγητής";
        } 
        else 
        {
            role = "Άγνωστος Ρόλος";
        }
        mvprintw(7, 2, "-> Ιδιότητα: %s", role.c_str());
        
        mvprintw(9, 2, "[ Σχετιζόμενα Μαθήματα ]");
        int line = 10;
        auto courses = p->getCourses();
        
        if (courses.empty()) 
        {
            mvprintw(line, 4, "- Κανένα μάθημα");
        } 
        else 
        {
            for (const auto* c : courses) 
            {
                mvprintw(line++, 4, "- %s (%s)", c->getDescription().c_str(), c->getID().c_str());
            }
        }
    } 
    else 
    {
        mvprintw(5, 2, "[!] Δεν βρέθηκε άτομο με ΑΜ: %s", am_input);
    }
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Αναζήτησης Μαθήματος
void TerminalUI::searchCourse() 
{
    clear();
    mvprintw(1, 2, "=== ΑΝΑΖΗΤΗΣΗ ΜΑΘΗΜΑΤΟΣ ΒΑΣΕΙ ΚΩΔΙΚΟΥ ===");
    mvprintw(3, 2, "Πληκτρολογήστε τον Κωδικό του Μαθήματος: ");
    
    echo();       
    curs_set(1);  
    
    char id_input[50];
    getnstr(id_input, 49); 
    
    noecho();
    curs_set(0);
    
    // Αναζήτηση του μαθήματος στη Γραμματεία
    Course* c = registry.findCourse(std::string(id_input));
    
    if (c != nullptr) 
    {
        mvprintw(5, 2, "-> ΒΡΕΘΗΚΕ: %s", c->getDescription().c_str());
        mvprintw(6, 2, "-> Κωδικός: %s", c->getID().c_str());
        mvprintw(7, 2, "-> Εξάμηνο: %d", c->getSemester());
        
        std::string profName = "Κανένας";
        if (c->getMainProfessor() != nullptr) 
        {
            profName = c->getMainProfessor()->getFullname();
        }
        mvprintw(8, 2, "-> Υπεύθυνος Καθηγητής: %s", profName.c_str());
    } 
    else 
    {
        mvprintw(5, 2, "[!] Δεν βρέθηκε μάθημα με Κωδικό: %s", id_input);
    }
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Τροποποίησης Ατόμου (με οπτική επιλογή)
void TerminalUI::updatePerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν άτομα στο σύστημα για τροποποίηση.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0; // Βοηθάει στην κύλιση αν η λίστα είναι μεγάλη
    int choice = -1;
    bool selecting = true;

    // --- ΜΕΡΟΣ 1ο: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ---
    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΕΠΙΛΟΓΗ ΑΤΟΜΟΥ ΠΡΟΣ ΤΡΟΠΟΠΟΙΗΣΗ ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter για επιλογή.");

        int max_display = max_y - 6; // Πόσες γραμμές χωράνε στην οθόνη
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + max_display) offset = highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + offset) < persons.size(); i++) 
        {
            size_t actual_idx = i + offset;
            
            if (actual_idx == highlight) attron(A_REVERSE);
            
            std::string role = persons[actual_idx]->isStudent() ? "Φοιτητής/τρια" : "Καθηγητής/τρια";
            mvprintw(4 + i, 4, "[%s] ΑΜ: %s | %s", role.c_str(), persons[actual_idx]->getAM(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: 
                highlight = (highlight == 0) ? persons.size() - 1 : highlight - 1; 
                break;
            case KEY_DOWN: 
                highlight = (highlight == persons.size() - 1) ? 0 : highlight + 1; 
                break;
            case 10: 
                choice = highlight; 
                selecting = false; 
                break;
        }
    }

    // --- ΜΕΡΟΣ 2ο: ΤΡΟΠΟΠΟΙΗΣΗ ΣΤΟΙΧΕΙΩΝ ---
    Person* p = persons[choice];
    std::string am_input = p->getAM(); // Αποθηκεύουμε τον ΑΜ από την επιλογή
    
    clear();
    mvprintw(1, 2, "=== ΤΡΟΠΟΠΟΙΗΣΗ ΣΤΟΙΧΕΙΩΝ ΑΤΟΜΟΥ ===");
    mvprintw(3, 2, "Επιλέχθηκε: %s (ΑΜ: %s)", p->getFullname().c_str(), am_input.c_str());
    
    echo();       
    curs_set(1);  

    // Ζητάμε νέο όνομα
    mvprintw(5, 2, "-> Τρέχον Όνομα: %s", p->getFullname().c_str());
    mvprintw(6, 2, "Νέο Όνομα (Αφήστε κενό και πατήστε Enter για διατήρηση): ");
    char name_input[100];
    getnstr(name_input, 99);
    std::string new_name = (strlen(name_input) > 0) ? name_input : p->getFullname();
    
    // Ζητάμε νέο φύλο
    mvprintw(7, 2, "-> Τρέχον Φύλο: %s", p->getGender().c_str());
    mvprintw(8, 2, "Νέο Φύλο (Αφήστε κενό και πατήστε Enter για διατήρηση): ");
    char gender_input[50];
    getnstr(gender_input, 49);
    std::string new_gender = (strlen(gender_input) > 0) ? gender_input : p->getGender();

    if (p->isStudent()) 
    {
        Student* s = dynamic_cast<Student*>(p);
        mvprintw(9, 2, "-> Τρέχον Εξάμηνο: %d", s->getSemester());
        mvprintw(10, 2, "Νέο Εξάμηνο (Αφήστε κενό και πατήστε Enter για διατήρηση): ");
        char sem_input[10];
        getnstr(sem_input, 9);
        
        int new_sem = s->getSemester();
        if (strlen(sem_input) > 0) {
            try { new_sem = std::stoi(sem_input); } catch(...) {} 
        }

        noecho();
        curs_set(0);

        // ΠΡΟΣΟΧΗ ΕΔΩ: Χρησιμοποιούμε am_input.c_str()
        Student tempStudent(am_input.c_str(), new_name, new_gender, new_sem);
        tempStudent.setCourses(p->getCourses()); 
        registry.updatePerson(am_input.c_str(), tempStudent);
    }
    else if (p->isProfessor()) 
    {
        Professor* prof = dynamic_cast<Professor*>(p);
        mvprintw(9, 2, "-> Τρέχουσα Ειδικότητα: %s", prof->getSpecialty().c_str());
        mvprintw(10, 2, "Νέα Ειδικότητα (Αφήστε κενό και πατήστε Enter για διατήρηση): ");
        char spec_input[100];
        getnstr(spec_input, 99);
        std::string new_spec = (strlen(spec_input) > 0) ? spec_input : prof->getSpecialty();

        noecho();
        curs_set(0);

        // ΠΡΟΣΟΧΗ ΕΔΩ: Χρησιμοποιούμε am_input.c_str()
        Professor tempProf(am_input.c_str(), new_name, new_gender, new_spec);
        tempProf.setCourses(p->getCourses()); 
        registry.updatePerson(am_input.c_str(), tempProf);
    }
    
    mvprintw(12, 2, "[+] Η τροποποίηση ολοκληρώθηκε επιτυχώς!");
    noecho();
    curs_set(0);
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Προσθήκης Νέου Ατόμου (Χωρίς έλεγχο φύλου)
void TerminalUI::addPerson() 
{
    clear();
    mvprintw(1, 2, "=== ΠΡΟΣΘΗΚΗ ΝΕΟΥ ΑΤΟΜΟΥ ===");
    mvprintw(3, 2, "Επιλέξτε Ιδιότητα (1 = Φοιτητής, 2 = Καθηγητής): ");
    
    echo();
    curs_set(1);
    
    char type_input[10];
    getnstr(type_input, 9);
    std::string type_str(type_input);
    
    if (type_str != "1" && type_str != "2") 
    {
        noecho();
        curs_set(0);
        mvprintw(5, 2, "[!] Σφάλμα: Μη έγκυρη επιλογή. Πρέπει να είναι 1 ή 2.");
        mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή...");
        getch();
        return;
    }
    
    mvprintw(5, 2, "Αριθμός Μητρώου (ΑΜ): ");
    char am_input[50];
    getnstr(am_input, 49);
    
    // Έλεγχος αν υπάρχει ήδη
    if (registry.findPerson(std::string(am_input)) != nullptr) 
    {
        noecho();
        curs_set(0);
        mvprintw(7, 2, "[!] Σφάλμα: Υπάρχει ήδη άτομο με αυτόν τον ΑΜ!");
        mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή...");
        getch();
        return;
    }
    
    mvprintw(6, 2, "Ονοματεπώνυμο: ");
    char name_input[100];
    getnstr(name_input, 99);
    
    // Απλή ανάγνωση Φύλου
    mvprintw(7, 2, "Φύλο: ");
    char gender_input[50];
    getnstr(gender_input, 49);
    std::string gender_str(gender_input);
    
    // Ειδικά πεδία ανάλογα με την ιδιότητα
    if (type_str == "1") // Αν είναι Φοιτητής
    {
        mvprintw(8, 2, "Εξάμηνο Φοίτησης: ");
        char sem_input[10];
        getnstr(sem_input, 9);
        int sem = 1;
        try { sem = std::stoi(sem_input); } catch(...) {} // Ασφαλής μετατροπή
        
        Student* newStudent = new Student(am_input, name_input, gender_str, sem);
        registry.addPerson(*newStudent);
    }
    else // Αν είναι Καθηγητής
    {
        mvprintw(8, 2, "Ειδικότητα: ");
        char spec_input[100];
        getnstr(spec_input, 99);
        
        Professor* newProf = new Professor(am_input, name_input, gender_str, spec_input);
        registry.addPerson(*newProf);
    }
    
    noecho();
    curs_set(0);
    
    mvprintw(10, 2, "[+] Το άτομο προστέθηκε επιτυχώς στο Φοιτητολόγιο!");
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Προσθήκης Νέου Μαθήματος
void TerminalUI::addCourse() 
{
    clear();
    mvprintw(1, 2, "=== ΠΡΟΣΘΗΚΗ ΝΕΟΥ ΜΑΘΗΜΑΤΟΣ ===");
    
    echo();
    curs_set(1);
    
    // 1. Ανάγνωση Κωδικού
    mvprintw(3, 2, "Κωδικός Μαθήματος (π.χ. ICE-1234): ");
    char id_input[50];
    getnstr(id_input, 49);
    
    // Έλεγχος αν ο κωδικός υπάρχει ήδη
    if (registry.findCourse(std::string(id_input)) != nullptr) 
    {
        noecho();
        curs_set(0);
        mvprintw(5, 2, "[!] Σφάλμα: Υπάρχει ήδη μάθημα με αυτόν τον κωδικό!");
        mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή...");
        getch();
        return;
    }
    
    // 2. Ανάγνωση Περιγραφής
    mvprintw(4, 2, "Περιγραφή Μαθήματος: ");
    char desc_input[100];
    getnstr(desc_input, 99);
    
    // 3. Ανάγνωση Εξαμήνου
    mvprintw(5, 2, "Εξάμηνο Διδασκαλίας: ");
    char sem_input[10];
    getnstr(sem_input, 9);
    int sem = 1;
    try { sem = std::stoi(sem_input); } catch(...) {} // Ασφαλής μετατροπή
    
    // 4. Υπεύθυνος Καθηγητής
    mvprintw(6, 2, "ΑΜ Υπεύθυνου Καθηγητή (Αφήστε κενό για κανέναν): ");
    char prof_input[50];
    getnstr(prof_input, 49);
    
    Professor* mainProf = nullptr;
    
    // Αν ο χρήστης πληκτρολόγησε κάτι (δεν πάτησε απλά Enter)
    if (strlen(prof_input) > 0) 
    {
        Person* p = registry.findPerson(std::string(prof_input));
        if (p != nullptr && p->isProfessor()) 
        {
            mainProf = dynamic_cast<Professor*>(p);
            mvprintw(7, 2, "-> Συνδέθηκε επιτυχώς με τον/την: %s", mainProf->getFullname().c_str());
        } 
        else 
        {
            mvprintw(7, 2, "[!] Δεν βρέθηκε καθηγητής με αυτόν τον ΑΜ. Θα αποθηκευτεί χωρίς υπεύθυνο.");
        }
    }
    
    noecho();
    curs_set(0);
    
    // Δημιουργία και προσθήκη του μαθήματος στη Γραμματεία
    Course newCourse(id_input, desc_input, sem, mainProf);
    registry.createCourse(newCourse);
    
    mvprintw(9, 2, "[+] Το μάθημα προστέθηκε επιτυχώς στο Φοιτητολόγιο!");
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Τροποποίησης Μαθήματος (με οπτική επιλογή)
void TerminalUI::updateCourse() 
{
    std::vector<Course> courses = registry.getCourses();
    
    if (courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν μαθήματα στο σύστημα για τροποποίηση.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0;
    int choice = -1;
    bool selecting = true;

    // --- ΜΕΡΟΣ 1ο: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ---
    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΕΠΙΛΟΓΗ ΜΑΘΗΜΑΤΟΣ ΠΡΟΣ ΤΡΟΠΟΠΟΙΗΣΗ ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter για επιλογή.");

        int max_display = max_y - 6;
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + max_display) offset = highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + offset) < courses.size(); i++) 
        {
            size_t actual_idx = i + offset;
            
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
            case KEY_UP: 
                highlight = (highlight == 0) ? courses.size() - 1 : highlight - 1; 
                break;
            case KEY_DOWN: 
                highlight = (highlight == courses.size() - 1) ? 0 : highlight + 1; 
                break;
            case 10: 
                choice = highlight; 
                selecting = false; 
                break;
        }
    }

    // --- ΜΕΡΟΣ 2ο: ΤΡΟΠΟΠΟΙΗΣΗ ΣΤΟΙΧΕΙΩΝ ---
    // Βρίσκουμε τον pointer του μαθήματος από την Registry για να το αλλάξουμε
    Course* c = registry.findCourse(courses[choice].getID());
    std::string id_input = c->getID(); 
    
    clear();
    mvprintw(1, 2, "=== ΤΡΟΠΟΠΟΙΗΣΗ ΣΤΟΙΧΕΙΩΝ ΜΑΘΗΜΑΤΟΣ ===");
    mvprintw(3, 2, "Επιλέχθηκε: %s (Κωδικός: %s)", c->getDescription().c_str(), id_input.c_str());
    
    echo();       
    curs_set(1);  
    
    // 1. Ζητάμε νέο Κωδικό
    mvprintw(5, 2, "-> Τρέχων Κωδικός: %s", c->getID().c_str());
    mvprintw(6, 2, "Νέος Κωδικός (Αφήστε κενό για διατήρηση): ");
    char new_id_input[50];
    getnstr(new_id_input, 49);
    std::string new_id = (strlen(new_id_input) > 0) ? new_id_input : c->getID();
    
    // 2. Ζητάμε νέα Περιγραφή
    mvprintw(7, 2, "-> Τρέχουσα Περιγραφή: %s", c->getDescription().c_str());
    mvprintw(8, 2, "Νέα Περιγραφή (Αφήστε κενό για διατήρηση): ");
    char desc_input[100];
    getnstr(desc_input, 99);
    std::string new_desc = (strlen(desc_input) > 0) ? desc_input : c->getDescription();

    // 3. Ζητάμε νέο Εξάμηνο
    mvprintw(9, 2, "-> Τρέχον Εξάμηνο: %d", c->getSemester());
    mvprintw(10, 2, "Νέο Εξάμηνο (Αφήστε κενό για διατήρηση): ");
    char sem_input[10];
    getnstr(sem_input, 9);
    int new_sem = c->getSemester();
    if (strlen(sem_input) > 0) {
        try { new_sem = std::stoi(sem_input); } catch(...) {} 
    }

    // 4. Ζητάμε νέο Υπεύθυνο Καθηγητή
    std::string profName = "Κανένας";
    if (c->getMainProfessor() != nullptr) {
        profName = c->getMainProfessor()->getFullname();
    }
    
    mvprintw(11, 2, "-> Τρέχων Υπεύθυνος: %s", profName.c_str());
    mvprintw(12, 2, "Νέος Υπεύθυνος ΑΜ (Κενό=διατήρηση, '-'=Διαγραφή υπευθύνου): ");
    char prof_input[50];
    getnstr(prof_input, 49);
    
    Professor* newProf = c->getMainProfessor(); 
    
    if (strcmp(prof_input, "-") == 0) 
    {
        newProf = nullptr; 
        mvprintw(13, 2, "-> Ο υπεύθυνος καθηγητής αφαιρέθηκε.");
    } 
    else if (strlen(prof_input) > 0) 
    {
        Person* p = registry.findPerson(std::string(prof_input));
        if (p != nullptr && p->isProfessor()) 
        {
            newProf = dynamic_cast<Professor*>(p);
            mvprintw(13, 2, "-> Νέος Υπεύθυνος: %s", newProf->getFullname().c_str());
        } 
        else 
        {
            mvprintw(13, 2, "[!] Δεν βρέθηκε καθηγητής με αυτόν τον ΑΜ. Διατηρείται ο παλιός.");
        }
    }

    noecho();
    curs_set(0);

    registry.updateCourse(id_input, new_id, new_desc, new_sem, newProf);
    
    mvprintw(15, 2, "[+] Η τροποποίηση του μαθήματος ολοκληρώθηκε επιτυχώς!");
    
    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}
// Υλοποίηση Διαγραφής Ατόμου (με οπτική επιλογή)
void TerminalUI::deletePerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν άτομα στο σύστημα για διαγραφή.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0; 
    int choice = -1;
    bool selecting = true;

    // --- ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ---
    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΔΙΑΓΡΑΦΗ ΑΤΟΜΟΥ ΑΠΟ ΤΟ ΦΟΙΤΗΤΟΛΟΓΙΟ ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter για επιλογή.");

        int max_display = max_y - 6; 
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + max_display) offset = highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + offset) < persons.size(); i++) 
        {
            size_t actual_idx = i + offset;
            
            if (actual_idx == highlight) attron(A_REVERSE);
            
            std::string role = persons[actual_idx]->isStudent() ? "Φοιτητής/τρια" : "Καθηγητής/τρια";
            mvprintw(4 + i, 4, "[%s] ΑΜ: %s | %s", role.c_str(), persons[actual_idx]->getAM(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: 
                highlight = (highlight == 0) ? persons.size() - 1 : highlight - 1; 
                break;
            case KEY_DOWN: 
                highlight = (highlight == persons.size() - 1) ? 0 : highlight + 1; 
                break;
            case 10: 
                choice = highlight; 
                selecting = false; 
                break;
        }
    }

    // --- ΕΠΙΒΕΒΑΙΩΣΗ ΚΑΙ ΔΙΑΓΡΑΦΗ ---
    Person* p = persons[choice];
    
    clear();
    mvprintw(1, 2, "=== ΕΠΙΒΕΒΑΙΩΣΗ ΔΙΑΓΡΑΦΗΣ ===");
    mvprintw(3, 2, "Πρόκειται να διαγράψετε: %s (ΑΜ: %s)", p->getFullname().c_str(), p->getAM());
    mvprintw(5, 2, "Είστε σίγουροι; Πατήστε 'y' για ΝΑΙ ή οποιοδήποτε άλλο πλήκτρο για ΑΚΥΡΩΣΗ: ");
    
    int confirm = getch();
    
    if (confirm == 'y' || confirm == 'Y') 
    {
        // Αναλόγως πώς έχεις ορίσει την removePerson, αν παίρνει const char* ή Object reference
        // Αν η συνάρτησή σου δέχεται pointer/reference (όπως είχαμε πει πριν):
        registry.removePerson(*p); 
        mvprintw(7, 2, "[+] Το άτομο διαγράφηκε επιτυχώς από το σύστημα!");
    } 
    else 
    {
        mvprintw(7, 2, "[-] Η διαγραφή ακυρώθηκε.");
    }

    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Διαγραφής Μαθήματος (με οπτική επιλογή)
void TerminalUI::deleteCourse() 
{
    std::vector<Course> courses = registry.getCourses();
    
    if (courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν μαθήματα στο σύστημα για διαγραφή.");
        refresh();
        getch();
        return;
    }

    int highlight = 0;
    int offset = 0;
    int choice = -1;
    bool selecting = true;

    // --- ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ---
    while (selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΔΙΑΓΡΑΦΗ ΜΑΘΗΜΑΤΟΣ ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter για επιλογή.");

        int max_display = max_y - 6;
        if (highlight < offset) offset = highlight;
        if (highlight >= offset + max_display) offset = highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + offset) < courses.size(); i++) 
        {
            size_t actual_idx = i + offset;
            
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
            case KEY_UP: 
                highlight = (highlight == 0) ? courses.size() - 1 : highlight - 1; 
                break;
            case KEY_DOWN: 
                highlight = (highlight == courses.size() - 1) ? 0 : highlight + 1; 
                break;
            case 10: 
                choice = highlight; 
                selecting = false; 
                break;
        }
    }

    // --- ΕΠΙΒΕΒΑΙΩΣΗ ΚΑΙ ΔΙΑΓΡΑΦΗ ---
    Course selectedCourse = courses[choice];
    
    clear();
    mvprintw(1, 2, "=== ΕΠΙΒΕΒΑΙΩΣΗ ΔΙΑΓΡΑΦΗΣ ===");
    mvprintw(3, 2, "Πρόκειται να διαγράψετε το μάθημα: %s (%s)", selectedCourse.getDescription().c_str(), selectedCourse.getID().c_str());
    mvprintw(5, 2, "Είστε σίγουροι; Πατήστε 'y' για ΝΑΙ ή οποιοδήποτε άλλο πλήκτρο για ΑΚΥΡΩΣΗ: ");
    
    int confirm = getch();
    
    if (confirm == 'y' || confirm == 'Y') 
    {
        // Αναλόγως πώς έχεις ορίσει την removeCourse. Αν δέχεται Course object:
        registry.removeCourse(selectedCourse); 
        mvprintw(7, 2, "[+] Το μάθημα διαγράφηκε επιτυχώς από το σύστημα!");
    } 
    else 
    {
        mvprintw(7, 2, "[-] Η διαγραφή ακυρώθηκε.");
    }

    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Δήλωσης Μαθήματος σε Άτομο
void TerminalUI::assignCourseToPerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    std::vector<Course> courses = registry.getCourses();
    
    if (persons.empty() || courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 60) / 2, "Απαιτούνται τουλάχιστον 1 άτομο και 1 μάθημα στο σύστημα.");
        refresh();
        getch();
        return;
    }

    // --- ΒΗΜΑ 1: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ΑΤΟΜΟΥ ---
    int p_highlight = 0, p_offset = 0, p_choice = -1;
    bool p_selecting = true;

    while (p_selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΔΗΛΩΣΗ ΜΑΘΗΜΑΤΟΣ: ΒΗΜΑ 1/2 (Επιλογή Ατόμου) ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter.");

        int max_display = max_y - 6; 
        if (p_highlight < p_offset) p_offset = p_highlight;
        if (p_highlight >= p_offset + max_display) p_offset = p_highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + p_offset) < persons.size(); i++) 
        {
            size_t actual_idx = i + p_offset;
            if (actual_idx == p_highlight) attron(A_REVERSE);
            
            std::string role = persons[actual_idx]->isStudent() ? "Φοιτητής/τρια" : "Καθηγητής/τρια";
            mvprintw(4 + i, 4, "[%s] ΑΜ: %s | %s", role.c_str(), persons[actual_idx]->getAM(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == p_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: p_highlight = (p_highlight == 0) ? persons.size() - 1 : p_highlight - 1; break;
            case KEY_DOWN: p_highlight = (p_highlight == persons.size() - 1) ? 0 : p_highlight + 1; break;
            case 10: p_choice = p_highlight; p_selecting = false; break;
        }
    }

    Person* selectedPerson = persons[p_choice];

    // --- ΒΗΜΑ 2: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ΜΑΘΗΜΑΤΟΣ ---
    int c_highlight = 0, c_offset = 0, c_choice = -1;
    bool c_selecting = true;

    while (c_selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΔΗΛΩΣΗ ΜΑΘΗΜΑΤΟΣ: ΒΗΜΑ 2/2 (Επιλογή Μαθήματος) ===");
        mvprintw(2, 2, "Άτομο: %s | Επιλέξτε μάθημα με τα βελάκια:", selectedPerson->getFullname().c_str());

        int max_display = max_y - 6;
        if (c_highlight < c_offset) c_offset = c_highlight;
        if (c_highlight >= c_offset + max_display) c_offset = c_highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + c_offset) < courses.size(); i++) 
        {
            size_t actual_idx = i + c_offset;
            if (actual_idx == c_highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses[actual_idx].getID().c_str(), courses[actual_idx].getDescription().c_str());
            
            if (actual_idx == c_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: c_highlight = (c_highlight == 0) ? courses.size() - 1 : c_highlight - 1; break;
            case KEY_DOWN: c_highlight = (c_highlight == courses.size() - 1) ? 0 : c_highlight + 1; break;
            case 10: c_choice = c_highlight; c_selecting = false; break;
        }
    }

    // --- ΒΗΜΑ 3: ΣΥΝΔΕΣΗ ΔΕΔΟΜΕΝΩΝ ---
    clear();
    mvprintw(1, 2, "=== ΑΠΟΤΕΛΕΣΜΑ ΔΗΛΩΣΗΣ ===");

    // Παίρνουμε τον πραγματικό pointer του μαθήματος από την Registry
    Course* selectedCourse = registry.findCourse(courses[c_choice].getID());
    
    // Ελέγχουμε αν το έχει ήδη δηλώσει 
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
        mvprintw(3, 2, "[!] Το άτομο έχει ήδη δηλωμένο αυτό το μάθημα!");
    } 
    else 
    {
        // Προσθήκη του μαθήματος στη λίστα (χωρίς αυτόματο ορισμό υπευθύνου)
        current_courses.push_back(selectedCourse);
        selectedPerson->setCourses(current_courses);

        mvprintw(4, 2, "[+] Η δήλωση του μαθήματος ολοκληρώθηκε επιτυχώς!");
    }

    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Αφαίρεσης Μαθήματος από Άτομο
void TerminalUI::removeCourseFromPerson() 
{
    std::vector<Person*> persons = registry.getPersons();
    
    if (persons.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 45) / 2, "Δεν υπάρχουν άτομα στο σύστημα.");
        refresh();
        getch();
        return;
    }

    // --- ΒΗΜΑ 1: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ΑΤΟΜΟΥ ---
    int p_highlight = 0, p_offset = 0, p_choice = -1;
    bool p_selecting = true;

    while (p_selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΑΦΑΙΡΕΣΗ ΜΑΘΗΜΑΤΟΣ: ΒΗΜΑ 1/2 (Επιλογή Ατόμου) ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter.");

        int max_display = max_y - 6; 
        if (p_highlight < p_offset) p_offset = p_highlight;
        if (p_highlight >= p_offset + max_display) p_offset = p_highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + p_offset) < persons.size(); i++) 
        {
            size_t actual_idx = i + p_offset;
            if (actual_idx == p_highlight) attron(A_REVERSE);
            
            std::string role = persons[actual_idx]->isStudent() ? "Φοιτητής/τρια" : "Καθηγητής/τρια";
            mvprintw(4 + i, 4, "[%s] ΑΜ: %s | %s", role.c_str(), persons[actual_idx]->getAM(), persons[actual_idx]->getFullname().c_str());
            
            if (actual_idx == p_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: p_highlight = (p_highlight == 0) ? persons.size() - 1 : p_highlight - 1; break;
            case KEY_DOWN: p_highlight = (p_highlight == persons.size() - 1) ? 0 : p_highlight + 1; break;
            case 10: p_choice = p_highlight; p_selecting = false; break;
        }
    }

    Person* selectedPerson = persons[p_choice];
    std::list<const Course*> current_courses = selectedPerson->getCourses();

    // Αν το άτομο δεν έχει κανένα μάθημα, σταματάμε
    if (current_courses.empty()) 
    {
        clear();
        mvprintw(max_y / 2, (max_x - 55) / 2, "Το συγκεκριμένο άτομο δεν έχει κανένα δηλωμένο μάθημα.");
        refresh();
        getch();
        return;
    }

    // Μετατρέπουμε προσωρινά τη λίστα σε vector ΜΟΝΟ για να τη διαχειριστούμε εύκολα στο UI με δείκτες
    std::vector<const Course*> courses_vector(current_courses.begin(), current_courses.end());

    // --- ΒΗΜΑ 2: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ΜΑΘΗΜΑΤΟΣ ΠΡΟΣ ΑΦΑΙΡΕΣΗ ---
    int c_highlight = 0, c_offset = 0, c_choice = -1;
    bool c_selecting = true;

    while (c_selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΑΦΑΙΡΕΣΗ ΜΑΘΗΜΑΤΟΣ: ΒΗΜΑ 2/2 (Επιλογή Μαθήματος) ===");
        mvprintw(2, 2, "Άτομο: %s | Επιλέξτε μάθημα προς αφαίρεση:", selectedPerson->getFullname().c_str());

        int max_display = max_y - 6;
        if (c_highlight < c_offset) c_offset = c_highlight;
        if (c_highlight >= c_offset + max_display) c_offset = c_highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + c_offset) < courses_vector.size(); i++) 
        {
            size_t actual_idx = i + c_offset;
            if (actual_idx == c_highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses_vector[actual_idx]->getID().c_str(), courses_vector[actual_idx]->getDescription().c_str());
            
            if (actual_idx == c_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: c_highlight = (c_highlight == 0) ? courses_vector.size() - 1 : c_highlight - 1; break;
            case KEY_DOWN: c_highlight = (c_highlight == courses_vector.size() - 1) ? 0 : c_highlight + 1; break;
            case 10: c_choice = c_highlight; c_selecting = false; break;
        }
    }

    const Course* courseToRemove = courses_vector[c_choice];

    // --- ΒΗΜΑ 3: ΟΡΙΣΤΙΚΗ ΑΦΑΙΡΕΣΗ ---
    clear();
    mvprintw(1, 2, "=== ΑΠΟΤΕΛΕΣΜΑ ΑΦΑΙΡΕΣΗΣ ===");

    // Αφαιρούμε το μάθημα από τη λίστα του ατόμου
    current_courses.remove(courseToRemove);
    selectedPerson->setCourses(current_courses);

    mvprintw(3, 2, "[+] Το μάθημα [%s] αφαιρέθηκε επιτυχώς από τη λίστα του ατόμου!", courseToRemove->getID().c_str());

    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Υλοποίηση Ορισμού Υπεύθυνου Καθηγητή σε Μάθημα
void TerminalUI::assignProfessorToCourse() 
{
    std::vector<Course> courses = registry.getCourses();
    std::vector<Person*> persons = registry.getPersons();
    
    // Φιλτράρουμε τη λίστα για να κρατήσουμε ΜΟΝΟ τους καθηγητές
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
        mvprintw(max_y / 2, (max_x - 65) / 2, "Απαιτούνται τουλάχιστον 1 μάθημα και 1 καθηγητής στο σύστημα.");
        refresh();
        getch();
        return;
    }

    // --- ΒΗΜΑ 1: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ΜΑΘΗΜΑΤΟΣ ---
    int c_highlight = 0, c_offset = 0, c_choice = -1;
    bool c_selecting = true;

    while (c_selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΟΡΙΣΜΟΣ ΥΠΕΥΘΥΝΟΥ: ΒΗΜΑ 1/2 (Επιλογή Μαθήματος) ===");
        mvprintw(2, 2, "Χρησιμοποιήστε τα βελάκια και πατήστε Enter.");

        int max_display = max_y - 6; 
        if (c_highlight < c_offset) c_offset = c_highlight;
        if (c_highlight >= c_offset + max_display) c_offset = c_highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + c_offset) < courses.size(); i++) 
        {
            size_t actual_idx = i + c_offset;
            if (actual_idx == c_highlight) attron(A_REVERSE);
            
            mvprintw(4 + i, 4, "[%s] %s", courses[actual_idx].getID().c_str(), courses[actual_idx].getDescription().c_str());
            
            if (actual_idx == c_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: c_highlight = (c_highlight == 0) ? courses.size() - 1 : c_highlight - 1; break;
            case KEY_DOWN: c_highlight = (c_highlight == courses.size() - 1) ? 0 : c_highlight + 1; break;
            case 10: c_choice = c_highlight; c_selecting = false; break;
        }
    }

    // Κρατάμε το επιλεγμένο μάθημα (pointer από τη Registry)
    Course* selectedCourse = registry.findCourse(courses[c_choice].getID());

    // --- ΒΗΜΑ 2: ΟΠΤΙΚΗ ΕΠΙΛΟΓΗ ΚΑΘΗΓΗΤΗ ΜΕ ΕΙΔΙΚΟΤΗΤΑ ---
    int p_highlight = 0, p_offset = 0, p_choice = -1;
    bool p_selecting = true;

    while (p_selecting) 
    {
        clear();
        mvprintw(1, 2, "=== ΟΡΙΣΜΟΣ ΥΠΕΥΘΥΝΟΥ: ΒΗΜΑ 2/2 (Επιλογή Καθηγητή) ===");
        mvprintw(2, 2, "Μάθημα: %s | Επιλέξτε Υπεύθυνο Καθηγητή:", selectedCourse->getDescription().c_str());

        int max_display = max_y - 6;
        if (p_highlight < p_offset) p_offset = p_highlight;
        if (p_highlight >= p_offset + max_display) p_offset = p_highlight - max_display + 1;

        for (size_t i = 0; i < max_display && (i + p_offset) < professors.size(); i++) 
        {
            size_t actual_idx = i + p_offset;
            if (actual_idx == p_highlight) attron(A_REVERSE);
            
            // ΕΔΩ ΕΓΙΝΕ Η ΑΛΛΑΓΗ ΓΙΑ ΤΗΝ ΕΜΦΑΝΙΣΗ ΤΗΣ ΕΙΔΙΚΟΤΗΤΑΣ
            mvprintw(4 + i, 4, "[Καθηγητής/τρια] ΑΜ: %s | %s | Ειδικότητα: %s", 
                     professors[actual_idx]->getAM(), 
                     professors[actual_idx]->getFullname().c_str(),
                     professors[actual_idx]->getSpecialty().c_str());
            
            if (actual_idx == p_highlight) attroff(A_REVERSE);
        }
        refresh();

        int c = getch();
        switch (c) 
        {
            case KEY_UP: p_highlight = (p_highlight == 0) ? professors.size() - 1 : p_highlight - 1; break;
            case KEY_DOWN: p_highlight = (p_highlight == professors.size() - 1) ? 0 : p_highlight + 1; break;
            case 10: p_choice = p_highlight; p_selecting = false; break;
        }
    }

    Professor* selectedProf = professors[p_choice];

    // --- ΒΗΜΑ 3: ΑΝΑΘΕΣΗ ---
    clear();
    mvprintw(1, 2, "=== ΑΠΟΤΕΛΕΣΜΑ ΑΝΑΘΕΣΗΣ ===");

    // Θέτουμε τον καθηγητή ως υπεύθυνο στο μάθημα
    selectedCourse->setMainProfessor(selectedProf);

    // Σύνδεση διπλής κατεύθυνσης: Προσθέτουμε το μάθημα και στη λίστα του καθηγητή (αν δεν υπάρχει ήδη)
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

    mvprintw(3, 2, "[+] Ο/Η %s ορίστηκε επιτυχώς ως Υπεύθυνος/η του μαθήματος %s!", 
             selectedProf->getFullname().c_str(), selectedCourse->getDescription().c_str());

    mvprintw(max_y - 1, 2, "Πατήστε οποιοδήποτε πλήκτρο για επιστροφή στο μενού...");
    refresh();
    getch();
}

// Η κεντρική λούπα του προγράμματος
void TerminalUI::run() 
{
    int choice = 0;
    int highlight = 0;
    const int NUM_CHOICES = 14; // Αριθμός επιλογών στο μενού

    bool isRunning = true;

    while (isRunning) 
    {
        drawMainMenu(highlight);
        
        int c = getch(); // Διάβασμα πλήκτρου
        switch (c) 
        {
            case KEY_UP:
                highlight = (highlight == 0) ? (NUM_CHOICES - 1) : (highlight - 1);
                break;
            case KEY_DOWN:
                highlight = (highlight == NUM_CHOICES - 1) ? 0 : (highlight + 1);
                break;
            case 10: // Το πλήκτρο Enter στον πίνακα ASCII είναι ο αριθμός 10
                choice = highlight;
                break;
            default:
                break;
        }

        // Αν ο χρήστης πάτησε Enter, εκτελούμε την επιλογή του
        if (c == 10) 
        {
            if (choice == 0) 
            {
                viewPersons();
            } 
            else if (choice == 1) 
            {
                viewCourses();
            } 
            else if (choice == 2) 
            {
                searchPerson();
            }
            else if (choice == 3) 
            {
                searchCourse();
            }
            else if (choice == 4) 
            {
                addPerson();
            }
            else if (choice == 5) 
            {
                updatePerson();
            }
            else if (choice == 6) 
            {
                addCourse();
            }
            else if (choice == 7) 
            {
                updateCourse();
            }
            else if (choice == 8) 
            {
                deletePerson();
            }
            else if (choice == 9) 
            {
                deleteCourse();
            }
            else if (choice == 10) 
            {
                assignCourseToPerson();
            }
            else if (choice == 11) 
            {
                removeCourseFromPerson();
            }
            else if (choice == 12) 
            {
                assignProfessorToCourse();
            }
            else if (choice == 13) 
            {
                // Αποθήκευση και Έξοδος
                try {
                    registry.saveCoursesToCsv("courses.csv");
                    registry.saveProfessorsToCsv("professors.csv");
                    registry.saveStudentsToCsv("students.csv");
                    showMessage("Τα δεδομένα αποθηκεύτηκαν επιτυχώς!");
                } catch (MyException& e) {
                    showMessage("Σφάλμα αποθήκευσης: " + e.getErrorMessage());
                }
                isRunning = false; // Τερματισμός του προγράμματος
            }
        }
    }
}