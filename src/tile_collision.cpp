#include "tile_collision.h"
#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/input.hpp"
#include "godot_cpp/classes/input_event_key.hpp"
#include "godot_cpp/classes/input_event_mouse_button.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

void TileCollision::_bind_methods() {

}

TileCollision::TileCollision() {

}

TileCollision::~TileCollision() {

}

void TileCollision::_input_event(Camera3D *camera, const Ref<InputEvent> &event, const Vector3 &position, const Vector3 &normal, int32_t shape_idx) {
  //UtilityFunctions::print(event->as_text());
  if(event->is_action_pressed("mouse_left")) {
    UtilityFunctions::print("I was pressed lmao");
  }
}
