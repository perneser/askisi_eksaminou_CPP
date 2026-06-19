#include "registry.h"
#include <iostream>
#include "student.h"
#include "professor.h"
#include "person.h"
#include <cstring>
#include "course.h"
#include "myException.h"
#include <fstream>
#include <sstream>

std::vector<Person*> Registry::getPersons() const
{
    return persons;
}

std::vector<const Student*> Registry::getStudents()const
{
    std::vector<const Student*> students;
    for (const Person *person : persons)
    {
        // Εάν το άτομο είναι φοιτητής
        const Student* student = dynamic_cast<const Student*>(person);
        if (student != nullptr)
            students.push_back(student);
    }
    return students;
}
std::vector<const Professor*> Registry::getProfessors()const
{
    std::vector<const Professor*> professors;
    for (const Person *person : persons)
    {
        // Εάν το άτομο είναι φοιτητής
        const Professor* professor = dynamic_cast<const Professor*>(person);
        if (professor != nullptr)
            professors.push_back(professor);
    }
    return professors;
}

std::vector<Course> Registry::getCourses()const
{
    return courses;
}


void Registry::addPerson(Person &constPerson)
{
    persons.push_back(&constPerson);
}    


//Διόρθωση στοιχείων φοιτητή/καθηγητή του φοιτητολογίου 
void Registry::updatePerson(const char* keyPersonAM, const Person &in_person)
{
	// Για κάθε μέλος του φοιτητολογίου
	for (auto &person: persons)
	{
		// Εάν ο εισαγόμενος αριθμός ταυτότητας (κλειδί) ταυτίζεται με τον
		// αριθμό ταυτότητας του συγκεκριμένου μέλους του φοιτητολογίου
		if ( strcmp(keyPersonAM, person->getAM()) ==  0 )
		{
			// Αντικατάσταση των τιμών των base class (Person) attributes 
			// του μέλους του φοιτητολογίου
			person->setAM(in_person.getAM());
			person->setFullname(in_person.getFullname());
			person->setGender(in_person.getGender());
            person->setCourses(in_person.getCourses());

			// Εάν το συγκεκριμένο μέλος του φοιτητολογίου είναι φοιτητής
			// και επιπλέον το εισαγόμενο μέλος (in_person) είναι φοιτητής
			Student *student = dynamic_cast <Student *> (person);
			Student *in_student = dynamic_cast <Student *>  ( const_cast<Person*>(&in_person) );
			if (student != NULL && in_student != NULL)
			{
				// Αντικαστάσταση των derived class (Student) attributes 
				// του μέλους του φοιτητολογίου
				student->setSemester(in_student->getSemester());
			}
			// Εάν το συγκεκριμένο μέλος του φοιτητολογίου είναι καθηγητής
			// και επιπλέον το εισαγόμενο μέλος (in_person) είναι καθηγητής
			Professor *professor = dynamic_cast <Professor *> (person);
			Professor *in_professor = dynamic_cast <Professor *> ( const_cast<Person*>(&in_person) );
			if (professor != NULL && in_professor != NULL)
			{
				// Αντικαστάσταση των derived class (Professor) attributes 
				// του μέλους του φοιτητολογίου
				professor->setSpecialty(in_professor->getSpecialty());
			}
		}
	}

}


// Διαγραφή φοιτητή/καθηγητή από τον κατάλογο
void Registry::removePerson(const Person &in_person)
{
	// Για κάθε μάθημα που παρακολουθεί ο φοιτητής/ διδάσκει ο καθηγητής

	// Για κάθε μάθημα του φοιτητολογίου: ελέγχουμε πρώτα ότι υπάρχει
	// υπεύθυνος καθηγητής πριν προσπελάσουμε τον δείκτη.
	for (Course &course : courses)
	{
		Professor* mainProf = course.getMainProfessor();
		if (mainProf != nullptr && strcmp(mainProf->getAM(), in_person.getAM()) == 0)
		{
			// Αφαίρεση του καθηγητή από το μάθημα (αν ήταν υπεύθυνος)
			course.setMainProfessor(nullptr);
		}
	}
	

	std::vector<Person*>::iterator it;
    for (it = persons.begin(); it != persons.end(); ++it)
    {
        if ( strcmp((*it)->getAM(), in_person.getAM()) == 0 )
        {	
			persons.erase(it); // Διαγραφή μέσω του iterator
            break;
        }    
    }    
}

void Registry::createCourse(const Course &course)
{
    courses.push_back(course);
}

// Διαγραφή μαθηματος από τον κατάλογο
void Registry::removeCourse(const Course &in_course)
{
	
	// Για κάθε μέλος του φοιτητολογίου
	for (auto &personPtr : persons)
	{
		// Διαγραφή του μαθήματος και από τους φοιτητές που το έχουν δηλώσει/καθηγητές που το διδάσκουν
		personPtr->removeCourse(in_course);
	}

	std::vector<Course>::iterator it = courses.begin();
    while( it != courses.end())
    {
        if ( (*it).getID() == in_course.getID())
        {
            courses.erase(it); // Διαγραφή μέσω του iterator
        }
		else
		{
			++it;
		}
    }    
	
}

// Διόρθωση μαθήματος στο φοιτητολόγιο
void Registry::updateCourse(std::string keyCourseID,
	std::string in_ID,
	std::string in_description,
	unsigned int in_semester,
	Professor *in_mainProfessor)
{
	// Για κάθε ακαδημαϊκό μάθημα του φοιτητολογίου
	for (auto &course : courses) 
	{
		if (keyCourseID == course.getID())
		{
			course.setID(in_ID);
			course.setDescription(in_description);
			course.setSemester(in_semester);
			course.setMainProfessor(in_mainProfessor);
		}
	}
}

// Αναζήτηση μαθήματος στο φοιτητολόγιο με βάση τον κωδικό του
Course& Registry::findCourseByID(std::string keyCourseID)
{
    for (auto &course : courses) 
	{
		if (keyCourseID == course.getID())
		{
			return course;
		}
	}//else
    std::string msg = "Δεν βρέθηκε μάθημα με κωδικό ";
    msg += keyCourseID;
    throw MyException(msg);
}


// Εμφάνιση ατόμων εγγεγραμένων στο φοιτητολόγιο
void Registry::printPersons()const
{
    for (const Person *person : persons)
    {
        if (person->isStudent())
        {
            if (person->isFemale())
                std::cout << "Φοιτήτρια | ";
            else
                std::cout << "Φοιτητής | ";

        }    
        else if (person->isProfessor())
        {
            if (person->isFemale())
                std::cout << "Καθηγήτρια | ";
            else
                std::cout << "Καθηγητής | ";
        }     
        
        std::cout << *person << std::endl;
    }    
    
}    

void Registry::printCourses()const
{
    for (auto course : courses)
    {
        std::cout << course << std::endl;
    }
}

void Registry::printAllData() const
{
	std::cout << "==============================================================" << std::endl;
    std::cout << "Άτομα στο φοιτητολόγιο:" << std::endl;
	std::cout << "==============================================================" << std::endl;
    printPersons();
	std::cout << "==============================================================" << std::endl;
    std::cout << "Μαθήματα στο φοιτητολόγιο:" << std::endl;
	std::cout << "==============================================================" << std::endl;
    printCourses();
	std::cout << "==============================================================" << std::endl;
    std::cout << "Μαθήματα ανά άτομο:" << std::endl;;
	std::cout << "==============================================================" << std::endl;
    for (const Person* person : persons)
    {
        if (person->isStudent())
        {
            std::cout << (person->isFemale() ? "Φοιτήτρια " : "Φοιτητής ")
                      << person->getFullname() << ":\n";
        }
        else if (person->isProfessor())
        {
            std::cout << (person->isFemale() ? "Καθηγήτρια " : "Καθηγητής ")
                      << person->getFullname() << ":\n";
        }
		std::cout << "-------------------------------" << std::endl;
        person->printCourses();
		std::cout << "==============================================================" << std::endl;
    }
}

// Αποστολή email σε όλους τους καθηγητές / όλες τις καθηγήτριες του τμήματος
void Registry::sendEmailToAllProfessors() const
{
	for (const auto &person: persons)
	{
		const Professor *professor = dynamic_cast <const Professor *> (person);
		if (professor != NULL)
		{
			std::cout << "Στάλθηκε email " 
                      <<  ((professor->isFemale())? "στην καθηγήτρια ": "στον καθηγητή " ) // Φυλετική διάκριση
                      << professor->getFullname() << std::endl;
		}
	}
}

// Αποστολή email σε όλους τους φοιτητές / όλες τις φοιτήτριες του τμήματος
void Registry::sendEmailToAllStudents() const
{
	for (const auto &person: persons)
	{
		const Student *student = dynamic_cast <const Student *> (person);
		if (student != NULL)
		{
			std::cout << "Στάλθηκε email " 
                      <<  ((student->isFemale())? "στην φοιτήτρια ": "στον φοιτητή " ) // Φυλετική διάκριση
                     << student->getFullname() << std::endl;
		}
	}
}

Course* Registry::findCourse(std::string in_ID) 
{
	for (auto &course : courses) {
		if (course.getID() == in_ID)
			return &course;
	}
	return nullptr;
}
const Course* Registry::findCourse(std::string in_ID) const
{
	for (const auto &course : courses) {
		if (course.getID() == in_ID)
			return &course;
	}
	return nullptr;
}

Person* Registry::findPerson(std::string in_AM) const 
{
    for (Person* p : persons) {
        if (p->getAM() == in_AM)
			return p;
    }
    return nullptr;
}

void Registry::saveCoursesToCsv(const std::string &filename)const
{
	std::ofstream oFile(filename);
	if (!oFile.is_open())
	{
		throw MyException("Η εγγραφή των μαθημάτων στο αρχείο " + filename + " απέτυχε.\n");
	}
	//else
	
	for (const Course &course : courses)
	{
		std::string courseStr = course.toString();
		courseStr += '\n';
		oFile << courseStr;
	}	
}

void Registry::saveProfessorsToCsv(const std::string &filename)const
{
	std::ofstream oFile(filename);
	if (!oFile.is_open())
	{
		throw MyException("Η εγγραφή των καθηγητών στο αρχείο " + filename + " απέτυχε.\n");
	}
	//else
	
	for (const Person* personPtr : persons)
	{
		const Professor *professor = dynamic_cast <const Professor *> (personPtr);
		if (professor != NULL)
		{
			std::string professorStr = professor->toString();
			professorStr += '\n';
			oFile << professorStr;
		}
	}	
}

void Registry::saveStudentsToCsv(const std::string &filename)const
{
	std::ofstream oFile(filename);
	if (!oFile.is_open())
	{
		throw MyException("Η εγγραφή των φοιτητών στο αρχείο " + filename + " απέτυχε.\n");
	}
	//else
	
	for (const Person* personPtr : persons)
	{
		const Student *student = dynamic_cast <const Student *> (personPtr);
		if (student != NULL)
		{
			std::string studentStr = student->toString();
			studentStr += '\n';
			oFile << studentStr;
		}
	}
}

std::vector<Professor*> Registry::getProfessorsFromCsv(const std::string &filename)const
{
    std::vector<Professor*> loadedProfessors;
    std::ifstream iFile(filename);

    if (!iFile.is_open()) throw MyException("Το άνοιγμα του αρχείου " + filename + " απέτυχε.\n");

    std::string line;
    while (std::getline(iFile, line)) 
    {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string AM, fullname, gender, specialty;
		
        std::getline(ss, AM, ',');
        std::getline(ss, fullname, ',');
        std::getline(ss, gender, ',');
        std::getline(ss, specialty, ',');

        Professor* prof = new Professor(AM.c_str(), fullname, gender, specialty);
		
        std::string courseID;
        while (std::getline(ss, courseID, ',')) 
        {
            if (!courseID.empty()) 
            {
				const Course* c = this->findCourse(courseID);
				if (c != nullptr) prof->addCourse(*c);
            }
        }
        loadedProfessors.push_back(prof);
    }
    return loadedProfessors;
}

std::vector<Student*> Registry::getStudentsFromCsv(const std::string &filename)const
{
    std::vector<Student*> loadedStudents;
    std::ifstream iFile(filename);

    if (!iFile.is_open()) throw MyException("Το άνοιγμα του αρχείου " + filename + " απέτυχε.\n");

    std::string line;
    while (std::getline(iFile, line)) 
    {
        if (line.empty())	// Παράλειψη κενών γραμμών
			continue;
			
			std::stringstream ss(line);
			
			std::string AM, fullname, gender, semesterStr;
			
			std::getline(ss, AM, ',');
			std::getline(ss, fullname, ',');
			std::getline(ss, gender, ',');
			std::getline(ss, semesterStr, ',');
			
			Student* student = new Student(AM.c_str(), fullname, gender, std::stoi(semesterStr));
			student->setSemester(std::stoi(semesterStr));
			
        std::string courseID;
        while (std::getline(ss, courseID, ',')) 
        {
            if (!courseID.empty()) 
            {
				const Course* c = this->findCourse(courseID);
				if (c != nullptr) student->addCourse(*c);
            }
        }
        loadedStudents.push_back(student);
    }
    return loadedStudents;
}

std::vector<Course> Registry::getCoursesFromCsv(const std::string &filename) const
{
    std::vector<Course> loadedCourses;
    std::ifstream iFile(filename);

    if (!iFile.is_open()) throw MyException("Το άνοιγμα του αρχείου " + filename + " απέτυχε.\n");

    std::string line;
    while (std::getline(iFile, line)) 
    {
        if (line.empty())	// Παράλειψη κενών γραμμών
			continue;

        std::stringstream ss(line);
        std::string ID, description, semesterStr, professorAM;

        std::getline(ss, ID, ',');
        std::getline(ss, description, ',');
        std::getline(ss, semesterStr, ',');
        std::getline(ss, professorAM, ',');

        Course course(ID, description, std::stoi(semesterStr), nullptr);
        loadedCourses.push_back(course);
    }
    return loadedCourses;
}

void Registry::linkMainProfessors(const std::string &professorsCsvfile) 
{
    std::ifstream iFile(professorsCsvfile);
    std::string line;
    
    while (std::getline(iFile, line)) 
    {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string ID, description, semester, professorAM;
        
        std::getline(ss, ID, ',');
        std::getline(ss, description, ',');
        std::getline(ss, semester, ',');
        std::getline(ss, professorAM, ',');

        if (professorAM != "Κανένας" && !professorAM.empty()) 
        {
            Course* c = this->findCourse(ID);
            Person* p = this->findPerson(professorAM);

            if (c != nullptr && p != nullptr) 
            {
                Professor* prof = dynamic_cast<Professor*>(p);
                if (prof != nullptr) 
                {
                    c->setMainProfessor(prof); 
                }
            }
        }
    }
}

void Registry::loadAllData(std::string coursesCsvFile, std::string professorsCsvFile, std::string studentsCsvFile) 
{
    // Φορτώνουμε πρώτα τα Μαθήματα
    this->courses = this->getCoursesFromCsv(coursesCsvFile);

    // Φορτώνουμε τους καθηγητές και τους βάζουμε στη λίστα persons
    std::vector<Professor*> loadedProfs = this->getProfessorsFromCsv(professorsCsvFile);
    this->persons.insert(this->persons.end(), loadedProfs.begin(), loadedProfs.end());

    // Φορτώνουμε τους φοιτητές και τους βάζουμε στη λίστα persons
    std::vector<Student*> loadedStudents = this->getStudentsFromCsv(studentsCsvFile);
    this->persons.insert(this->persons.end(), loadedStudents.begin(), loadedStudents.end());

    // Τελική Σύνδεση Υπεύθυνων Καθηγητών στα Μαθήματα
    this->linkMainProfessors(professorsCsvFile);
    
    std::cout << "Όλα τα δεδομένα φορτώθηκαν και συνδέθηκαν επιτυχώς!" << std::endl;
}