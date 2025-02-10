using Godot;
using System.Collections;
using HCoroutines;

using ProjGate.Services;

public partial class camera_movement : Node3D
{
  [Export]
  public float camera_movement_speed { get; set; } = 1.2f;

  [Export]
  public float camera_panning_speed { get; set; } = .03f;

  [Export]
  public float rotation_duration { get; set; } = 0.8f;

  [Export]
  public float camera_movement_multiplier { get; set; } = 1.0f;

  [Export]
  public int current_camera_position = 0;

  private float angle = 121;
  private bool rotating = false;
  private bool scrolling = false;
  private Camera3D camera;


  private Basis[] camera_angles = {new Basis(new Vector3(1, 0, 0), new Vector3(0, 0.83867f, -0.544639f), new Vector3(0, 0.544639f, 0.83867f)),
    new Basis(new Vector3(1, 0, 0), new Vector3(0, 0.766044f, -0.642788f), new Vector3(0, 0.642788f, 0.76044f)),
                                     new Basis(new Vector3(1, 0, 0), new Vector3(0, 0.707107f, -0.707107f), new Vector3(0, 0.707107f, 0.707107f)),
                                      new Basis(new Vector3(1, 0, 0), new Vector3(0, 0.5f, -0.866025f), new Vector3(0, 0.866025f, 0.5f)),
   new Basis(new Vector3(1, 0, 0), new Vector3(0, 0.375f, -0.927f), new Vector3(0, 0.927f, 0.375f)),
   new Basis(new Vector3(1, 0, 0), new Vector3(0, 0.174f, -0.985f), new Vector3(0, 0.985f, 0.174f))};
  private Vector3[] camera_positions = { new Vector3(0, -15, 0), new Vector3(0, -7, 0), new Vector3(0, 0, 0), new Vector3(0, 10, 0), new Vector3(0, 18, 0), new Vector3(0, 24, 0) };
  private int[] camera_FOV = { 70, 70, 70, 80, 90, 110 };
  public override void _Ready()
  {
    InputHandler i_handle = GetTree().GetNodesInGroup("InputHandler")[0] as InputHandler;
    camera = GetNode<Camera3D>("Camera");
    //GD.Print("Test");
    //GD.Print(i_handle.GetSignalList());
    //GD.Print("Test");
    i_handle.MovedCamera += moveCamera;
    i_handle.PannedCamera += panCamera;
    i_handle.RotatedCameraLeft += RotateCameraLeft;
    i_handle.RotatedCameraRight += RotateCameraRight;
    i_handle.ScrollCameraFurther += ScrollCameraOut;
    i_handle.ScrollCameraCloser += ScrollCameraIn;
    i_handle.IncreaseScrollSpeed += IncreaseCameraSpeed;
    i_handle.DecreaseScrollSpeed += DecreaseCameraSpeed;
    camera.Position = camera_positions[current_camera_position];
    camera.Basis = camera_angles[current_camera_position];
    camera.Fov = camera_FOV[current_camera_position];
  }

  private void IncreaseCameraSpeed()
  {
    GD.Print("Increasing Speed");
    camera_movement_multiplier *= 2;
  }

  private void DecreaseCameraSpeed()
  {
    GD.Print("Decreasing");
    camera_movement_multiplier /= 2;
  }

  private void ScrollCameraIn()
  {
    /*Quaternion end_rotation = (new Quaternion(Vector3.Right, 2 * Mathf.DegToRad(-10))) * camera.Quaternion;
    Vector3 end_position = (new Vector3(0, -15, 0)) + camera.Position;
    GD.Print(end_position, " | ", camera.Position);
    end_rotation = end_rotation.Normalized();
    //end_position = end_position.Normalized();
    //camera.Position = Position.Normalized();
    if(!scrolling) {
      scrolling = true;
      Co.Run(ScrollCameraAsync(end_rotation, end_position));
    } */
    //camera.Basis = camera_angles[0];
    //camera.Position = camera.Position - new Vector3(0, 15, 0);
    GD.Print("Basis: ", camera.Basis);
    GD.Print(camera.Quaternion);
    GD.Print(camera.Position);
    GD.Print(camera.Rotation);
    if (current_camera_position > 0)
    {
      if (!scrolling)
      {
        current_camera_position--;
        scrolling = true;
        Co.Run(ScrollCameraAsync(camera_angles[current_camera_position], camera_positions[current_camera_position], camera_FOV[current_camera_position]));
      }
    }
  }

  private void ScrollCameraOut()
  {

    if (current_camera_position < camera_angles.Length - 1)
    {
      if (!scrolling)
      {
        current_camera_position++;
        scrolling = true;
        Co.Run(ScrollCameraAsync(camera_angles[current_camera_position], camera_positions[current_camera_position], camera_FOV[current_camera_position]));
      }
    }
    // camera.Basis = camera_angles[1];
    // camera.Position = camera.Position + new Vector3(0, 15, 0);
  }

  private void RotateCameraLeft()
  {
    Quaternion end_rotation = (new Quaternion(Vector3.Up, 2 * Mathf.DegToRad(30))) * this.Quaternion;
    end_rotation = end_rotation.Normalized();
    GD.Print($"Angle {angle}");
    if (!rotating)
    {
      rotating = true;
      angle = Mathf.Abs(angle) % 361 == 0 ? 61 : angle += 60;
      Co.Run(RotateCameraAsync(end_rotation));
    }
  }

  private void RotateCameraRight()
  {
    Quaternion end_rotation = (new Quaternion(Vector3.Up, 2 * Mathf.DegToRad(-30))) * this.Quaternion;
    end_rotation = end_rotation.Normalized();
    GD.Print($"Angle {angle}");
    if (!rotating)
    {
      rotating = true;
      angle = 1 % Mathf.Abs(angle) == 0 ? 301 : angle -= 60;
      Co.Run(RotateCameraAsync(end_rotation));
    }
  }

  private void moveCamera(Vector3 direction)
  {
    Vector3 cameraMovement = new Vector3(direction[0], 0, direction[2]);
    Translate(cameraMovement.Normalized() * camera_movement_speed * camera_movement_multiplier);
  }

  private void panCamera(Vector2 original_mouse_location, Vector2 moved_mouse_location)
  {
    Vector2 cameraOffset = original_mouse_location - moved_mouse_location;
    //float xMovement = cameraOffset[0] * Mathf.Cos(Mathf.DegToRad(angle)) + cameraOffset[1] * -Mathf.Sin(Mathf.DegToRad(angle));
    //float yMovement = cameraOffset[1] * Mathf.Cos(Mathf.DegToRad(angle)) + cameraOffset[0] * -Mathf.Sin(Mathf.DegToRad(angle));
    float xMovement = cameraOffset[0];
    float yMovement = cameraOffset[1];
    Vector3 cameraMovement = new Vector3(xMovement, 0, yMovement);
    Translate(cameraMovement * camera_panning_speed);
  }

  IEnumerator RotateCameraAsync(Quaternion end_rotation)
  {
    float timeElapsed = 0.0f;
    Quaternion start_rotation = this.Quaternion;
    GD.Print("Starting Rotation");
    do
    {
      float lerpStep = timeElapsed / rotation_duration;
      this.Quaternion = Quaternion.Slerp(end_rotation, lerpStep);
      timeElapsed += Co.DeltaTime;
      yield return null;
    } while (timeElapsed < rotation_duration && Quaternion != end_rotation);
    GD.Print("Finished Rotation");
    Quaternion = end_rotation;
    rotating = false;
  }

  IEnumerator ScrollCameraAsync(Basis end_rotation, Vector3 end_position, int end_fov)
  {
    float timeElapsed = 0.0f;
    Quaternion start_rotation = camera.Quaternion;
    Vector3 start_position = camera.Position;
    camera.Basis = camera.Basis.Orthonormalized();
    end_rotation = end_rotation.Orthonormalized();
    GD.Print("Starting scroll");
    do
    {
      float lerpStep = timeElapsed / rotation_duration;
      camera.Basis = camera.Basis.Slerp(end_rotation, lerpStep);
      camera.Position = camera.Position.Lerp(end_position, lerpStep);
      camera.Fov = (1 - lerpStep) * camera.Fov + lerpStep * end_fov;
      timeElapsed += Co.DeltaTime;
      yield return null;
    } while (timeElapsed < rotation_duration && camera.Basis != end_rotation);
    GD.Print("Finished Rotation");
    camera.Basis = end_rotation;
    camera.Position = end_position;
    camera.Fov = end_fov;
    scrolling = false;
  }

}
