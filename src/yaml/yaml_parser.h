#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/char_string.hpp"
#include "godot_cpp/variant/dictionary.hpp"
namespace godot {
class YamlParser : public Object {
  GDCLASS(YamlParser, Object);

public:
  YamlParser();
  ~YamlParser();

  void _init() {};

  static void test_yaml_caller();
  static void test_yaml(godot::CharString text);
  static godot::Dictionary parse_file(godot::String file);

protected:
  static void _bind_methods();


};

}


#endif
