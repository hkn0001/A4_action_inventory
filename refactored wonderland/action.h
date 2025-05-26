#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <unordered_map>  

class game;
class character;
class item;
class room;
class player;



class action {
public:
    std::unordered_map<std::string, std::string> action_alias_map;
    action(game* g);
    void load_actions(const std::string& filename);
    std::string resolve_action_id(const std::string& user_input); 
    void take(const std::string& raw);
    void use(const std::string& raw);
    void drop(const std::string& raw);
    void store(const std::string& raw);
    void retrieve(const std::string& raw);
    void attack(const std::string& raw);
    void talk(const std::string& raw);
    void throw_item(const std::string& raw);
private:
    game* game_manager;
    player* player_data;
    item* item_manager;
    character* character_manager;
    room* room_manager;
};

#endif // ACTION_H
