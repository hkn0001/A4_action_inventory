#ifndef GAME_H
#define GAME_H


#include <string>
#include <map>
#include <vector>
#include <unordered_map>

class player;
class control;
class room;
class character;
class item;
class action;

class game {
private: 
    void handle_combat(character& enemy, const std::string& enemy_id, int damage_dealt);
public:
    std::vector<std::string> required_treasures;
    player* player_data;
   room* room_manager; 
   control* control_manager;
   item* item_manager;
   character* character_manager;
   action* action_manager;
   bool is_running;
    std::string resolve_item_id(const std::string& user_input);
    std::string resolve_character_id(const std::string& user_input);
    game();
    ~game();
    void setup();
    void run();
    void load_rooms(const std::string& filename);
    void load_items(const std::string& filename);
    void load_characters(const std::string& filename);
    std::unordered_map<std::string, std::string> item_alias_map;
    std::unordered_map<std::string, std::string> character_alias_map;    
    void load_item_aliases(const std::string& filename);
    void load_character_aliases(const std::string& filename);
    void load_required_treasures(const std::string& filename);
    void show_status();
};


#endif
