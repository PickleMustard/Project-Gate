#ifndef GOAP_AGENT_H
#define GOAP_AGENT_H

#include "autonomous-agents/finite_state_machine_base.h"
#include "autonomous-agents/goap_action.h"
#include "autonomous-agents/goap_planner.h"
#include "autonomous-agents/igoap.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_set.hpp"
namespace godot {
class GoapAgent : Node {
	GDCLASS(GoapAgent, Node);

private:
	Ref<FiniteStateMachineBase> state_machine;
	Callable idle_state;
	Callable move_to_state;
	Callable perform_action_state;

	HashSet<Ref<GoapAction>> available_actions;
	Vector<Ref<GoapAction>> current_actions;

	Ref<IGoap> data_provider;
  Ref<GoapPlanner> planner;

public:
  GoapAgent();
  ~GoapAgent();
  void AddAction(Ref<GoapAction> action);
  Ref<GoapAction> GetAction(String type);
  void RemoveAction(Ref<GoapAction> action);
  void _ready() override;
  void _process(double p_delta) override;
protected:
  static void _bind_methods();
private:
  bool HasActionPlan();
  void IdleState(FiniteStateMachineBase *, GodotObject *);
  void MoveToState(FiniteStateMachineBase *, GodotObject *);
  void PerformActionState(FiniteStateMachineBase *, GodotObject *);
  void FindDataProvider();
  void LoadActions();


};
} //namespace godot

#endif
