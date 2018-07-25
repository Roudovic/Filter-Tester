//
//  TestRandomDevice.cpp
//  
//
//  Created by Ludovic Theobald on 25/7/2561 BE.
//

#include "TestRandomDevice.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <random>
#include <string>
#include "hashutil.h"
#include <iostream>


::std::random_device rd;
int main(){
for (int i =0 ; i<10 ; i++){
    std::cout << rd() << std::endl;
}
    std::cout <<"" << std::endl;
for (int i =0 ; i<10 ; i++){
    std::cout << rd() << std::endl;
}
    
}
