using System;
using System.Collections.Generic;
using System.Threading;
using ProjGate.Pickups;
namespace ProjGate.AssetLoading
{
  public static class LoadedAssetLibrary
  {
    public static class TypeDictionary
    {
      public static readonly string weapons = "BaseWeapon";
      public static readonly string grenades = "BaseGrenade";
      public static readonly string items = "BaseItems";
      public static readonly Dictionary<Type, string> ReflectTypeToString = new Dictionary<Type, String>(){
        [typeof(BaseWeapon)] = weapons,
      };

  }
  private static Mutex mut = new Mutex();
  static List<BaseWeapon> _loadedWeapons;
  static List<BaseGrenade> _loadedGrenades;
  static List<BaseItem> _loadedItems;

}
}
