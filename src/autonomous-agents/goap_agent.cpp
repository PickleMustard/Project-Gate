#include "goap_agent.h"
#include "autonomous-agents/finite_state_machine_base.h"
#include "autonomous-agents/goap_action.h"
#include "autonomous-agents/goap_planner.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/hash_set.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

godot::GoapAgent::GoapAgent() {
}

godot::GoapAgent::~GoapAgent() {
}

void godot::GoapAgent::_ready() {
	state_machine = Ref<FiniteStateMachineBase>(memnew(FiniteStateMachineBase()));
	available_actions = HashSet<Ref<GoapAction>>();
	current_actions = Vector<Ref<GoapAction>>{};
	planner = Ref<GoapPlanner>(memnew(GoapPlanner()));
	FindDataProvider();
	idle_state = Callable(this, "IdleState");
	move_to_state = Callable(this, "MoveToState");
	perform_action_state = Callable(this, "PerformActionState");
	state_machine->PushState(idle_state);
	LoadActions();
}

void godot::GoapAgent::_process(double p_delta) {
	state_machine->Update(this);
}

void godot::GoapAgent::AddAction(Ref<GoapAction> action) {
	available_actions.insert(action);
}

godot::Ref<godot::GoapAction> godot::GoapAgent::GetAction(godot::String type) {
	for (Ref<GoapAction> act : available_actions) {
		if (act->get_class() == type) {
			return act;
		}
	}
	return nullptr;
}

void godot::GoapAgent::RemoveAction(Ref<GoapAction> action) {
	available_actions.erase(action);
}

bool godot::GoapAgent::HasActionPlan() {
	return current_actions.size() > 0;
}

/* Example I'm using creates a delegate in the FiniteStateMachineBase class and creates the definition in this GoapAgent class
 * Under a function titled createIdleState.
 *
 */
void godot::GoapAgent::IdleState(godot::FiniteStateMachineBase *fsm, GodotObject *gd) {
	HashMap<String, Variant> world_state = data_provider.GetWorldState();
	HashMap<String, Variant> goal = data_provider.CreateGoalState();

	//Plan
	Vector<Ref<GoapAction>> plan = planner.plan(gd, available_actions, world_state, goal);
	if (plan.size() > 0) {
		current_actions = plan;
		data_provider.PlanFound(goal, plan);

		fsm->PopState();
		fsm->PushState(perform_action_state);
	} else {
		data_provider.PlanFailed(goal);
		fsm->PopState();
		fsm->PushState(idle_state);
	}
}
