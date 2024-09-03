#ifndef GOAP_PLANNER_H
#define GOAP_PLANNER_H
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
namespace godot {
class GoapPlanner : Resource {
  GDCLASS(GoapPlanner, Resource);
public:
  GoapPlanner();
  ~GoapPlanner();

protected:
  static void _bind_methods();
};
}

#endif
