#include "interactable.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

godot::Interactable::Interactable() :
		Tile() {
}

godot::Interactable::Interactable(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t tile_type, int interactable_type) :
		Tile(position, r, c, flat_topped, outer_size, inner_size, height, tile_type) {
	m_interactable_type = interactable_type;
}

godot::Interactable::~Interactable() {
}

void godot::Interactable::TileSteppedOnEvent() {
	UtilityFunctions::print("Interacting");
	/*if (SteppedOnTileCallables.size() > 0) {
		for (int i = 0; i < SteppedOnTileCallables.size(); i++) {
			Callable event = SteppedOnTileCallables[i];
			UtilityFunctions::print("Calling Event ", event.get_method());
			if (event.get_method().is_subsequence_of("GenerateItem")) {
				event.call(m_interactable_type);
			} else {
				event.call();
			}
		}
	}*/
}

void godot::Interactable::TileSteppedOffEvent() {
	UtilityFunctions::print("Goodbye, you stepped off me!");
}

void godot::Interactable::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("TileSteppedOnEvent"), &Interactable::TileSteppedOnEvent);
	godot::ClassDB::bind_method(godot::D_METHOD("TileSteppedOffEvent"), &Interactable::TileSteppedOffEvent);
	godot::ClassDB::bind_method(godot::D_METHOD("AddStepOnEvent", "event"), &Interactable::AddStepOnEvent);
	godot::ClassDB::bind_method(godot::D_METHOD("AddStepOffEvent", "event"), &Interactable::AddStepOffEvent);
	godot::ClassDB::bind_method(godot::D_METHOD("RemoveStepOnEvent", "event"), &Interactable::RemoveStepOnEvent);
	godot::ClassDB::bind_method(godot::D_METHOD("RemoveStepOffEvent", "event"), &Interactable::RemoveStepOffEvent);
}

void godot::Interactable::AddStepOnEvent(Callable event) {
	SteppedOnTileCallables.append(event);
}
bool godot::Interactable::RemoveStepOnEvent(Callable event) {
	if (SteppedOnTileCallables.has(event)) {
		int position = SteppedOnTileCallables.find(event);
		SteppedOnTileCallables.remove_at(position);
		return true;
	}
	return false;
}
void godot::Interactable::AddStepOffEvent(Callable event) {
	SteppedOffTileCallables.append(event);
}
bool godot::Interactable::RemoveStepOffEvent(Callable event) {
	if (SteppedOffTileCallables.has(event)) {
		int position = SteppedOffTileCallables.find(event);
		SteppedOffTileCallables.remove_at(position);
		return true;
	}
	return false;
}
