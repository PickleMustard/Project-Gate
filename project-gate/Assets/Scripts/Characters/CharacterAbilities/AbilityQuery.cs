using System.Collections.Generic;

namespace ProjGate.Character
{
  public class AbilityQuery
  {
    public List<AbilityTag> RequiredTags;
    public List<AbilityTag> BannedTags;
    public string FuzzySearchName;
  }
}
