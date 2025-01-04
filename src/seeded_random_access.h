#ifndef SEEDED_RANDOM_ACCESS_H
#define SEEDED_RANDOM_ACCESS_H

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/ref.hpp"
#include <cstdint>
namespace godot {
/*
 * The Seeded Random Access Class creates a singleton object with a defined seed
 * This is to allow the random aspects of the game to be predictable and repeatable
 * All aspects of the game that use randomness will use these functions to ensure the same seed is used
 */
class SeededRandomAccess : public Object {
	GDCLASS(SeededRandomAccess, Object);

public:
	static SeededRandomAccess *GetInstance();
	int GetWholeNumber(int upper_limit);
  float GetWholeFloat(float upper_limit);
	int GetInteger(int lower_limit, int upper_limit);
  float GetFloatRange(float lower_limit, float upper_limit);
	SeededRandomAccess();
	SeededRandomAccess(uint32_t seed);
	SeededRandomAccess(uint64_t seed);
	~SeededRandomAccess();
	void _init();

protected:
	static void _bind_methods();

private:
	static SeededRandomAccess *g_instance;
	godot::Ref<RandomNumberGenerator> m_generator;
	uint64_t m_seed;
};
} //namespace godot
#endif
