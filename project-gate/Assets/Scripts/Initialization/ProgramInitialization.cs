using Godot;

public partial class ProgramInitialization : Node {
  //Import Assets then move onto Main Menu
  public override void _Ready() {
    AssetLoader al = new AssetLoader();
    al.LoadBaseContent();
  }
}
