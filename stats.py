from collections import Counter, defaultdict
from util import hand_str, num_hand
import sys

def has_pair(h):
    c = Counter(v//4 for v in h)
    mc = c.most_common()
    #return mc[0][1] == 2 or mc[1][1] == 2
    return mc[0][1] > 1

filename = sys.argv[1] if len(sys.argv) > 1 else 'opti.txt'
print('Opening', filename)

right = {}
with open(filename) as fin:
    I = 0
    cnt = Counter()
    c2 = Counter()
    numSum = 0
    selSum = 0
    pSum = 0.0
    wins = defaultdict(set)
    for line in fin:
        num, sel, p = line.strip().split()
        num = int(num)
        sel = int(sel)
        p = float(p)
        numSum += num
        selSum += sel
        pSum += p
        I = I+1
        right[num] = sel
        ones = bin(sel).count('1')
        cnt[ones] += 1
        wins[ones].add(p)
        if ones == 1 and p > 0.3:
            print(hand_str(num_hand(num), sel), p)
        continue
        h = num_hand(num)
        if h[4] == 52:
            cnt['joker'] += 1
            if ones == 1:
                cnt['sel1joker'] += 1
                print('Unicorn!', bin(sel)[::-1].find('1'), h[bin(sel)[::-1].find('1')])
                print(hand_str(h, sel), sel, p)
        if has_pair(h):
            cnt['pair'] += 1
            if ones == 1: cnt['sel1pair'] += 1
        if ones == 1:
            c2[h[bin(sel)[::-1].find('1')]//4] += 1
        #if ones == 1: 
        #if I > 10000: break
    print(numSum, selSum, pSum)
    for n in cnt:
        print(n, cnt[n], '%.1f' % (100.0 * cnt[n]/I))
        print(len(wins[n]), wins[n])
    #print(cnt)
    #print(c2)
    #print(I)

    exit(1)
    with open('opti.txt') as fin:
        #diff = 0
        for line in fin:
            num, sel, p = line.strip().split()
            num = int(num)
            sel = int(sel)
            p = float(p)
            if right[num] != sel:
                h = num_hand(num)
                print(hand_str(h, sel), sel, 'vs', right[num], p)
            numSum += num
            selSum += sel
            pSum += p
            I = I+1
            right[num] = sel
            continue
