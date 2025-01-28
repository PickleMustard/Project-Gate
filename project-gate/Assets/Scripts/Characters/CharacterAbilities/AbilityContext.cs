using Godot;
using System.Collections.Generic;

namespace ProjGate.Character
{
  public class AbilityContext
  {
    public BaseCharacter Source { get; }
    public Node TileGrid { get; }

    public Vector3I TargetPosition { get; set; }
    public BaseCharacter PrimaryTarget { get; set; }
    public List<BaseCharacter> SecondaryTargets { get; } = new();
  }
}
