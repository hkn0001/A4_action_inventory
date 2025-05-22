#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

class room;
class control;
class character;
class game;
class item;



class player {
private:
    std::vector<std::string> inventory;
    std::string size;
    int hunger;
    int health;
    int base_damage;

public:
    player();

    int get_health() const;
    void take_damage(int amount) ;
    void heal(int amount);
    int get_base_damage () const;
    void set_base_damage(int damage) ;
    void add_item(const std::string& item_id);
    bool has_item(const std::string& item_id) const;
    void remove_item(const std::string& item_id);
    const std::vector<std::string>& get_inventory() const;

    void set_size(const std::string& new_size);
    std::string get_size() const;

    void change_hunger(int amount);
    int get_hunger() const;
};

#endif
