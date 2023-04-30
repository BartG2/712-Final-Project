// myclass.h

#ifndef TESTCLASS_H
#define TESTCLASS_H

class testClass {
public:
int n;

    testClass(int number){
        n = number;
    }

    int addToN(int number){
        return n + number;
    }
    
};

#endif
