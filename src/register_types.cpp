#include "register_types.h"
#include "godot_cpp/classes/engine.hpp"
#include "seeded_random_access.h"
#include "level-generation/tile_collision.h"
#include "level-generation/tile_mesh_generator.h"
#include "level-generation/tile_notifier.h"
#include "level-generation/tilegrid.h"
#include "level-generation/level.h"
#include "level-generation/level_generator.h"
#include "tiles/interactable.h"
#include "tiles/obstacle.h"
#include "tiles/ordinary.h"
#include "tiles/unit_spawner.h"
#include "yaml/yaml_parser.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
  ClassDB::register_class<YamlParser>();
  //Tile Objects
  ClassDB::register_class<Tile>();
  ClassDB::register_class<Interactable>();
  ClassDB::register_class<Ordinary>();
  ClassDB::register_class<Obstacle>();
  ClassDB::register_class<UnitSpawner>();

  //Physical Tile Objects
  ClassDB::register_class<TileCollision>();
  ClassDB::register_class<TileGrid>();
  ClassDB::register_class<TileMeshGenerator>();
  //Level Objects
  ClassDB::register_class<Level>();
  ClassDB::register_class<LevelGenerator>();


  //Singletons
  ClassDB::register_class<SeededRandomAccess>();
  ClassDB::register_class<TileNotifier>();

  //Register Singletons
	SeededRandomAccess *SRA = memnew(SeededRandomAccess);
	Engine::get_singleton()->register_singleton("GlobalSeededRandom", SRA);

  TileNotifier *Notifier = memnew(TileNotifier);
  Engine::get_singleton()->register_singleton("GlobalTileNotifier", Notifier);

}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.register_terminator(uninitialize_gdextension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
