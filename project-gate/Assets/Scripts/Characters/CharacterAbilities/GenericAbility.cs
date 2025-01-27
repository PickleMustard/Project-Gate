using Godot;

public class GenericAbility{
  public enum ABILITY_TYPE {
    SECONDARY,
    UTILITY,
    MOBILITY,
    ULTIMATE,
  }
  string AbilityName;
  string AbilityDescription;
  Texture2D AbilityIcon;
}
