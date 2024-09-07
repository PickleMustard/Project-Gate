#include "goap_planner.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/hash_set.hpp"
#include "godot_cpp/templates/pair.hpp"
#include "godot_cpp/templates/vector.hpp"

using namespace godot;
GoapPlanner::GoapPlanner() {
}

GoapPlanner::~GoapPlanner() {
}

void GoapPlanner::_bind_methods() {
}

Vector<Ref<GoapAction>> GoapPlanner::Plan(GodotObject *gd, HashSet<Ref<GoapAction>> available_actions, HashMap<String, Variant> world_state, HashMap<String, Variant> goal) {
	HashSet<Ref<GoapAction>> usable_actions{};
	for (Ref<GoapAction> action : available_actions) {
		action->DoReset();
		if (action->CheckProceduralPrecondition(gd)) {
			usable_actions.insert(action);
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

  Vector<Ref<GoapAction>> result {};
  Node *n = cheapest;
  while(n != nullptr) {
    if(n->action != nullptr) {
      result.push_back(n->action);
    }
    n = n->parent;
  }
  result.reverse();
  return result;
}

bool GoapPlanner::BuildGraph(Node *parent, Vector<Node *> &leaves, HashSet<Ref<GoapAction>> usable_actions, HashMap<String, Variant> goal) {
  bool found_viable_goal = false;
  for(Ref<GoapAction> action : usable_actions) {
    if(InState(action->GetPreconditions(), parent->state)) {
      HashMap<String, Variant> current_state = PopulateState(parent->state, action->GetEffects());
      Node *node = memnew(Node(parent, parent->running_cost + action->cost, current_state, action));
      if(InState(goal, current_state)) {
        leaves.push_back(node);
        found_viable_goal = true;
      } else {
        HashSet<Ref<GoapAction>> subset = ActionSubset(usable_actions, action);
        bool found = BuildGraph(node, leaves, subset, goal);
        if(found) {
          found_viable_goal = true;
        }
      }
    }
  }
  return found_viable_goal;
}

HashSet<Ref<GoapAction>> GoapPlanner::ActionSubset(HashSet<Ref<GoapAction>> actions, Ref<GoapAction> to_remove) {
  HashSet<Ref<GoapAction>> subset {};
  for(Ref<GoapAction> action : actions) {
    if(action != to_remove) {
      subset.insert(action);
    }
  }
  return subset;
}

bool GoapPlanner::InState(HashMap<String, Variant> test, HashMap<String, Variant> state) {
  for(KeyValue<String, Variant> match : test) {
    if(state.has(match.key) && state.get(match.key) == match.value) {
      continue;
    } else {
      return false;
    }
  }
  return true;
}

HashMap<String, Variant> GoapPlanner::PopulateState(HashMap<String, Variant> current_state, HashMap<String, Variant> state_change) {
  for(KeyValue<String, Variant> change : state_change) {
    if(current_state.has(change.key) && current_state.get(change.key) == change.value) {
      state_change[change.key] = change.value;
    } else {
      state_change.insert(change.key, change.value);
    }
  }

  return state_change;
}


