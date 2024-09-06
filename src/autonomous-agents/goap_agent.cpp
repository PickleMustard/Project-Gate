#include "goap_agent.h"
#include "autonomous-agents/finite_state_machine_base.h"
#include "autonomous-agents/goap_action.h"
#include "autonomous-agents/goap_planner.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/hash_set.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

godot::GoapAgent::GoapAgent() {
}

godot::GoapAgent::~GoapAgent() {
}

void godot::GoapAgent::Update() {
	state_machine->Update(this);
}
void godot::GoapAgent::_ready() {
	UtilityFunctions::print("Creating Agent");
	state_machine = Ref<FiniteStateMachineBase>(memnew(FiniteStateMachineBase()));
	m_available_actions = HashSet<Ref<GoapAction>>();
	current_actions = Vector<Ref<GoapAction>>{};
	planner = Ref<GoapPlanner>(memnew(GoapPlanner()));
	UtilityFunctions::print("Getting Data Provider");
	FindDataProvider();
	UtilityFunctions::print("Setting Callables");
	idle_state = Callable(this, "IdleState");
	move_to_state = Callable(this, "MoveToState");
	perform_action_state = Callable(this, "PerformActionState");
	UtilityFunctions::print("Start the state machine");
	state_machine->PushState(idle_state);
	UtilityFunctions::print("Agent Created");
}

void godot::GoapAgent::_process(double p_delta) {
	/*UtilityFunctions::print("Updating State machine");
	  state_machine->Update(this);
	UtilityFunctions::print("State machine updated");*/
}

void godot::GoapAgent::AddAction(Ref<GoapAction> action) {
	m_available_actions.insert(action);
}

godot::Ref<godot::GoapAction> godot::GoapAgent::GetAction(godot::String type) {
	for (Ref<GoapAction> act : m_available_actions) {
		if (act->get_class() == type) {
			return act;
		}
	}
	return nullptr;
}

void godot::GoapAgent::RemoveAction(Ref<GoapAction> action) {
	m_available_actions.erase(action);
}

bool godot::GoapAgent::HasActionPlan() {
	return current_actions.size() > 0;
}

/* Example I'm using creates a delegate in the FiniteStateMachineBase class and creates the definition in this GoapAgent class
 * Under a function titled createIdleState.
 *
 */
void godot::GoapAgent::IdleState(godot::FiniteStateMachineBase *fsm, GodotObject *gd) {
	HashMap<String, Variant> world_state = data_provider->GetWorldState();
	HashMap<String, Variant> goal = data_provider->CreateGoalState();

	//Plan
	Vector<Ref<GoapAction>> plan = planner->Plan(gd, m_available_actions, world_state, goal);
	if (plan.size() > 0) {
		current_actions = plan;
		data_provider->PlanFound(goal, plan);

		fsm->PopState();
		fsm->PushState(perform_action_state);
	} else {
		data_provider->PlanFailed(goal);
		fsm->PopState();
		fsm->PushState(idle_state);
	}
}

void godot::GoapAgent::MoveToState(godot::FiniteStateMachineBase *fsm, GodotObject *gd) {
	Ref<GoapAction> action = current_actions[0];
	if (action->RequiresInRange() && action->target == nullptr) {
		fsm->PopState();
		fsm->PopState();
		fsm->PushState(idle_state);
		return;
	}

	if (data_provider->MoveAgent(action)) {
		fsm->PopState();
	}
}

void godot::GoapAgent::PerformActionState(godot::FiniteStateMachineBase *fsm, GodotObject *gd) {
	if (!HasActionPlan()) {
		fsm->PopState();
		fsm->PushState(idle_state);
		data_provider->ActionsFinished();
		return;
	}

	Ref<GoapAction> action = current_actions[0];
	if (action->IsDone()) {
		current_actions.remove_at(0);
	}

	if (HasActionPlan()) {
		action = current_actions[0];
		bool in_range = action->RequiresInRange() ? action->GetInRange() : true;

		if (in_range) {
			bool success = action->Perform(gd);

			if (!success) {
				fsm->PopState();
				fsm->PushState(idle_state);
				data_provider->PlanAborted(action);
			}
		} else {
			fsm->PushState(move_to_state);
		}
	} else {
		fsm->PopState();
		fsm->PushState(idle_state);
		data_provider->ActionsFinished();
	}
}

void godot::GoapAgent::FindDataProvider() {
	Node *parent = get_parent();
	Node *potential_provider = parent->find_child("data_provider");
	/*if(potential_provider->get_class() == "IGoap") {
	  data_provider = cast_to<IGoap>(potential_provider);
	}*/
}

void godot::GoapAgent::SetAvailableActions(Dictionary actions) {
	m_available_actions.clear();
	Array action_keys = actions.keys();
	for (int i = 0; i < action_keys.size(); i++) {
		UtilityFunctions::print("Value Type: ", actions[action_keys[i]].get_type(), "| ", actions[action_keys[i]].get_type_name(actions[action_keys[i]].get_type()));
		if (actions[action_keys[i]].get_type() == 24) {
			if (actions[action_keys[i]].can_convert_strict(actions[action_keys[i]].get_type(), Variant::OBJECT)) {
				m_available_actions.insert(Object::cast_to<GoapAction>(actions[action_keys[i]]));
			}
			//m_available_actions
		}
	}
}

godot::Dictionary godot::GoapAgent::GetAvailableActions() {
	Dictionary actions;
	for (Ref<GoapAction> action : m_available_actions) {
		actions[action->get_name()] = action;
	}

	return actions;
}

void godot::GoapAgent::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("AddAvailableAction", "actions"), &GoapAgent::SetAvailableActions);
	godot::ClassDB::bind_method(godot::D_METHOD("GetAvailableActions"), &GoapAgent::GetAvailableActions);
	ADD_GROUP("Available Actions", "m_available_");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "m_available_actions"), "AddAvailableAction", "GetAvailableActions");
}
