from collections import Counter
from functools import reduce
from operator import mul
from random import sample
from itertools import product, combinations
import os.path
import pickle

def rawwin(v):
    c1 = v.count(v[0])
    if c1==5: return 50 # This will not actually be used
    c2 = v.count(v[c1])
    if c2>c1: c1, c2 = c2, c1 # Swap
    if c1==4: return 15 # Four of a kind
    if c1+c2==5: return 8 # Full house
    c3 = v.count(v[4])
    if c3>c2: c2, c3 = c3, c2
    if c1+c2==4: return 2 # Set or two pairs
    if c1+c2+c3==3 and v[2]!=v[3] and (v[4]-v[0]==4 or (v[3]==3 and v[4]==12)):
        return 3 # Straight
    return 0


def win(h):
    h = sorted(h)
    if 52 in h: # Handle jokers by replacement
        best = 0
        for c in range(h[0]&3, 52, 4): # Try suited values
            h[4] = c
            best = max(best, win(h))
        return best
    v = [c//4 for c in h]

    if all((c&3) == (h[0]&3) for c in h[1:] if c!=52):
        return 30 if rawwin(v)==3 else 4
    return rawwin(v)

def prewin(h):
    if 52 in h: # Handle jokers by replacement
        h = sorted(h)
        best = 0
        for c in range(h[0]&3, 52, 4): # Try suited values
            h[4] = c
            best = max(best, prewin(h))
        return best

    flush = True
    off = 0
    for i in range(5):
        off = off*16 + (h[i]>>2)
        if (h[i]&3) != (h[0]&3): flush = False
    if flush: return 30 if wintab[off]==3 else 4
    return wintab[off]

if __name__ == "__main__":
    print(win([0,4,8,12,16]))
    exit(1)
    if os.path.isfile('jokeri.dat'):
        with open ('jokeri.dat', 'rb') as fp:
                wintab = pickle.load(fp)
    else:        
        wintab = [0] * (2**20)

        # Precalculate win rates by pure card numbers, no jokers or suits
        for h in product(range(13), repeat=5):
            off = 0
            for i in range(5): off = off*16 + h[i]
            wintab[off] = rawwin(sorted(h))

        with open('jokeri.dat', 'wb') as fp:
            pickle.dump(wintab, fp)
            print('Wrote cache file')

    while True:
        hand = [deckS.index(s.upper()) for s in input().strip().split()]
        if not hand: break
        left = [c for c in range(53) if c not in hand]

        ans = []
        for p in range(1,5):
            for s in combinations(hand, p):
                S, I = 0, 0
                for a in combinations(left, 5-p):
                    S += prewin(s+a)
                    I += 1
                print('%.8f'%(S/I), ' '.join(deckS[c] for c in s), S, I)
                ans.append((S/I, ' '.join(deckS[c] for c in s)))

        for i,(prob,hand) in enumerate(sorted(ans, reverse=True)[:3]):
            print('%d. %8.5f %s' % (i+1, prob, hand))
