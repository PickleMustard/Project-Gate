#ifndef IGOAP_H
#define IGOAP_H

#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class IGoap {
	virtual void temp();
public:
  virtual HashMap<String, Variant> GetWorldState();
  virtual HashMap<String, Variant> CreateGoalState();
};
} //namespace godot

#endif
