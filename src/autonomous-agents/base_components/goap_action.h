#ifndef GOAP_ACTION_H
#define GOAP_ACTION_H

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
namespace godot {
class GoapAction : public Resource {
	GDCLASS(GoapAction, Resource);

private:
	Dictionary preconditions;
	Dictionary effects;
  Variant target_enemy;
  Variant target_tile;
  String m_name;

public:
	float m_cost;
	Node *target;

	GoapAction();
  GoapAction(String name, float cost);
	~GoapAction();

	void DoReset();
	void AddPrecondition(String key, Variant value);
	void RemovePrecondition(String key);
	void AddEffect(String key, Variant value);
	void RemoveEffect(String key);
	Dictionary GetPreconditions();
	Dictionary GetEffects();
  String GetActionName();
	virtual void Reset();
  virtual bool GetInRange(Node *goap_agent, Dictionary world_data);
	virtual bool IsDone(Node *goap_agent);
  virtual bool InProgress(Node *goap_agent);
	virtual bool CheckProceduralPrecondition(Node *goap_agent, Dictionary world_data);
	virtual bool Perform(Node *goap_agent);
	virtual bool RequiresInRange();

protected:
	static void _bind_methods();
};

} //namespace godot

#endif
