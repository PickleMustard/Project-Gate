using Godot;
using Godot.Collections;

public partial class Enemy : Character
{
    [Signal]
    public delegate void EnemyKilledEventHandler();

    Node level;

    private int m_AmountCurrencyDroppedOnKill;

    public override void _Ready()
    {
        Connect(SignalName.EnemyKilled, CommunicationBus.Instance.GetEnemyKilledEventCallable());
        team = Character.CHARACTER_TEAM.enemy;
        Callable SetPositionCall = new Callable(this, "SetPosition");
        var test = ResourceLoader.Load("res://Assets/Scripts/User-Interface/GenericCharacterBanner.cs") as CSharpScript;
    }

    public void GenerateCharacter(string name,
        Weapon weapon, Grenade grenade,
        Array<WEAPON_PROFICIENCIES> proficiencies,
        int movementDistance, int actionPoints,
        int health, float accumulationRate,
        float requeueSpeed, int turnPriority, int CurrencyDropRate)
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

    protected override void KillCharacter()
    {
        CharacterTurnController.Instance.RemoveCharacterFromTurnController(this);
        CharacterTurnController.Instance.RemoveUpdateCharacterMovementCallable(updateMovementCalcs);
        AudioStreamPlayer3D player = (AudioStreamPlayer3D)FindChild("character_death");
        player.Play();
        Visible = false;
        EmitSignal(SignalName.EnemyKilled, this);
    }

    public int GetAmountOfCurrencyDroppedOnKill()
    {
        return m_AmountCurrencyDroppedOnKill;
    }
}
