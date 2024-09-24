using Godot;
using Godot.Collections;

public partial class EnemyGenerator : Resource
{
  private const string CONFIGURATION_BASE_PATH = "res://Configuration/";
  private const string ENEMY_CONFIGURATION_PATH = "res://Configuration/Enemies/";

  public Character GenerateEnemy(string configurationPath)
  {
    Enemy enemy = new Enemy();

    GD.Print(ClassDB.ClassExists("AttackEnemyAction"));
    Resource behavior = (Resource)ClassDB.Instantiate("AttackEnemyAction");

    return enemy;

  }
}
