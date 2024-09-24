#include "goap_agent.h"
#include "autonomous-agents/base_components/finite_state_machine_base.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "autonomous-agents/base_components/goap_planner.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level-generation/tilegrid.h"

godot::GoapAgent::GoapAgent() {
}
godot::GoapAgent::GoapAgent(Dictionary available_actions) {
	m_available_actions = available_actions;
}

godot::GoapAgent::~GoapAgent() {
}

void godot::GoapAgent::Update() {
	should_continue = state_machine->Update(this);
}
void godot::GoapAgent::_ready() {
	UtilityFunctions::print("Creating Agent");
	state_machine = Ref<FiniteStateMachineBase>(memnew(FiniteStateMachineBase()));
	//m_available_actions = Dictionary();
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
	/*if (should_continue) {
		should_continue = state_machine->Update(this);
    if(!should_continue) {
      EndTurn();
    }
	}
	/*UtilityFunctions::print("Updating State machine");
	  state_machine->Update(this);
	UtilityFunctions::print("State machine updated");*/
}

void godot::GoapAgent::_physics_process(double p_delta) {
	if (should_continue) {
		should_continue = state_machine->Update(this);
    if(!should_continue) {
      EndTurn();
    }
	}
	/*UtilityFunctions::print("Updating State machine");
	  state_machine->Update(this);
	UtilityFunctions::print("State machine updated");*/
}


void godot::GoapAgent::RunAI() {
	//Create a plan, then run through the plan while action / movement points remain
	UtilityFunctions::print("Running AI");
	Node *enemy = get_parent();
	UtilityFunctions::print("Got Parent");
	Update();
	UtilityFunctions::print("Updated");
}

void godot::GoapAgent::EndTurn() {
	SceneTree *tree = get_tree();
	TypedArray<Node> turn_controllers = tree->get_nodes_in_group("TurnController");
	Node *turn_controller = cast_to<Node>(turn_controllers[0]);
  turn_controller->call_deferred("EndTurn");
}

void godot::GoapAgent::AddAction(Ref<GoapAction> action) {
	m_available_actions[action->GetActionName()] = action;
}

godot::Ref<godot::GoapAction> godot::GoapAgent::GetAction(godot::String type) {
	if (m_available_actions.has(type)) {
		return m_available_actions[type];
	}
	return nullptr;
}

godot::TileGrid *godot::GoapAgent::GetTileGrid() {
	SceneTree *tree = get_tree();
	TypedArray<Node> tilegrid = tree->get_nodes_in_group("Tilegrid");
	TileGrid *tilegrid_obj = cast_to<TileGrid>(tilegrid[0]);
	return tilegrid_obj;
}

godot::Node3D *godot::GoapAgent::GetUnitController() {
	SceneTree *tree = get_tree();
	TypedArray<Node> unit_control_group = tree->get_nodes_in_group("UnitControl");
	Node3D *unit_controller = cast_to<Node3D>(unit_control_group[0]);
	return unit_controller;
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
bool godot::GoapAgent::IdleState(godot::FiniteStateMachineBase *fsm) {
	UtilityFunctions::print("Idling: ", data_provider);
	Dictionary world_state = data_provider->GetWorldState();
	Dictionary goal = data_provider->CreateGoalState();
	//Plan
	UtilityFunctions::print("Planning");
	Vector<Ref<GoapAction>> plan = planner->Plan(this, m_available_actions, world_state, goal);
	if (plan.size() > 0) {
		UtilityFunctions::print("Plan found");
		current_actions = Vector<Ref<GoapAction>>(plan);
		data_provider->PlanFound(goal, plan);

		fsm->call_deferred("PopState");
		fsm->call_deferred("PushState", perform_action_state);
		return true;
	} else {
		UtilityFunctions::print("Plan not found");
		data_provider->PlanFailed(goal);
		return false;
		//fsm->call_deferred("PopState");
		//fsm->call_deferred("PushState", idle_state);
	}
	UtilityFunctions::print("Ending Idle State");
}

bool godot::GoapAgent::MoveToState(godot::FiniteStateMachineBase *fsm) {
	Ref<GoapAction> action = current_actions[0];
	if (action->RequiresInRange() && action->target == nullptr) {
		fsm->PopState();
		fsm->PopState();
		fsm->PushState(idle_state);
		return true;
	}

	if (data_provider->MoveAgent(action)) {
		fsm->PopState();
	}
	return true;
}

bool godot::GoapAgent::PerformActionState(godot::FiniteStateMachineBase *fsm) {
	UtilityFunctions::print("Performing an action | ", HasActionPlan());
	if (!HasActionPlan()) {
		fsm->PopState();
		fsm->PushState(idle_state);
		data_provider->ActionsFinished();
		return true;
	}

	Ref<GoapAction> action = current_actions[0];
	UtilityFunctions::print("Is Done? ", action->IsDone(this));
	UtilityFunctions::print("name: ", action->GetActionName());
  if(action->InProgress(this)) {
    return true;
  }
  if (action->IsDone(this)) {
    current_actions.remove_at(0);
  }

	if (HasActionPlan()) {
		UtilityFunctions::print("2");
		action = current_actions[0];
		UtilityFunctions::print("Requires in Range? ", action->RequiresInRange());
		bool in_range = action->RequiresInRange() ? action->GetInRange() : true;

		if (in_range) {
			bool success = action->Perform(this);

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
	return true;
}

void godot::GoapAgent::FindDataProvider() {
	Node *parent = get_parent();
	UtilityFunctions::print("Parent: ", parent);
	Node *potential_provider = parent->find_child("data_provider");
	UtilityFunctions::print("Data provider: ", potential_provider);
	data_provider = cast_to<IGoap>(potential_provider);
}

void godot::GoapAgent::SetAvailableActions(Dictionary actions) {
	//m_available_actions.clear();
	Array action_keys = actions.keys();
	for (int i = 0; i < action_keys.size(); i++) {
		UtilityFunctions::print("Value Type: ", actions[action_keys[i]].get_type(), "| ", actions[action_keys[i]].get_type_name(actions[action_keys[i]].get_type()));
		if (actions[action_keys[i]].get_type() == 24) {
			if (actions[action_keys[i]].can_convert_strict(actions[action_keys[i]].get_type(), Variant::OBJECT)) {
				m_available_actions[action_keys[i]] = (Object::cast_to<GoapAction>(actions[action_keys[i]]));
			}
			//m_available_actions
		}
	}
}

godot::Dictionary godot::GoapAgent::GetAvailableActions() {
	return m_available_actions;
}

void godot::GoapAgent::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("AddAvailableAction", "actions"), &GoapAgent::SetAvailableActions);
	godot::ClassDB::bind_method(godot::D_METHOD("GetAvailableActions"), &GoapAgent::GetAvailableActions);
	godot::ClassDB::bind_method(godot::D_METHOD("GetTileGrid"), &GoapAgent::GetTileGrid);
	godot::ClassDB::bind_method(godot::D_METHOD("GetUnitController"), &GoapAgent::GetUnitController);
	godot::ClassDB::bind_method(godot::D_METHOD("RunAI"), &GoapAgent::RunAI);
	godot::ClassDB::bind_method(godot::D_METHOD("IdleState", "FSM"), &GoapAgent::IdleState);
	godot::ClassDB::bind_method(godot::D_METHOD("MoveToState", "FSM"), &GoapAgent::MoveToState);
	godot::ClassDB::bind_method(godot::D_METHOD("PerformActionState", "FSM"), &GoapAgent::PerformActionState);
	ADD_GROUP("Available Actions", "m_available_");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "m_available_actions"), "AddAvailableAction", "GetAvailableActions");
}
