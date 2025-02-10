using Godot;
using Godot.Collections;
using ProjGate.Pickups;
using ProjGate.TargetableEntities;

namespace ProjGate.Services
{
    public partial class CommunicationBus : Node
    {
        [Signal]
        public delegate void IdentifyStrayNodeEventHandler();
        [Signal]
        public delegate void AddCurrencyEventHandler();
        [Signal]
        public delegate void DecementCurrencyEventHandler();
        [Signal]
        public delegate void UpdateUnitControllerDisplayStateEventHandler(InteractionStates nextState);
        [Signal]
        public delegate void ProcessUnitCommandEventHandler(Node tile_collider);
        [Signal]
        public delegate void EndCharacterTurnEventHandler();
        [Signal]
        public delegate void LevelGeneratedEventHandler();

        public static CommunicationBus Instance { get; private set; }

        public BaseCharacter ActiveCharacter { get; set; }

        public Callable AltClickDownEvent { get; private set; }
        public Callable AltClickUpEvent { get; private set; }
        public Callable EndTurnEvent { get; private set; }
        public Callable LevelLoadingFinishedCall { get; private set; }
        public Callable TileNotifiedEvent { get; private set; }
        public Callable SpawnEnemyCall { get; private set; }
        public Callable SpawnCharacterCall { get; private set; }
        public Callable UpdateCharacterCall { get; private set; }
        public Callable GenerateItemCall { get; private set; }
        public Callable UpdateCharacterInteractionStateEvent { get; private set; }
        public Callable CharacterKilledCall { get; private set; }

        private Node TileGrid;
        private Node Level;
        private UnitController unitController;

        public override void _EnterTree()
        {
            Instance = this;
            Engine.RegisterSingleton("CommunicationBus", this);
            AltClickDownEvent = new Callable(this, "OnAltClickDownEvent");
            AltClickUpEvent = new Callable(this, "OnAltClickUpEvent");
            EndTurnEvent = new Callable(this, "EndTurn");
            TileNotifiedEvent = new Callable(this, "OnTileNotifiedEvent");
            UpdateCharacterCall = new Callable(this, "UpdateCurrentCharacter");
            GenerateItemCall = new Callable(this, "GenerateItem");
            SpawnEnemyCall = new Callable(this, "SpawnEnemy");
            SpawnCharacterCall = new Callable(this, "SpawnPlayerCharacter");
            CharacterKilledCall = new Callable(this, "CharacterKilled");
            LevelLoadingFinishedCall = new Callable(this, "OnLevelLoadingFinish");
            UpdateCharacterInteractionStateEvent = new Callable(this, MethodName.UpdateCharacterInteractionState);
        }


        public override void _Ready()
        {
            GodotObject TileNotifierSingleton = Engine.GetSingleton("GlobalTileNotifier");
            var signals = TileNotifierSingleton.GetSignalList();
            TileNotifierSingleton.Connect(signals[0]["name"].ToString(), TileNotifiedEvent);

            InputHandler i_handle = GetTree().GetNodesInGroup("InputHandler")[0] as InputHandler;
            i_handle.AltClickDown += OnAltClickDownEvent;
            i_handle.AltClickUp += OnAltClickUpEvent;

            Connect(SignalName.AddCurrency, CurrencyService.Instance.GetIncrementCurrencyCallable());
            Connect(SignalName.DecementCurrency, CurrencyService.Instance.GetDecrementCurrencyCallable());
            Level = GetNodeOrNull<Node3D>("/root/Level");

            unitController = GetTree().GetNodesInGroup("UnitControl")[0] as UnitController;
            Array<Node> NodeGroups = GetTree().GetNodesInGroup("Level");
            if (NodeGroups.Count > 0)
            {
                Level = NodeGroups[0];
            }
            Level.Connect("LevelGenerated", new Callable(this, MethodName.SendLevelGeneratedNotification));
        }

        public void SendLevelGeneratedNotification()
        {
            EmitSignal(SignalName.LevelGenerated);
        }
        public void OnTileNotifiedEvent(Node tile_collider)
        {
            GD.Print("In CommunicationBus Tile Notified Event");
            EmitSignal(SignalName.ProcessUnitCommand, tile_collider);
        }

        public void OnAltClickDownEvent()
        {
            GD.Print("Alt Click Down Event");
            unitController.DisplayOptionsOnGrid();
        }

        public void OnAltClickUpEvent()
        {
            GD.Print("Alt Click Up Event");
            unitController.HideOptionsOnGrid();
        }

        private void EndTurn()
        {
            EmitSignal(SignalName.EndCharacterTurn);
        }

        public void UpdateCharacterInteractionState(string StateName)
        {
            GD.Print("StateButton Name: ", StateName);
            if (StateName == "MoveCharacter")
            {
                GD.Print("Moving Character");
                unitController.UpdateDisplayState(InteractionStates.DisplayMovement);
            }
            else if (StateName == "UseWeapon")
            {
                GD.Print("Using Weapon");
                unitController.UpdateDisplayState(InteractionStates.DisplayWeaponDetails);
            }
            else if (StateName == "UseGrenade")
            {
                unitController.UpdateDisplayState(InteractionStates.DisplayGrenadeDetails);
            }
            else
            {
                unitController.UpdateDisplayState(InteractionStates.DisplayAbilityDetails);
            }
        }

        public void GenerateItem(int ty)
        {
            WeaponGenerator weaponGenerator = new WeaponGenerator();
            weaponGenerator.GenerateWeapon("testweapon");
            BaseWeapon weapon = new BaseWeapon();
            weapon.SetWeaponName("Ooga Booga Gun");
            ActiveCharacter.SetMainWeapon(weapon);
        }

        public void AddCharacter(BaseCharacter character, GenericCharacterBanner CharacterBanner)
        {
            Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
            MarginContainer bannerMargin = new MarginContainer();
            bannerMargin.SizeFlagsStretchRatio = 2;
            bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;
            bannerMargin.AddChild(CharacterBanner, true);
            charInf.AddChild(bannerMargin, true);
            GD.Print("Character Name: ", character.CharacterName);
            GD.Print("OOOOGA GOOGOASDFAL:KJDFG");
            CharacterBanner.UpdateCharacterName(character.CharacterName);
            CharacterBanner.UpdateIconBanner(character.GetCharacterIcon());
            CharacterBanner.UpdateMovementRemaining(character.GetDistanceRemaining());
            CharacterBanner.UpdateHeapPriority(character.HeapPriority);
            CharacterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
        }

        public void SpawnCharacter(Resource Tile, BaseCharacter generatedCharacter, Node resourceProvider, Godot.Collections.Array<Resource> Actions)
        {
            Node character = ResourceLoader.Load<PackedScene>("res://Assets/Units/character.tscn").Instantiate();
            GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
            WeaponGenerator weaponGenerator = new WeaponGenerator();
            BaseWeapon startingWeapon = weaponGenerator.GenerateWeapon("res://Configuration/Weapons/testweapon.yml");
            Level.AddChild(character, true);
            Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
            MarginContainer bannerMargin = new MarginContainer();
            bannerMargin.AddChild(characterBanner, true);
            charInf.AddChild(bannerMargin, true);
            bannerMargin.SizeFlagsStretchRatio = 2;
            bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;

            characterBanner.UpdateCharacterName("This is a temporary name");
            characterBanner.UpdateMovementRemaining(ActiveCharacter.GetDistanceRemaining());
            characterBanner.UpdateHeapPriority(ActiveCharacter.HeapPriority);
            characterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
            ActiveCharacter.Connect(ActiveCharacter.GetSignalList()[1]["name"].ToString(), characterBanner.GetUpdateMovementCallable());
            ActiveCharacter.Connect(ActiveCharacter.GetSignalList()[2]["name"].ToString(), characterBanner.GetUpdateHeapPriorityCallable());
            character.Call("SetPosition", Tile);
        }



        public void SpawnPlayerCharacter(Resource Tile, BaseCharacter generatedCharacter)
        {
            Node3D character = ResourceLoader.Load<PackedScene>("res://Assets/Units/playerteamcharacter.tscn").Instantiate() as Node3D;
            GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
            Level.AddChild(character, true);

            string name = character.Name;
            character.ReplaceBy(generatedCharacter, true);
            character.QueueFree();
            generatedCharacter.Name = name;
            Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
            MarginContainer bannerMargin = new MarginContainer();
            bannerMargin.AddChild(characterBanner, true);
            charInf.AddChild(bannerMargin, true);
            bannerMargin.SizeFlagsStretchRatio = 2;
            bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;

            characterBanner.UpdateCharacterName(generatedCharacter.CharacterName);
            characterBanner.UpdateIconBanner(generatedCharacter.GetCharacterIcon());
            characterBanner.UpdateMovementRemaining(generatedCharacter.GetDistanceRemaining());
            characterBanner.UpdateHeapPriority(generatedCharacter.HeapPriority);
            characterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
            generatedCharacter.Connect(generatedCharacter.GetSignalList()[1]["name"].ToString(), characterBanner.GetUpdateMovementCallable());
            generatedCharacter.Connect(generatedCharacter.GetSignalList()[2]["name"].ToString(), characterBanner.GetUpdateHeapPriorityCallable());
            generatedCharacter.AddToGroup("PlayerTeam");
            generatedCharacter.SetupCharacter();
            generatedCharacter.Call("SetPosition", Tile);
        }

        public void SpawnEnemy(Resource Tile, BaseCharacter generatedCharacter)
        {
            BaseCharacter enemy = ResourceLoader.Load<PackedScene>("res://Assets/Units/enemy.tscn").Instantiate() as BaseCharacter;
            Level.AddChild(enemy, true);
            enemy.ReplaceBy(generatedCharacter);
            generatedCharacter.AddToGroup("Enemies");
            generatedCharacter.Name = generatedCharacter.CharacterName;
            generatedCharacter.SetupCharacter();
            enemy.QueueFree();

            //Godot.Collections.Array<Node> children = generatedCharacter.GetChildren();
            //for (int i = 0; i < children.Count; i++)
            //{
            //children[i].Owner = generatedCharacter;
            //}

            generatedCharacter.Call("SetPosition", Tile);
        }
        public Callable GetUpdateCharacterSignal()
        {
            return UpdateCharacterCall;
        }

        public void UpdateCurrentCharacter(BaseCharacter NextCharacter)
        {
            ActiveCharacter = NextCharacter;
        }

        public void OnLevelLoadingFinish()
        {

        }

        private void CharacterKilled(BaseCharacter character)
        {
            GD.Print("Enemy Has Been Killed: ", character.Name);
            if (character.GetCharacterTeam() == (int)BaseCharacter.CHARACTER_TEAM.enemy)
            {
                Enemy enemy = character as Enemy;
                EmitSignal(SignalName.AddCurrency, enemy.GetAmountOfCurrencyDroppedOnKill());
            }
            CharacterTurnController.Instance.RemoveCharacterFromTurnController(character);
            CharacterTurnController.Instance.RemoveUpdateCharacterMovementCallable(character.GetUpdateMovementCalculation());
            (GetTree().GetNodesInGroup("UnitControl")[0] as UnitController).ResetTileAfterCharacterDeath(character);
            CharacterTurnController.Instance.UpdateMovementQueue();
        }

    }
}
