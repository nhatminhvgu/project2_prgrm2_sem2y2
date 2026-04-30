#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <map>
#include "Product.h"

// Base class for all users in the system
class User {
protected:
    std::string username;
    std::string password;
    std::string role; // "customer" or "admin"

public:
    User(std::string username, std::string password, std::string role);
    virtual ~User() {}

    std::string getUsername() const;
    std::string getPassword() const;
    std::string getRole() const;

    // Each subclass shows its own menu
    virtual void showMenu() = 0;
};

// ---------------------------------------------------------------------------

class Customer : public User {
private:
    // Maps product_id -> quantity the user added to their cart
    std::map<int, int> cart;

public:
    Customer(std::string username, std::string password);

    void addToCart(int product_id, int quantity);
    void removeFromCart(int product_id);
    void updateCartQuantity(int product_id, int new_quantity);
    void viewCart(const std::vector<Product>& all_products) const;
    void clearCart();

    std::map<int, int> getCart() const;
    bool isCartEmpty() const;

    void showMenu() override;
};

// ---------------------------------------------------------------------------

class Admin : public User {
public:
    Admin(std::string username, std::string password);
    void showMenu() override;
};

#endif
