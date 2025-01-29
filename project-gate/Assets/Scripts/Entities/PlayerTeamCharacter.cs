using Godot;
using System;

namespace ProjGate.TargetableEntities
{
  public partial class PlayerTeamCharacter : BaseCharacter
  {

    private GenericCharacterBanner m_CharacterBanner;
    public override void _Ready()
    {
      team = BaseCharacter.CHARACTER_TEAM.player;
    }

    public void SetCharacterBanner(GenericCharacterBanner CharacterBanner)
    {
      m_CharacterBanner = CharacterBanner;
      m_CharacterBanner.UpdateCharacterName(CharacterName);
      m_CharacterBanner.UpdateIconBanner(GetCharacterIcon());
      m_CharacterBanner.UpdateMovementRemaining(GetDistanceRemaining());
      m_CharacterBanner.UpdateHeapPriority(HeapPriority);
      m_CharacterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
    }

    public void UpdateCharacterBanner()
    {

    }
  }
}
