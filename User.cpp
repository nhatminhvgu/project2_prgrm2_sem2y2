#include "User.h"
#include <iostream>
#include <iomanip>

// ============================================================
// User (base)
// ============================================================

User::User(std::string username, std::string password, std::string role)
    : username(username), password(password), role(role) {}

std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
std::string User::getRole() const { return role; }

// ============================================================
// Customer
// ============================================================

Customer::Customer(std::string username, std::string password)
    : User(username, password, "customer") {}

void Customer::addToCart(int product_id, int quantity) {
    // If it's already in the cart, add to the existing quantity
    if (cart.count(product_id)) {
        cart[product_id] += quantity;
    } else {
        cart[product_id] = quantity;
    }
}

void Customer::removeFromCart(int product_id) {
    cart.erase(product_id);
}

void Customer::updateCartQuantity(int product_id, int new_quantity) {
    if (new_quantity <= 0) {
        removeFromCart(product_id);
    } else {
        cart[product_id] = new_quantity;
    }
}

void Customer::viewCart(const std::vector<Product>& all_products) const {
    if (cart.empty()) {
        std::cout << "Your cart is empty.\n";
        return;
    }

    std::cout << "\n--- Your Cart ---\n";
    std::cout << std::left << std::setw(28) << "Product"
              << std::setw(8) << "Qty"
              << "Subtotal\n";
    std::cout << std::string(50, '-') << "\n";

    double total = 0.0;
    for (const auto& cart_item : cart) {
        int product_id = cart_item.first;
        int quantity   = cart_item.second;

        for (const Product& p : all_products) {
            if (p.getId() == product_id) {
                double subtotal = p.getPrice() * quantity;
                total += subtotal;
                std::cout << std::left << std::setw(28) << p.getName()
                          << std::setw(8) << quantity
                          << "$" << std::fixed << std::setprecision(2) << subtotal << "\n";
                break;
            }
        }
    }

    std::cout << std::string(50, '-') << "\n";
    std::cout << "Cart Total: $" << std::fixed << std::setprecision(2) << total << "\n";
}

void Customer::clearCart() {
    cart.clear();
}

std::map<int, int> Customer::getCart() const { return cart; }

bool Customer::isCartEmpty() const { return cart.empty(); }

void Customer::showMenu() {
    std::cout << "\n=== Customer Menu ===\n";
    std::cout << "1. Browse / View Product Details\n";
    std::cout << "2. Add Product to Cart\n";
    std::cout << "3. View Cart\n";
    std::cout << "4. Remove Item from Cart\n";
    std::cout << "5. Update Cart Quantity\n";
    std::cout << "6. Checkout\n";
    std::cout << "7. View Order History\n";
    std::cout << "8. Get Recommendations\n";
    std::cout << "0. Logout\n";
    std::cout << "Choice: ";
}

// ============================================================
// Admin
// ============================================================

Admin::Admin(std::string username, std::string password)
    : User(username, password, "admin") {}

void Admin::showMenu() {
    std::cout << "\n=== Admin Menu ===\n";
    std::cout << "1. View All Products\n";
    std::cout << "2. Add New Product\n";
    std::cout << "3. Edit Product\n";
    std::cout << "4. Delete Product\n";
    std::cout << "5. View Most Viewed Products\n";
    std::cout << "6. View Best Selling Products\n";
    std::cout << "7. View Most Active Users\n";
    std::cout << "0. Logout\n";
    std::cout << "Choice: ";
}
