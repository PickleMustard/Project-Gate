using Godot;
using System.Collections.Generic;

public struct Face {
    public List<Vector3> vertices {get; private set;}
    public List<int> indices {get; private set;}
    public List<Vector2> uvs{ get; private set;}
    public List<Vector3> normals { get; private set;}

    public Face(List<Vector3> vertices, List<Vector3> normals, List<int> indices, List<Vector2> uvs) {
        this.vertices = vertices;
        this.indices = indices;
        this.uvs = uvs;
        this.normals = normals;
    }
}

public partial class HexagonalTileRenderer : MeshInstance3D
{
    Godot.Collections.Array surfaceArray;
    private List<Face> m_faces;

    public float innerSize;
    public float outerSize;
    public float height;
    public bool isFlatTopped;

    public HexagonalTileRenderer() {
        this.innerSize = 0.0f;
        this.outerSize = 1.0f;
        this.height = 1.0f;
        this.isFlatTopped = false;
    }

    public HexagonalTileRenderer(float innerSize, float outerSize, float height, bool isFlatTopped) {
        this.innerSize = innerSize;
        this.outerSize = outerSize;
        this.height = height;
        this.isFlatTopped = isFlatTopped;
    }

    private void DrawFaces() {
        m_faces = new List<Face>();

        for(int point = 0; point < 6; point++) {
            m_faces.Add(CreateFace(innerSize, outerSize, height / 2f, height / 2f, point));
        }

        for(int point = 0; point < 6; point++) {
            m_faces.Add(CreateFace(innerSize, outerSize, -height / 2f, -height / 2f, point));
        }


        for(int point = 0; point < 6; point++) {
            m_faces.Add(CreateFace(outerSize, outerSize, height / 2f, -height / 2f, point));
        }

        for(int point = 0; point < 6; point++) {
            m_faces.Add(CreateFace(innerSize, innerSize, -height / 2f, height / 2f, point));
        }
    }

    private void CombineFaces() {
        List<Vector3> vertices = new List<Vector3>();
        List<int> indices = new List<int>();
        List<Vector2> uvs = new List<Vector2>();
        List<Vector3> normals = new List<Vector3>();

        for (int i = 0; i < m_faces.Count; i++) {
            vertices.AddRange(m_faces[i].vertices);
            normals.AddRange(m_faces[i].normals);
            uvs.AddRange(m_faces[i].uvs);

            int offset = (4 * i);
            foreach (int index in m_faces[i].indices) {
                indices.Add(index + offset);
            }
        }

        surfaceArray[(int)Mesh.ArrayType.Vertex] = vertices.ToArray();
        surfaceArray[(int)Mesh.ArrayType.TexUV] = uvs.ToArray();
        surfaceArray[(int)Mesh.ArrayType.Normal] = normals.ToArray();
        surfaceArray[(int)Mesh.ArrayType.Index] = indices.ToArray();
    }


    private Face CreateFace(float innerRadius, float outerRadius, float heightA, float heightB, int point, bool reverse = false) {
        Vector3 pointA = GetPoint(innerRadius, heightA, point);
        Vector3 pointB = GetPoint(innerRadius, heightA, (point < 5) ? point + 1 : 0);
        Vector3 pointC = GetPoint(outerRadius, heightB, (point < 5) ? point + 1 : 0);
        Vector3 pointD = GetPoint(outerRadius, heightB, point);

        List<Vector3> vertices = new List<Vector3>() { pointA, pointB, pointC, pointD};
        List<Vector3> normals = new List<Vector3>() { pointA.Normalized(), pointB.Normalized(), pointC.Normalized(), pointD.Normalized() };
        List<int> indices = new List<int>() {2,1,0,0,3,2};
        List<Vector2> uvs = new List<Vector2>() {new Vector2(0,0), new Vector2(1,0), new Vector2(1,1), new Vector2(0,1)};

        if(reverse) {
            vertices.Reverse();
        }

        return new Face(vertices, normals, indices, uvs);
    }

    private Vector3 GetPoint(float size, float height, int index) {
        float angle_deg = isFlatTopped ? 60 * index : 60 * index - 30;
        float angle_rad = Mathf.Pi / 180f * angle_deg;
        return new Vector3((size * Mathf.Cos(angle_rad)), height ,size * Mathf.Sin(angle_rad));
    }

	public void DrawMesh() {
        if(!ResourceLoader.Exists("res://sphere.tres")) {
            surfaceArray = new Godot.Collections.Array();
            surfaceArray.Resize((int)Mesh.ArrayType.Max);

            DrawFaces();
            CombineFaces();


            ArrayMesh arrMesh = new ArrayMesh();
            GD.Print(arrMesh);
            if(arrMesh != null) {
                GD.Print("Adding surfaces");
                arrMesh.AddSurfaceFromArrays(Mesh.PrimitiveType.Triangles, surfaceArray);
                this.Mesh = arrMesh;
                ResourceSaver.Save(Mesh, "res://sphere.tres", ResourceSaver.SaverFlags.Compress);
            }


        }
        else {
            this.Mesh = GD.Load<Mesh>("res://sphere.tres");
        }
	}
}
