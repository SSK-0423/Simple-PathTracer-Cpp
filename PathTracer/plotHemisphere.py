import matplotlib.pyplot as plt
import numpy as np

t = np.random.random((2,10000))
u1 = t[0]
u2 = t[1]
phi = 2 * np.pi * u1
x = np.sqrt(u2) * np.cos(phi)
y = np.sqrt(u2) * np.sin(phi)
z = np.sqrt(1.0 - u2)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.scatter(x,y,z,s=2)
plt.show()
# 基底変換
EPSILON = 0.0001

normal = np.array([1,0,0])
# normal = np.array([np.random.rand(),np.random.rand(),np.random.rand()])
normal = normal / np.linalg.norm(normal)

w = normal
if abs(w[0]) < EPSILON:
    u = np.cross([1,0,0],w)
    u = u / np.linalg.norm(u)
else:
    u = np.cross([0,1,0],w)
    u = u / np.linalg.norm(u)
v = np.cross(w,u)

dirX = []
dirY = []
dirZ = []

for i in range(0,len(x)):
    dir = u * x[i] + v * y[i] + w * z[i]
    dirX.append(dir[0])
    dirY.append(dir[1])
    dirZ.append(dir[2])

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.scatter(dirX,dirY,dirZ,s=2)
plt.show()