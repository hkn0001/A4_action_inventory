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
item::item() : damage(0), hunger_restore(0), size_change("") {

  

}

item::item(const std::string& id, const std::string& description,
           int damage, int hunger_restore, const std::string& size_change)
    : id(id), description(description), damage(damage),
      hunger_restore(hunger_restore), size_change(size_change) {}

std::string item::get_id() const { return id; }
std::string item::get_description() const { return description; }
int item::get_damage() const { return damage; }
int item::get_hunger_restore() const { return hunger_restore; }
std::string item::get_size_change() const { return size_change; }
