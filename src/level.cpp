#include "level.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "level_generator.h"
#include "tilegrid.h"

godot::Level::Level() {
	m_level_generator = memnew(LevelGenerator);
	if (get_child_count() > 0) {
		for (int i = 0; i < get_child_count(); i++) {
			get_child(i)->queue_free();
		}
	}
	m_tile_grid = memnew(TileGrid);
  m_level_grid_def["origin"] = Vector3(0,0,0);
  m_level_grid_def["num_rooms"] = 5;

	//Engine::get_singleton()->connect("RegenerateGrid", callable_mp(this, &godot::Level::GenerateLevel));
	UtilityFunctions::print(get_child_count());
}

godot::Level::~Level() {
}

void godot::Level::_bind_methods() {
	ADD_SIGNAL(MethodInfo("RegenerateGrid", PropertyInfo(Variant::OBJECT, "engine", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_DEFAULT, "Engine")));
	godot::ClassDB::bind_method(godot::D_METHOD("GenerateLevel"), &Level::GenerateLevel);

	ADD_GROUP("Level Properties", "m_level_");
  //ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "m_level_grid_def"), "SetGridDefinitionOrigin", "GetGridDefinitionOrigin");
}

void godot::Level::GenerateLevel() {
	if (get_child_count() > 0) {
		for (int i = 0; i < get_child_count(); i++) {
			get_child(i)->queue_free();
		}
	}
	UtilityFunctions::print(m_tile_grid);

	m_tile_grid = memnew(TileGrid);
	add_child(m_tile_grid);
	m_tile_grid->set_owner(this->get_owner());
	m_tile_grid->GenerateTileGrid();


//m_tile_grid->GenerateTileGrid();
}

void godot::Level::SetGridDefinitionOrigin(Vector3 origin) {
  m_level_grid_def["origin"] = origin;
}

godot::Vector3 godot::Level::GetGridDefinitionOrigin() {
  return m_level_grid_def["origin"];
}

void godot::Level::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		add_child(m_tile_grid);
		m_tile_grid->set_owner(this->get_owner());
		m_tile_grid->GenerateTileGrid();
		connect("RegenerateGrid", callable_mp(this, &godot::Level::GenerateLevel));
	}
}
