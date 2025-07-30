from stl import mesh
from mpl_toolkits import mplot3d
from matplotlib import pyplot
import open3d as o3d
import os

"""
Render a .stl mesh. 
Download Open3D https://www.open3d.org/ for a better user experience.

July 9th, 2025
"""

MESH_FILENAME = "my_mesh.stl"

mesh = o3d.io.read_triangle_mesh(os.path.join("meshes", MESH_FILENAME))
mesh = mesh.compute_vertex_normals()
o3d.visualization.draw_geometries([mesh], window_name="Mesh Viewer", left=1000, top=200, width=800, height=650)

""" This would prolly be more lightweight but doesn't work for some reason
# Create a new plot
figure = pyplot.figure()
axes = mplot3d.Axes3D(figure)

# Load the STL files and add the vectors to the plot
your_mesh = mesh.Mesh.from_file('my_mesh.stl')
axes.add_collection3d(mplot3d.art3d.Poly3DCollection(your_mesh.vectors))

# Auto scale to the mesh size
scale = your_mesh.points.flatten()
axes.auto_scale_xyz(scale, scale, scale)

# Show the plot to the screen
pyplot.show()
"""