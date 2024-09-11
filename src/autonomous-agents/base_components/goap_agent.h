#ifndef GOAP_AGENT_H
#define GOAP_AGENT_H

#include "autonomous-agents/base_components/finite_state_machine_base.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "autonomous-agents/base_components/goap_planner.h"
#include "autonomous-agents/base_components/igoap.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "level-generation/tilegrid.h"
namespace godot {
class GoapAgent : public Node {
	GDCLASS(GoapAgent, Node);

private:
	Ref<FiniteStateMachineBase> state_machine;
	Callable idle_state;
	Callable move_to_state;
	Callable perform_action_state;

	Vector<Ref<GoapAction>> current_actions;

	IGoap *data_provider;
  Ref<GoapPlanner> planner;

public:
  Dictionary m_available_actions;

  GoapAgent();
  GoapAgent(Dictionary available_actions);
  ~GoapAgent();
  void AddAction(Ref<GoapAction> action);
  Ref<GoapAction> GetAction(String type);
  void RunAI();
  void RemoveAction(Ref<GoapAction> action);
  void Update();
  void _ready() override;
  void _process(double p_delta) override;

  TileGrid *GetTileGrid();
  void SetAvailableActions(Dictionary actions);
  Dictionary GetAvailableActions();
protected:
  static void _bind_methods();
private:
  bool HasActionPlan();
  void IdleState(FiniteStateMachineBase *);
  void MoveToState(FiniteStateMachineBase *, Node *goap_agent);
  void PerformActionState(FiniteStateMachineBase *, Node *goap_agent);
  void FindDataProvider();
  void LoadActions();


};
} //namespace godot

#endif
