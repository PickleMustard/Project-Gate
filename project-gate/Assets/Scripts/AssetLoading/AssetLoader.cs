using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Godot;
using Godot.Collections;

namespace ProjGate.AssetLoading
{

  /// <summary>
  /// <class> AssetLoader </class> is responsible for loading all assets into memory upon program initialization;
  /// </summary>
  public class AssetLoader
  {
    private CancellationTokenSource _cts;
    private Task _loadingTask;
    private Exception _loadingException;
    private object _exceptionLock = new object();
    private GodotObject _yamlParser;

    public bool IsLoading => _loadingTask?.Status == TaskStatus.Running;
    public bool IsCompleted => _loadingTask?.IsCompleted ?? false;
    public bool IsFaulted => _loadingTask?.IsFaulted ?? false;
    public Exception Exception => _loadingException;

    public event Action<Exception> LoadingCompleted;

    private static DirAccess GetBaseDirectory()
    {
      DirAccess topDirectory = DirAccess.Open("res://");
#if TOOLS
      GD.Print("Tools");
      topDirectory.ChangeDir("Assets");
#else
      topDirectory.ChangeDir("Content-Packs");
      string baseDirectory = topDirectory.GetCurrentDir() + "Content-Packs/";
#endif
      return topDirectory;
    }

    public void StartLoading()
    {
      _cts?.Cancel();
      _cts = new CancellationTokenSource();
      DirAccess directory = GetBaseDirectory();
      List<string> indexLocation = AssetFileLocationDiscoverer.DiscoverFileLocation(directory.GetCurrentDir());
      _yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();

      _loadingTask = Task.Run(async () =>
      {
        try
        {
          await InternalLoadAssetsAsync(indexLocation, _cts.Token);
          LoadingCompleted?.Invoke(null);
        }
        catch (Exception ex)
        {
          lock (_exceptionLock)
          {
            _loadingException = ex;
          }
          LoadingCompleted?.Invoke(ex);
        }
      }, _cts.Token);

    }

    public void CancelLoading()
    {
      _cts?.Cancel();
    }

    private async Task InternalLoadAssetsAsync(List<string> indexPaths, CancellationToken ct)
    {
      List<Task> tasks = new List<Task>();
      object exceptionLock = new object();
      Exception firstException = null;

      foreach (string indexPath in indexPaths)
      {
        tasks.Add(Task.Run(async () =>
        {
          try
          {
            await ProcessIndexAsync(indexPath, ct);
          }
          catch (Exception ex) when (ex is not OperationCanceledException)
          {
            lock (exceptionLock)
            {
              if (firstException == null)
              {
                firstException = new Exception($"Loading failed at {indexPath}", ex);
                _cts.Cancel();
              }
            }
            throw;
          }
        }, ct));
      }
      try
      {
        await Task.WhenAll(tasks);
      }
      catch
      {
        if (firstException != null)
        {
          throw firstException;
        }
        throw;
      }
    }

    private async Task ProcessIndexAsync(string indexPath, CancellationToken ct)
    {
      ct.ThrowIfCancellationRequested();
      string directoryPath = indexPath.Split("index")[0];
      DirAccess indexDirectory = DirAccess.Open(directoryPath);
      Dictionary parsedData = (Dictionary)_yamlParser.Call("parse_file", indexPath);
      Godot.Collections.Array definitions = (Godot.Collections.Array)parsedData["definitions"];
      foreach (Dictionary indexItem in definitions)
      {
        string type = (string)indexItem["type"];
        if (type == "resource")
        {
          string assetType = (string)indexItem["asset_type"];
          string filePath = directoryPath + indexItem["file"];
          GD.Print(string.Format("File Exists: {0}, {1}", filePath, FileAccess.FileExists(filePath)));
          if (!FileAccess.FileExists(directoryPath + indexItem["file"]))
          {
            GD.Print("File doesn't exist");
            throw new Exception($"Unable to find file {indexItem["file"]}");
          }
          try {
          LoadedAssetLibrary.CreateLoadedAsset(LoadedAssetLibrary.TypeDictionary.ReflectStringToType[assetType],
              ResourceLoader.Load(filePath, null, ResourceLoader.CacheMode.IgnoreDeep));
          } catch (Exception ex) {
            GD.Print($"Encountered exception {ex.Message}");
          }
        }
      }

    }

  }
}
