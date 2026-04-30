#include "Recommender.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>

Recommender::Recommender() {
    loadInteractions();
}

void Recommender::loadInteractions() {
    all_interactions.clear();
    std::ifstream file("interactions.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string token;
        UserInteraction entry;

        std::getline(ss, entry.username, ',');
        std::getline(ss, token, ',');
        entry.product_id = std::stoi(token);
        std::getline(ss, token, ',');
        entry.view_count = std::stoi(token);
        std::getline(ss, token, ',');
        entry.purchase_count = std::stoi(token);

        all_interactions.push_back(entry);
    }
    file.close();
}

void Recommender::saveInteractions() {
    std::ofstream file("interactions.txt");
    for (const auto& entry : all_interactions) {
        file << entry.username << ","
             << entry.product_id << ","
             << entry.view_count << ","
             << entry.purchase_count << "\n";
    }
    file.close();
}

UserInteraction& Recommender::findOrCreateInteraction(const std::string& username, int product_id) {
    for (auto& entry : all_interactions) {
        if (entry.username == username && entry.product_id == product_id) {
            return entry;
        }
    }
    // Not found — create a new blank entry
    all_interactions.push_back({username, product_id, 0, 0});
    return all_interactions.back();
}

void Recommender::recordView(const std::string& username, int product_id) {
    UserInteraction& entry = findOrCreateInteraction(username, product_id);
    entry.view_count++;
    saveInteractions();
}

void Recommender::recordPurchase(const std::string& username, int product_id) {
    UserInteraction& entry = findOrCreateInteraction(username, product_id);
    entry.purchase_count++;
    saveInteractions();
}

std::vector<Product> Recommender::getRecommendations(const std::string& username,
                                                      const std::vector<Product>& all_products,
                                                      int top_n) {
    // Step 1: find which categories this user has interacted with
    std::set<std::string> preferred_categories;
    for (const auto& entry : all_interactions) {
        if (entry.username == username && (entry.view_count > 0 || entry.purchase_count > 0)) {
            for (const Product& p : all_products) {
                if (p.getId() == entry.product_id) {
                    preferred_categories.insert(p.getCategory());
                    break;
                }
            }
        }
    }

    if (preferred_categories.empty()) {
        // The user hasn't interacted with anything yet
        return {};
    }

    // Step 2: score every in-stock product
    std::vector<std::pair<double, int>> scored_products; // <score, product_id>

    for (const Product& p : all_products) {
        if (!p.isInStock()) continue;

        int view_count     = 0;
        int purchase_count = 0;

        for (const auto& entry : all_interactions) {
            if (entry.username == username && entry.product_id == p.getId()) {
                view_count     = entry.view_count;
                purchase_count = entry.purchase_count;
                break;
            }
        }

        // +1.0 bonus if the product is in a category the user already likes
        double category_bonus = preferred_categories.count(p.getCategory()) ? 1.0 : 0.0;

        double score = (view_count * 0.2) + (purchase_count * 0.8) + category_bonus;

        // Only include products with some relevance score
        if (score > 0.0) {
            scored_products.push_back({score, p.getId()});
        }
    }

    // Step 3: sort by score descending
    std::sort(scored_products.begin(), scored_products.end(),
              [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                  return a.first > b.first;
              });

    // Step 4: build the result list
    std::vector<Product> recommendations;
    int count = 0;
    for (const auto& score_pair : scored_products) {
        if (count >= top_n) break;
        for (const Product& p : all_products) {
            if (p.getId() == score_pair.second) {
                recommendations.push_back(p);
                break;
            }
        }
        count++;
    }

    return recommendations;
}

std::vector<std::pair<int, int>> Recommender::getMostViewedProducts(int top_n) {
    std::map<int, int> total_views_per_product;
    for (const auto& entry : all_interactions) {
        total_views_per_product[entry.product_id] += entry.view_count;
    }

    std::vector<std::pair<int, int>> result(total_views_per_product.begin(),
                                            total_views_per_product.end());
    std::sort(result.begin(), result.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return a.second > b.second;
              });

    if ((int)result.size() > top_n) result.resize(top_n);
    return result;
}

std::vector<std::pair<int, int>> Recommender::getBestSellingProducts(int top_n) {
    std::map<int, int> total_purchases_per_product;
    for (const auto& entry : all_interactions) {
        total_purchases_per_product[entry.product_id] += entry.purchase_count;
    }

    std::vector<std::pair<int, int>> result(total_purchases_per_product.begin(),
                                            total_purchases_per_product.end());
    std::sort(result.begin(), result.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return a.second > b.second;
              });

    if ((int)result.size() > top_n) result.resize(top_n);
    return result;
}

std::vector<std::string> Recommender::getActiveUsers(int top_n) {
    // Activity score = total views + total purchases across all products
    std::map<std::string, int> activity_score;
    for (const auto& entry : all_interactions) {
        activity_score[entry.username] += entry.view_count + entry.purchase_count;
    }

    std::vector<std::pair<std::string, int>> sorted_users(activity_score.begin(),
                                                          activity_score.end());
    std::sort(sorted_users.begin(), sorted_users.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second > b.second;
              });

    std::vector<std::string> active_users;
    int count = 0;
    for (const auto& user_pair : sorted_users) {
        if (count >= top_n) break;
        active_users.push_back(user_pair.first + " (" + std::to_string(user_pair.second) + " interactions)");
        count++;
    }
    return active_users;
}
