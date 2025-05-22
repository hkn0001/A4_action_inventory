#ifndef ITEM_H
#define ITEM_H


class player;
class control;
class room;
class game;
class character;

#include <string>
#include <unordered_map>

class item {
private:
    std::string id;
    std::string description;
    int damage;
    int hunger_restore;
    std::string size_change; // "small", "big", "normal" or ""

public:
    item();
    item(const std::string& id, const std::string& description,
         int damage, int hunger_restore, const std::string& size_change);

    std::string get_id() const;
    std::string get_description() const;
    int get_damage() const;
    int get_hunger_restore() const;
    std::string get_size_change() const;
    std::unordered_map<std::string, item> all_items;

};

#endif
