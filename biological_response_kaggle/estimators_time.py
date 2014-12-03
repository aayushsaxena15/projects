import math
import matplotlib.pyplot as plt
y=[24.767,101.131,156.569,310.540,853.456,1031.199]
x=[1,5,10,25,75,100]
plt.plot(x, y)
plt.xlabel("Number of estimators")
plt.ylabel("Time (sec.)")
plt.title("Number of estimators vs Time (sec.)")
plt.show()
