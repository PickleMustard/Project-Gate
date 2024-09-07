#ifndef BASE_ENEMY_H
#define BASE_ENEMY_H

#include "autonomous-agents/base_components/goap_action.h"
#include "autonomous-agents/base_components/igoap.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
namespace godot {
class BaseEnemy : public IGoap {
  GDCLASS(BaseEnemy, IGoap);
public:
  BaseEnemy();
  ~BaseEnemy();
  HashMap<String, Variant> GetWorldState() override;
  virtual HashMap<String, Variant> CreateGoalState() override;
  void PlanFailed(HashMap<String, Variant> failed_goal) override;
  void PlanFound(HashMap<String, Variant> goal, Vector<Ref<GoapAction>> actions) override;
  void ActionsFinished() override;
  void PlanAborted(Ref<GoapAction> aborter) override;
  bool MoveAgent(Ref<GoapAction> next_action) override;

protected:
  static void _bind_methods();
};

} //namespace godot

#endif
