using Godot;
using System.Collections;
using HCoroutines;

public partial class camera_movement : Node3D
{
    [Export]
    public float camera_movement_speed { get; set; } = 1.0f;

    [Export]
    public float camera_panning_speed { get; set; } = .01f;

    [Export]
    public float rotation_duration {get; set; } = 1.0f;

    private float angle = 121;
    private bool rotating = false;

    public override void _Ready() {
        input_handler i_handle = GetNode<Node>("/root/true_parent/input_handler") as input_handler;
        GD.Print(i_handle);
        i_handle.MovedCamera += moveCamera;
        i_handle.PannedCamera += panCamera;
        i_handle.RotatedCameraLeft += RotateCameraLeft;
        i_handle.RotatedCameraRight += RotateCameraRight;
        //Position = TileGrid.GetPositionForHexFromCoordinate(new Vector2I(500, 500), 1.0f, true) + new Vector3(0, 45, 0);
    }

    private void RotateCameraLeft() {
        Quaternion end_rotation = (new Quaternion(Vector3.Up, 2 *Mathf.DegToRad(30))) * this.Quaternion;
        end_rotation = end_rotation.Normalized();
        GD.Print($"Angle {angle}");
        if(!rotating) {
            rotating = true;
            angle = Mathf.Abs(angle) % 361 == 0 ? 61 : angle += 60;
            Co.Run(RotateCameraAsync(end_rotation));
        }
    }

    private void RotateCameraRight() {
        Quaternion end_rotation = (new Quaternion(Vector3.Up, 2 * Mathf.DegToRad(-30))) * this.Quaternion;
        end_rotation = end_rotation.Normalized();
        GD.Print($"Angle {angle}");
        if(!rotating) {
            rotating = true;
            angle = 1 % Mathf.Abs(angle) == 0 ? 301 : angle -= 60;
            Co.Run(RotateCameraAsync(end_rotation));
        }
    }

    private void moveCamera(Vector3 direction) {
        float xMovement = direction[0] * 2 * Mathf.Cos(Mathf.DegToRad(angle-121)) + direction[2] * 2 * -Mathf.Sin(Mathf.DegToRad(angle-121));
        float yMovement = direction[2] * 2 * Mathf.Cos(Mathf.DegToRad(angle-121)) + direction[0] * 2 * -Mathf.Sin(Mathf.DegToRad(angle-121));
        Vector3 cameraMovement = new Vector3(xMovement, 0, yMovement);
        Translate(cameraMovement.Normalized() * camera_movement_speed);
    }

    private void panCamera(Vector2 original_mouse_location, Vector2 moved_mouse_location) {
        Vector2 cameraOffset = original_mouse_location - moved_mouse_location;
        //float xMovement = cameraOffset[0] * Mathf.Cos(Mathf.DegToRad(angle)) + cameraOffset[1] * -Mathf.Sin(Mathf.DegToRad(angle));
        //float yMovement = cameraOffset[1] * Mathf.Cos(Mathf.DegToRad(angle)) + cameraOffset[0] * -Mathf.Sin(Mathf.DegToRad(angle));
        float xMovement = cameraOffset[0];
        float yMovement = cameraOffset[1];
        Vector3 cameraMovement = new Vector3(xMovement, 0, yMovement);
        Translate(cameraMovement * camera_panning_speed);
    }

     IEnumerator RotateCameraAsync(Quaternion end_rotation) {
        float timeElapsed = 0.0f;
        Quaternion start_rotation = this.Quaternion;
        GD.Print("Starting Rotation");
        do {
            float lerpStep = timeElapsed / rotation_duration;
            this.Quaternion = Quaternion.Slerp(end_rotation, lerpStep);
            timeElapsed += Co.DeltaTime;
            yield return null;
        } while (timeElapsed < rotation_duration && Quaternion != end_rotation);
        GD.Print("Finished Rotation");
        Quaternion = end_rotation;
        rotating = false;
    }


}
