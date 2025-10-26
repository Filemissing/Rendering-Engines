// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Student.h"
using namespace std;

int main()
{
	printf("Enter number of students: ");
    int StudentCount;
    scanf_s("%d", &StudentCount);

	Student* students = new Student[StudentCount];

    while (true) {
		printf("Enter command (1: Add Student, 2: Update Name, 3: Add Module, 4: print student info, 0: Exit)\ncommand: ");
        int command;
        scanf_s("%d", &command);

        if (command == 0) {
			delete[] students;
            break;
        }
        else if (command == 1) {
            int studentNumber;
			char name[100];
			int modulesLen;

			printf("Format: studentNumber, name, numberOfModules\nEnter: ");
            scanf_s("%d, %99[^,], %d", &studentNumber, name, 100, &modulesLen);

			students[studentNumber] = Student(name, studentNumber);
        }
        else if (command == 2) {
            printf("Format: studentNumber, newName\nEnter: ");
			int studentNumber;
            char newName[100];
			scanf_s("%d, %99[^\n]", &studentNumber, newName, 100);

            students[studentNumber].SetName(newName);
        }
        else if (command == 3) {
			printf("Format: studentNumber, moduleIndex, moduleName\nEnter: ");
            int studentNumber;
            int moduleIndex;
			char moduleName[100];
			scanf_s("%d, %d, %99[^\n]", &studentNumber, &moduleIndex, moduleName, 100);

			students[studentNumber].AddModule(moduleName);
        }
        else if (command == 4) {
			printf("Format: studentNumber\nEnter: ");
			int studentNumber;
			scanf_s("%d", &studentNumber);

            Student* student = &students[studentNumber];
			student->PrintInfo();
        }
        else if (command == 5) {
            for (int i = 0; i < StudentCount; i++) {
                Student* student = &students[i];

				if (student->GetStudentNumber() == -1) continue; // ignore uninitialized students

				student->PrintInfo();
            }
        }
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
