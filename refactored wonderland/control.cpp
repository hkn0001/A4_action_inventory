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

control::control(game* g) {
    game_manager = g;
    player_data = g->player_data;
    room_manager = g->room_manager;
    item_manager = g->item_manager;
    character_manager = g->character_manager;
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

    if (cleaned_input.substr(0, 7) == "attack ") {
        std::string target_id = game_manager->resolve_character_id(cleaned_input.substr(7));
        if (target_id.empty()) {
            std::cout << "I don't know who you mean.\n";
            return;
        }
        auto& room_characters = room_manager->characters_in_rooms[room_manager->current_room];
        auto it = std::find(room_characters.begin(), room_characters.end(), target_id);

        if (it == room_characters.end()) {
            std::cout << "There is no one named '" << target_id << "' here.\n";
            return;
        }

        character& enemy = character_manager->all_characters[target_id];

        int damage = player_data->get_base_damage();
        bool is_critical = (rand() % 100) < 25;
        if (is_critical) {
            damage = int(damage * 1.5);
            std::cout << "Critical hit!\n";
        }

        std::cout << "You attack " << target_id << " for " << damage << " damage.\n";
        enemy.take_damage(damage);

        if (!enemy.is_alive()) {
            std::cout << target_id << " is defeated!\n";
            for (const auto& drop : enemy.get_drops()) {
                room_manager->items_in_rooms[room_manager->current_room].push_back(drop);
                std::cout << target_id << " dropped a " << drop << ".\n";
            }
            room_characters.erase(it);
            return;
        }

        int retaliation = enemy.get_damage();
        player_data->take_damage(retaliation);
        std::cout << target_id << " attacks you back for " << retaliation << " damage!\n";
        game_manager->show_status();

        if (player_data->get_health() <= 0) {
            std::cout << "You have been defeated...\n";
            game_manager->is_running = false;
        }
        return;
    }

    if (cleaned_input.substr(0, 4) == "talk") {
        if (cleaned_input.length() <= 5) {
            std::cout << "Talk to who?\n";
            return;
        }

        std::string target_raw = input;
        if (target_raw.substr(0, 7) == "talk to") {
            if (target_raw.length() <= 8) {
                std::cout << "Talk to who?\n";
                return;
            }
            target_raw = target_raw.substr(8);
        } else {
            target_raw = target_raw.substr(5);
        }

        std::string target_id = game_manager->resolve_character_id(target_raw);
        if (target_id.empty()) {
            std::cout << "I don't know who you want to talk to.\n";
            return;
        }

        auto& room_characters = room_manager->characters_in_rooms[room_manager->current_room];
        auto it = std::find(room_characters.begin(), room_characters.end(), target_id);
        if (it == room_characters.end()) {
            std::cout << "There is no one named '" << target_id << "' here.\n";
            return;
        }

        character& npc = character_manager->all_characters[target_id];

        if (!npc.is_peaceful()) {
            std::cout << target_id << " growls and attacks you instead!\n";
            int retaliation = npc.get_damage();
            bool crit = (rand() % 100) < 20;
            if (crit) {
                retaliation = int(retaliation * 1.5);
                std::cout << "Critical hit from " << target_id << "!\n";
            }

            player_data->take_damage(retaliation);
            std::cout << target_id << " hits you for " << retaliation << " damage!\n";

            if (player_data->get_health() <= 0) {
                std::cout << "You have been defeated...\n";
                game_manager->is_running = false;
            }
            return;
        }

        std::cout << target_id << " says: " << npc.get_greeting() << "\n";

        std::string gift = npc.get_gift();
        if (!gift.empty() && item_manager->all_items.count(gift)) {
            player_data->add_item(gift);
            std::cout << target_id << " gives you a " << gift << ".\n";
            npc.clear_gift();
        }

        return;
    }

    if (cleaned_input.substr(0, 5) == "take ") {
        std::string item_id = game_manager->resolve_item_id(cleaned_input.substr(5));
        auto& items_here = room_manager->items_in_rooms[room_manager->current_room];
        auto it = std::find(items_here.begin(), items_here.end(), item_id);

        if (it != items_here.end()) {
            player_data->add_item(item_id);
            items_here.erase(it);
            std::cout << "You picked up the " << item_id << ".\n";
        } else {
            std::cout << "There is no " << item_id << " here.\n";
        }
        return;
    }

    if (cleaned_input.substr(0, 4) == "use ") {
        std::string item_id = game_manager->resolve_item_id(cleaned_input.substr(4));
        if (!player_data->has_item(item_id)) {
            std::cout << "You don't have that item.\n";
            return;
        }

        const item& i = item_manager->all_items[item_id];

        std::string new_size = i.get_size_change();
        if (!new_size.empty()) {
            player_data->set_size(new_size);
            std::cout << "You feel yourself change size! You are now " << new_size << ".\n";
        }

        int hunger = i.get_hunger_restore();
        if (hunger > 0) {
            player_data->change_hunger(hunger);
            std::cout << "You feel nourished. Hunger: " << player_data->get_hunger() << "\n";
        }

        player_data->remove_item(item_id);
        std::cout << "You used the " << item_id << ".\n";
        return;
    }

    if (cleaned_input.substr(0, 5) == "drop ") {
        std::string item_id = game_manager->resolve_item_id(cleaned_input.substr(5));
        if (!player_data->has_item(item_id)) {
            std::cout << "You don't have that item.\n";
            return;
        }

        player_data->remove_item(item_id);
        room_manager->items_in_rooms[room_manager->current_room].push_back(item_id);
        std::cout << "You dropped the " << item_id << ".\n";
        return;
    }

    if (cleaned_input.substr(0, 6) == "store ") {
        std::string item_id = game_manager->resolve_item_id(cleaned_input.substr(6));
        if (!player_data->has_item(item_id)) {
            std::cout << "You don't have that item.\n";
            return;
        }

        auto& chest = room_manager->chests[room_manager->current_room];
        if (chest.size() >= 3) {
            std::cout << "The chest is full. You can only store 3 items here.\n";
            return;
        }

        chest.push_back(item_id);
        player_data->remove_item(item_id);
        std::cout << "You stored the " << item_id << " in the chest.\n";
        return;
    }

    if (cleaned_input.substr(0, 9) == "retrieve ") {
        std::string item_id = game_manager->resolve_item_id(cleaned_input.substr(9));
        auto& chest = room_manager->chests[room_manager->current_room];
        auto it = std::find(chest.begin(), chest.end(), item_id);

        if (it == chest.end()) {
            std::cout << "That item isn't in the chest.\n";
            return;
        }

        player_data->add_item(item_id);
        chest.erase(it);
        std::cout << "You retrieved the " << item_id << " from the chest.\n";
        return;
    }

    if (cleaned_input.substr(0, 6) == "throw ") {
        size_t at_pos = cleaned_input.find(" at ");
        if (at_pos == std::string::npos) {
            std::cout << "Invalid command. Use: throw <item> at <character>\n";
            return;
        }

        std::string item_id = game_manager->resolve_item_id(cleaned_input.substr(6, at_pos - 6));
        std::string target_id = game_manager->resolve_character_id(cleaned_input.substr(at_pos + 4));
        if (target_id.empty()) {
            std::cout << "I don't know who you mean.\n";
            return;
        }

        if (!player_data->has_item(item_id)) {
            std::cout << "You don't have that item.\n";
            return;
        }

        const item& i = item_manager->all_items[item_id];
        int item_damage = i.get_damage();
        if (item_damage <= 0) {
            std::cout << "That item cannot be thrown effectively.\n";
            return;
        }

        auto& room_characters = room_manager->characters_in_rooms[room_manager->current_room];
        auto it = std::find(room_characters.begin(), room_characters.end(), target_id);
        if (it == room_characters.end()) {
            std::cout << "There's no one named '" << target_id << "' here.\n";
            return;
        }

        player_data->remove_item(item_id);

        bool hit = (rand() % 100) < 70;
        if (!hit) {
            std::cout << "You threw the " << item_id << " at " << target_id << " but missed!\n";
            return;
        }

        int damage = item_damage;
        bool is_critical = (rand() % 100) < 20;
        if (is_critical) {
            damage = int(damage * 1.5);
            std::cout << "Critical throw!\n";
        }

        std::cout << "You hit " << target_id << " with the " << item_id << " for " << damage << " damage.\n";
        character& enemy = character_manager->all_characters[target_id];
        enemy.take_damage(damage);

        if (!enemy.is_alive()) {
            std::cout << target_id << " is defeated!\n";
            for (const auto& drop : enemy.get_drops()) {
                room_manager->items_in_rooms[room_manager->current_room].push_back(drop);
                std::cout << target_id << " dropped a " << drop << ".\n";
            }
            room_characters.erase(it);
            return;
        }

        int retaliation = enemy.get_damage();
        player_data->take_damage(retaliation);
        std::cout << target_id << " strikes back for " << retaliation << " damage!\n";

        if (player_data->get_health() <= 0) {
            std::cout << "You have been defeated...\n";
            game_manager->is_running = false;
        }
        return;
    }

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
