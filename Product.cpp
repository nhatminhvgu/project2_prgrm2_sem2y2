#include "Product.h"
#include <iostream>
#include <iomanip>

Product::Product()
    : product_id(0), name(""), category(""), price(0.0), stock_quantity(0) {}

Product::Product(int id, std::string name, std::string category, double price, int stock)
    : product_id(id), name(name), category(category), price(price), stock_quantity(stock) {}

int Product::getId() const { return product_id; }
std::string Product::getName() const { return name; }
std::string Product::getCategory() const { return category; }
double Product::getPrice() const { return price; }
int Product::getStock() const { return stock_quantity; }

void Product::setName(std::string new_name) { name = new_name; }
void Product::setCategory(std::string new_category) { category = new_category; }
void Product::setPrice(double new_price) { price = new_price; }
void Product::setStock(int new_stock) { stock_quantity = new_stock; }

bool Product::isInStock() const { return stock_quantity > 0; }

void Product::display() const {
    std::cout << std::left
              << std::setw(5)  << product_id
              << std::setw(28) << name
              << std::setw(15) << category
              << "$" << std::fixed << std::setprecision(2) << std::setw(10) << price
              << "Stock: " << stock_quantity
              << "\n";
}
