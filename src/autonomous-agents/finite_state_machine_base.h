#ifndef FINITE_STATE_MACHINE_BASE_H
#define FINITE_STATE_MACHINE_BASE_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/vector.hpp"

namespace godot {

class FiniteStateMachineBase : public Resource {
  GDCLASS(FiniteStateMachineBase, Resource)
public:
	FiniteStateMachineBase();
	~FiniteStateMachineBase();
	//typedef void (*FSMState)(FiniteStateMachineBase *, GodotObject *);

	void Update(GodotObject *gd);
	void PushState(Callable state);
	void PopState();

protected:
	static void _bind_methods();

private:
  Callable test;
	Vector<Callable> state_stack {};
	int stack_location = -1;
};
} //namespace godot

#endif
