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

	Vector<Node *> leaves{};
	Node *start = memnew(Node(nullptr, 0.0f, world_state, nullptr));
	bool success = BuildGraph(start, leaves, usable_actions, goal);

	if (!success) {
		return Vector<Ref<GoapAction>>{};
	}

	Node *cheapest = nullptr;
	for (Node *leaf : leaves) {
		if (cheapest == nullptr) {
			cheapest = leaf;
		} else {
			if (leaf->running_cost < cheapest->running_cost) {
				cheapest = leaf;
			}
		}
	}

	Vector<Ref<GoapAction>> result{};
	Node *n = cheapest;
	while (n != nullptr) {
		if (n->action != nullptr) {
			result.push_back(n->action);
		}
		n = n->parent;
	}
	result.reverse();
	return result;
}

bool GoapPlanner::BuildGraph(Node *parent, Vector<Node *> &leaves, HashSet<Ref<GoapAction>> &usable_actions, Dictionary goal) {
	bool found_viable_goal = false;
	for (Ref<GoapAction> action : usable_actions) {
		UtilityFunctions::print("checking usuable action: ", action->GetActionName(), "| Preconditions: ", action->GetPreconditions(), "| state: ", parent->state);
		if (InState(action->GetPreconditions(), parent->state)) {
			Dictionary current_state = PopulateState(parent->state, action->GetEffects());
			UtilityFunctions::print("current state");
			Node *node = memnew(Node(parent, parent->running_cost + action->m_cost, current_state, action));
			UtilityFunctions::print("checking usuable action: ", action->GetActionName(), "| Preconditions: ", goal, "| state: ", current_state);
			if (InState(goal, current_state)) {
				leaves.push_back(node);
				found_viable_goal = true;
			} else {
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

bool GoapPlanner::InState(Dictionary test, Dictionary state) {
	Array keys = test.keys();
	for (int i = 0; i < keys.size(); i++) {
		UtilityFunctions::print("Testing key: ", keys[i], "| ", state.has(keys[i]), " | ", state[keys[i]] == test[keys[i]]);
		UtilityFunctions::print(state[keys[i]], " | ", test[keys[i]]);
		UtilityFunctions::print("Type: ", test[keys[i]].get_type());
		if (state.has(keys[i])) {
			if (test[keys[i]].get_type() == 25 && (Callable(test[keys[i]])).call(state[keys[i]])) {
        UtilityFunctions::print(Callable(test[keys[i]]).call(state[keys[i]]));
        UtilityFunctions::print("Callable Precondition");
        continue;
			} else if (state[keys[i]] == test[keys[i]]) {
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
