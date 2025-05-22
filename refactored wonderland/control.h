
#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include <map>
#include <vector>
#include <unordered_map>


class player;
class character;
class room;
class game;
class item;


class control {
private:
std::vector<std::string> required_treasures = {"crown", "scepter", "cloak"};
void handle_combat(character& enemy, const std::string& enemy_id, int damage_dealt);

public:

    control(game* g);
    ~control();

    player* player_data;
    game* game_manager;
    room* room_manager; 
    item* item_manager;
    character* character_manager;

void process_command(const std::string& input);
std::string normalize_input(const std::string& raw);
std::string normalize_direction(const std::string& input);

};


#endif


