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


character::character() : id(""), description(""), health(0), damage(0), peaceful(false), greeting(""), gift_item("") {}

character::character(const std::string& id,
    const std::string& description,
    int health,
    int damage,
    const std::vector<std::string>& drop_items,
    bool is_peaceful,
    const std::string& greeting_text,
    const std::string& gift_item_id) : id(id), description(description), health(health), damage(damage),
drop_items(drop_items), peaceful(is_peaceful),
greeting(greeting_text), gift_item(gift_item_id) {}


std::string character::get_id() const { return id; }

std::string character::get_description() const { return description; }

int character::get_health() const { return health; }

int character::get_damage() const { return damage; }

std::vector<std::string> character::get_drops() const {
    return drop_items;
}

void character::take_damage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

bool character::is_alive() const {
    return health > 0;
}

bool character::is_peaceful() const {
    return peaceful;
}

std::string character::get_greeting() const {
    return greeting;
}

std::string character::get_gift() const {
    return gift_item;
}

void character::clear_gift() {
    gift_item = "";
}
