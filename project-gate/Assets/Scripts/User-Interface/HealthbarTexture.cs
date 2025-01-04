using System;
using Godot;

public partial class HealthbarTexture : TextureProgressBar {
  float totalHealth;
  float currentHealth;

  public void SetupHealth(int totalHealth) {
    this.totalHealth = totalHealth;
    currentHealth = totalHealth;
    Value = currentHealth / totalHealth * 100.0f;
  }
  public void UpdateHealth(int newHealth) {
    currentHealth = newHealth;
    Value = Math.Round((currentHealth / totalHealth) * 100.0f);
    GD.Print("CurrentHealth: ", currentHealth, "| Max Health: ", totalHealth, "| Value", Value);
  }
}
