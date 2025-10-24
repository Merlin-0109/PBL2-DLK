// main_gui.cpp - Entry point for MediCare GUI application
#include "GUI.h"
#include <iostream>

int main() {
    std::cout << "Starting MediCare Application..." << std::endl;
    
    GUI app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application!" << std::endl;
        return -1;
    }
    
    std::cout << "Application initialized successfully." << std::endl;
    std::cout << "Running main loop..." << std::endl;
    
    app.run();
    
    std::cout << "Application closed." << std::endl;
    return 0;
}
