#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

class Product {
private:
    int product_id;
    std::string name;
    std::string category;
    double price;
    int stock_quantity;

public:
    Product();
    Product(int id, std::string name, std::string category, double price, int stock);

    int getId() const;
    std::string getName() const;
    std::string getCategory() const;
    double getPrice() const;
    int getStock() const;

    void setName(std::string new_name);
    void setCategory(std::string new_category);
    void setPrice(double new_price);
    void setStock(int new_stock);

    void display() const;
    bool isInStock() const;
};

#endif
