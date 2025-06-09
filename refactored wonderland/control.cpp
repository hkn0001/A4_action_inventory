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

size_t space_pos = cleaned_input.find(' ');
std::string verb = (space_pos != std::string::npos) ? cleaned_input.substr(0, space_pos) : cleaned_input;
std::string param = (space_pos != std::string::npos) ? cleaned_input.substr(space_pos + 1) : "";

std::string action_id = action_manager->resolve_action_id(verb);

if (action_id == "attack")   { action_manager->attack(param);   return; }
if (action_id == "take")     { action_manager->take(param);     return; }
if (action_id == "drop")     { action_manager->drop(param);     return; }
if (action_id == "use")      { action_manager->use(param);      return; }
if (action_id == "store")    { action_manager->store(param);    return; }
if (action_id == "retrieve") { action_manager->retrieve(param); return; }
if (action_id == "talk")     { action_manager->talk(input);     return; }
if (action_id == "throw")    { action_manager->throw_item(input); return; }



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

    if (
        cleaned_input.substr(0, 3) == "go " ||
        cmd != cleaned_input
    ) {
        std::cout << "You can't go that way.\n";
    } else {
        std::cout << "I don't understand what you are saying.\n";
    }
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
