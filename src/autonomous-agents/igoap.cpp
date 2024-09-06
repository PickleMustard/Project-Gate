#include "igoap.h"
#include "autonomous-agents/goap_action.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/templates/vector.hpp"

using namespace godot;
godot::IGoap::IGoap() {
}

godot::IGoap::~IGoap() {
}

void godot::IGoap::_bind_methods() {
}

HashMap<String, Variant> IGoap::GetWorldState() { return HashMap<String, Variant>{}; }
HashMap<String, Variant> IGoap::CreateGoalState() { return HashMap<String, Variant>{}; }
void IGoap::PlanFailed(HashMap<String, Variant> failed_goal) {}
void IGoap::PlanFound(HashMap<String, Variant> goal, Vector<Ref<GoapAction>> actions) {}
void IGoap::ActionsFinished() {}
void IGoap::PlanAborted(Ref<GoapAction> aborter) {}
bool IGoap::MoveAgent(Ref<GoapAction> next_action) { return true; }
