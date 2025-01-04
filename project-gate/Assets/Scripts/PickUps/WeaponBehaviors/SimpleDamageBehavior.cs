using Godot;
using System;

public partial class SimpleDamageBehavior: Resource, IOnHitBehavior {
  void IOnHitBehavior.CalculateOnHit(Weapon attackingWeapon, Character Attacker, Resource targetedLocation, Node TileGrid) {
    Character target = targetedLocation.Call("GetCharacterOnTile").AsGodotObject() as Character;
    float proficiency = 1.0f / (int)Attacker.proficiencies[(int)attackingWeapon.weaponType];
    target.RecieveDamage((int)Math.Round(attackingWeapon.BaseDamage * proficiency));
  }
}
