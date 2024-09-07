#ifndef IGOAP_H
#define IGOAP_H

#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class IGoap : public Node {
	GDCLASS(IGoap, Node)
public:
	IGoap();
	~IGoap();
	virtual HashMap<String, Variant> GetWorldState();
	virtual HashMap<String, Variant> CreateGoalState();
	virtual void PlanFailed(HashMap<String, Variant> failed_goal);
	virtual void PlanFound(HashMap<String, Variant> goal, Vector<Ref<GoapAction>> actions);
	virtual void ActionsFinished();
	virtual void PlanAborted(Ref<GoapAction> aborter);
	virtual bool MoveAgent(Ref<GoapAction> next_action);

protected:
	static void _bind_methods();
};
} //namespace godot

#endif
