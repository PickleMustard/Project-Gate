#ifndef LEVEL_H
#define LEVEL_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "level_generator.h"
#include "tilegrid.h"

namespace godot {
class Level : public Node {
	GDCLASS(Level, Node);

public:
	Level();
  Level(bool should_generate, String file);
	~Level();
  void _ready() override;
  void _enter_tree() override;
  void ReadyLevel();
	void GenerateLevel();

  void SetGridDefinitionOrigin(Vector3 origin);
  void SetGridDefinitionNumRooms(int num_rooms);



  Vector3 GetGridDefinitionOrigin();
  int GetGridDefinitionNumRooms();

  void SetShouldGenerateLevel(bool decision);
  bool GetShouldGenerateLevel();
  void SetFileLocation(String file);
  String GetFileLocation();


protected:
	static void _bind_methods();
	void _notification(int p_what);

private:
	LevelGenerator *m_level_generator;
	TileGrid *m_tile_grid;
  //For now, just stores a single tile grid,
  Dictionary m_level_grid_def;
  bool m_level_should_generate_level = false;
  String m_level_file_location;
};
} //namespace godot

#endif
