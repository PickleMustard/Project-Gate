#ifndef GOAP_ACTION_H
#define GOAP_ACTION_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class GoapAction : public Resource {
	GDCLASS(GoapAction, Resource);

private:
	HashMap<String, Variant> preconditions;
	HashMap<String, Variant> effects;
	bool in_range = false;
  Variant target_enemy;
  Variant target_tile;

public:
	float cost = 1.0f;
	GodotObject *target;

	GoapAction();
	~GoapAction();

	void DoReset();
	bool GetInRange();
	void SetInRange(bool range);
	void AddPrecondition(String key, Variant value);
	void RemovePrecondition(String key);
	void AddEffect(String key, Variant value);
	void RemoveEffect(String key);
	HashMap<String, Variant> GetPreconditions();
	HashMap<String, Variant> GetEffects();
	virtual void Reset();
	virtual bool IsDone();
	virtual bool CheckProceduralPrecondition(GodotObject *agent);
	virtual bool Perform(GodotObject *agent);
	virtual bool RequiresInRange();

protected:
	static void _bind_methods();
};

} //namespace godot

#endif
