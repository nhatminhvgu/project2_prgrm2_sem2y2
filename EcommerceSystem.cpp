#include "EcommerceSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

EcommerceSystem::EcommerceSystem() {
    loadProducts();
    loadUsers();
    loadOrders();
}

EcommerceSystem::~EcommerceSystem() {
    // Free the polymorphic user objects we created with new
    for (User* user : all_users) {
        delete user;
    }
}

// ============================================================
// File I/O
// ============================================================

void EcommerceSystem::loadProducts() {
    all_products.clear();
    std::ifstream file("products.txt");
    if (!file.is_open()) {
        std::cout << "[Warning] products.txt not found. Starting with empty inventory.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        int product_id, stock_quantity;
        std::string name, category;
        double price;

        std::getline(ss, token, ',');   product_id = std::stoi(token);
        std::getline(ss, name, ',');
        std::getline(ss, category, ',');
        std::getline(ss, token, ',');   price = std::stod(token);
        std::getline(ss, token, ',');   stock_quantity = std::stoi(token);

        all_products.push_back(Product(product_id, name, category, price, stock_quantity));
    }
    file.close();
}

void EcommerceSystem::saveProducts() {
    std::ofstream file("products.txt");
    for (const Product& p : all_products) {
        file << p.getId() << ","
             << p.getName() << ","
             << p.getCategory() << ","
             << std::fixed << std::setprecision(2) << p.getPrice() << ","
             << p.getStock() << "\n";
    }
    file.close();
}

void EcommerceSystem::loadUsers() {
    for (User* user : all_users) delete user;
    all_users.clear();

    std::ifstream file("users.txt");
    if (!file.is_open()) {
        std::cout << "[Warning] users.txt not found. No accounts loaded.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string username, password, role;

        std::getline(ss, username, ',');
        std::getline(ss, password, ',');
        std::getline(ss, role, ',');

        if (role == "admin") {
            all_users.push_back(new Admin(username, password));
        } else {
            all_users.push_back(new Customer(username, password));
        }
    }
    file.close();
}

void EcommerceSystem::saveUsers() {
    std::ofstream file("users.txt");
    for (const User* user : all_users) {
        file << user->getUsername() << ","
             << user->getPassword() << ","
             << user->getRole() << "\n";
    }
    file.close();
}

void EcommerceSystem::loadOrders() {
    all_orders.clear();
    std::ifstream file("orders.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        Order order;

        std::getline(ss, order.order_id, ',');
        std::getline(ss, order.username, ',');
        std::getline(ss, token, ',');   order.product_id = std::stoi(token);
        std::getline(ss, token, ',');   order.quantity = std::stoi(token);
        std::getline(ss, token, ',');   order.unit_price = std::stod(token);
        std::getline(ss, order.date, ',');

        all_orders.push_back(order);
    }
    file.close();
}

void EcommerceSystem::saveOrders() {
    std::ofstream file("orders.txt");
    for (const Order& order : all_orders) {
        file << order.order_id << ","
             << order.username << ","
             << order.product_id << ","
             << order.quantity << ","
             << std::fixed << std::setprecision(2) << order.unit_price << ","
             << order.date << "\n";
    }
    file.close();
}

int EcommerceSystem::getNextProductId() {
    int max_id = 0;
    for (const Product& p : all_products) {
        if (p.getId() > max_id) max_id = p.getId();
    }
    return max_id + 1;
}

// ============================================================
// Authentication
// ============================================================

User* EcommerceSystem::login(const std::string& username, const std::string& password) {
    for (User* user : all_users) {
        if (user->getUsername() == username && user->getPassword() == password) {
            return user;
        }
    }
    return nullptr;
}

bool EcommerceSystem::registerCustomer(const std::string& username, const std::string& password) {
    for (const User* user : all_users) {
        if (user->getUsername() == username) {
            return false; // username already taken
        }
    }
    all_users.push_back(new Customer(username, password));
    saveUsers();
    return true;
}

// ============================================================
// Product helpers
// ============================================================

void EcommerceSystem::displayAllProducts() {
    if (all_products.empty()) {
        std::cout << "No products available.\n";
        return;
    }
    std::cout << "\n--- Product Catalog ---\n";
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(28) << "Name"
              << std::setw(15) << "Category"
              << std::setw(12) << "Price"
              << "Stock\n";
    std::cout << std::string(65, '-') << "\n";
    for (const Product& p : all_products) {
        p.display();
    }
}

Product* EcommerceSystem::findProductById(int product_id) {
    for (Product& p : all_products) {
        if (p.getId() == product_id) return &p;
    }
    return nullptr;
}

// ============================================================
// Customer menu & actions
// ============================================================

void EcommerceSystem::handleCustomerMenu(Customer* current_customer) {
    int choice;
    do {
        current_customer->showMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: { // Browse and optionally view a product detail
                displayAllProducts();
                std::cout << "Enter Product ID to see details (0 to go back): ";
                int view_id;
                std::cin >> view_id;
                std::cin.ignore();
                if (view_id != 0) {
                    Product* product = findProductById(view_id);
                    if (product) {
                        std::cout << "\n--- Product Details ---\n";
                        product->display();
                        // Count this as a view for the recommendation engine
                        recommender.recordView(current_customer->getUsername(), view_id);
                        std::cout << "(View recorded for recommendations)\n";
                    } else {
                        std::cout << "Product not found.\n";
                    }
                }
                break;
            }
            case 2: { // Add to cart
                displayAllProducts();
                int product_id, quantity;
                std::cout << "Enter Product ID to add: ";
                std::cin >> product_id;
                std::cout << "Enter quantity: ";
                std::cin >> quantity;
                std::cin.ignore();

                Product* product = findProductById(product_id);
                if (!product) {
                    std::cout << "Product not found.\n";
                } else if (quantity <= 0) {
                    std::cout << "Quantity must be at least 1.\n";
                } else if (product->getStock() < quantity) {
                    std::cout << "Not enough stock. Only " << product->getStock() << " left.\n";
                } else {
                    current_customer->addToCart(product_id, quantity);
                    std::cout << quantity << "x \"" << product->getName() << "\" added to cart.\n";
                    // Browsing to add is also a view interaction
                    recommender.recordView(current_customer->getUsername(), product_id);
                }
                break;
            }
            case 3: // View cart
                current_customer->viewCart(all_products);
                break;

            case 4: { // Remove item
                current_customer->viewCart(all_products);
                std::cout << "Enter Product ID to remove (0 to cancel): ";
                int remove_id;
                std::cin >> remove_id;
                std::cin.ignore();
                if (remove_id != 0) {
                    current_customer->removeFromCart(remove_id);
                    std::cout << "Item removed.\n";
                }
                break;
            }
            case 5: { // Update quantity
                current_customer->viewCart(all_products);
                int update_id, new_qty;
                std::cout << "Enter Product ID to update: ";
                std::cin >> update_id;
                std::cout << "Enter new quantity (0 removes the item): ";
                std::cin >> new_qty;
                std::cin.ignore();
                current_customer->updateCartQuantity(update_id, new_qty);
                std::cout << "Cart updated.\n";
                break;
            }
            case 6: // Checkout
                checkoutCart(current_customer);
                break;

            case 7: // Order history
                viewOrderHistory(current_customer->getUsername());
                break;

            case 8: // Recommendations
                showRecommendations(current_customer->getUsername(), 5);
                break;

            case 0:
                std::cout << "Logging out...\n";
                break;

            default:
                std::cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

void EcommerceSystem::checkoutCart(Customer* current_customer) {
    if (current_customer->isCartEmpty()) {
        std::cout << "Your cart is empty — nothing to checkout.\n";
        return;
    }

    current_customer->viewCart(all_products);

    std::cout << "Confirm purchase? (y/n): ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore();

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "Checkout cancelled.\n";
        return;
    }

    // Get today's date for the order record
    time_t now = time(nullptr);
    char date_buffer[20];
    strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", localtime(&now));
    std::string today(date_buffer);

    std::map<int, int> cart_snapshot = current_customer->getCart();
    bool had_any_issue = false;

    for (const auto& cart_item : cart_snapshot) {
        int product_id = cart_item.first;
        int quantity   = cart_item.second;

        Product* product = findProductById(product_id);
        if (!product) {
            std::cout << "  [Skipped] Product ID " << product_id << " no longer exists.\n";
            had_any_issue = true;
            continue;
        }
        if (product->getStock() < quantity) {
            std::cout << "  [Skipped] \"" << product->getName()
                      << "\" — only " << product->getStock() << " in stock.\n";
            had_any_issue = true;
            continue;
        }

        // Build order record
        Order new_order;
        new_order.order_id   = "ORD" + std::to_string(all_orders.size() + 1);
        new_order.username   = current_customer->getUsername();
        new_order.product_id = product_id;
        new_order.quantity   = quantity;
        new_order.unit_price = product->getPrice();
        new_order.date       = today;

        all_orders.push_back(new_order);

        // Deduct from stock
        product->setStock(product->getStock() - quantity);

        // Record the purchase so the recommender learns from it
        recommender.recordPurchase(current_customer->getUsername(), product_id);
    }

    saveProducts();
    saveOrders();
    current_customer->clearCart();

    if (had_any_issue) {
        std::cout << "Order placed with some items skipped. Check the details above.\n";
    } else {
        std::cout << "Order placed successfully! Thank you for shopping.\n";
    }
}

void EcommerceSystem::viewOrderHistory(const std::string& username) {
    std::cout << "\n--- Order History for \"" << username << "\" ---\n";
    bool found_any = false;
    double grand_total = 0.0;

    for (const Order& order : all_orders) {
        if (order.username != username) continue;

        found_any = true;
        Product* product = findProductById(order.product_id);
        std::string product_name = product ? product->getName() : "(deleted product)";
        double item_total = order.unit_price * order.quantity;
        grand_total += item_total;

        std::cout << "[" << order.order_id << "] " << order.date
                  << "  " << product_name
                  << "  x" << order.quantity
                  << "  @ $" << std::fixed << std::setprecision(2) << order.unit_price
                  << "  = $" << item_total << "\n";
    }

    if (!found_any) {
        std::cout << "No orders yet.\n";
    } else {
        std::cout << std::string(55, '-') << "\n";
        std::cout << "Total spent: $" << std::fixed << std::setprecision(2) << grand_total << "\n";
    }
}

void EcommerceSystem::showRecommendations(const std::string& username, int top_n) {
    std::cout << "\n--- Recommendations for \"" << username << "\" ---\n";

    std::vector<Product> recommendations =
        recommender.getRecommendations(username, all_products, top_n);

    if (recommendations.empty()) {
        std::cout << "No recommendations yet. Browse or purchase some products first!\n";
        return;
    }

    std::cout << "Top " << recommendations.size() << " picks for you:\n";
    for (int i = 0; i < (int)recommendations.size(); i++) {
        std::cout << (i + 1) << ". ";
        recommendations[i].display();
    }
}

// ============================================================
// Admin menu & actions
// ============================================================

void EcommerceSystem::handleAdminMenu(Admin* current_admin) {
    int choice;
    do {
        current_admin->showMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1:
                displayAllProducts();
                break;

            case 2: { // Add product
                std::string name, category;
                double price;
                int stock;

                std::cout << "Product name: ";
                std::getline(std::cin, name);
                std::cout << "Category: ";
                std::getline(std::cin, category);
                std::cout << "Price: $";
                std::cin >> price;
                std::cout << "Stock quantity: ";
                std::cin >> stock;
                std::cin.ignore();

                addProduct(name, category, price, stock);
                std::cout << "Product added successfully.\n";
                break;
            }
            case 3: { // Edit product
                displayAllProducts();
                std::cout << "Enter Product ID to edit: ";
                int edit_id;
                std::cin >> edit_id;
                std::cin.ignore();
                editProduct(edit_id);
                break;
            }
            case 4: { // Delete product
                displayAllProducts();
                std::cout << "Enter Product ID to delete: ";
                int delete_id;
                std::cin >> delete_id;
                std::cin.ignore();
                deleteProduct(delete_id);
                break;
            }
            case 5:
                showMostViewedProducts();
                break;
            case 6:
                showBestSellingProducts();
                break;
            case 7:
                showActiveUsers();
                break;
            case 0:
                std::cout << "Logging out...\n";
                break;
            default:
                std::cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

void EcommerceSystem::addProduct(const std::string& name, const std::string& category,
                                  double price, int stock) {
    int new_id = getNextProductId();
    all_products.push_back(Product(new_id, name, category, price, stock));
    saveProducts();
}

void EcommerceSystem::editProduct(int product_id) {
    Product* product = findProductById(product_id);
    if (!product) {
        std::cout << "Product not found.\n";
        return;
    }

    std::cout << "Currently: ";
    product->display();
    std::cout << "\nWhat to edit?\n";
    std::cout << "1. Name\n2. Category\n3. Price\n4. Stock\nChoice: ";

    int field_choice;
    std::cin >> field_choice;
    std::cin.ignore();

    switch (field_choice) {
        case 1: {
            std::cout << "New name: ";
            std::string new_name;
            std::getline(std::cin, new_name);
            product->setName(new_name);
            break;
        }
        case 2: {
            std::cout << "New category: ";
            std::string new_category;
            std::getline(std::cin, new_category);
            product->setCategory(new_category);
            break;
        }
        case 3: {
            std::cout << "New price: $";
            double new_price;
            std::cin >> new_price;
            std::cin.ignore();
            product->setPrice(new_price);
            break;
        }
        case 4: {
            std::cout << "New stock: ";
            int new_stock;
            std::cin >> new_stock;
            std::cin.ignore();
            product->setStock(new_stock);
            break;
        }
        default:
            std::cout << "Invalid choice. No changes made.\n";
            return;
    }

    saveProducts();
    std::cout << "Product updated.\n";
}

void EcommerceSystem::deleteProduct(int product_id) {
    auto it = std::remove_if(all_products.begin(), all_products.end(),
                             [product_id](const Product& p) {
                                 return p.getId() == product_id;
                             });
    if (it == all_products.end()) {
        std::cout << "Product not found.\n";
        return;
    }
    all_products.erase(it, all_products.end());
    saveProducts();
    std::cout << "Product deleted.\n";
}

void EcommerceSystem::showMostViewedProducts() {
    std::cout << "\n--- Top 5 Most Viewed Products ---\n";
    auto top_viewed = recommender.getMostViewedProducts(5);
    if (top_viewed.empty()) {
        std::cout << "No view data yet.\n";
        return;
    }
    for (int i = 0; i < (int)top_viewed.size(); i++) {
        Product* product = findProductById(top_viewed[i].first);
        std::string product_name = product ? product->getName() : "(unknown)";
        std::cout << (i + 1) << ". " << product_name
                  << "  — " << top_viewed[i].second << " views\n";
    }
}

void EcommerceSystem::showBestSellingProducts() {
    std::cout << "\n--- Top 5 Best Selling Products ---\n";
    auto top_sellers = recommender.getBestSellingProducts(5);
    if (top_sellers.empty()) {
        std::cout << "No sales data yet.\n";
        return;
    }
    for (int i = 0; i < (int)top_sellers.size(); i++) {
        Product* product = findProductById(top_sellers[i].first);
        std::string product_name = product ? product->getName() : "(unknown)";
        std::cout << (i + 1) << ". " << product_name
                  << "  — " << top_sellers[i].second << " units sold\n";
    }
}

void EcommerceSystem::showActiveUsers() {
    std::cout << "\n--- Top 5 Most Active Users ---\n";
    auto active_users = recommender.getActiveUsers(5);
    if (active_users.empty()) {
        std::cout << "No user activity data yet.\n";
        return;
    }
    for (int i = 0; i < (int)active_users.size(); i++) {
        std::cout << (i + 1) << ". " << active_users[i] << "\n";
    }
}
