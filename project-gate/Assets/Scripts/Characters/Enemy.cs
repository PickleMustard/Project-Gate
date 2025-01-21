using Godot;
using Godot.Collections;
using ProjGate.Pickups;

public partial class Enemy : Character
{

  Node level;

  private int m_AmountCurrencyDroppedOnKill;

  public override void _Ready()
  {
    Connect(SignalName.CharacterKilled, CommunicationBus.Instance.GetCharacterKilledEventCallable());
    team = Character.CHARACTER_TEAM.enemy;
    Callable SetPositionCall = new Callable(this, "SetPosition");
    var test = ResourceLoader.Load("res://Assets/Scripts/User-Interface/GenericCharacterBanner.cs") as CSharpScript;
  }

  public void GenerateCharacter(string name,
      BaseWeapon weapon, BaseGrenade grenade,
      Array<WEAPON_PROFICIENCIES> proficiencies,
      int movementDistance, int actionPoints,
      int health, float accumulationRate,
      float requeueSpeed, int turnPriority, int CurrencyDropRate,
      Texture2D Icon)
  {
    this.CharacterName = name;
    this.MainWeapon = weapon;
    this.grenade = grenade;
    this.proficiencies = proficiencies;
    this.TotalDistance = movementDistance;
    this.TotalActionPoints = actionPoints;
    this.TotalHealth = health;
    this.BaseSpeedAccumulator = accumulationRate;
    this.SpeedNeededToRequeue = requeueSpeed;
    this.HeapPriority = turnPriority;
    this.m_AmountCurrencyDroppedOnKill = CurrencyDropRate;
    this.Icon = Icon;
  }

  //Needs to set the behaviors of the Goap agent underneath the node
  public void SetAIBehaviors(Godot.Collections.Array behaviors)
  {

  }

  public void RunAI()
  {
    Node aiAgent = this.FindChild("agent", true, false);
    if (aiAgent.HasMethod("RunAI"))
    {
      aiAgent.Call("RunAI");
    }
  }

  public int GetAmountOfCurrencyDroppedOnKill()
  {
    return m_AmountCurrencyDroppedOnKill;
  }
}
