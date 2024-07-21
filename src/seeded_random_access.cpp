#include "seeded_random_access.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cstdint>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/builtin_types.hpp>

using namespace godot;

SeededRandomAccess *SeededRandomAccess::m_instance = nullptr;

SeededRandomAccess::SeededRandomAccess() {
	m_generator.instantiate();
	m_generator->init_ref();
	//UtilityFunctions::print(m_generator->get_reference_count());
	m_seed = 15672;
	m_generator->set_seed(m_seed);
	ERR_FAIL_COND(m_instance != nullptr);
	m_instance = this;
}

SeededRandomAccess::SeededRandomAccess(uint32_t seed) {
	m_generator.instantiate();
	m_generator->init_ref();
	m_seed = seed;
	m_generator->set_seed(seed);
	ERR_FAIL_COND(m_instance != nullptr);
	m_instance = this;
}

SeededRandomAccess::SeededRandomAccess(uint64_t seed) {
	m_generator.instantiate();
	m_generator->init_ref();
	m_seed = seed;
	m_generator->set_seed(seed);
	ERR_FAIL_COND(m_instance != nullptr);
	m_instance = this;
}

SeededRandomAccess::~SeededRandomAccess() {
	ERR_FAIL_COND(m_instance != this);
	m_instance = nullptr;
}

void SeededRandomAccess::_init() {
	m_generator->init_ref();
	m_generator->set_seed(m_seed);
}

int SeededRandomAccess::GetInteger(int lower_limit, int upper_limit) {
	//UtilityFunctions::print(m_generator.is_valid());
	//ERR_FAIL_COND_V_MSG(m_generator == nullptr, 0, "Something happened");
	ERR_FAIL_NULL_V_MSG(m_generator, 0, "Soemthing happened");
	return m_generator->randi_range(lower_limit, upper_limit);
}

int SeededRandomAccess::GetWholeNumber(int upper_limit) {
	ERR_FAIL_NULL_V_MSG(m_generator, 0, "Soemthing happened");
	return m_generator->randi_range(0, upper_limit);
}

void SeededRandomAccess::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("GetInteger", "lower_limit", "upper_limit"), &SeededRandomAccess::GetInteger, int(), int());
	godot::ClassDB::bind_method(godot::D_METHOD("GetWholeNumber", "upper_limit"), &SeededRandomAccess::GetWholeNumber, int());
}

SeededRandomAccess *SeededRandomAccess::GetInstance() {
	if (m_instance != nullptr) {
		return m_instance;
	} else {
		return nullptr;
	}
}
