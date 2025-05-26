// #include "control.h"
// #include "game.h"
// #include "location.h"
// #include "characters.h"
// #include "player.h"
// #include "item.h"
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <algorithm>
// #include <cstdlib>

#include "inventory.h"
#include <algorithm>

void inventory::add_item(const std::string& item_id) {
    items.push_back(item_id);
}

void inventory::remove_item(const std::string& item_id) {
    auto it = std::remove(items.begin(), items.end(), item_id);
    if (it != items.end()) {
        items.erase(it, items.end());
    }
}

bool inventory::has_item(const std::string& item_id) const {
    return std::find(items.begin(), items.end(), item_id) != items.end();
}

const std::vector<std::string>& inventory::get_items() const {
    return items;
}
