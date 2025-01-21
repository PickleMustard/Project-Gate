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
      public static readonly Dictionary<Type, string> ReflectTypeToString = new Dictionary<Type, String>()
      {
        [typeof(BaseWeapon)] = weapons,
      };

    }
    private static System.Threading.Mutex mut = new System.Threading.Mutex();
    static List<BaseWeapon> _loadedWeapons;
    static List<BaseGrenade> _loadedGrenades;
    static List<BaseItem> _loadedItems;

    //Load an asset into the LoadedAssetLibrary
    static void CreateLoadedAsset(Type assetType, GodotObject loadedAsset)
    {
      mut.WaitOne();

      //Go through the Lists of the library, if the type matches, insert it into the list
      FieldInfo[] fields = typeof(LoadedAssetLibrary).GetFields(BindingFlags.Static | BindingFlags.Public);
      foreach (FieldInfo field in fields)
      {
        if (IsList(field.FieldType) && ListType(field.FieldType) == assetType)
        {
          dynamic ListToAppendTo = typeof(LoadedAssetLibrary)?.GetField(field.Name)?.GetValue(assetType);
          ListToAppendTo.Add(loadedAsset);
        }
      }

      mut.ReleaseMutex();
    }

    public static void ReadLoadedAssetLibraries()
    {
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
