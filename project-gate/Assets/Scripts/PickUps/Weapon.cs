using Godot;

public partial class Weapon : Resource {
  [Export]
  public string WeaponName {get; private set;}
  [Export]
  public int MaxRange {get; private set;}
  [Export]
  public bool IgnoresLineSight {get; private set;}

  public void SetWeaponName(string name) {
    WeaponName = name;
  }

 public string GetWeaponName() {
   return WeaponName;
 }

 public int GetMaxRange() {
   return MaxRange;
 }

 public void SetMaxRange(int range) {
   MaxRange = range;
 }


 public override string ToString() {
   return "Weapon of name" + WeaponName;

 }

}
