#include "tile_collision.h"
#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/input.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/signal.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "tile_notifier.h"

using namespace godot;

void TileCollision::_bind_methods() {
  ADD_SIGNAL(MethodInfo("NotifyLog"));

}

/*
* Default constructor, unused for now
*/
TileCollision::TileCollision() {

}

/*
* Default destructor, unused for now
*/
TileCollision::~TileCollision() {

}

/*
* Overriden _input_event function
* Each Tile contains a tile_collision node as a child
* The Tile Collision handles user interaction with the tile
* Will output signal indicating that it was the tile that was clicked
*/
void TileCollision::_input_event(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx) {
  if(event->is_action_released("mouse_right")) {
    TileCollision::NotifyLog();
  }
}

void TileCollision::NotifyLog() {
  TileNotifier::getInstance()->TileSelected(this);
}
