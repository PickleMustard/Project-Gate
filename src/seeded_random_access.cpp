#include "seeded_random_access.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/string.hpp"
#include <cstdint>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/variant/builtin_types.hpp>

using namespace godot;

SeededRandomAccess *SeededRandomAccess::g_instance = nullptr;

/*
* Default Object Constructor, will only be called once to create the singleton
* Uses a reference to the Godot Random Number Generator to generator its numbers
* Seed is hard set for the current moment to allow for consistent testing of level generation features
* If the global singleton pointer is already occupied, panic and crash
*/
SeededRandomAccess::SeededRandomAccess() {
	m_generator.instantiate();
	m_generator->init_ref();
	//UtilityFunctions::print(m_generator->get_reference_count());
	m_seed = 15672;
	m_generator->set_seed(m_seed);
	ERR_FAIL_COND(g_instance != nullptr);
	g_instance = this;
}

/*
* Parameterized Object Cosntructor
* Has the same function as the default constructor
*
* Parameters:
* seed: unsigned 32 bit integer to use as the seed for the Random Number Generator
*/
SeededRandomAccess::SeededRandomAccess(uint32_t seed) {
	m_generator.instantiate();
	m_generator->init_ref();
	m_seed = seed;
	m_generator->set_seed(seed);
	ERR_FAIL_COND(g_instance != nullptr);
	g_instance = this;
}

/*
* Parameterized Object Constructor
* Has the same function as the default constructor
*
* Parameters:
* seed: unsigned 64 bit integer to be used as the Random Number Generator seed
*/
SeededRandomAccess::SeededRandomAccess(uint64_t seed) {
	m_generator.instantiate();
	m_generator->init_ref();
	m_seed = seed;
	m_generator->set_seed(seed);
	ERR_FAIL_COND(g_instance != nullptr);
	g_instance = this;
}

/*
* Default Destructor
* Will only run once at the close of the game application
* Destroys the singleton pointer and closes out references
* If the pointer doesn't exist, panics and crashes
*/
SeededRandomAccess::~SeededRandomAccess() {
	ERR_FAIL_COND(g_instance != this);
	g_instance = nullptr;
}

/*
* Test function to ensure Random Number Generator can be changed to accept different seeds at runtime
*/
void SeededRandomAccess::_init() {
	m_generator->init_ref();
	m_generator->set_seed(m_seed);
}

/*
* Function to generate a random integer in a defined range
*
* Parameters:
* lower_limit: Integer defining the lowest number that can be returned
* upper_limit: Integer defining the largest number that can be returned
*
* Returns:
* int: Random Integer from between the range of lower_limit and upper_limit
*/
int SeededRandomAccess::GetInteger(int lower_limit, int upper_limit) {
	ERR_FAIL_NULL_V_MSG(m_generator, 0, "Soemthing happened");
	return m_generator->randi_range(lower_limit, upper_limit);
}

/*
* Function to generate a random number from 0 - upper_limit
*
* Parameters:
* upper_limit: Integer defining the largest number that can be returned
*
* Returns:
* int: Random Whole Number from 0 - upper_limit
*/
int SeededRandomAccess::GetWholeNumber(int upper_limit) {
	ERR_FAIL_NULL_V_MSG(m_generator, 0, "Soemthing happened");
	return m_generator->randi_range(0, upper_limit);
}

/*
* Godot Bind Methods to the engines function database
* Exports the functions defined alongside their function pointer, and arguments
* Allows GetInteger and GetWholeNumber to be used in non-C++ scripts
*/
void SeededRandomAccess::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("GetInteger", "lower_limit", "upper_limit"), &SeededRandomAccess::GetInteger, int(), int());
	godot::ClassDB::bind_method(godot::D_METHOD("GetWholeNumber", "upper_limit"), &SeededRandomAccess::GetWholeNumber, int());
  godot::ClassDB::bind_static_method("SeededRandomAccess", godot::D_METHOD("GetInstance"), &SeededRandomAccess::GetInstance);
//  godot::ClassDB::bind_method(godot::D_METHOD("GetInstance"), &SeededRandomAccess::GetInstance());
}

/*
* Global Function to return the instance of the singleton
*/
SeededRandomAccess *SeededRandomAccess::GetInstance() {
	if (g_instance != nullptr) {
		return g_instance;
	} else {
		return nullptr;
	}
}
