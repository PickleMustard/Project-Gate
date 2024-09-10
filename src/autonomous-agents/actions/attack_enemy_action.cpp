#include "attack_enemy_action.h"
#include "godot_cpp/classes/wrapped.hpp"

using namespace godot;

AttackEnemyAction::AttackEnemyAction() {
}

AttackEnemyAction::~AttackEnemyAction() {
}

void AttackEnemyAction::_bind_methods() {
}

void AttackEnemyAction::Reset() {
  attacked_enemy = false;
}

bool AttackEnemyAction::IsDone() {
  return attacked_enemy;
}
bool AttackEnemyAction::RequiresInRange() {
  return true;
}

bool AttackEnemyAction::CheckProceduralPrecondition(GodotObject *gd) {

}
