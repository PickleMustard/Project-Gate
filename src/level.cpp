#include "level.h"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level_generator.h"

godot::Level::Level() {
  m_level_generator = memnew(LevelGenerator);
  if(get_child_count() > 0) {
    for(int i = 0; i < get_child_count(); i++) {
      get_child(i)->queue_free();
    }
  }
  m_tile_grid = memnew(TileGrid);

  OS::get_singleton()->connect("GenerateLevel", );
UtilityFunctions::print(get_child_count());

}

godot::Level::~Level() {

}

void godot::Level::_bind_methods() {
  ADD_SIGNAL(MethodInfo("RegenerateGrid", PropertyInfo(Variant::OBJECT, "level", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_DEFAULT, "Level")));
  godot::ClassDB::bind_method(godot::D_METHOD("GenerateLevel"), &Level::GenerateLevel);

}

void godot::Level::GenerateLevel() {
  if(get_child_count() > 0) {
    for(int i = 0; i < get_child_count(); i++) {
      get_child(i)->queue_free();
    }
  }

  m_tile_grid->GenerateTileGrid();
}


void godot::Level::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
    add_child(m_tile_grid);
    m_tile_grid->set_owner(this->get_owner());
    m_tile_grid->GenerateTileGrid();
  }
}
