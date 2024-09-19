using Godot;

public partial class Weapon : Resource
{
  [Export]
  public string WeaponName { get; private set; }
  [Export]
  public int MaxRange { get; private set; }
  [Export]
  public bool IgnoresLineSight { get; private set; }
  [Export]
  public int BaseDamage { get; private set; }
  //[Export]
  public IOnHitBehavior OnHitBehavior;
  //[Export]
  public IGetTargetingBehavior TargetingBehavior;

  public void SetWeaponDamage(int newDamage)
  {
    BaseDamage = newDamage;
  }
  public void SetOnHitBehavior(IOnHitBehavior newBehavior)
  {
    OnHitBehavior = newBehavior;
  }
  public void OnHit(Character target)
  {
    OnHitBehavior.CalculateOnHit(BaseDamage, target);
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
