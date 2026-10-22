#pragma once

#include <iostream>
#include <vector>
using namespace std;

class Student
{
    string name;
    int studentNumber;
    vector<string> modules = vector<string>();

public:
    Student() {
        name = "";
        studentNumber = -1; // uninitialized
    }
    Student(string name, int studentNumber)
    {
        this->name = name;
        this->studentNumber = studentNumber;
    }
    Student(const Student& other) {
        name = other.name;
        studentNumber = other.studentNumber;
        modules = other.modules;
	}
    Student& operator=(const Student& other) {
        if (this != &other) {
            name = other.name;
            studentNumber = other.studentNumber;
            modules = other.modules;
        }
        return *this;
	}
    ~Student() = default;

    int GetStudentNumber();
    void SetName(string newName);
    void AddModule(string module);
	void PrintInfo();
};
