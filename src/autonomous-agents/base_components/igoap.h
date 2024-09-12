#ifndef IGOAP_H
#define IGOAP_H

#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
namespace godot {
class IGoap : public Node {
	GDCLASS(IGoap, Node)
public:
	IGoap();
	~IGoap();
	virtual Dictionary GetWorldState();
	virtual Dictionary CreateGoalState();
	virtual void PlanFailed(Dictionary failed_goal);
	virtual void PlanFound(Dictionary goal, Vector<Ref<GoapAction>> actions);
	virtual void ActionsFinished();
	virtual void PlanAborted(Ref<GoapAction> aborter);
	virtual bool MoveAgent(Ref<GoapAction> next_action);

protected:
	static void _bind_methods();
};
} //namespace godot

#endif
