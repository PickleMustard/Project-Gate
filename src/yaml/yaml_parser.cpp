#include "yaml_parser.h"
#include "ryml.hpp"
#include <c4/yml/parse.hpp>
#include <c4/yml/tree.hpp>


void godot::YamlParser::test_yaml() {
  ryml::Parser parser;
  EventHandlerTree evt_handler = {};
  c4::yml::Parser parser(&evt_handler);
  c4::yml::Tree tree;
  tree.reserve(30);

}
