#ifndef IGOAP_H
#define IGOAP_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class IGoap : public Resource {
  GDCLASS(IGoap, Resource)
	virtual void temp();
public:
  IGoap();
  ~IGoap();
  virtual HashMap<String, Variant> GetWorldState();
  virtual HashMap<String, Variant> CreateGoalState();

  protected:
  static void _bind_methods();
};
} //namespace godot

#endif
