#include <iostream>

#include "uma.h"

int main(void) {
    Uma::Uma* uma { };

    try {
        uma = new Uma::Uma();

        uma->Attach(L"");
    }
    catch (const std::exception& exeption) {
        MessageBox(nullptr,
            exeption.what(), "Error", MB_OK | MB_ICONERROR | MB_TOPMOST
        );

        return EXIT_FAILURE;
    }
}