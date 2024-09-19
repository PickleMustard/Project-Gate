using Godot;
using System;

public partial class SimpleDamageBehavior: Resource, IOnHitBehavior {
  void IOnHitBehavior.CalculateOnHit(int baseDamage, Character target) {
    GD.Print("I am calculating the hit on the opponent!");
    target.RecieveDamage(baseDamage);
  }
}
