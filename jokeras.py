from keras.models import load_model
from convopti import hand2hotone
from util import str_hand
import numpy as np

model = load_model('kerastest.h5')


while True:
    h = str_hand(input('Give hand: '))
    h = sorted(h)
    x = hand2hotone(sorted(h))
    print(h, x)

    p = model.predict(np.array(x).reshape(1,90))
    p[p >= 0.5] = 1
    p[p < 0.5] = 0
    print(p)
