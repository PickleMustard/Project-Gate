using Godot;

public partial class SecondTestBehavior: Resource, IOnHitBehavior {
  void IOnHitBehavior.CalculateOnHit(int baseDamage, float proficiency, Character targer) {
    GD.Print("Second Test Behavior OnHit Method");

  }
}
