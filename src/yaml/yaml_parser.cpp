#include "yaml_parser.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/char_string.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "ryml.hpp"
#include <c4/std/string_fwd.hpp>
#include <c4/substr.hpp>
#include <c4/substr_fwd.hpp>
#include <c4/yml/node.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/tree.hpp>
#include <c4/yml/yml.hpp>
#include <sstream>
#include <string>

godot::YamlParser::YamlParser() {

}

godot::YamlParser::~YamlParser() {

}

void godot::YamlParser::_bind_methods() {
  godot::ClassDB::bind_static_method("YamlParser", godot::D_METHOD("parse_file", "file"), &YamlParser::parse_file);
}

godot::Dictionary godot::YamlParser::parse_file(godot::String file) {
  godot::JSON parse_result;
  godot::Dictionary results;
  ryml::Parser parser;
  godot::String input_string = "";
  godot::Ref<godot::FileAccess> fa = godot::FileAccess::open(file, FileAccess::READ);
  while(!fa->eof_reached()) {
    input_string += fa->get_line() + '\n';
  }
  godot::CharString temporary_string = input_string.utf8();
  c4::substr text_string = ryml::to_substr(temporary_string.ptrw());
  c4::yml::Tree tree = parser.parse_in_place("", text_string);
  char buf[100000] {};
  c4::substr output_json = buf;
  ryml::emit_json(tree, output_json);
  input_string = output_json.str;
  input_string = input_string.substr(0, input_string.rfind("}") + 1);
  UtilityFunctions::print(vformat("Input String: %s", input_string));
  results = parse_result.parse_string(input_string);
  fa->close();
  return results;
}
void godot::YamlParser::test_yaml(godot::CharString text) {
	ryml::Parser parser;
	godot::JSON *parseResult = memnew(godot::JSON);
	c4::substr text_string = ryml::to_substr(text.ptrw());
	c4::yml::Tree tree = parser.parse_in_place("", text_string);
  std::stringstream ss;
  ss << ryml::as_json(tree);
  std::string s = ss.str();
	godot::Variant var;
	//tree.rootref() >> var;
	parseResult->set_data(var);
  godot::String gs(s.c_str());
  printf("Here\n");

	UtilityFunctions::print(vformat("Parser Output: %s", gs));
  memdelete(parseResult);

}


void godot::YamlParser::test_yaml_caller() {
  godot::String input_string = "";
  godot::Ref<godot::FileAccess> fa = godot::FileAccess::open("res://Configuration/Testing/test.yaml", FileAccess::READ);
  while(!fa->eof_reached()) {
    godot::String json_string = fa->get_line();
    printf("Line read: %s\n", json_string.utf8().ptrw());
    input_string += json_string + '\n';
  }
  CharStringT<char> ymlb = input_string.utf8();
  printf("Final Input: %s\n", ymlb.ptr());


  YamlParser::test_yaml(ymlb);
}

