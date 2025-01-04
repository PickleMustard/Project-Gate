using Godot;
using System;

public partial class IncreaseDamageFromDistanceRemaining : Resource,IOnHitBehavior {
  void IOnHitBehavior.CalculateOnHit(Weapon attackingWeapon, Character Attacker, Resource TargetedLocation, Node TileGrid) {

    Character target = TargetedLocation.Call("GetCharacterOnTile").AsGodotObject() as Character;
    float proficiency = 1.0f / (int)Attacker.proficiencies[(int)attackingWeapon.weaponType];
    float distanceRemaining = (float)Attacker.distanceRemaining / (float)Attacker.TotalDistance;
    GD.Print("Distance Remaining Coefficient: ", distanceRemaining, "| " ,(float)Attacker.distanceRemaining, ", ", (float)Attacker.TotalDistance);
    target.RecieveDamage((int)Math.Round(attackingWeapon.BaseDamage * proficiency));
  }
}
