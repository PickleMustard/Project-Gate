#ifndef ATTEMPT_TO_DISCOVER_ENEMY_ACTION_H
#define ATTEMPT_TO_DISCOVER_ENEMY_ACTION_H

#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/callable.hpp"
namespace godot {
class AttemptToDiscoverEnemyAction : public GoapAction {
GDCLASS(AttemptToDiscoverEnemyAction, GoapAction);
public:
  AttemptToDiscoverEnemyAction();
  ~AttemptToDiscoverEnemyAction();
	void Reset() override;
	bool IsDone(Node *goap_agent) override;
  bool InProgress(Node *goap_agent) override;
	bool RequiresInRange() override;
	bool CheckProceduralPrecondition(Node *goap_agent, Dictionary) override;
	bool Perform(Node *goap_agent) override;
  bool HasMovementRangeRemaining(int distance);

  protected:
  static void _bind_methods();

private:
  bool seen_enemy = false;
  Callable movement_remaining;
};
}


#endif
