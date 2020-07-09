import numpy as np

#data=np.loadtxt('opti3np.txt')
data=np.load('opti3.npy')

print('Loaded data with shape', data.shape) # should be (n, 95)

np.random.shuffle(data)

N = data.shape[0]
N_train = N * 8 // 10

data_train = data[0:N_train]
data_valid = data[N_train:]

print('Training and validation shapes:', data_train.shape, data_valid.shape)

X_train = data_train[:, 0:90]
Y_train = data_train[:, 90:]

X_valid = data_valid[:, 0:90]
Y_valid = data_valid[:, 90:]

print('Training X and Y shapes:', X_train.shape, Y_train.shape)
print('Validation X and Y shapes:', X_valid.shape, Y_valid.shape)

from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation
from keras.optimizers import SGD
from keras.callbacks import EarlyStopping

model = Sequential()
model.add(Dense(1500, activation='relu', input_dim=X_train.shape[1]))
model.add(Dropout(0.1))
model.add(Dense(500, activation='relu'))
model.add(Dropout(0.1))
model.add(Dense(Y_train.shape[1], activation='sigmoid'))

sgd = SGD(lr=0.03, decay=1e-6, momentum=0.9, nesterov=True)
model.compile(loss='binary_crossentropy', optimizer=sgd)

model.fit(X_train, Y_train, epochs=500, batch_size=2000, validation_data=(X_valid, Y_valid),
        callbacks=[EarlyStopping(monitor='val_loss', patience=5)])

preds = model.predict(X_valid)
preds[preds>=0.5] = 1
preds[preds<0.5] = 0

print(Y_valid.shape[0], 'items in validation set', sum(1 for i in range(Y_valid.shape[0]) if (Y_valid[i] == preds[i]).all()))

model.save('kerastest.h5')
