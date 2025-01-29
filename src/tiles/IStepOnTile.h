#ifndef ISTEPONTILE_H
#define ISTEPONTILE_H

#include "godot_cpp/variant/variant.hpp"
namespace godot {
class IStepOnTile {
public: virtual ~IStepOnTile() {}
  virtual void TileSteppedOnEvent(godot::Variant) = 0;
  virtual void TileSteppedOffEvent(godot::Variant) = 0;

};
}


#endif
