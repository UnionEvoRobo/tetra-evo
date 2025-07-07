"""
Represents a tetrahedron comprised of faces.

July 7th, 2025
By Thomas Breimer
"""

import numpy as np
from dataclasses import dataclass, field
from collections import defaultdict
from stl import mesh

@dataclass
class Face:
    """
    Represents a face, defined by three points in space.

    Attributes:
        name (str): Name of the face, usually a letter.
        vertices (tuple[int]): The three vertices of the face, where each int represents the index of the 
                               vertex in the associated TetrahedralMesh.vertices list.
    """
    name: str
    vertices: tuple[int, int, int]  # indices into global vertex list

@dataclass
class TetrahedralMesh:
    """
    Represents a tetrahedral mesh, defined as a collection of vertices and faces.

    Attributes:
        vertices (list[np.ndarray]): A list of vertices, each with a ndarray of length three representing 
                                     the vertex's coordinates in space.
        faces (list[Face]) A list of faces.
    """
    vertices: list[np.ndarray] = field(default_factory=list)
    faces: list[Face] = field(default_factory=list)

    def find_vertex(self, target):
        """
        Find the index of a vertex in self.vertices if it exists.

        Parameters:
            target (np.ndarray): The target vertex to find.

        Returns:
            int: The index of target in self.vertices if it exists, -1 otherwise.
        """
        for i, arr in enumerate(self.vertices):
            if np.array_equal(arr, target):
                return i
        return -1 

    def add_vertex(self, new_vertex: np.ndarray) -> int:
        """
        Adds a vertex to the mesh.

        Parameters:
            point (np.ndarray): The vertex's position in space, represented by a vector of floats of length 3.
-
        Returns:
            int: The index of the vertex in self.vertices.
        """

        # Don't make a new vertex if it already exists.
        index = self.find_vertex(new_vertex)

        if index != -1: 
            return index
        else:
            self.vertices.append(new_vertex)
            return len(self.vertices) - 1

    def add_face(self, name: str, points: list):
        """
        Adds a face to the mesh.

        Parameters:
            name (str): Name of the new face.
            points (list): Points of the new face. Should have length 3. Elements can either be type int, 
                           in which case they represent points already in the mesh as an index of self.points.
                           Alternatively, elements may be of type np.ndarray, in which case they represent a new
                           point to be added to the mesh, and thus the 3 elements of the np.ndarray should be type float.
        """

        # Handle different vertex types and create new ones if necessary.
        assert len(points) == 3, "Expected 3 points in new face, but got {}!".format(len(points))

        vertices = []

        for point in points:
            if isinstance(point, int): # Handle already existing vertices.
                if point > 0 and point < len(self.vertices):
                    vertices.append(point)
                else:
                    raise IndexError("Vertex {} out of bounds for tetrahedral mesh with {} vertices".format(point, len(self.vertices)))
            elif isinstance(point, np.ndarray): # Handle new vertices.
                vertices.append(self.add_vertex(point))
            else:
                raise TypeError("Expected type int or np.ndarray as vertex in new face, but got {}!".format(type(point)))

        # Make and add new face
        self.faces.append(Face(name, tuple(vertices)))

    def grow_face(self, face: Face, new_face_names: list[str], scale: float):
        """
        Grow a face by appending a new tetrahedron to the mesh with the given face as the base.
        
        Parameters:
            face (Face): Face object that will be grown.
            new_face_names (list[str]): Names of the new faces. Length should be 3.
            scale (float): How tall the new tetrahedron should be. Units TBD.
        """

        assert face in self.faces, "Tried to grow a face {}, but it didn't exist in this mesh!".format(str(face))
        assert len(new_face_names) == 3, "Expected three new faces names on face grow command, but got {}!".format(len(new_face_names))

        v0, v1, v2 = [self.vertices[i] for i in face.vertices]

        # Calculate apex point (normal direction + offset)
        normal = np.cross(v1 - v0, v2 - v0)
        normal = normal / np.linalg.norm(normal)
        apex = (v0 + v1 + v2) / 3 + normal * scale

        # Build new faces
        self.add_face(new_face_names[0], [v0, v1, apex])
        self.add_face(new_face_names[0], [v1, v2, apex])
        self.add_face(new_face_names[0], [v0, apex, v2])

    def export_to_stl(self, filename: str):
        face_count = defaultdict(list)  # canonical key -> list of face indices

        for i, face in enumerate(self.faces):
            key = tuple(sorted(face.vertices))

        external_faces = [self.faces[i] for key, indices in face_count.items() if len(indices) == 1 for i in indices]

        stl_faces = mesh.Mesh(np.zeros(len(external_faces), dtype=mesh.Mesh.dtype))

        for i, face in enumerate(external_faces):
            v1, v2, v3 = [self.vertices[idx] for idx in face.vertices]
            stl_faces.vectors[i] = np.array([v1, v2, v3])

        stl_faces.save(filename)

my_mesh = TetrahedralMesh()
my_mesh.add_face("A", [np.array([0, 0, 0]), np.array([1, 0, 0]), np.array([0, 1, 0])])
my_mesh.grow_face(my_mesh.faces[0], ["B", "C", "D"], 1.0)

print(my_mesh.faces)
print(my_mesh.vertices)

my_mesh.export_to_stl("my_mesh.stl")