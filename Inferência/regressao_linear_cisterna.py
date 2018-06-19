import json
from pprint import pprint

# with open('data.json') as f:
with open('dados02.json') as f:
    data = json.load(f)

# pprint(data)


import numpy as np
from util import r_squared
from plots import plot_points_regression 


# print(data[0])

# dados

X = []
y = []
i = 0
for item in data:
	if(i % 2 == 0):
		X.append(item["tempo"])
	else:
		y.append(item["distancia"])
	i += 1


# print(X)
# print(y)

plot_points_regression(X,
                       y,
                       title='Histórico do nível de água',
                       xlabel="tempo",
                       ylabel='nível')

np_X = np.empty([len(X), 1])
np_y = np.empty([len(y), 1])

for i in range(0, len(X)):
	np_X[i][0] = X[i]
	np_y[i][0] = y[i]


# X = np.asarray(X)
# y = np.asarray(y)



# print(" type X ", X.dtype)
# print(" type y ", y.dtype)

# np.reshape(X, (X.shape[0], 1))
# np.reshape(y, (y.shape[0], 1))


print(" type np_X ", np_X.dtype)
print(" type np_y ", np_y.dtype)



print("shape np_X ", np_X.shape)
print("shape np_y ", np_y.shape)



def normal_equation_prediction(X, y):
    """
    Calculates the prediction using the normal equation method.
    You should add a new row with 1s.

    :param X: design matrix
    :type X: np.ndarray(shape=(N, d))
    :param y: regression targets
    :type y: np.ndarray(shape=(N, 1))
    :return: prediction
    :rtype: np.ndarray(shape=(N, 1))
    """
    X_new = np.concatenate((np.ones((X.shape[0], 1)), X), axis=1)
    w = np.linalg.inv(X_new.T.dot(X_new)).dot(X_new.T).dot(y)
    prediction = X_new.dot(w)
    return prediction


prediction = normal_equation_prediction(np_X, np_y)
print("shape prediction ", prediction.shape)

r_2 = r_squared(np_y, prediction)
plot_points_regression(np_X,
                       np_y,
                       title='Estimativa do nível da água',
                       xlabel="tempo",
                       ylabel='nível',
                       prediction=prediction,
                       legend=True,
                       r_squared=r_2)