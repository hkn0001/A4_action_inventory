#include "control.h"
#include "game.h"
#include "location.h"
#include "characters.h"
#include "player.h"
#include "item.h"
#include "action.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>


control::control(game* g) {
    game_manager = g;
    player_data = g->player_data;
    room_manager = g->room_manager;
    item_manager = g->item_manager;
    character_manager = g->character_manager;
    action_manager = g->action_manager;
    
}


control::~control(){

};

void control::process_command(const std::string& input) {

    std::string cleaned_input = normalize_input(input);
    std::string previous_room = room_manager->current_room;

    if (cleaned_input == "look") {
        room_manager->print_room_state(false);
        return;
    }

    if (room_manager->current_room != previous_room) {
        room_manager->print_room_state(true);
    }

    if (cleaned_input == "quit") {
        game_manager->is_running = false;
        std::cout << "Goodbye.\n";
        return;
    }

// INVENTORY
if (cleaned_input == "inventory" || cleaned_input == "i" || cleaned_input == "invent") {
    const auto& inv = player_data->get_inventory();
    if (inv.empty()) {
        std::cout << "Your inventory is empty.\n";
    } else {
        std::cout << "You are carrying:\n";
        for (const auto& item_id : inv) {
            std::cout << "- " << item_manager->all_items[item_id].get_description() << "\n";
        }
    }
    return;
}

// ACTION DISPATCH
if (cleaned_input.rfind("take ",0) == 0)      { action_manager->take(cleaned_input.substr(5));       return; }
if (cleaned_input.rfind("use ",0) == 0)       { action_manager->use(cleaned_input.substr(4));        return; }
if (cleaned_input.rfind("drop ",0) == 0)      { action_manager->drop(cleaned_input.substr(5));       return; }
if (cleaned_input.rfind("store ",0) == 0)     { action_manager->store(cleaned_input.substr(6));      return; }
if (cleaned_input.rfind("retrieve ",0) == 0)  { action_manager->retrieve(cleaned_input.substr(9));   return; }
if (cleaned_input.rfind("attack ",0) == 0)    { action_manager->attack(cleaned_input.substr(7));     return; }
if (cleaned_input.rfind("talk",0) == 0)       { action_manager->talk(input);                         return; }
if (cleaned_input.rfind("throw ",0) == 0)     { action_manager->throw_item(cleaned_input);           return; }


    std::string cmd = normalize_direction(cleaned_input);
    std::string next_room = room_manager->rooms[room_manager->current_room].get_next_room(cmd);

    if (!next_room.empty()) {
        std::string required_size = room_manager->rooms[next_room].get_size_required();
        if (required_size.empty() || required_size == player_data->get_size()) {
            room_manager->current_room = next_room;
        } else {
            std::cout << "You can't enter this room at your current size.\n";
        }
    } else {
        std::cout << "You can't go that way.\n";
    }

    bool moved = (room_manager->current_room != previous_room);
    room_manager->print_room_state(moved);

    if (room_manager->current_room == "safe_room") {
        bool has_all = true;
        for (const auto& treasure : required_treasures) {
            if (!player_data->has_item(treasure)) {
                has_all = false;
                break;
            }
        }
        if (has_all) {
            std::cout << "You have returned to the safe room with all the treasures!\n";
            std::cout << "Congratulations — you win!\n";
            game_manager->is_running = false;
            return;
        }
    }
}

std::string control::normalize_input(const std::string& raw) {
    std::string cleaned = raw;
    std::transform(cleaned.begin(), cleaned.end(), cleaned.begin(), ::tolower);
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), ::ispunct), cleaned.end());
    return cleaned;
}

std::string control::normalize_direction(const std::string& input) {
    static std::unordered_map<std::string, std::string> direction_aliases = {
        {"north", "go north"}, {"n", "go north"}, {"walk north", "go north"}, {"move north", "go north"},
        {"south", "go south"}, {"s", "go south"}, {"walk south", "go south"}, {"move south", "go south"},
        {"east",  "go east"},  {"e", "go east"},  {"walk east",  "go east"},  {"move east",  "go east"},
        {"west",  "go west"},  {"w", "go west"},  {"walk west",  "go west"},  {"move west",  "go west"},
        {"up",    "go up"},    {"u", "go up"},    {"climb up",   "go up"},
        {"down",  "go down"},  {"d", "go down"},  {"climb down", "go down"}, {"fall", "go down"}
    };

    std::string cleaned = input;
    std::transform(cleaned.begin(), cleaned.end(), cleaned.begin(), ::tolower);
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), ::ispunct), cleaned.end());

    if (direction_aliases.count(cleaned)) {
        return direction_aliases[cleaned];
    }

    return cleaned;
}
