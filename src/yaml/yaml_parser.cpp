#include "yaml_parser.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/variant/char_string.hpp"
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

}


void godot::YamlParser::test_yaml_caller() {
  godot::String input_string = "";
  godot::Ref<godot::FileAccess> fa = godot::FileAccess::open("res://Configuration/Testing/test.yaml", FileAccess::READ);
  while(!fa->eof_reached()) {
    godot::String temp = fa->get_line();
    printf("Line read: %s\n", temp.utf8().ptrw());
    input_string += temp + '\n';
  }
  CharStringT<char> ymlb = input_string.utf8();
  printf("Final Input: %s\n", ymlb.ptr());


  YamlParser::test_yaml(ymlb);

}

