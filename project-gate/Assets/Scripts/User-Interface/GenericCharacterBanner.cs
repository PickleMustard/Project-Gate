using Godot;
using System;

public partial class GenericCharacterBanner : Control
{
  Texture2D unitIcon;
  String characterName;
  int movementRemaining;
  float heapPriority;

  Label CharacterInfo;
  TextureRect UnitIconDisplay;

  Callable updateMovementRemainingCall;
  Callable UpdateHeapPriorityCall;

  public override void _EnterTree() {
    updateMovementRemainingCall = new Callable(this, "UpdateMovementRemaining");
    UpdateHeapPriorityCall = new Callable(this, "UpdateHeapPriority");
    UnitIconDisplay = (TextureRect)GetChildren()[0].GetChildren()[0];
    CharacterInfo = (Label)GetChildren()[0].GetChildren()[1];
    CharacterInfo.Text = GenerateCharacterBannerText();

  }
  public void UpdateMovementRemaining(int newValue) {
    movementRemaining = newValue;
    CharacterInfo.Text = GenerateCharacterBannerText();
  }

  public void UpdateHeapPriority(float newValue) {
    heapPriority = newValue;
    CharacterInfo.Text = GenerateCharacterBannerText();
  }

  public void UpdateCharacterName(String newName) {
    characterName = newName;
    CharacterInfo.Text = GenerateCharacterBannerText();
  }

  public Callable GetUpdateMovementCallable() {
    return updateMovementRemainingCall;
  }

  public Callable GetUpdateHeapPriorityCallable() {
    return UpdateHeapPriorityCall;
  }

  private String GenerateCharacterBannerText() {
    return "Character: " + characterName + "\nMovement Remaining: " + movementRemaining + "\nHeap Priority: " + heapPriority;
  }
}
