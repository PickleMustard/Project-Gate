#include "finite_state_machine_base.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

void FiniteStateMachineBase::Update(GodotObject *gd) {
  UtilityFunctions::print("Stack Location: ", stack_location);
	if (stack_location > -1) {
		(state_stack[stack_location]).call(this, gd);
	}
}

void FiniteStateMachineBase::PushState(Callable state) {
	state_stack.push_back(state);
	stack_location++;
}

void FiniteStateMachineBase::PopState() {
	if (stack_location > -1) {
		state_stack.remove_at(stack_location);
		stack_location--;
	}
}

FiniteStateMachineBase::FiniteStateMachineBase() {
}

FiniteStateMachineBase::~FiniteStateMachineBase() {

}

void FiniteStateMachineBase::_bind_methods() {

}
