import math
import matplotlib.pyplot as plt
y=[0.49210039,0.490868714,0.49391924,0.49120819,0.49000761,0.488725404]
x=[2,3,4,5,6,7]
plt.plot(x, y)
plt.xlabel("Number of folds")
plt.ylabel("LogLoss")
plt.title("Number of folds vs Logloss(inverse accuracy)")
plt.show()
