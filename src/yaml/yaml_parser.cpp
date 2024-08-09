#include "yaml_parser.h"
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

void godot::YamlParser::test_yaml(godot::CharString text) {
	ryml::Parser parser;
	godot::JSON *parseResult = memnew(godot::JSON);
	c4::substr text_string = ryml::to_substr(text.ptrw());
	c4::yml::Tree tree = parser.parse_in_place("", text_string);
	godot::Variant var;
	//tree.rootref() >> var;
	parseResult->set_data(var);

	UtilityFunctions::print(vformat("Yaml results: %s", tree.));
}
