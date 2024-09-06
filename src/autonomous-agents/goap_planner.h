#ifndef GOAP_PLANNER_H
#define GOAP_PLANNER_H
#include "autonomous-agents/goap_action.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/hash_set.hpp"

#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class GoapPlanner : public Resource {
	GDCLASS(GoapPlanner, Resource)
public:
	struct Node {
		Node *parent;
		float running_cost;
		HashMap<String, Variant> state;
		Ref<GoapAction> action;

		Node(Node *n_parent, float n_running_cost, HashMap<String, Variant> n_state, Ref<GoapAction> n_action) {
			parent = n_parent;
			running_cost = n_running_cost;
			state = n_state;
			action = n_action;
		}
	};
	GoapPlanner();
	~GoapPlanner();

	Vector<Ref<GoapAction>> Plan(GodotObject *agent, HashSet<Ref<GoapAction>> available_actions, HashMap<String, Variant> world_state, HashMap<String, Variant> goal);

protected:
	static void _bind_methods();

private:
	bool BuildGraph(Node *parent, Vector<Node *> &leaves, HashSet<Ref<GoapAction>> usuable_actions, HashMap<String, Variant> goal);
	HashSet<Ref<GoapAction>> ActionSubset(HashSet<Ref<GoapAction>> actions, Ref<GoapAction> to_remove);
	bool InState(HashMap<String, Variant> test, HashMap<String, Variant> state);
	HashMap<String, Variant> PopulateState(HashMap<String, Variant> current_state, HashMap<String, Variant> state_change);
};
} //namespace godot

#endif
