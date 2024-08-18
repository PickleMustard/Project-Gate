#ifndef ISTEPONTILE_H
#define ISTEPONTILE_H

namespace godot {
class IStepOnTile {
public: virtual ~IStepOnTile() {}
  virtual void TileSteppedOnEvent() = 0;
  virtual void TileSteppedOffEvent() = 0;

};
}


#endif
