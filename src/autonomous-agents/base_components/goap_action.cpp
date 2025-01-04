#include "goap_action.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

godot::GoapAction::GoapAction() {
	preconditions = Dictionary();
	effects = Dictionary();
}

godot::GoapAction::GoapAction(String name, float cost) {
	preconditions = Dictionary();
	effects = Dictionary();
  m_name = name;
  m_cost = cost;
}


godot::GoapAction::~GoapAction() {
}

void godot::GoapAction::_bind_methods() {
  godot::ClassDB::bind_method(godot::D_METHOD("GetActionName"),&GoapAction::GetActionName);
  godot::ClassDB::bind_method(godot::D_METHOD("DoReset"), &GoapAction::DoReset);
	//godot::ClassDB::bind_method(godot::D_METHOD("CheckProceduralPrecondition", "goap_agent", "world_data"), &GoapAction::CheckProceduralPrecondition);
	//godot::ClassDB::bind_method(godot::D_METHOD("Perform", "goap_agent"), &GoapAction::Perform);
}

void godot::GoapAction::DoReset() {
	target = nullptr;
	Reset();
}

void godot::GoapAction::Reset() {
}

bool godot::GoapAction::IsDone(Node *goap_agent) {
	return false;
}

bool godot::GoapAction::GetInRange(Node *goap_agent, Dictionary world_data) {
  return false;
}

bool godot::GoapAction::InProgress(Node *goap_agent) {
  return false;
}
bool godot::GoapAction::CheckProceduralPrecondition(Node *goap_agent, Dictionary world_data) {
	return true;
}
bool godot::GoapAction::Perform(Node *goap_agent) {
	return true;
}
bool godot::GoapAction::RequiresInRange() {
  return true;
}

void godot::GoapAction::AddPrecondition(String key, Variant value) {
	preconditions[key] = value;
}

void godot::GoapAction::RemovePrecondition(String key) {
	if (preconditions.has(key)) {
		preconditions.erase(key);
	}
}

void godot::GoapAction::AddEffect(String key, Variant value) {
	effects[key] = value;
}

void godot::GoapAction::RemoveEffect(String key) {
	if (effects.has(key)) {
		effects.erase(key);
	}
}

godot::Dictionary godot::GoapAction::GetPreconditions() {
	return preconditions;
}

godot::Dictionary godot::GoapAction::GetEffects() {
	return effects;
}

godot::String godot::GoapAction::GetActionName() {
  return m_name;
}
