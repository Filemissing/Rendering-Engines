#include "Student.h"

int Student::GetStudentNumber() {
    return studentNumber;
}
void Student::SetName(string newName){
	name = newName;
}
void Student::AddModule(string module) {
	modules.push_back(module);
}
void Student::PrintInfo() {
	printf("Student Info:\n");
    printf("Student Info:\n");
    printf("Name: %s\n", name.c_str());
    printf("Student Number: %d\n", studentNumber);
    printf("Modules:\n");
    for (string module : modules) {
        printf(" - %s\n", module.c_str());
    }
}