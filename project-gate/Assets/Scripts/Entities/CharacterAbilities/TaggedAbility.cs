using System.Collections.Generic;
using Godot;

namespace ProjGate.TargetableEntities
{
  public partial class TaggedAbility : Resource
  {
    public BaseAbility Ability;
    public List<AbilityTag> Tags;
    public int SelectionWeight;
    //public List<CharacterClass> ?? Maybe use this
  }
}
