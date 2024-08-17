#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/wrapped.hpp"
namespace godot {
class Interactable : public Node2D {
  GDCLASS(Interactable, Node2D);

};
}

#endif
