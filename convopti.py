from collections import Counter, defaultdict
from util import hand_str, num_hand
import sys

def has_pair(h):
    c = Counter(v//4 for v in h)
    mc = c.most_common()
    #return mc[0][1] == 2 or mc[1][1] == 2
    return mc[0][1] > 1

hotone = lambda v,n: [1 if v==i else 0 for i in range(n)]

def hand2hotone(h):
    rep = []
    for c in h:
        rep += hotone(c%4, 4)
        rep += hotone(c//4, 14)
    return rep

if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else 'opti.txt'

    with open(filename) as fin:
        I = 0
        for line in fin:
            num, sel, p = line.strip().split()
            num = int(num)
            sel = int(sel)
            p = float(p)
            h = num_hand(num)
            #print(h, sel)
            rep = hand2hotone(h)
            rep += [((sel>>i) & 1) for i in range(5)]
            print(' '.join(str(v) for v in rep))
            #I += 1
            #if I > 10: break
