using System.Collections.Generic;

namespace ProjGate.TargetableEntities
{
  public class AbilityQuery
  {
    public List<AbilityTag> RequiredTags;
    public List<AbilityTag> BannedTags;
    public string FuzzySearchName;
  }
}
