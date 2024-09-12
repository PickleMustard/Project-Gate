#include "finite_state_machine_base.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

bool FiniteStateMachineBase::Update(Node *goap_agent) {
	bool has_actions_remaining = false;
	if (stack_location > -1) {
		UtilityFunctions::print("Whats on the stack: ", state_stack[stack_location]);
		has_actions_remaining = (state_stack[stack_location]).call(this);
	}
	return has_actions_remaining;
}

void FiniteStateMachineBase::PushState(Callable state) {
	UtilityFunctions::print("Pushing onto State stack: ", stack_location);
	state_stack.push_back(state);
	stack_location++;
}

void FiniteStateMachineBase::PopState() {
	UtilityFunctions::print("Popping off of State stack: ", stack_location);
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
	godot::ClassDB::bind_method(godot::D_METHOD("PopState"), &FiniteStateMachineBase::PopState);
	godot::ClassDB::bind_method(godot::D_METHOD("PushState", "state"), &FiniteStateMachineBase::PushState);
}
