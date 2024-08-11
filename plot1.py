import numpy as np
import matplotlib.pyplot as plt
from matplotlib.tri import Triangulation

vertices = []  # Array of vertex coordinates
faces = []     # Array of face indices

with open('hand.obj', 'r') as f:
    for line in f:
        parts = line.strip().split()
        if parts[0] == 'v':
            vertices.append([float(parts[1]), float(parts[2]), float(parts[3])])  # Only 2d
        elif parts[0] =='f':
            faces.append([int(part.split('/')[0]) for part in parts[1:]])

#true values
values =[]
with open('true_values.txt', 'r') as f:
    for line in f:
        parts = line.strip().split()
        values.append(float(parts[0]))
#calc values
values1=[]
with open('valuescalc.txt', 'r') as f:
    for line in f:
        parts = line.strip().split()
        values1.append(float(parts[0]))
errors=[]
with open('errors.txt', 'r') as f:
    for line in f:
        parts1=line.strip().split()
        errors.append(float(parts1[0]))

vertices =np.array(vertices)
faces= np.array(faces)
values = np.array(values)
values1= np.array(values1)
errors= np.array(errors)

# Create a triangulation of the vertices
triang =Triangulation(vertices[:, 0], vertices[:, 1], faces-1 )

plt.figure()
plt.triplot(triang, color='k', linewidth=0.5)
plt.xlabel('X')
plt.ylabel('Y')
plt.title('2D obj Mesh Visualization')  
#plt.show()


# Create a 2D plot
plt.figure()
plt.tripcolor(triang, values,cmap='viridis',edgecolor='k', linewidth=0.5)
plt.colorbar()

plt.xlabel('X')
plt.ylabel('Y')
plt.title('True values')
#plt.show()

plt.figure()
plt.tripcolor(triang, values1,cmap='viridis', edgecolor='k',linewidth=0.5)
plt.colorbar()

plt.xlabel('X')
plt.ylabel('Y')
plt.title('Calculated values')

plt.figure()
plt.tripcolor(triang, errors, cmap='viridis', edgecolor='k', linewidth=0.5)
plt.colorbar()
plt.title('errors')

# plt.savefig('triangle_mesh.png')
# plt.savefig('triange_true.png')
# plt.savefig('triangle_calc.png')
plt.savefig('circle_error.png')
plt.show()




