#include "finite_state_machine_base.h"
#include "godot_cpp/classes/wrapped.hpp"

void godot::FiniteStateMachineBase::Update(godot::GodotObject *gd) {
	if (stack_location > -1) {
		(state_stack[stack_location]).call(this, gd);
	}
}

void godot::FiniteStateMachineBase::PushState(Callable state) {
	state_stack.push_back(state);
	stack_location++;
}

void godot::FiniteStateMachineBase::PopState() {
	if (stack_location > -1) {
		state_stack.remove_at(stack_location);
		stack_location--;
	}
}

godot::FiniteStateMachineBase::FiniteStateMachineBase() {
}

godot::FiniteStateMachineBase::~FiniteStateMachineBase() {

}

void godot::FiniteStateMachineBase::_bind_methods() {

}
