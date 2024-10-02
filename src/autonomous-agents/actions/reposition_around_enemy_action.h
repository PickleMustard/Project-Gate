#ifndef REPOSITION_AROUND_ENEMY_ACTION_H
#define REPOSITION_AROUND_ENEMY_ACTION_H

#include "autonomous-agents/base_components/goap_action.h"
namespace godot {
class RepositionAroundEnemyAction : public GoapAction {
	GDCLASS(RepositionAroundEnemyAction, GoapAction);

private:
	bool attacked_enemy = false;

public:
	RepositionAroundEnemyAction();
	~RepositionAroundEnemyAction();
	void Reset() override;
	bool IsDone(Node *goap_agent) override;
  bool InProgress(Node *goap_agent) override;
	bool RequiresInRange() override;
	bool CheckProceduralPrecondition(Node *goap_agent, Dictionary) override;
	bool Perform(Node *goap_agent) override;

protected:
	static void _bind_methods();
};
} //namespace godot

#endif
