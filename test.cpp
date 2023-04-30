#include "testClass.h"
#include <iostream>

int main(){
    int a = 10;
    testClass b(a);
    int c = b.addToN(20);
    
    std::cout << c << std::endl;

    return 0;
}