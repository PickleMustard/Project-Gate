using System;
using ProjGate.AssetLoading;
using Godot;

public partial class ProgramInitialization : Node
{
  AssetLoader al;

  //Import Assets then move onto Main Menu
  public override void _Ready()
  {
    al = new AssetLoader();
    al.LoadingCompleted += OnLoadingFinished;
    al.StartLoading();
  }

  public override void _PhysicsProcess(double delta)
  {
    if (al.IsLoading)
    {
      GD.Print("Loading....");
    }
    if (al.IsCompleted)
    {
      if (al.IsFaulted)
      {
        GD.Print($"Load failed: {al.Exception.Message}");
      }
    }
  }

  public void OnLoadingFinished(Exception error)
  {
    if (error != null)
    {
      GD.Print($"Error Loading: {error.Message}");
    }
    else
    {
      GD.Print("Loading Completed without error");
      LoadedAssetLibrary.ReadLoadedAssetLibraries();
    }
  }
}
