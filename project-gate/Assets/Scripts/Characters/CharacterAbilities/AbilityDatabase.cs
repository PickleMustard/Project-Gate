using Godot;
using System;
using System.Linq;
using System.Collections.Generic;

namespace ProjGate.Character
{
  public static class AbilityDatabase
  {
    public static List<BaseAbility> AllAbilities;
    public static List<TaggedAbility> AllProceduralAbilities;

    public static List<BaseAbility> GetAbilitiesMatchingQuery(AbilityQuery query)
    {
      if (AllProceduralAbilities != null)
      {
        return AllProceduralAbilities
          .Where(a => a.Tags.All(tag => query.RequiredTags.Contains(tag)))
          .Where(a => !a.Tags.Any(tag => query.BannedTags.Contains(tag)))
          .Select(a => a.Ability)
          .ToList();
      } else {
        return new List<BaseAbility>();
      }
    }

    public static void InitializedAbilityList(List<TaggedAbility> abilities)
    {
      AllProceduralAbilities = new List<TaggedAbility>(abilities);
    }
  }

 }
