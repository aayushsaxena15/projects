import math
import matplotlib.pyplot as plt
y=[0.56042043,0.49368716,0.49120819,0.48893779,0.48611452,0.48607925]
x=[1,5,10,25,75,100]
plt.plot(x, y)
plt.xlabel("Number of estimators")
plt.ylabel("LogLoss (inverse accuracy)")
plt.title("Number of estimators vs Logloss (inverse accuracy)")
plt.show()
