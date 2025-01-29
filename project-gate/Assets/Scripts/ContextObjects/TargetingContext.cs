using Godot;
using System.Collections.Generic;
using ProjGate.TargetableEntities;

namespace ProjGate.ContextObjects
{
  public class TargetingContext
  {
    public Node TargetTile {get; set;}

    public BaseCharacter Source {get; set;}
    public Vector3 SourcePosition {get; set;}

    public BaseCharacter PrimaryTarget { get; set; }
    public Vector3 TargetPosition { get; set; }

    public List<BaseCharacter> SecondaryTargets { get; } = new();

    public TargetingContext(Node tile) {
      TargetTile = tile;
    }




  }
}
