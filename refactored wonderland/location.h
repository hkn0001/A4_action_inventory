#ifndef room_h
#define room_h

#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>

class player;
class control;
class character;
class game;
class item;


class room{

    private:
    std::string command;
    std::string id;
    std::string room_description;
    std::map<std::string, std::string> exits;
    std::string size_required;

    public:


    std::unordered_map<std::string, std::vector<std::string>> chests;
    room();
    ~room();
    room(const std::string& id, const std::string& description, const std::string& size_required = "");
    std::unordered_map<std::string, std::vector<std::string>> items_in_rooms;
    std::unordered_map<std::string, std::vector<std::string>> characters_in_rooms;
    std::unordered_map<std::string, bool> visited_rooms;
    std::string current_room;
    std::string get_size_required() const;
    std::map<std::string, std::string> get_all_exits() const;
    void add_exit(std::string command, std::string destination);
    std::string get_description() const;
    std::string get_next_room(const std::string& command) const;
    void print_room_state(bool just_moved);
    std::map<std::string, room> rooms;    

    item* item_manager;
    player* player_data;
    character* character_manager;
    game* game_manager;
    room* room_manager;
};


#endif