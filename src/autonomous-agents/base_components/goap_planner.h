#ifndef GOAP_PLANNER_H
#define GOAP_PLANNER_H
#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/templates/hash_set.hpp"

#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class GoapPlanner : public Resource {
	GDCLASS(GoapPlanner, Resource)
public:
	struct PlanningNode {
		PlanningNode *parent;
		float running_cost;
		Dictionary state;
		Ref<GoapAction> action;

		PlanningNode(PlanningNode *n_parent, float n_running_cost, Dictionary n_state, Ref<GoapAction> n_action) {
			parent = n_parent;
			running_cost = n_running_cost;
			state = n_state;
			action = n_action;
		}
	};
	GoapPlanner();
	~GoapPlanner();

	Vector<Ref<GoapAction>> Plan(godot::Node *goap_agent, Dictionary available_actions, Dictionary world_state, Dictionary goal);

protected:
	static void _bind_methods();

private:
	bool BuildGraph(PlanningNode *parent, Vector<PlanningNode *> &leaves, HashSet<Ref<GoapAction>> &usuable_actions, Dictionary goal);
	HashSet<Ref<GoapAction>> ActionSubset(HashSet<Ref<GoapAction>> actions, Ref<GoapAction> to_remove);
	bool InState(Dictionary test, Dictionary state);
	Dictionary PopulateState(Dictionary current_state, Dictionary state_change);
};
} //namespace godot

#endif
