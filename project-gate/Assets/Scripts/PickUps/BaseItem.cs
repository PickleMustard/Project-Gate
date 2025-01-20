using Godot;

namespace ProjGate.Pickups
{
  [GlobalClass]
  public class BaseItem : Resource
  {
    private int _index;
    public string ItemName;
    public Texture2D ItemIcon;
  }
}
