using Godot;

public partial class Weapon : Resource {
  private string weapon_name;

  public void SetWeaponName(string name) {
    weapon_name = name;
  }

 public string GetWeaponName() {
   return weapon_name;
 }

 public override string ToString() {
   return "Weapon of name" + weapon_name;

 }

}
