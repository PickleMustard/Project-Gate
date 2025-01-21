using Godot.Collections;
using System.Collections.Generic;
public static class AssetFileLocationDiscoverer
{
  public delegate Godot.Collections.Dictionary<string, List<string>> DiscoverFileLocationDelegate(GenericAssetLibrary library);

  public static Godot.Collections.Dictionary<string, List<string>> DiscoverFileLocation(GenericAssetLibrary library)
  {

  }
}
