#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include <string>
#include <vector>
#include <map>
#include "Product.h"

// Tracks how many times a specific user viewed or purchased a product
struct UserInteraction {
    std::string username;
    int product_id;
    int view_count;
    int purchase_count;
};

class Recommender {
private:
    std::vector<UserInteraction> all_interactions;

    void loadInteractions();
    void saveInteractions();

    // Helper: find or create an interaction entry for this user+product pair
    UserInteraction& findOrCreateInteraction(const std::string& username, int product_id);

public:
    Recommender();

    void recordView(const std::string& username, int product_id);
    void recordPurchase(const std::string& username, int product_id);

    // Returns top_n recommended products using the scoring formula:
    // score = (view_count * 0.2) + (purchase_count * 0.8) + category_bonus
    std::vector<Product> getRecommendations(const std::string& username,
                                            const std::vector<Product>& all_products,
                                            int top_n);

    // Admin stat helpers
    std::vector<std::pair<int, int>> getMostViewedProducts(int top_n);  // <product_id, total_views>
    std::vector<std::pair<int, int>> getBestSellingProducts(int top_n); // <product_id, total_purchases>
    std::vector<std::string> getActiveUsers(int top_n);
};

#endif
