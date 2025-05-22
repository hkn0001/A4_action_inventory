#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include <unordered_map>

class player;
class control;
class room;
class game;
class item;

class character {
    private:
        std::string id;
        std::string description;
        int health;
        int damage;
    
        std::vector<std::string> drop_items;
        bool peaceful;
        std::string greeting;
        std::string gift_item;
    
    public:
        character();  // default constructor
    
        character(
const std::string& id,
const std::string& description,
int health,
int damage,
const std::vector<std::string>& drop_items,
bool is_peaceful,
const std::string& greeting_text,
const std::string& gift_item_id);
std::string get_id() const;
int get_health() const;
int get_damage() const;
bool is_alive() const;
void take_damage(int amount);
std::vector<std::string> get_drops() const;
bool is_peaceful() const;
std::string get_greeting() const;
std::string get_gift() const;
void clear_gift();
std::string get_description() const;
std::unordered_map<std::string, character> all_characters;
    };

#endif


