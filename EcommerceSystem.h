#ifndef ECOMMERCESYSTEM_H
#define ECOMMERCESYSTEM_H

#include <string>
#include <vector>
#include "User.h"
#include "Product.h"
#include "Recommender.h"

// One line in orders.txt represents one product purchased in one checkout
struct Order {
    std::string order_id;
    std::string username;
    int product_id;
    int quantity;
    double unit_price;
    std::string date;
};

class EcommerceSystem {
private:
    std::vector<Product> all_products;
    std::vector<User*> all_users; // polymorphic — could be Customer or Admin
    std::vector<Order> all_orders;
    Recommender recommender;

    // File I/O helpers
    void loadProducts();
    void saveProducts();
    void loadUsers();
    void saveUsers();
    void loadOrders();
    void saveOrders();

    int getNextProductId();

public:
    EcommerceSystem();
    ~EcommerceSystem();

    // Authentication
    User* login(const std::string& username, const std::string& password);
    bool registerCustomer(const std::string& username, const std::string& password);

    // Product browsing
    void displayAllProducts();
    Product* findProductById(int product_id);

    // Customer-side actions (called after login)
    void handleCustomerMenu(Customer* current_customer);
    void checkoutCart(Customer* current_customer);
    void viewOrderHistory(const std::string& username);
    void showRecommendations(const std::string& username, int top_n);

    // Admin-side actions
    void handleAdminMenu(Admin* current_admin);
    void addProduct(const std::string& name, const std::string& category, double price, int stock);
    void editProduct(int product_id);
    void deleteProduct(int product_id);
    void showMostViewedProducts();
    void showBestSellingProducts();
    void showActiveUsers();
};

#endif
