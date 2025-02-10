using Godot;
using System.Collections.Generic;
using System.Linq;
/// <summary>
/// <class> AssetFileLocationDiscoverer </class>
/// </summary>
public static class AssetFileLocationDiscoverer
{

  private const string INDEX_FILE_NAME = "index.yml";
  /* Function to discover all the index.yml's in a content pack or Mod
   *
   * Given a the file address of Content-Pack, Mod, or Assets if in TOOL
   * Runs through all the subdirectories and finds the index.yml
   * Adds the absolute location to the list of paths
   * Returns List of Absolute paths
   */
  public static List<string> DiscoverFileLocation(string BaseContentPackDirectoryPath)
  {
    List<string> indexPaths = new List<string>();
    DirAccess baseDirectory = DirAccess.Open(BaseContentPackDirectoryPath);
    string[] subdirectories = baseDirectory.GetDirectories();
    foreach (string dir in subdirectories)
    {
      indexPaths.AddRange(RecursivelyDiveDirectory(BaseContentPackDirectoryPath + "/" + dir));
    }

    return indexPaths;


  }

  private static List<string> RecursivelyDiveDirectory(string DirectoryPath)
  {
    DirAccess directory = DirAccess.Open(DirectoryPath);
    List<string> absolutePaths = new List<string>();
    List<string> files = directory.GetFiles().ToList();
    if (files.Contains(INDEX_FILE_NAME))
    {
      GD.Print("Contains");
      absolutePaths.Add(directory.GetCurrentDir() + "/" + INDEX_FILE_NAME);
    }
    if (directory.GetDirectories().Length > 0)
    {
      string[] subdirectories = directory.GetDirectories();
      foreach (string dir in subdirectories)
      {
        absolutePaths.AddRange(RecursivelyDiveDirectory(directory.GetCurrentDir() + "/" + dir));
      }
    }
    return absolutePaths;

  }
}
