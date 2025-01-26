using System;
using System.Collections.Generic;
//using System.Threading.Mutex;
using System.Reflection;
using Godot;
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
      public static readonly Dictionary<Type, string> ReflectTypeToString = new Dictionary<Type, string>()
      {
        [typeof(BaseWeapon)] = weapons,
        [typeof(BaseGrenade)] = grenades,
        [typeof(BaseItem)] = items,
      };
      public static readonly Dictionary<string, Type> ReflectStringToType = new Dictionary<string, Type>()
      {
        [weapons] = typeof(BaseWeapon),
        [grenades] = typeof(BaseGrenade),
        [items] = typeof(BaseItem),
      };

    }

    private static System.Threading.Mutex mut = new System.Threading.Mutex();
    private static List<BaseWeapon> _loadedWeapons = new List<BaseWeapon>();
    private static List<BaseGrenade> _loadedGrenades = new List<BaseGrenade>();
    private static List<BaseItem> _loadedItems = new List<BaseItem>();

    //Load an asset into the LoadedAssetLibrary
    public static void CreateLoadedAsset(Type assetType, GodotObject loadedAsset)
    {
      GD.Print($"Asset to Load: {loadedAsset} of type {assetType}");
      mut.WaitOne();

      //Go through the Lists of the library, if the type matches, insert it into the list
      FieldInfo[] fields = typeof(LoadedAssetLibrary)?.GetFields(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic);
      foreach (FieldInfo field in fields)
      {
        if (IsList(field.FieldType) && ListType(field.FieldType) == assetType)
        {
          GD.Print(field?.GetValue(assetType));
          dynamic ListToAppendTo = field?.GetValue(null);
          ListToAppendTo.Add((dynamic)loadedAsset);
        }
      }

      mut.ReleaseMutex();
    }

    public static void ReadLoadedAssetLibraries()
    {
      GD.Print($"Number of Items in weapon list {_loadedWeapons.Count}");
      foreach (BaseWeapon weapon in _loadedWeapons)
      {
        GD.Print(weapon);
      }
    }

    //Check the type of a field, ensure it is a list
    static bool IsList(this Type type)
    {
      return type.IsGenericType && type.GetGenericTypeDefinition() == typeof(List<>);
    }

    //Check the generic type of the list
    static Type ListType(this Type type)
    {
      return type.GetGenericArguments()[0];
    }

  }
}
