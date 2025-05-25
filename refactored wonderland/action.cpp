#include "action.h"
#include "game.h"
#include "player.h"
#include "item.h"
#include "characters.h"
#include "location.h"
#include <algorithm>
#include <iostream>

action::action(game* g) {
    game_manager = g;
    player_data = g->player_data;
    item_manager = g->item_manager;
    character_manager = g->character_manager;
    room_manager = g->room_manager;
}


// ──────── take ────────
// from control.cpp “take ” block
void action::take(const std::string& raw) {
    std::string item_id = game_manager->resolve_item_id(raw);
    if (item_id.empty()) {
        std::cout << "There is no item called '" << raw << "' here.\n";
        return;
    }

    auto& items_here = room_manager->items_in_rooms[room_manager->current_room];
    auto it = std::find(items_here.begin(), items_here.end(), item_id);
    if (it != items_here.end()) {
        player_data->add_item(item_id);
        items_here.erase(it);
        std::cout << "You picked up the " << item_id << ".\n";
    } else {
        std::cout << "There is no " << item_id << " here.\n";
    }
}

// ──────── use ────────
// from control.cpp “use ” block
void action::use(const std::string& raw) {
    std::string item_id = game_manager->resolve_item_id(raw);
    if (! player_data->has_item(item_id)) {
        std::cout << "You don't have that item.\n";
        return;
    }
    const item& i = item_manager->all_items[item_id];
    std::string new_size = i.get_size_change();
    if (new_size != "") {
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
}

// ──────── drop ────────
// from control.cpp “drop ” block
void action::drop(const std::string& raw) {
    std::string item_id = game_manager->resolve_item_id(raw);
    if (! player_data->has_item(item_id)) {
        std::cout << "You don't have that item.\n";
        return;
    }
    player_data->remove_item(item_id);
    room_manager->items_in_rooms[room_manager->current_room].push_back(item_id);
    std::cout << "You dropped the " << item_id << ".\n";
}

// ──────── store ────────
// from control.cpp “store ” block
void action::store(const std::string& raw) {
    std::string item_id = game_manager->resolve_item_id(raw);
    if (! player_data->has_item(item_id)) {
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
}

// ───── retrieve ───────
// from control.cpp “retrieve ” block
void action::retrieve(const std::string& raw) {
    std::string item_id = game_manager->resolve_item_id(raw);
    auto& chest = room_manager->chests[room_manager->current_room];
    auto it = std::find(chest.begin(), chest.end(), item_id);
    if (it == chest.end()) {
        std::cout << "That item isn't in the chest.\n";
        return;
    }
    player_data->add_item(item_id);
    chest.erase(it);
    std::cout << "You retrieved the " << item_id << " from the chest.\n";
}

// ─────── attack ────────
// from control.cpp “attack ” block
void action::attack(const std::string& raw) {
    std::string target_id = game_manager->resolve_character_id(raw);
    if (target_id == "") {
        std::cout << "I don't know who you mean.\n";
        return;
    }
    auto& room_chars = room_manager->characters_in_rooms[room_manager->current_room];
    auto it = std::find(room_chars.begin(), room_chars.end(), target_id);
    if (it == room_chars.end()) {
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
    if (! enemy.is_alive()) {
        std::cout << target_id << " is defeated!\n";
        for (const auto& drop : enemy.get_drops()) {
            room_manager->items_in_rooms[room_manager->current_room].push_back(drop);
            std::cout << target_id << " dropped a " << drop << ".\n";
        }
        room_chars.erase(it);
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
}

// ──────── talk ────────
// from control.cpp “talk” block
void action::talk(const std::string& raw_input) {
    std::string target_raw = raw_input;
    if (target_raw.substr(0,7) == "talk to") {
        if (target_raw.length() <= 8) {
            std::cout << "Talk to who?\n";
            return;
        }
        target_raw = target_raw.substr(8);
    } else {
        if (target_raw.length() <= 5) {
            std::cout << "Talk to who?\n";
            return;
        }
        target_raw = target_raw.substr(5);
    }
    std::string target_id = game_manager->resolve_character_id(target_raw);
    if (target_id.empty()) {
        std::cout << "I don't know who you want to talk to.\n";
        return;
    }
    auto& room_chars = room_manager->characters_in_rooms[room_manager->current_room];
    auto it = std::find(room_chars.begin(), room_chars.end(), target_id);
    if (it == room_chars.end()) {
        std::cout << "There is no one named '" << target_id << "' here.\n";
        return;
    }
    character& npc = character_manager->all_characters[target_id];
    if (! npc.is_peaceful()) {
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
    if (gift != "" && item_manager->all_items.count(gift)) {
        player_data->add_item(gift);
        std::cout << target_id << " gives you a " << gift << ".\n";
        npc.clear_gift();
    }
}

// ───── throw_item ─────
// from control.cpp “throw ” block
void action::throw_item(const std::string& raw) {
    size_t at_pos = raw.find(" at ");
    if (at_pos == std::string::npos) {
        std::cout << "Invalid command. Use: throw <item> at <character>\n";
        return;
    }
    std::string item_id = game_manager->resolve_item_id(raw.substr(6, at_pos - 6));
    std::string target_id = game_manager->resolve_character_id(raw.substr(at_pos + 4));
    if (target_id.empty()) {
        std::cout << "I don't know who you mean.\n";
        return;
    }
    if (! player_data->has_item(item_id)) {
        std::cout << "You don't have that item.\n";
        return;
    }
    const item& i = item_manager->all_items[item_id];
    int item_damage = i.get_damage();
    if (item_damage <= 0) {
        std::cout << "That item cannot be thrown effectively.\n";
        return;
    }
    auto& room_chars = room_manager->characters_in_rooms[room_manager->current_room];
    auto it = std::find(room_chars.begin(), room_chars.end(), target_id);
    if (it == room_chars.end()) {
        std::cout << "There's no one named '" << target_id << "' here.\n";
        return;
    }
    player_data->remove_item(item_id);
    bool hit = (rand() % 100) < 70;
    if (! hit) {
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
    if (! enemy.is_alive()) {
        std::cout << target_id << " is defeated!\n";
        for (const auto& drop : enemy.get_drops()) {
            room_manager->items_in_rooms[room_manager->current_room].push_back(drop);
            std::cout << target_id << " dropped a " << drop << ".\n";
        }
        room_chars.erase(it);
        return;
    }
    int retaliation = enemy.get_damage();
    player_data->take_damage(retaliation);
    std::cout << target_id << " strikes back for " << retaliation << " damage!\n";
    if (player_data->get_health() <= 0) {
        std::cout << "You have been defeated...\n";
        game_manager->is_running = false;
    }
}
