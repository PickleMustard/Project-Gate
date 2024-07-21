#ifndef SEEDED_RANDOM_ACCESS_H
#define SEEDED_RANDOM_ACCESS_H

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/ref.hpp"
#include <cstdint>
namespace godot {
class SeededRandomAccess : public godot::Object {
public:
	GDCLASS(SeededRandomAccess, Object);

	static SeededRandomAccess *GetInstance();
	int GetWholeNumber(int upper_limit);
	int GetInteger(int lower_limit, int upper_limit);
	SeededRandomAccess();
	SeededRandomAccess(uint32_t seed);
	SeededRandomAccess(uint64_t seed);
	~SeededRandomAccess();
	void _init();

protected:
	static void _bind_methods();

private:
	static SeededRandomAccess *m_instance;
	godot::Ref<RandomNumberGenerator> m_generator;
	uint64_t m_seed;
};
} //namespace godot
#endif
