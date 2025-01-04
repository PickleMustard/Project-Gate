#ifndef ATTACK_ENEMY_ACTION_H
#define ATTACK_ENEMY_ACTION_H

#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/dictionary.hpp"
namespace godot {
class AttackEnemyAction : public GoapAction {
	GDCLASS(AttackEnemyAction, GoapAction);

private:
	bool attacked_enemy = false;

public:
	AttackEnemyAction();
	~AttackEnemyAction();
	void Reset() override;
  bool GetInRange(Node *goap_agent, Dictionary world_data) override;
	bool IsDone(Node *goap_agent) override;
  bool InProgress(Node *goap_agent) override;
	bool RequiresInRange() override;
	bool CheckProceduralPrecondition(Node *goap_agent, Dictionary) override;
	bool Perform(Node *goap_agent) override;
  bool HasActionPointsRemaining(int remaining);
  bool UsedAllActionPoints(int remaining);
  bool HasMovementRemaining(int remaining);

protected:
	static void _bind_methods();

private:
  Callable actions_remaining;
  Callable used_all_actions;
  Callable movement_remaining;
};
} //namespace godot

#endif
