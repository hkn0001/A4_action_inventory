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

game::game() : is_running(true) {
    player_data = new player();
    item_manager = new item();
    character_manager = new character(); 
    room_manager = new room();
    room_manager->game_manager = this;
    action_manager = new action(this);
    control_manager = new control(this);



    room_manager->character_manager = character_manager;
    room_manager->item_manager = item_manager;
    room_manager->player_data = player_data;
    room_manager->game_manager = this;
    

}

game::~game(){
    delete player_data;
    delete room_manager;
    delete control_manager;
}

void game::setup() {
    load_rooms("rooms.txt");
    load_items("items.txt");
    load_characters("characters.txt");
    load_item_aliases("item_aliases.txt");
    load_character_aliases("character_aliases.txt");
    load_required_treasures("required_treasures.txt");
    action_manager->load_actions("actions.txt");
}

void game::run() {

    // Clear screen by printing many newlines
for (int i = 0; i < 20; ++i) std::cout << "\n";

// ASCII Art Title
std::cout << R"(

  /$$$$$$  /$$ /$$                           /$$                 /$$      /$$                           /$$                     /$$                           /$$
 /$$__  $$| $$|__/                          |__/                | $$  /$ | $$                          | $$                    | $$                          | $$
| $$  \ $$| $$ /$$  /$$$$$$$  /$$$$$$        /$$ /$$$$$$$       | $$ /$$$| $$  /$$$$$$  /$$$$$$$   /$$$$$$$  /$$$$$$   /$$$$$$ | $$  /$$$$$$  /$$$$$$$   /$$$$$$$
| $$$$$$$$| $$| $$ /$$_____/ /$$__  $$      | $$| $$__  $$      | $$/$$ $$ $$ /$$__  $$| $$__  $$ /$$__  $$ /$$__  $$ /$$__  $$| $$ |____  $$| $$__  $$ /$$__  $$
| $$__  $$| $$| $$| $$      | $$$$$$$$      | $$| $$  \ $$      | $$$$_  $$$$| $$  \ $$| $$  \ $$| $$  | $$| $$$$$$$$| $$  \__/| $$  /$$$$$$$| $$  \ $$| $$  | $$
| $$  | $$| $$| $$| $$      | $$_____/      | $$| $$  | $$      | $$$/ \  $$$| $$  | $$| $$  | $$| $$  | $$| $$_____/| $$      | $$ /$$__  $$| $$  | $$| $$  | $$
| $$  | $$| $$| $$|  $$$$$$$|  $$$$$$$      | $$| $$  | $$      | $$/   \  $$|  $$$$$$/| $$  | $$|  $$$$$$$|  $$$$$$$| $$      | $$|  $$$$$$$| $$  | $$|  $$$$$$$
|__/  |__/|__/|__/ \_______/ \_______/      |__/|__/  |__/      |__/     \__/ \______/ |__/  |__/ \_______/ \_______/|__/      |__/ \_______/|__/  |__/ \_______/

)";

// Game intro text
std::cout << "\nWelcome to Alice in Wonderland.\n";
std::cout << "Goal: Find all required treasures and return them to the safe room.\n";
std::cout << "Commands: look, go <direction>, inventory, take <item>, drop <item>, use <item>, attack <character>, talk to <character>, throw <item> at <character>\n";
std::cout << "Explore carefully. Some rooms require specific sizes to enter.\n\n";
std::cout << "You feel the ground vanish beneath your feet. With a startled gasp, you plunge into darkness—an endless vertical tunnel lined with crooked shelves, clocks spinning backward, and floating teacups suspended in midair. The air rushes past you, tugging at your clothes as the walls twist and shimmer with impossible geometry. You fall, and fall, and fall—so long that you begin to wonder if the descent will ever end.\nSuddenly, the tunnel flares open. You crash through a hanging curtain, land hard on a slope of soft moss, and bounce—once, then twice—tumbling uncontrollably through a dim burrow carved from earth and shadow. Shapes blur past. A startled white rabbit in a waistcoat glances up mid-mutter as you barrel past him with no time to stop or steer.\nBefore you can regain your footing, your momentum carries you straight through a crooked wooden doorway. Light explodes around you. You tumble out onto a sunlit meadow, the air filled with the scent of wildflowers and the hush of distant breezes. The grass is impossibly green. Towering petals bend overhead like parasols. Behind you, the doorway yawns silently in the hillside, swallowed by the shadow of the burrow.\n\n";


    std::string input;

    if (!room_manager->visited_rooms[room_manager->current_room]) {
        std::cout << "\n" << room_manager->rooms[room_manager->current_room].get_description() << "\n";
       room_manager->visited_rooms[room_manager->current_room] = true;
    } else {
        std::cout << "\nYou are back in the " << room_manager->current_room << ".\n";
    }
    
    // Show visible exits
    const auto& exits = room_manager->rooms[room_manager->current_room].get_all_exits();
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
    
    // Show characters
    if (room_manager->characters_in_rooms.count(room_manager->current_room)) {
        std::cout << "You see someone:\n";

        for (const auto& char_id : room_manager->characters_in_rooms[room_manager->current_room]) {
            std::cout << "- " << character_manager->all_characters[char_id].get_description() << "\n";
        }
    }

    
    // Show items
    if (room_manager->items_in_rooms.count(room_manager->current_room)) {
        std::cout << "You see:\n";
        for (const std::string& resolve_item_id : room_manager->items_in_rooms[room_manager->current_room]) {
            std::cout << "- " << item_manager->all_items[resolve_item_id].get_description() << "\n";
        }
    }
    
    // Show chest contents
    if (room_manager->chests.count(room_manager->current_room) && !room_manager->chests[room_manager->current_room].empty()) {
        std::cout << "The chest contains:\n";
        for (const auto& id : room_manager->chests[room_manager->current_room]) {
            std::cout << "- " << item_manager->all_items[id].get_description() << "\n";
        }
    }

    show_status();
    
    while (is_running) {
        
        std::cout << "> ";
        std::getline(std::cin, input);
        // Hunger decreases over time
        int hunger_loss = 1 + rand() % 3;  // lose 1–3 points
        player_data->change_hunger(-hunger_loss);

        if (player_data->get_hunger() <= 0) {
            std::cout << "You have died of starvation.\n";
            is_running = false;
            return;
        }
        int current_hunger = player_data->get_hunger();
        if (current_hunger < 30 && current_hunger > 0) {
            std::cout << "You feel very hungry. Hunger: " << current_hunger << "\n";
        }
        control_manager->process_command(input);
    }
}

void game::load_rooms(const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open room file.\n";
        is_running = false;
        return;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream line_stream(line);
        std::string id, description, size_required, exit_string;

        std::getline(line_stream, id, '|');
        std::getline(line_stream, description, '|');
        std::getline(line_stream, size_required, '|');
        std::getline(line_stream, exit_string);

        room r(id, description, size_required);

        r.character_manager = character_manager;
        r.item_manager = item_manager;
        r.player_data = player_data;
        r.game_manager = this;

        std::istringstream exit_stream(exit_string);
        std::string exit_entry;
        while (std::getline(exit_stream, exit_entry, ',')) {
            size_t eq_pos = exit_entry.find('=');
            if (eq_pos != std::string::npos) {
                std::string command = exit_entry.substr(0, eq_pos);
                std::string destination = exit_entry.substr(eq_pos + 1);
                r.add_exit(command, destination);
            }
        }

        room_manager->rooms[id] = r;
}
    
    room_manager->current_room = "meadow";
}

void game::load_items(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open item file.\n";
        is_running = false;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream line_stream(line);
        std::string id, description, location, damage_str, hunger_str, size_change, aliases_str;

        std::getline(line_stream, id, '|');
        std::getline(line_stream, description, '|');
        std::getline(line_stream, location, '|');
        std::getline(line_stream, damage_str, '|');
        std::getline(line_stream, hunger_str, '|');
        std::getline(line_stream, size_change, '|');
        std::getline(line_stream, aliases_str);

        int damage = std::stoi(damage_str);
        int hunger_restore = std::stoi(hunger_str);

        item new_item(id, description, damage, hunger_restore, size_change);
        item_manager->all_items[id] = new_item;

        if (!location.empty()) {
            room_manager->items_in_rooms[location].push_back(id);
        }

        // Aliases
        std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        item_alias_map[id] = id;

        std::istringstream alias_stream(aliases_str);
        std::string alias;
        while (std::getline(alias_stream, alias, ',')) {
            alias.erase(std::remove_if(alias.begin(), alias.end(), ::isspace), alias.end());
            std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);
            if (!alias.empty()) item_alias_map[alias] = id;
        }
    }
}

void game::load_characters(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open character file.\n";
        is_running = false;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream line_stream(line);
        std::string id, description, location, health_str, damage_str, drop_str, aliases_str, peaceful_str, greeting, gift;

        std::getline(line_stream, id, '|');
        std::getline(line_stream, description, '|');
        std::getline(line_stream, location, '|');
        std::getline(line_stream, health_str, '|');
        std::getline(line_stream, damage_str, '|');
        std::getline(line_stream, drop_str, '|');
        std::getline(line_stream, aliases_str, '|');
        std::getline(line_stream, peaceful_str, '|');
        std::getline(line_stream, greeting, '|');
        std::getline(line_stream, gift);

        int health = std::stoi(health_str);
        int damage = std::stoi(damage_str);
        bool peaceful = (peaceful_str == "1");

        std::vector<std::string> drops;
        std::istringstream drop_stream(drop_str);
        std::string item;
        while (std::getline(drop_stream, item, ',')) {
            if (!item.empty()) drops.push_back(item);
        }

        character c(id, description, health, damage, drops, peaceful, greeting, gift);
        character_manager->all_characters[id] = c;
        room_manager->characters_in_rooms[location].push_back(id);

        // Aliases
        std::transform(id.begin(), id.end(), id.begin(), ::tolower);
        character_alias_map[id] = id;

        std::istringstream alias_stream(aliases_str);
        std::string alias;
        while (std::getline(alias_stream, alias, ',')) {
            alias.erase(std::remove_if(alias.begin(), alias.end(), ::isspace), alias.end());
            std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);
            if (!alias.empty()) character_alias_map[alias] = id;
        }
    }
}

void game::handle_combat(character& enemy, const std::string& enemy_id, int damage_dealt) {
    enemy.take_damage(damage_dealt);
    if (!enemy.is_alive()) {
        std::cout << enemy_id << " is defeated!\n";
        for (const auto& drop : enemy.get_drops()) {
            room_manager->items_in_rooms[room_manager->current_room].push_back(drop);
            std::cout << enemy_id << " dropped a " << drop << ".\n";
        }
        auto& room_characters = room_manager->characters_in_rooms[room_manager->current_room];
        auto it = std::find(room_characters.begin(), room_characters.end(), enemy_id);
        if (it != room_characters.end()) room_characters.erase(it);
        return;
    }

    // Enemy counterattacks
    int retaliation = enemy.get_damage();
    bool crit = (rand() % 100) < 20;
    if (crit) {
        retaliation = int(retaliation * 1.5);
        std::cout << "Critical hit from " << enemy_id << "!\n";
    }

    player_data->take_damage(retaliation);
    std::cout << enemy_id << " strikes back for " << retaliation << " damage!\n";

    if (player_data->get_health() <= 0) {
        std::cout << "You have been fatally wounded...\n";
        is_running = false;
    }
}

void game::load_item_aliases(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open item_aliases.txt\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id, aliases;
        if (std::getline(iss, id, '|') && std::getline(iss, aliases)) {
            std::istringstream alias_stream(aliases);
            std::string alias;
            while (std::getline(alias_stream, alias, ',')) {
                std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);
                alias.erase(std::remove_if(alias.begin(), alias.end(), ::isspace), alias.end());
                alias.erase(std::remove_if(alias.begin(), alias.end(), ::ispunct), alias.end());
                item_alias_map[alias] = id;
            }
        }
    }
}

void game::load_character_aliases(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open character_aliases.txt\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id, aliases;
        if (std::getline(iss, id, '|') && std::getline(iss, aliases)) {
            std::istringstream alias_stream(aliases);
            std::string alias;
            while (std::getline(alias_stream, alias, ',')) {
                std::transform(alias.begin(), alias.end(), alias.begin(), ::tolower);
                alias.erase(std::remove_if(alias.begin(), alias.end(), ::isspace), alias.end());
                alias.erase(std::remove_if(alias.begin(), alias.end(), ::ispunct), alias.end());
                character_alias_map[alias] = id;
            }
        }
    }
}

std::string game::resolve_item_id(const std::string& user_input) {
    std::string key = user_input;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    key.erase(std::remove_if(key.begin(), key.end(), ::ispunct), key.end());

    if (item_alias_map.count(key)) {
        return item_alias_map[key];
    }
    return "";
}

std::string game::resolve_character_id(const std::string& user_input) {
    std::string key = user_input;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    key.erase(std::remove_if(key.begin(), key.end(), ::ispunct), key.end());

    if (character_alias_map.count(key)) {
        return character_alias_map[key];
    }
    return "";
}

void game::load_required_treasures(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open required_treasures.txt\n";
        is_running = false;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (!line.empty()) {
            required_treasures.push_back(line);
        }
    }
}

void game::show_status() {

    int hp = player_data->get_health();
    int max_hp = 100;
    int hunger = player_data->get_hunger();
    int max_hunger = 100;
    std::string size = player_data->get_size();

    // Build ASCII bars
    int hp_blocks = hp * 20 / max_hp;
    int hunger_blocks = hunger * 20 / max_hunger;

    std::cout << "                                                                      HEALTH: [";
    for (int i = 0; i < 20; ++i) std::cout << (i < hp_blocks ? "\u2588" : " ");
    std::cout << "] " << hp << "/" << max_hp << "\n";

    std::cout << "                                                                      HUNGER: [";
    for (int i = 0; i < 20; ++i) std::cout << (i < hunger_blocks ? "\u2588" : " ");
    std::cout << "] " << hunger << "/" << max_hunger << "\n";

    std::cout << "                                                                      SIZE: " << size << "\n\n";
}