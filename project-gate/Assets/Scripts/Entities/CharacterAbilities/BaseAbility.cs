using Godot;

namespace ProjGate.TargetableEntities
{
  public partial class BaseAbility : Resource
  {
    public enum ABILITY_TYPE
    {
      NONE = 0,
      APPLYSTATUS = 1,
      DIRECTDAMAGE = 2,
      HEALING = 4,
      BUFFING = 8,
      MOBILITY = 16,
      LAYHAZARD = 32,
      SPAWNING = 64,
    }

    public ABILITY_TYPE AbilityType { get; }
    string AbilityName;
    string AbilityDescription;
    int AbilityCooldown;
    int NumCharges;
    Texture2D AbilityIcon;
  }
}
