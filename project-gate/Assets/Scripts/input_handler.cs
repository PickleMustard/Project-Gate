using Godot;
using System;

public partial class input_handler : Node
{
    [Signal]
    public delegate void MovedCameraEventHandler(Vector3 direction);

    [Signal]
    public delegate void PannedCameraEventHandler(Vector2 original_mouse_position, Vector2 moved_mouse_position);

    [Signal]
    public delegate void PickTileEventHandler(Vector2 mouse_screen_position);

    [Signal]
    public delegate void RotatedCameraLeftEventHandler();

    [Signal]
    public delegate void RotatedCameraRightEventHandler();

    [Signal]
    public delegate void ScrollCameraCloserEventHandler();

    [Signal]
    public delegate void ScrollCameraFurtherEventHandler();

    [Signal]
    public delegate void IncreaseScrollSpeedEventHandler();

    [Signal]
    public delegate void DecreaseScrollSpeedEventHandler();

    private Vector2 mouse_position;
    private Vector2 moved_mouse_position;
    private bool panning = false;
    private Node Level;
    private Godot.Collections.Dictionary RegenerateGrid;

    public override void _Ready() {
      Level = GetNode<Node>("/root/Top/Level");
      GD.Print(Level.ToString());
      RegenerateGrid = Level.GetSignalList()[0];
      GD.Print(Level.GetSignalConnectionList(RegenerateGrid["name"].ToString()));
      GD.Print(RegenerateGrid);

    }

    public override void _Input(InputEvent @event) {
        if(@event is InputEventMouseButton mouseEventMid && mouseEventMid.ButtonIndex == MouseButton.Middle) {
            if(!panning && mouseEventMid.Pressed) {
            panning = true;
            mouse_position = GetViewport().GetMousePosition();
            }
            if( panning && !mouseEventMid.Pressed) {
                panning = false;
                mouse_position = Vector2.Zero;
            }
        } else if(@event is InputEventMouseMotion motionEvent && panning) {
            moved_mouse_position = GetViewport().GetMousePosition();
            EmitSignal(SignalName.PannedCamera, mouse_position, moved_mouse_position);
            mouse_position = moved_mouse_position;
        } else if(@event is InputEventKey keyEvent && keyEvent.Pressed && keyEvent.Keycode == Key.R) {
          Level.EmitSignal(RegenerateGrid["name"].ToString());
        } else if(@event is InputEventKey printEvent && printEvent.Keycode == Key.P) {
          PrintOrphanNodes();
        } else if(@event is InputEventMouseButton mouseEventScrollin && mouseEventScrollin.ButtonIndex == MouseButton.WheelUp ) {
          EmitSignal(SignalName.ScrollCameraCloser);
        } else if(@event is InputEventMouseButton mouseEventScrollout && mouseEventScrollout.ButtonIndex == MouseButton.WheelDown ) {
          EmitSignal(SignalName.ScrollCameraFurther);
        } else if(@event is InputEventKey upSpeed && upSpeed.Pressed && upSpeed.Keycode == Key.Shift) {
          EmitSignal(SignalName.IncreaseScrollSpeed);
        } else if(@event is InputEventKey downSpeed && !downSpeed.IsPressed() && downSpeed.Keycode == Key.Shift) {
          EmitSignal(SignalName.DecreaseScrollSpeed);
        }

    }

    public override void _PhysicsProcess(double delta) {
        Vector3 direction = Vector3.Zero;
        bool shouldMoveCamera = false;



        if(Input.IsActionPressed("move_right")) {
            direction.X += 1.0f;
            shouldMoveCamera = true;
        }
        if(Input.IsActionPressed("move_left")) {
            direction.X -= 1.0f;
            shouldMoveCamera = true;
        }
        if(Input.IsActionPressed("move_back")) {
            direction.Z += 1.0f;
            shouldMoveCamera = true;
        }
        if(Input.IsActionPressed("move_forward")) {
            direction.Z -= 1.0f;
            shouldMoveCamera = true;
        }
        if(Input.IsActionPressed("rotate_left")) {
            EmitSignal(SignalName.RotatedCameraLeft);
        }
        if(Input.IsActionPressed("rotate_right")) {
            EmitSignal(SignalName.RotatedCameraRight);
        }

        if(shouldMoveCamera) {
            EmitSignal(SignalName.MovedCamera, direction);
            direction = Vector3.Zero;
            shouldMoveCamera = false;
        }
    }
}
