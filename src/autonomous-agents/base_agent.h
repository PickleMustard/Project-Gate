#ifndef BASE_AGENT_H
#define BASE_AGENT_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
namespace godot {
class BaseAgent : public Resource {
  GDCLASS(BaseAgent, Resource)

public:
  BaseAgent();
  ~BaseAgent();

  protected:
  static void _bind_methods();


};
}


#endif
