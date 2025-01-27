using Godot;
using System.Collections.Generic;

public class AbilityContext
{
  public Character Source { get; }

  public Vector3I TargetPosition { get; set; }
  public Character PrimaryTarget { get; set; }
  public List<Character> SecondaryTargets { get; } = new();

  public Dictionary<string, object> CustomData {get; } = new();
}
