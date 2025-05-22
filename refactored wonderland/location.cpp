#include "control.h"
#include "game.h"
#include "location.h"
#include "characters.h"
#include "player.h"
#include "item.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

room::room(){


};
room::~room(){

};
room::room(const std::string& id, const std::string& desc, const std::string& required_size) : id(id), room_description(desc), size_required(required_size) {}

std::string room::get_size_required() const {
    return size_required;
}

std::string room::get_description() const{
    return room_description;
}
std::string room::get_next_room(const std::string& command) const {
    auto it = exits.find(command);
    if (it != exits.end()) {
        return it->second;
    }
    return "";
}
void room::add_exit(std::string command, std::string destination) {
    exits[command] = destination;
}
std::map<std::string, std::string> room::get_all_exits() const {
    return exits;
}    
void room::print_room_state(bool just_moved) {
        if (!visited_rooms[current_room]) {
        std::cout << "\n" << rooms[current_room].get_description() << "\n";
        visited_rooms[current_room] = true;
    } else if (just_moved) {
        std::cout << "\nYou are back in the " << current_room << ".\n";
    }

    const auto& exits = rooms[current_room].get_all_exits();
    if (!exits.empty()) {
        std::cout << "Exits visible: ";
        bool first = true;
        for (const auto& [dir, _] : exits) {
            if (!first) std::cout << ", ";
            std::cout << dir;
            first = false;
        }
        std::cout << ".\n";
    }

    if (characters_in_rooms.count(current_room) && !characters_in_rooms[current_room].empty()) {
        std::cout << "You see someone:\n";
        for (const auto& char_id : characters_in_rooms[current_room]) {
            std::cout << "- " << character_manager->all_characters[char_id].get_description() << "\n";
        }
    }    

    if (items_in_rooms.count(current_room) && !items_in_rooms[current_room].empty()) {
        std::cout << "You see:\n";
        for (const std::string& resolve_item_id : items_in_rooms[current_room]) {
            std::string desc = item_manager->all_items[resolve_item_id].get_description();
            if (desc.empty()) desc = resolve_item_id;
            std::cout << "- " << desc << "\n";                    }
    }
    
    if (chests.count(current_room) && !chests[current_room].empty()) {
        std::cout << "The chest contains:\n";
        for (const auto& id : chests[current_room]) {
            std::cout << "- " << item_manager->all_items[id].get_description() << "\n";
        }
    }
  game_manager->show_status();  
}


