#include <iostream>

#include "Uma.h"

int main(void) {
    Uma::Uma* uma { };

    try {
        uma = new Uma::Uma();

        uma->Attach(L"Example.exe");
    }
    catch (const std::exception& exeption) {
        std::cerr << exeption.what() << std::endl;
        
        std::cin.get();

        if (uma)
            delete uma;

        return -1;
    }

    delete uma;

    return 0;
}