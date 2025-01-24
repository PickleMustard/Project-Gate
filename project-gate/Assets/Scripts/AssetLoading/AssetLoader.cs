using System.Collections.Generic;
using Godot.Collections;
using System.Linq;
using Godot;
public class AssetLoader
{

  /* TODO:
   * The basegame loader needs to navigate through the repository to the Content-Packs directory
   * From there, it needs to parse through each of the content packs to load the values into the game
   *
   * Currently, grab the current directory and work to the 1st Content pack
   */
  public void LoadBaseContent()
  {
    DirAccess workingDirectory = DirAccess.Open("res://");
    string[] dirs = workingDirectory.GetDirectories();
    foreach (string d in dirs)
    {
      GD.Print(d);
    }
#if TOOLS
    GD.Print("Tools");
    workingDirectory.ChangeDir("Assets");
    string BaseDirectory = workingDirectory.GetCurrentDir();
    GD.Print("Base Directory: " + BaseDirectory);
#else
    string BaseDirectory = workingDirectory.GetCurrentDir() + "Content-Packs/";
#endif
    AssetFileLocationDiscoverer.DiscoverFileLocation(BaseDirectory);

    /*string testReadWeapons = BaseDirectory + "/Configured-Assets/Weapons/";

    GodotObject yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();
    string toLoad = testReadWeapons + "index.yml";
    GD.Print(toLoad);
    Dictionary parsedData = (Dictionary)yamlParser.Call("parse_file", toLoad);

    GD.Print(parsedData);
    Array definitions = (Array)parsedData["definitions"];
    GD.Print(definitions[0]);
    Dictionary indexItem = (Dictionary)definitions[0];
    GD.Print(string.Format("File Exists: {0}, {1}", testReadWeapons + indexItem["file"], FileAccess.FileExists(testReadWeapons + indexItem["file"])));
    */
  }
}
