#include "gdexample.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GDExample::_bind_methods() {
}

GDExample::GDExample() {
	_time_passed = 0.0;
}

GDExample::~GDExample() {
	//cleanup
}

void GDExample::_process(double delta) {
	_time_passed += delta;

	Vector2 new_position = Vector2(10.0 + (10.0 * sin(_time_passed * 2.0)), 10.0 + (10.0 * cos(_time_passed * 1.5)));

	set_position(new_position);
}
