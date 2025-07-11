"""
Represents a tetrahedron comprised of faces.

By Thomas Breimer
July 7th, 2025
"""

import numpy as np
import math
from dataclasses import dataclass, field
from stl import mesh
import random
import os
from collections import deque
from grammar import Grammar

DEFAULT_MESH_FILENAME = "my_mesh"

@dataclass
class Face:
    """
    Represents a face, defined by three points in space.

    Attributes:
        name (str): Name of the face, usually a letter.
        vertices (tuple[int]): The three vertices of the face, where each int represents the index of the 
                               vertex in the associated TetrahedralMesh.vertices list. Order matters! The 
                               order of the vertices in the tuple decides which way the normal vector points.
                               Use the right hand rule!
    """
    label: str
    vertices: tuple[int, int, int]  # indices in TetrahedralMesh.vertices list

    def measure_distances(self, vertices) -> np.ndarray:
        """
        Returns the distances between points of the face as a np.ndarray. Elements are absolute distances v1-v0, v2-v1, v2-v0 

        Parameters:
            vertices (np.ndarray): The TetrahedralMesh.verices array containing the coordinates of points in the mesh.

        Returns:
            np.ndarray: Distances between vertices in the face.
        """

        v0, v1, v2 = [vertices[i] for i in self.vertices]

        return np.array([np.linalg.norm(v1-v0), np.linalg.norm(v2-v1), np.linalg.norm(v2-v0)])


class TetrahedralMesh:
    """
    Represents a tetrahedral mesh, defined as a collection of vertices and faces.

    Attributes:
        vertices (list[np.ndarray]): A list of vertices, each with a ndarray of length three representing 
                                     the vertex's coordinates in space.
        faces (list[Face]) A list of faces.
        grammar (Grammar): Grammar object to use to grow the tetrahedron.
        queue (deqeue): Queue that holds faces, determines which face will have a rule applied next.
    """
    
    def __init__(self, grammar: Grammar = None):
        """
        Make a simple mesh with a single tetrahedron, with faces named "A", "B", "C", "D".
        Will be saved in the meshes directory.

        Parameters:
            grammar (Grammar): Grammar to use to grow the mesh.

        Returns:
            TetrahedralMesh: A simple mesh with a single tetrahedron.
        """

        self.vertices = []
        self.faces = []
        self.grammar = grammar
        self.queue = deque()
        
        """"
        # Grow seed by laying down a face and growing it (bad- A's normal will point inward)
        self.add_face("A", [np.array([0, 0, 0]), np.array([1, 0, 0]), np.array([0.5, 0, math.sqrt(3)/2])])
        self.grow_face(0, ["B", "C", "D"])
        """

        # Grow seed by laying down points and vertices
        v0 = self.add_vertex(np.array([0, 0, 0]))
        v1 = self.add_vertex(np.array([1, 0, 0]))
        v2 = self.add_vertex(np.array([0.5, 0, math.sqrt(3)/2]))
        v3 = self.add_vertex(np.array([0.5, math.sqrt(2/3), math.sqrt(1/12)]))

        self.add_face("A", [v0, v1, v2])
        self.add_face("B", [v0, v2, v3])
        self.add_face("C", [v1, v3, v2])
        self.add_face("D", [v0, v3, v1])

        """ 
        # Test split & grow normals
        self.split_face(0, ["A", "B", "C", "D"])
        self.grow_face(0, ["A", "A", "A"])
        self.grow_face(1, ["A", "A", "A"])
        self.grow_face(2, ["A", "A", "A"])
        self.grow_face(3, ["A", "A", "A"])
        """

    def find_vertex(self, target) -> int:
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

    def add_face(self, name: str, points: list) -> Face:
        """
        Adds a face to the mesh.

        Parameters:
            name (str): Name of the new face.
            points (list): Points of the new face. Should have length 3. Elements can either be type int, 
                           in which case they represent points already in the mesh as an index of self.points.
                           Alternatively, elements may be of type np.ndarray, in which case they represent a new
                           point to be added to the mesh, and thus the 3 elements of the np.ndarray should be type float.

        Returns:
            Face: Face object that was just added.
        """

        # Handle different vertex types and create new ones if necessary.
        assert len(points) == 3, "Expected 3 points in new face, but got {}!".format(len(points))

        vertices = []

        for point in points:
            if isinstance(point, int): # Handle already existing vertices.
                if point >= 0 and point < len(self.vertices):
                    vertices.append(point)
                else:
                    raise IndexError("Vertex {} out of bounds for tetrahedral mesh with {} vertices".format(point, len(self.vertices)))
            elif isinstance(point, np.ndarray): # Handle new vertices.
                vertices.append(self.add_vertex(point))
            else:
                raise TypeError("Expected type int or np.ndarray as vertex in new face, but got {}!".format(type(point)))

        # Make, add, and queue new face
        new_face = Face(name, tuple(vertices))
        self.faces.append(new_face)
        self.queue.append(new_face)

        return new_face

    def grow_face(self, face: Face, new_face_names: list[str]):
        """
        Grow a face by appending a new tetrahedron to the mesh with the given face as the base.
        
        Parameters:
            face (Face): Face to grow. Can either be the face object itself or the index in self.faces.
            new_face_names (list[str]): Names of the new faces. Length should be 3.
        """

        # Parse args
        if isinstance(face, int):
            if face > -1 and face < len(self.faces):
                face = self.faces[face]
            else:
                raise IndexError("Index {} out of bounds for mesh with {} faces.".format(face, len(self.faces)))
        elif isinstance(face, Face):
            assert face in self.faces, "Tried to grow Face {} but it doesn't exist in this mesh!".format(face)
        else:
            raise TypeError("Expected either a Face object or an int id, but got {}!".format(type(face)))
        
        assert len(new_face_names) == 3, "Expected 3 new faces names on face grow command, but got {}!".format(len(new_face_names))

        v0, v1, v2 = [self.vertices[i] for i in face.vertices]

        length = face.measure_distances(self.vertices).mean()

        # Calculate apex point (normal direction + offset)
        normal = np.cross(v1 - v0, v2 - v0)
        normal = normal / np.linalg.norm(normal) * (length * math.sqrt(2/3))
        apex = (v0 + v1 + v2) / 3 + normal 

        # Build new faces
        self.add_face(new_face_names[0], [v0, v1, apex])
        self.add_face(new_face_names[1], [v1, v2, apex])
        self.add_face(new_face_names[2], [v0, apex, v2])

    def rename_face(self, face, new_name: str):
        """
        Rename a face! Specify the face by either the face object itself, or its index in self.faces.

        Parameters:
            face: Face to reaname. Can either be the face object itself or the index in self.faces.
            new_name (str): The new name of the face.
        """
        # Parse args
        if isinstance(face, int):
            if face > -1 and face < len(self.faces):
                face = self.faces[face]
            else:
                raise IndexError("Index {} out of bounds for mesh with {} faces.".format(face, len(self.faces)))
        elif isinstance(face, Face):
            assert face in self.faces, "Tried to grow Face {} but it doesn't exist in this mesh!".format(face)
        else:
            raise TypeError("Expected either a Face object or an int id, but got {}!".format(type(face)))
        
        face.label = new_name

        # Requeue this face
        self.queue.append(face)

    def split_face(self, face, new_names: list):
        """
        Split a face into four smaller faces! Specify the face by either the face object itself, or its index in self.faces.

        Parameters:
            face: Face to reaname. Can either be the face objec itself or the index in self.faces.
            new_names (list[str]): Names of the new faces. Should be 4.
        """
        # Parse args
        if isinstance(face, int):
            if face > -1 and face < len(self.faces):
                face = self.faces[face]
            else:
                raise IndexError("Index {} out of bounds for mesh with {} faces.".format(face, len(self.faces)))
        elif isinstance(face, Face):
            assert face in self.faces, "Tried to grow Face {} but it doesn't exist in this mesh!".format(face)
        else:
            raise TypeError("Expected either a Face object or an int id, but got {}!".format(type(face)))
        
        # Get vertices of face to be split
        v0, v1, v2 = [self.vertices[i] for i in face.vertices]

        self.faces.remove(face) # Delete the face to be split

        # Find midpoints for vertices of new faces
        v05 = (v0 + v1) / 2
        v15 = (v1 + v2) / 2
        v25 = (v2 + v0) / 2

        # Add points to mesh
        self.add_vertex(v05)
        self.add_vertex(v15)
        self.add_vertex(v25)

        # Add and enqueue four new faces
        self.add_face(new_names[0], [v0, v05, v25])
        self.add_face(new_names[1], [v05, v1, v15])
        self.add_face(new_names[2], [v05, v15, v25])
        self.add_face(new_names[3], [v25, v15, v2])

    def export_to_stl(self, filename: str):
        """
        Export the mesh to a .stl file.

        Parameters:
            filename (str): Name of the file to export.        
        """

        faces = self.collect_faces()
        vertices = self.collect_vertices()

        my_mesh = mesh.Mesh(np.zeros(faces.shape[0], dtype=mesh.Mesh.dtype))
        for i, f in enumerate(faces):
            for j in range(3):
                my_mesh.vectors[i][j] = vertices[f[j],:]

        my_mesh.save(os.path.join("meshes", filename + ".stl"))

    def collect_vertices(self) -> np.ndarray:
        """
        Returns a np.ndarray containing all vertices in the mesh, each element itself being
        a np.ndarray of length 3 representing the coordinates of a single vertex.

        Returns:
            np.ndarray: An array containing all vertices in the mesh, each element itself being
                        a np.ndarray of length 3 representing the coordinates of a single vertex.
        """

        return np.array(self.vertices)
    
    def collect_faces(self):
        """
        Returns a 2D np.ndarray representing all the faces in the mesh.

        Returns:
            np.ndarray: A 2D np.ndarray with each element represening a face, and each element
                        itself being a length 3 np.ndarray of integers which corresponds to the
                        three points which make up the face. The integers correspond to the index
                        of each vertex in self.vertices.
        """

        vertices_list = []

        for face in self.faces:
            vertices_list.append(face.vertices)

        return np.array(vertices_list)
    
    def get_edge_distances(self):
        """
        Get the distance between all the points in every face.

        Returns:
            np.ndarray[np.ndarray]: A np.ndarray of np.ndarrays, each top level index representing a face,
                                    and 2nd level indices representing absolute distances v1-v0, v2-v1, v2-v0.             
        """

        distances = []

        for face in self.faces:
            distances.append(face.measure_distances(self.vertices))

        return np.array(distances)
    
    def apply_rule(self):
        """
        Apply a production rule on the next face in the queue.
        """

        next_face = self.queue.popleft()
        label = next_face.label

        operation = self.grammar.get_rule_operation(label)
        rhs = self.grammar.get_rule_rhs(label)

        if operation == "relabel":
            self.rename_face(next_face, rhs[0])
        elif operation == "grow":
            self.grow_face(next_face, rhs)
        elif operation == "divide":
            self.split_face(next_face, rhs)
        else:
            raise ValueError("Unexpected operation {} in rule.".format(operation))
    
def make_tetra(mesh_filename: str = DEFAULT_MESH_FILENAME) -> TetrahedralMesh:
    """
    Make a simple mesh with a single tetrahedron, with faces named "A", "B", "C", "D".
    Will be saved in the meshes directory.

    Parameters:
        mesh_filename (str): Filename to save mesh as.

    Returns:
        TetrahedralMesh: A simple mesh with a single tetrahedron.
    """

    # Make symmertic triangle at origin
    my_mesh = TetrahedralMesh()

    print("Faces: {}".format(my_mesh.faces))
    print("Vertices: {}".format(my_mesh.vertices))
    print("Edge distances by face: {}".format(my_mesh.get_edge_distances()))

    my_mesh.export_to_stl(mesh_filename)

if __name__ == "__main__":
    make_tetra()
