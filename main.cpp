#include <iostream>
#include <string>
#include "EcommerceSystem.h"
#include "User.h"

void show_main_menu() {
    std::cout << "\n=============================\n";
    std::cout << "   Smart E-Commerce System\n";
    std::cout << "=============================\n";
    std::cout << "1. Login\n";
    std::cout << "2. Register as Customer\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

int main() {
    EcommerceSystem system;

    std::cout << "Welcome to the Smart E-Commerce System!\n";
    std::cout << "Default accounts: admin/admin123 | alice/alice123 | bob/bob123\n";

    int choice;
    do {
        show_main_menu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: {
                std::string username, password;
                std::cout << "Username: ";
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);

                User* logged_in_user = system.login(username, password);

                if (!logged_in_user) {
                    std::cout << "Invalid username or password.\n";
                } else if (logged_in_user->getRole() == "admin") {
                    std::cout << "Welcome, Admin " << username << "!\n";
                    system.handleAdminMenu(dynamic_cast<Admin*>(logged_in_user));
                } else {
                    std::cout << "Welcome back, " << username << "!\n";
                    system.handleCustomerMenu(dynamic_cast<Customer*>(logged_in_user));
                }
                break;
            }
            case 2: {
                std::string username, password;
                std::cout << "Choose a username: ";
                std::getline(std::cin, username);
                std::cout << "Choose a password: ";
                std::getline(std::cin, password);

                if (system.registerCustomer(username, password)) {
                    std::cout << "Account created! You can now log in.\n";
                } else {
                    std::cout << "Username already taken. Please try a different one.\n";
                }
                break;
            }
            case 0:
                std::cout << "Goodbye!\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
