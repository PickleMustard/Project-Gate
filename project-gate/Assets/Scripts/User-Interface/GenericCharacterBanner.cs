using Godot;
using System;

public partial class GenericCharacterBanner : Control
{
  private Texture2D m_UnitIcon;
  private String m_CharacterName;
  private int m_MovementRemaining;
  private float m_HeapPriority;

  private Label m_CharacterInfo;
  private TextureRect m_UnitIconDisplay;

  Callable m_UpdateMovementRemainingCall;
  Callable m_UpdateHeapPriorityCall;

  public override void _EnterTree()
  {
    m_UpdateMovementRemainingCall = new Callable(this, "UpdateMovementRemaining");
    m_UpdateHeapPriorityCall = new Callable(this, "UpdateHeapPriority");
    m_UnitIconDisplay = (TextureRect)GetChildren()[0].GetChildren()[0];
    m_CharacterInfo = (Label)GetChildren()[0].GetChildren()[1];
    m_CharacterInfo.Text = GenerateCharacterBannerText();

    m_UnitIconDisplay.Texture = m_UnitIcon;
  }

  public void UpdateIconBanner(Texture2D UnitIcon)
  {
    this.m_UnitIcon = UnitIcon;
    m_UnitIconDisplay.Texture = m_UnitIcon;
  }
  public void UpdateMovementRemaining(int NewValue)
  {
    m_MovementRemaining = NewValue;
    m_CharacterInfo.Text = GenerateCharacterBannerText();
  }

  public void UpdateHeapPriority(float NewValue)
  {
    m_HeapPriority = NewValue;
    m_CharacterInfo.Text = GenerateCharacterBannerText();
  }

  public void UpdateCharacterName(String NewName)
  {
    m_CharacterName = NewName;
    m_CharacterInfo.Text = GenerateCharacterBannerText();
  }

  public Callable GetUpdateMovementCallable()
  {
    return m_UpdateMovementRemainingCall;
  }

  public Callable GetUpdateHeapPriorityCallable()
  {
    return m_UpdateHeapPriorityCall;
  }

  private String GenerateCharacterBannerText()
  {
    return "Character: " + m_CharacterName + "\nMovement Remaining: " + m_MovementRemaining + "\nHeap Priority: " + m_HeapPriority;
  }
}
