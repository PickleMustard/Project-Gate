#include "tile_notifier.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "level-generation/tilegrid.h"
#include "tile_collision.h"

using namespace godot;

TileNotifier *TileNotifier::g_instance = nullptr;

TileNotifier::TileNotifier() {
	ERR_FAIL_COND(g_instance != nullptr);
	g_instance = this;
}

TileNotifier::~TileNotifier() {
	ERR_FAIL_COND(g_instance != this);
	g_instance = nullptr;
}

TileNotifier *TileNotifier::getInstance() {
	if (g_instance != nullptr) {
		return g_instance;
	} else {
		return nullptr;
	}
}

void TileNotifier::GridCreationNotification(TileGrid* grid) {
  UtilityFunctions::print("Emiting Grid Created Signal");
  emit_signal("GridCreated", grid);
}

void TileNotifier::TileSelected(TileCollision* selected_tile){
  emit_signal("TileSelected", selected_tile);

}

void TileNotifier::_bind_methods() {
  ADD_SIGNAL(MethodInfo("TileSelected", PropertyInfo(Variant::OBJECT, "tile_collision", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_DEFAULT, "TileCollision")));
  ADD_SIGNAL(MethodInfo("GridCreated", PropertyInfo(Variant::OBJECT, "tilegrid", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_DEFAULT, "TileGrid")));
}
