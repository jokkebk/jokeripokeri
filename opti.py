from collections import Counter
from itertools import product, combinations
import sys
from util import num_hand, hand_str, val_mask

# Evaluator that will pick ready two pairs and better
def evPair(h):
    valC = Counter(c//4 for c in h).most_common(2)
    joker = h[4]==52
    if valC[0][1]==4:
        return (50.0, 31) if joker else (15+35.0/48, 31-val_mask(h, valC[1][0]))
    elif valC[0][1]==3:
        if joker: return (15+35.0/48, val_mask(h, valC[0][0])+16)
        elif valC[1][1]==2: return (8.0, 31) # Full house
        else: return (3.45390, val_mask(h, valC[0][0])) # Set with draws
    elif valC[0][1]==2:
        if valC[1][1]==2:
            if joker: return (8.0, 31)
            else: return (2.6250, val_mask(h, valC[0][0])+val_mask(h, valC[1][0]))
        else:
            if joker: return (3.45390, val_mask(h, valC[0][0]+16))
            else: return (0.84673, val_mask(h, valC[0][0]))
    return (0.0, 0)

#print(evPair([4,5,6,9,52]))
#exit(1)
I = 0
N = 1000 if len(sys.argv) < 3 else int(sys.argv[2])


odds = {} # category -> (min, max) pairs 

straightvals = set()
straightvals.add(15 + (1<<12))
for j in range(4): straightvals.add(15 + (1<<12) - (1<<j) + (1<<13))
for i in range(9):
    straightvals.add(31<<i)
    for j in range(5): straightvals.add((31<<i) - (1<<i+j) + (1<<13))

for line in open(sys.argv[1]):
    num, sel, p = line.strip().split()
    num, sel, p = int(num), int(sel), float(p)
    h = num_hand(num)
    hs = [h[i] for i in range(5) if sel & (1<<i)]

    evP, evS = evPair(h)
    if evS == sel: continue

    I += 1
    if I > 10: break

    print(hand_str(h, sel), format(sel, '05b'), format(evS, '05b'), p, evP)
    
    #if len(hs) < 4: continue # analyze large selections for starters
    # Categorize hands
    #joker = (52 in hs)

    # Find straights
    #mask = sum(1<<c//4 for c in hs)
    #straight = 'straight' if mask in straightvals else 'nostraight'

    #suitC = list(Counter(c%4 for c in hs if c!=52).most_common(1)[0])
    #if joker: suitC[1] += 1
    #flush = 'flush%d' % suitC[1]

    #if cat in ('fives', 'fours', 'full_house'): continue
    #if straight == 'straight' or flush == 'flush5': continue
