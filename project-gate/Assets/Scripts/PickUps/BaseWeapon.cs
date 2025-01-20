using Godot;

namespace ProjGate.Pickups
{
  [GlobalClass]
  public partial class BaseWeapon : Resource
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
    public uint WeaponIndex { get; private set; }
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

    [Export(PropertyHint.File, "*.cs")]
    public string OnHitBehaviorLocation { get; set; }
    [Export(PropertyHint.File, "*.cs")]
    public string StartTurnBehaviorLocation { get; set; }
    [Export(PropertyHint.File, "*.cs")]
    public string EndTurnBehaviorLocation { get; set; }
    [Export(PropertyHint.File, "*.cs")]
    public string TargetingBehaviorLocation { get; set; }

    private IOnHitBehavior OnHitBehavior;
    private IStartTurnBehavior StartTurnBehavior;
    private IEndTurnBehavior EndTurnBehavior;
    private IGetTargetingBehavior TargetingBehavior;

    public override void _SetupLocalToScene()
    {
      CSharpScript onHitBehavior = ResourceLoader.Load(OnHitBehaviorLocation) as CSharpScript;
      GD.Print("Local Weapon Behavior: OnHitBehavior using as: ", onHitBehavior);
      IOnHitBehavior OnHitBehavior = onHitBehavior.New().AsGodotObject() as IOnHitBehavior;

    }

    public void SetInitialWeaponParameters(string name, int maxRange, int effectiveRange, int baseDamage, string weaponType, bool ignoresLOS, bool canRicochet, bool canPierce,
        IOnHitBehavior onhit, IStartTurnBehavior startTurn, IEndTurnBehavior endTurn, IGetTargetingBehavior targeting)
    {
      this.WeaponName = name;
      this.MaxRange = maxRange;
      this.EffectiveRange = effectiveRange;
      this.BaseDamage = baseDamage;
      this.weaponType = (WEAPON_TYPES)System.Enum.Parse(typeof(WEAPON_TYPES), weaponType);
      this.IgnoresLineSight = ignoresLOS;

      this.OnHitBehavior = onhit;
      GD.Print("Set OnHitBehavior: ", OnHitBehavior);
      this.StartTurnBehavior = startTurn;
      this.EndTurnBehavior = endTurn;
      this.TargetingBehavior = targeting;
    }

    public void SetWeaponDamage(int newDamage)
    {
      BaseDamage = newDamage;
    }
    public void SetOnHitBehavior(IOnHitBehavior newBehavior)
    {
      OnHitBehavior = newBehavior;
    }
    public void SetWeaponType(WEAPON_TYPES type)
    {
      weaponType = type;
    }
    public void SetEffectiveRange(int range)
    {
      EffectiveRange = range;
    }
    public void SetIgnoreLineOfSight(bool shouldIgnore)
    {
      IgnoresLineSight = shouldIgnore;
    }
    public void OnHit(Character attacker, Resource targetedLocation, Node TileGrid)
    {
      if (OnHitBehavior != null)
      {
        GD.Print("OnHitBehavior");
        OnHitBehavior.CalculateOnHit(this, attacker, targetedLocation, TileGrid);
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
}
