#include "igoap.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/templates/vector.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
IGoap::IGoap() {
}

IGoap::~IGoap() {
}

void IGoap::_bind_methods() {
}

Dictionary IGoap::GetWorldState() { UtilityFunctions::print("I shouldn't be here"); return Dictionary{}; }
Dictionary IGoap::CreateGoalState() { return Dictionary{}; }
void IGoap::PlanFailed(Dictionary failed_goal) {}
void IGoap::PlanFound(Dictionary goal, Vector<Ref<GoapAction>> actions) {}
void IGoap::ActionsFinished() {}
void IGoap::PlanAborted(Ref<GoapAction> aborter) {}
bool IGoap::MoveAgent(Ref<GoapAction> next_action) { return true; }
