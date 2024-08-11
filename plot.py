import numpy as np
import matplotlib.pyplot as plt
from matplotlib.tri import Triangulation

def read_vtk_file(filename):
    vertices = []
    faces = []
    with open(filename, 'r') as file:
        for line in file:
            if line.startswith("POINTS"):
                num_vertices = int(line.split()[1])
                for _ in range(num_vertices):
                    line = next(file)
                    coords=line.strip().split()
                    vertices.append([float(coords[0]), float(coords[1]), float(coords[2])])
            elif line.startswith("CELLS"):
                num_faces = int(line.split()[1])
                for _ in range(num_faces):
                    line = next(file)
                    indices = line.strip().split()
                    faces.append([indices[1], indices[2], indices[3]])
    return vertices, faces

filename = "square_refine.vtk"
vertices, faces = read_vtk_file(filename)

vertices = np.array(vertices)
faces = np.array(faces)

center=[ (np.max(vertices[:,0])+np.min(vertices[:,0]))/2, (np.max(vertices[:,1])+np.min(vertices[:,1]))/2,0]


# center=[center[0]/len(vertices), center[1]/len(vertices), center[2]/len(vertices)]
print(center)
diff=np.linalg.norm(vertices-np.array(center), axis=1)

center_index=np.argmin(diff)
center=vertices[center_index]


circle_ind=[]
angles=np.linspace(0, 2*np.pi, 50, endpoint= False)
for angle in angles:
    x=center[0]+0.3*np.cos(angle)
    y=center[1]+0.3*np.sin(angle)
    z=0

    dist=np.linalg.norm(vertices-np.array([x,y,z]),axis=1)
    closest_vertex_index= np.argmin(dist)
    circle_ind.append(closest_vertex_index)

print(circle_ind)

square_ind=[]

square_coords=[[center[0]-0.3, center[1]-0.3, 0], [center[0]+0.3, center[1]-0.3, 0], [center[0]+0.3, center[1]+0.3, 0], [center[0]-0.3, center[1]+0.3, 0]]

## find the closest vertices in the mesh to the square boundary
## use the formula of distance between a point and a line

line1=[square_coords[0], square_coords[1]]
line2=[square_coords[1], square_coords[2]]
line3=[square_coords[2], square_coords[3]]
line4=[square_coords[3], square_coords[0]]

def distance_point_to_line(point, line):
    x0, y0, z0 = point
    x1, y1, z1 = line[0]
    x2, y2, z2 = line[1]
    a=y2-y1
    b=x1-x2
    c=x2*y1-x1*y2
    d=np.abs(a*x0+b*y0+c)/np.sqrt(a**2+b**2)
    return d

## find the closest vertices to the line1
for i in range(len(vertices)):
    dist=distance_point_to_line(vertices[i], line1)
    if dist<0.01:
        square_ind.append(i)

vertices = np.array(vertices)
faces = np.array(faces)

# Create a triangulation of the vertices
triang = Triangulation(vertices[:, 0], vertices[:, 1], faces)

plt.figure()
plt.triplot(triang, color='k', linewidth=0.5)

plt.xlabel('X')
plt.ylabel('Y')
plt.title('initial seed vertices')
plt.plot(vertices[circle_ind,0],vertices[circle_ind,1], 'ro')
plt.show()

plt.figure()
plt.triplot(triang, color='k', linewidth=0.5)

plt.xlabel('X')
plt.ylabel('Y')
plt.title('initial seed vertices')
plt.plot(vertices[square_ind,0],vertices[square_ind,1], 'ro')
plt.show()

#save circle indices in a file
with open('circle_indices.txt', 'w') as f:
    for item in circle_ind:
        f.write("%s\n" % item)






    
    




