//
// Created by davidm on 2021-02-12.
//

#ifndef LAME_BOY_CPU_H
#define LAME_BOY_CPU_H

#include <iostream>

class CPU {
public:
    CPU() {
        std::cout << "Hello CPU!" << std::endl;
    }

    int one() {return 1;}
};


#endif //LAME_BOY_CPU_H
