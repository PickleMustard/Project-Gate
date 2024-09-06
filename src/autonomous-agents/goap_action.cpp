#include "goap_action.h"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

godot::GoapAction::GoapAction() {
	preconditions = HashMap<String, Variant>();
	effects = HashMap<String, Variant>();
}

godot::GoapAction::~GoapAction() {
}

void godot::GoapAction::_bind_methods() {
}

void godot::GoapAction::DoReset() {
	in_range = false;
	target = nullptr;
	Reset();
}

void godot::GoapAction::Reset() {
}

bool godot::GoapAction::IsDone() {
	return true;
}
bool godot::GoapAction::CheckProceduralPrecondition(GodotObject *agent) {
	return true;
}
bool godot::GoapAction::Perform(GodotObject *agent) {
	return true;
}
bool godot::GoapAction::RequiresInRange() {
  return true;
}

bool godot::GoapAction::GetInRange() {
	return in_range;
}

void godot::GoapAction::SetInRange(bool range) {
	in_range = range;
}

void godot::GoapAction::AddPrecondition(String key, Variant value) {
	preconditions.insert(key, value);
}

void godot::GoapAction::RemovePrecondition(String key) {
	if (preconditions.has(key)) {
		preconditions.erase(key);
	}
}

void godot::GoapAction::AddEffect(String key, Variant value) {
	effects.insert(key, value);
}

void godot::GoapAction::RemoveEffect(String key) {
	if (effects.has(key)) {
		effects.erase(key);
	}
}

godot::HashMap<godot::String, godot::Variant> godot::GoapAction::GetPreconditions() {
	return preconditions;
}

godot::HashMap<godot::String, godot::Variant> godot::GoapAction::GetEffects() {
	return effects;
}
