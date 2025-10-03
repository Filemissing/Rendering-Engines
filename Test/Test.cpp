// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

void Exersize1() 
{
    int a;
    printf("Input 1 number\n");
    scanf_s("%d", &a);

    bool isEven = a % 2 == 0;

    if (isEven) printf("Number %d is even", a);
    else printf("Number %d is odd", a);
}
void Excersize2() 
{
    int a;
    int b;
    printf("Input 2 numbers\n");
    scanf_s("%d %d", &a, &b);
    int highest = a > b ? a : b;
    printf("Highest number %d", highest);
}
void Excersize3() 
{
    int a;
    int b;
    int c;
    printf("Input 3 numbers\n");
    scanf_s("%d %d %d", &a, &b, &c);
    int average = (a + b + c) / 3;
    printf("Average is %d", average);
}
void Excersize4() 
{
    int a;
    int b;
    int c;
    int d;
    printf("Input 4 numbers\n");
    scanf_s("%d %d %d %d", &a, &b, &c, &d);
    int total = a + b + c + d;
    printf("Sum is %d", total);
}

int main()
{
    int arrayOfInts[5];

    for (int i = 0; i < 5; i++) {
        printf("%d\n", arrayOfInts[i]);
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
