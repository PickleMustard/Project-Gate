#ifndef BASE_ENEMY_H
#define BASE_ENEMY_H

#include "autonomous-agents/base_components/goap_action.h"
#include "autonomous-agents/base_components/igoap.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
namespace godot {
class BaseEnemy : public IGoap {
  GDCLASS(BaseEnemy, IGoap);

private:
  Array known_enemies;
  Dictionary past_enemies;

public:
  BaseEnemy();
  ~BaseEnemy();
  Dictionary GetWorldState() override;
  Dictionary CreateGoalState() override;
  void PlanFailed(Dictionary failed_goal) override;
  void PlanFound(Dictionary goal, Vector<Ref<GoapAction>> actions) override;
  void ActionsFinished() override;
  void PlanAborted(Ref<GoapAction> aborter) override;
  bool MoveAgent(Ref<GoapAction> next_action) override;

protected:
  static void _bind_methods();

private:
  void CheckForEnemies();
  void AreEnemiesWithinRange();
};

} //namespace godot

#endif
