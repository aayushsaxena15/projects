import math
import matplotlib.pyplot as plt
labels=[]
y=[0.52275,0.4925,0.485,0.47]
x=[1,2,3,4]
plt.plot(x, y)
labels.append('with logistic regression')
plt.xlabel("Number of classifiers")
plt.ylabel("LogLoss (inverse accuracy)")
plt.title("Number of classifiers vs Logloss (inverse accuracy)")

y=[0.54,0.51,0.498,0.4967]
x=[1,2,3,4]
labels.append('without logistic regression')
plt.plot(x,y)
plt.legend(labels)
plt.show()
