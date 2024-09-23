using Godot;

public partial class Weapon : Resource
{
  public enum WEAPON_TYPES
  {
    melee,
    handgun,
    shotgun,
    carbine,
    rifle,
    longbarrel,
    special
  }
  [Export]
  public string WeaponName { get; private set; }
  [Export]
  public int MaxRange { get; private set; }
  [Export]
  public int EffectiveRange { get; private set; }
  [Export]
  public bool IgnoresLineSight { get; private set; }
  [Export]
  public int BaseDamage { get; private set; }
  [Export(PropertyHint.Enum)]
  public WEAPON_TYPES weaponType { get; private set; }

  public IOnHitBehavior OnHitBehavior;
  public IStartTurnBehavior StartTurnBehavior;
  public IEndTurnBehavior EndTurnBehavior;
  public IGetTargetingBehavior TargetingBehavior;

  public void SetWeaponDamage(int newDamage)
  {
    BaseDamage = newDamage;
  }
  public void SetOnHitBehavior(IOnHitBehavior newBehavior)
  {
    OnHitBehavior = newBehavior;
  }
  public void SetWeaponType(WEAPON_TYPES type) {
    weaponType = type;
  }
  public void SetEffectiveRange(int range) {
    EffectiveRange = range;
  }
  public void SetIgnoreLineOfSight(bool shouldIgnore) {
    IgnoresLineSight = shouldIgnore;
  }
  public void OnHit(float proficiency, Character target)
  {
    GD.Print(proficiency);
    if (OnHitBehavior != null)
    {
      OnHitBehavior.CalculateOnHit(BaseDamage, proficiency, target);
    }
  }
  public void SetWeaponName(string name)
  {
    WeaponName = name;
  }

  public string GetWeaponName()
  {
    return WeaponName;
  }

  public int GetMaxRange()
  {
    return MaxRange;
  }

  public void SetMaxRange(int range)
  {
    MaxRange = range;
  }
  public override string ToString()
  {
    return "Weapon of name" + WeaponName;

  }

}
