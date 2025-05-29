#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>

class inventory {
public:
    static const size_t INVENTORY_MAX_SIZE = 7;
    void add_item(const std::string& item_id);
    void remove_item(const std::string& item_id);
    bool has_item(const std::string& item_id) const;
    const std::vector<std::string>& get_items() const;

private:
    std::vector<std::string> items;
};

#endif
