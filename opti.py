from collections import Counter, defaultdict
from itertools import product, combinations
import sys, time
from util import num_hand, hand_str, C
from jokeri import win
from match import match_hand

N = 0
hist = Counter()
start = time.clock()
samp = defaultdict(list)

for line in open(sys.argv[1]):
    num, sel, w = line.split()
    w = '%.1f' % float(w)
    hist[w] += 1
    N += 1
    if N%1 or len(samp[w])>10: continue
    num, sel = int(num), int(sel)
    h = num_hand(int(num))
    if float(win(h)) == float(w): continue # Self-evident
    samp[w].append(hand_str(num_hand(num), int(sel)))
    h = num_hand(num)
    if N%10000: continue
    print(N)

print('%.3f seconds' % (time.clock() - start))
print(len(hist))
for w in sorted(hist, reverse=True):
    if float(w) <= 3: break
    #print('%6s: %.1f %%' % (w, 100.0*hist[w]/N))
    print('%6s: %4d %s' % (w, hist[w], ''.join(['#']*int(100*hist[w]/N))))
    print(' '.join(samp[w]))
    #for h in samp[w]: print('  ', h)
