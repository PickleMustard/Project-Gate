#include "goap_planner.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/hash_set.hpp"
#include "godot_cpp/templates/pair.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
GoapPlanner::GoapPlanner() {
}

GoapPlanner::~GoapPlanner() {
}

void GoapPlanner::_bind_methods() {
}

Vector<Ref<GoapAction>> GoapPlanner::Plan(godot::Node *goap_agent, Dictionary available_actions, Dictionary world_state, Dictionary goal) {
	HashSet<Ref<GoapAction>> usable_actions{};
	Array keys = available_actions.keys();
	UtilityFunctions::print("available_actions size: ", keys.size());
	for (int i = 0; i < keys.size(); i++) {
		available_actions[keys[i]].call("DoReset");
		if (available_actions[keys[i]].call("CheckProceduralPrecondition", goap_agent, world_state)) {
			UtilityFunctions::print("Can use ", ((Ref<GoapAction>)available_actions[keys[i]])->GetActionName());
			usable_actions.insert(available_actions[keys[i]]);
		}
	}

	Vector<PlanningNode *> leaves{};
	PlanningNode *start = memnew(PlanningNode(nullptr, 0.0f, world_state, nullptr));
	bool success = BuildGraph(start, leaves, usable_actions, goal);

	if (!success) {
    UtilityFunctions::print("Could not find plan");
		return Vector<Ref<GoapAction>>{};
	}

	PlanningNode *cheapest = nullptr;
	for (PlanningNode *leaf : leaves) {
		if (cheapest == nullptr) {
			cheapest = leaf;
		} else {
			if (leaf->running_cost < cheapest->running_cost) {
				cheapest = leaf;
			}
		}
	}

	Vector<Ref<GoapAction>> result{};
	PlanningNode *n = cheapest;
	while (n != nullptr) {
		if (n->action != nullptr) {
			result.push_back(n->action);
		}
		n = n->parent;
	}
	result.reverse();
	return result;
}

bool GoapPlanner::BuildGraph(PlanningNode *parent, Vector<PlanningNode *> &leaves, HashSet<Ref<GoapAction>> &usable_actions, Dictionary goal) {
	bool found_viable_goal = false;
	for (Ref<GoapAction> action : usable_actions) {
		UtilityFunctions::print("checking usuable action: ", action->GetActionName(), "| Preconditions: ", action->GetPreconditions(), "| state: ", parent->state);
		if (InState(action->GetPreconditions(), parent->state)) {
			Dictionary current_state = PopulateState(parent->state, action->GetEffects());
			UtilityFunctions::print("current state");
			PlanningNode *node = memnew(PlanningNode(parent, parent->running_cost + action->m_cost, current_state, action));
			UtilityFunctions::print("checking usuable action results: ", action->GetActionName(), "| Agent Goals: ", goal, "| state: ", current_state);
			if (InState(current_state, goal)) {
				leaves.push_back(node);
				found_viable_goal = true;
        UtilityFunctions::print("Found Viable goal");
			} else {
        UtilityFunctions::print("Could not find goal, going deeper");
				HashSet<Ref<GoapAction>> subset = ActionSubset(usable_actions, action);
				bool found = BuildGraph(node, leaves, subset, goal);
				if (found) {
					found_viable_goal = true;
				}
			}
		}
	}
	return found_viable_goal;
}

HashSet<Ref<GoapAction>> GoapPlanner::ActionSubset(HashSet<Ref<GoapAction>> actions, Ref<GoapAction> to_remove) {
	HashSet<Ref<GoapAction>> subset{};
	for (Ref<GoapAction> action : actions) {
		if (action != to_remove) {
			subset.insert(action);
		}
	}
	return subset;
}

bool GoapPlanner::InState(Dictionary stateToTest, Dictionary currentState) {
	Array testKeys = stateToTest.keys();
	for (int i = 0; i < testKeys.size(); i++) {
		UtilityFunctions::print("Testing key: ", testKeys[i], "| ", currentState.has(testKeys[i]), " | ", currentState[testKeys[i]] == stateToTest[testKeys[i]]);
		UtilityFunctions::print(currentState[testKeys[i]], " | ", stateToTest[testKeys[i]]);
		UtilityFunctions::print("Type: ", stateToTest[testKeys[i]].get_type());
		if (currentState.has(testKeys[i])) {
			if (stateToTest[testKeys[i]].get_type() == 25 && (Callable(stateToTest[testKeys[i]])).call(currentState[testKeys[i]])) {
        UtilityFunctions::print(Callable(stateToTest[testKeys[i]]).call(currentState[testKeys[i]]));
        UtilityFunctions::print("Callable Precondition");
        continue;
			} else if (currentState[testKeys[i]] == stateToTest[testKeys[i]]) {
				UtilityFunctions::print("Boolean Precondition");
				continue;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	return true;
}

Dictionary GoapPlanner::PopulateState(Dictionary current_state, Dictionary state_change) {
	Array keys = state_change.keys();
	for (int i = 0; i < keys.size(); i++) {
		current_state[keys[i]] = state_change[keys[i]];
	}

	return state_change;
}
