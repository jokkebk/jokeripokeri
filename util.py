import sys
from itertools import product
from operator import mul
from functools import reduce

#if sys.version_info >= (3,3):
#    from functools import lru_cache
#else:
#    def lru_cache(*args, **kwargs):
#        return lambda f: f
#
#@lru_cache(maxsize=None)
def C(n, k):
    if k > n: return 0
    k = min(k, n-k)
    if k == 0: return 1

    numer = reduce(mul, range(n, n-k, -1))
    denom = reduce(mul, range(1, k+1))
    return numer//denom

def num_hand(n):
    h = [0]*5
    for i in range(4,-1,-1):
        p=0;
        while C(p+1, i+1) <= n: p+=1
        h[i] = p;
        n -= C(p, i+1);
    return h

#       abcdefghijklm
valS = '23456789TJQKA'
deckS = [''.join(p) for p in product(valS, 'CDHS')]+['??']

def hand_str(h, s=0):
    return ' '.join('%s%s' % ('*' if (s&(1<<i)) else '', deckS[c]) for i,c in enumerate(h))

def str_hand(s):
    return [deckS.index(s.upper()) for s in s.strip().split()]

# Return bitmask of positions with value v
def val_mask(h, v):
    return sum(1<<i for i,c in enumerate(h) if c//4 == v)

# Return bitmask of positions with value v
def card_mask(h, v):
    return sum(1<<i for i,c in enumerate(h) if c == v)
