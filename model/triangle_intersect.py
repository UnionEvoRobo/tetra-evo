"""
Test whether two triangles intersect in 3D space.
Implements the algorithm described by Tomas Möller in "A Fast Triangle-Triangle Intersection Test", 1997
https://web.stanford.edu/class/cs277/resources/papers/Moller1997b.pdf

Note that this implementation considers the points of the triangles to be open. Thus, triangles which
share a point, or have a point which lies on the edge of another triangle, are not considered to intersect.
This is important for tetrahedral mesh purposes, where we do not wish to consider connecting triangles forming
a tetrahdron to "intersect".

By Thomas Breimer
July 17th, 2025
"""

import numpy as np
import trimesh
import os
from pathlib import Path

# Tolerance for floating point math
TOLERANCE = 1e-12


def test_collision():
    """
    Test the collision engine for debugging purposes. Change x_trans, etc. and angled to get different situations to test.
    Also exports the triangles to ./meshes/test/triangles.stl to visualize.
    """

    v1_0 = np.array([0, 0, 0])
    v1_1 = np.array([1, 0, 0])
    v1_2 = np.array([0, 1, 0])

    x_trans = 0.5
    y_trans = -1.75
    z_trans = 0

    angled = False

    if angled:
        v2_0 = np.array([0.5 + x_trans, 0.5 + y_trans, -1 + z_trans])
        v2_1 = np.array([0.5 + x_trans, 0.5 + y_trans, 1 + z_trans])
        v2_2 = np.array([1 + x_trans, 1 + y_trans, 0 + z_trans])
    else:
        v2_0 = np.array([0 + x_trans, 0 + y_trans, 0 + z_trans])
        v2_1 = np.array([1 + x_trans, 0 + y_trans, 0 + z_trans])
        v2_2 = np.array([0 + x_trans, 1 + y_trans, 0 + z_trans])

    # triangles
    t1 = np.array([v1_0, v1_1, v1_2])
    t2 = np.array([v2_0, v2_1, v2_2])

    print(intersect(t1, t2))

    export_triangles(t1, t2)


def export_triangles(t1: np.ndarray, t2: np.ndarray):
    """
    Exports two triangls to a mesh ./meshes/test/triangles.stl.

    Parameters:
        t1 (np.ndarray): np.ndarry of shape (3, 3) with each row representing a point in a triangle.
        t2 (np.ndarray): np.ndarry of shape (3, 3) with each row representing a point in a triangle.
    """

    my_mesh = trimesh.Trimesh(vertices=np.concatenate((t1, t2), axis=0),
                              faces=[[0, 1, 2], [3, 4, 5]],
                              process=False,
                              validate=False)
    current_file_path = Path(__file__).resolve().parent
    my_mesh.export(
        os.path.join(current_file_path, "meshes", "test", "triangles.stl"))


def intervals_overlap(interval1: tuple, interval2: tuple) -> bool:
    """
    Determine whether two intervals overlap. 
    Intervals are assumed to be open: (start, end)

    Parameters:
        interval1 (tuple): The first interval, represented as a tuple (start, end).
        interval2 (tuple): The second interval, represented as a tuple (start, end).

    Returns:
        bool: True if the intervals overlap, False otherwise.
    """
    # Normalize intervals so start < end
    start1, end1 = sorted(interval1)
    start2, end2 = sorted(interval2)

    overlap_start = max(start1, start2)
    overlap_end = min(end1, end2)

    return overlap_start < overlap_end


def point_in_triangle_3d(point: np.ndarray, tri: np.ndarray) -> bool:
    """
    Check if a point lies inside a triangle in 3D space. Assumes the point lies in the same plane as the triangle.

    Parameters:
        point (np.ndaray): The 3D coordinates of the point to test.
        tri (np.ndarray): The triangle to test, each row a point.

    Returns:
        bool: True if the point lies inside the triangle, False otherwise.
    """

    p = np.array(point)
    a, b, c = map(np.array, tri)

    v0 = c - a
    v1 = b - a
    v2 = p - a

    # Compute dot products
    dot00 = np.dot(v0, v0)
    dot01 = np.dot(v0, v1)
    dot02 = np.dot(v0, v2)
    dot11 = np.dot(v1, v1)
    dot12 = np.dot(v1, v2)

    # Compute barycentric coordinates
    denom = dot00 * dot11 - dot01 * dot01
    if np.abs(denom) < TOLERANCE:
        return False

    inv_denom = 1 / denom
    u = (dot11 * dot02 - dot01 * dot12) * inv_denom
    v = (dot00 * dot12 - dot01 * dot02) * inv_denom

    return (u > TOLERANCE) and (v > TOLERANCE) and (u + v < 1 - TOLERANCE)


def compute_line_interval(t: np.ndarray, dv: np.ndarray,
                          D: np.ndarray) -> tuple:
    """
    Computes the interval of intersection between a triangle and a line,
    in the context of Tomas Möller's triangle intersection algorithm where
    we also have signed distances to a plane containing that line.

    Parameters:
        t (np.ndarray): np.ndarry of shape (3, 3) with each row representing a point in a triangle.
        dv (np.ndarray): Signed distances from the v's to the plane containing the line.
        D (np.ndarray): The vector pointing in the direction of the line.
    
    Returns:
        tuple: The interval of the intersection of the line and triangle.
    """

    # WOLOG we assume that we are dealing with t1 and pi2 in the context of the algorithm.

    positive_indices = np.where(dv >= 0)[0]
    negative_indices = np.where(dv < 0)[0]

    # Also WOLOG we will relabel v's s.t. v0 and v2 lie on one side of pi2, and v1 on the other side.
    # dv lets us know which side of the plane the v's are on via the sign of each v.

    if len(positive_indices) > len(negative_indices):
        v0 = t[positive_indices[0]]
        d0 = dv[positive_indices[0]]

        v2 = t[positive_indices[1]]
        d2 = dv[positive_indices[1]]

        v1 = t[negative_indices[0]]
        d1 = dv[negative_indices[0]]
    else:
        v0 = t[negative_indices[0]]
        d0 = dv[negative_indices[0]]

        v2 = t[negative_indices[1]]
        d2 = dv[negative_indices[1]]

        v1 = t[positive_indices[0]]
        d1 = dv[positive_indices[0]]

    p0 = np.dot(D, v0)
    p2 = np.dot(D, v2)
    p1 = np.dot(D, v1)

    # compute interval on L
    start = p0 + (p1 - p0) * (d0 / (d0 - d1))
    end = p2 + (p1 - p2) * (d2 / (d2 - d1))

    return (start, end)


def intersect(t1: np.ndarray, n1: np.ndarray, d1: float, t2: np.ndarray,
              n2: np.ndarray, d2: float) -> bool:
    """
    Computes if two triangles in 3D space inersect using Tomas Möller's "Fast Triangle-Triangle Intersection Test". 
    
    Parameters:
        t1 (np.ndarray): np.ndarry of shape (3, 3) with each row representing a point in a triangle.
        n1 (np.ndarray): Normal vector of t1's plane.
        d1 (float): Translation of t1's plane w. r. t. origin.
        t2 (np.ndarray): np.ndarry of shape (3, 3) with each row representing a point in a triangle.
        n2 (np.ndarray): Normal vector of t1's plane.
        d2 (float): Translation of t1's plane w. r. t. origin.
    Returns:
        bool: True if the triangles intersect, False otherwise.
    """

    # First, we compute the signed distance from t1's points to t2's plane, pi2 and vice versa

    # Compute signed distances between vertices of t1 to pi2 mutiplied by constant (n2*n2)
    # By subbing in vertices of t1 into plane equation
    d_v1 = np.dot(t1, n2) + d2
    d_v2 = np.dot(t2, n1) + d1

    # Now d_v1 and d_v2 tell us the disances from each triangle's vertices to the other's triangle's plane.
    # Based on this we make a decision what to do next.

    if (d_v1 == 0).all():
        # If d_v1 is all 0, we know the triangles are coplanar.
        # We can simply check if any points of t1 are inside t2 and vice versa

        for t1_point in t1:
            if point_in_triangle_3d(t1_point, t2):
                return True

        for t2_point in t2:
            if point_in_triangle_3d(t2_point, t1):
                return True

        return False
    elif (d_v1 >= -TOLERANCE).all() or (d_v1 <= TOLERANCE).all() or (
            d_v2 >= -TOLERANCE).all() or (d_v2 <= TOLERANCE).all():
        # If a d_v has all the same sign (inlucing zero since we consider points open)
        # one triangle's vertices are all on one side of the other's plane.
        # Then they can't intersect and we are done, saving a lot of work.

        return False
    else:
        # Triangles are not coplanar and not on one side of eachother's plane.
        # Then there is a line containing both triangles.
        # We want to compute the projection of v's onto that line and ultimately find the interval of t1 and t2 on that line.
        # If the intervals overlap then the triangles intersect.

        D = np.cross(n1, n2)  # Direction of line

        t1_interval = compute_line_interval(t1, d_v1, D)
        t2_interval = compute_line_interval(t2, d_v2, D)

        # see if the intervals overlap
        return intervals_overlap(t1_interval, t2_interval)


if __name__ == "__main__":
    test_collision()
