#include "control.h"
#include "game.h"
#include "location.h"
#include "characters.h"
#include "player.h"
#include "item.h"
#include "inventory.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

player::player()
    : player_inventory(new inventory()), size("normal"), hunger(100), health(100), base_damage(7) {}

player::~player() {
    delete player_inventory;
}

int player::get_health() const {
    return health;
}

void player::take_damage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void player::heal(int amount) {
    health += amount;
    if (health > 100) health = 100;
}

int player::get_base_damage() const {
    return base_damage;
}

void player::set_base_damage(int damage) {
    base_damage = damage;
}

void player::add_item(const std::string& item_id) {
    player_inventory->add_item(item_id);
}

bool player::has_item(const std::string& item_id) const {
    return player_inventory->has_item(item_id);
}

void player::remove_item(const std::string& item_id) {
    player_inventory->remove_item(item_id);
}


const std::vector<std::string>& player::get_inventory() const {
    return player_inventory->get_items();
}

void player::set_size(const std::string& new_size) {
    size = new_size;
}

std::string player::get_size() const {
    return size;
}

void player::change_hunger(int amount) {
    hunger += amount;
    if (hunger > 100) hunger = 100;
    if (hunger < 0) hunger = 0;
}

int player::get_hunger() const {
    return hunger;
}
