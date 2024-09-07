#include "base_enemy.h"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

BaseEnemy::BaseEnemy() {

}

BaseEnemy::~BaseEnemy() {

}

void BaseEnemy::_bind_methods() {

}

HashMap<String, Variant> BaseEnemy::GetWorldState() {
  HashMap<String, Variant> world_data {};


  return world_data;
}

