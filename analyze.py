from collections import Counter
from itertools import product, combinations
import sys, time
from util import num_hand, hand_str
from match import match_hand

strategy = (
        ('uuuu?', 50.0), # fives (with joker)
        ('!s', 30.0), # straight flush
        ('uuu?', 15.72917), # fours with joker
        ('uuuu', 15.72917), # fours
        ('uuuvv', 8.0), # full house
        ('uuvv?', 8.0), # full house with joker

        ('3456789T abc s?', 4.04167), # open-ended straight flush draw with joker

        ('sssss', 4.0), # flush
        ('ssss?', 4.0), # flush with joker
        ('uuu', 3.4539), # set
        ('uu?', 3.4539), # set

        ('2J abc s?', 3.33333), # open-ended straight flush draw with joker at 234 and JKQ

        # one gapper straight flush draw with joker
        ('23456789T abd s?', 3.33333), 
        ('23456789T acd s?', 3.33333),

        ('!', 3.0), # straight

        ('uuvv', 2.625), # two pair

        # Open ended 4 card straight flush draw and one-gappers
        ('A23456789T abcd s', 2.83333),
        ('A23456789T acde s', 2.10417),
        ('A23456789T abde s', 2.10417),
        ('A23456789T abce s', 2.10417),
         
        #('3456789T ac s?', 1.25),
        #('3456789T ab s?', 0),

        ('uu', 0.84673), # pair
        #('sss?', 0.0), # flush draw
        ('ssss', 0.0), # flush draw
        )

I = 0
N = 50 if len(sys.argv) < 3 else int(sys.argv[2])

right = 0
best, worst = [0]*len(strategy), [99]*len(strategy)

print('Sampling %.2f %% of hands' % (100.0/N))

start = time.clock()
for line in open(sys.argv[1]):
    I += 1
    if I%N: continue
    num, sel, win = line.split()
    num, sel, win = int(num), int(sel), float(win)
    h = num_hand(num)

    s = 0
    for i,(p,w) in enumerate(strategy):
        s = match_hand(h, p)
        if s:
            si = i
            break

    if s:
        if s!=sel:
            print('FAIL', hand_str(h, sel), 'vs.', hand_str(h, s), 'payout', win)
            break
        best[si] = max(best[si], win)
        worst[si] = min(worst[si], win)
        right += 1

print(time.clock() - start, 's')
print('%d correct, est. %.2f %% coverage' % (right, float(N)*right/I))

for i,(p,w) in enumerate(strategy):
    print('%8.5f .. %8.5f for %s' % (worst[i], best[i], p))
