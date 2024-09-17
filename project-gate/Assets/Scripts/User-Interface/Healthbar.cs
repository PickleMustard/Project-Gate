using Godot;
using System;

public partial class Healthbar : Sprite3D
{
  public override void _Ready() {
    SubViewport character_health_bar = (SubViewport)GetChildren()[0];
    Texture = character_health_bar.GetTexture();
  }

  public void SetupHealthBar(int totalHealth) {
    HealthbarTexture text = GetChildren()[0].GetChildren()[0] as HealthbarTexture;
    text.SetupHealth(totalHealth);
  }

  public void UpdateHealthBar(int newHealth) {
    HealthbarTexture text = GetChildren()[0].GetChildren()[0] as HealthbarTexture;
    text.UpdateHealth(newHealth);
  }
}
