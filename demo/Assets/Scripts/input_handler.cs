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

    private Vector2 mouse_position;
    private Vector2 moved_mouse_position;
    private bool panning = false;

    public override void _Input(InputEvent @event) {
        /*if(@event is InputEventMouseButton mouseEvent && mouseEvent.ButtonIndex == MouseButton.Left) {
            GD.Print("Left mouse was pressed");
            EmitSignal(SignalName.PickTile, mouse_position);
        } else*/ if(@event is InputEventMouseButton mouseEventMid && mouseEventMid.ButtonIndex == MouseButton.Middle) {
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
        } else if(@event is InputEventKey keyEvent && keyEvent.Keycode == Key.R) {
          EmitSignal(SignalName.RegenerateGrid);
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
