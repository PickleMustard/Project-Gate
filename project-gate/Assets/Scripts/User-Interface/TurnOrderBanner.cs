using Godot;
using Godot.Collections;

public partial class TurnOrderBanner : Control
{
  public const int ICONS_IN_BANNER = 7;

  public override void _EnterTree()
  {
    AddToGroup("TurnOrderBanner");
  }

  public void UpdateTurnOrderBanner(Texture2D[] BannerGroup)
  {
    Array<Node> children = this.GetChildren();
    foreach (Node child in children)
    {
      child.QueueFree();
    }
    foreach (Texture2D c in BannerGroup)
    {
      TextureRect icon = new TextureRect();
      icon.Texture = c;
      icon.CustomMinimumSize = new Vector2(50,50);
      icon.ExpandMode = TextureRect.ExpandModeEnum.IgnoreSize;
      icon.StretchMode = TextureRect.StretchModeEnum.KeepAspectCentered;
      this.AddChild(icon);
    }
  }

}
