#ifndef ATTACK_ENEMY_ACTION_H
#define ATTACK_ENEMY_ACTION_H

#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/wrapped.hpp"
namespace godot {
class AttackEnemyAction : public GoapAction {
	GDCLASS(AttackEnemyAction, GoapAction);
private:
  bool attacked_enemy = false;

public:
	AttackEnemyAction();
	~AttackEnemyAction();
  void Reset() override;
  bool IsDone() override;
  bool RequiresInRange() override;
  bool CheckProceduralPrecondition(GodotObject *gd) override;
  bool Perform(GodotObject *gd) override;

protected:
	static void _bind_methods();
};
} //namespace godot

#endif
