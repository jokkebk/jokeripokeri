from collections import Counter, defaultdict
from itertools import product, combinations, islice
import sys, time
from util import num_hand, hand_str
from match import match_hand
from multiprocessing import Pool, cpu_count

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

        # Open ended 4 card straight flush draw
        ('A23456789T abcd s', 2.83333),

        # Straight flush draw with joker and 2 outs
        ('AQ abc s?', 2.667), # ends
        ('AJ abd s?', 2.667),
        ('AJ acd s?', 2.667),
        ('A23456789T abe s?', 2.667),
        ('A23456789T ace s?', 2.667),
        ('A23456789T ade s?', 2.667),

        ('uuvv', 2.625), # two pair

        # Straight flush draw with 1 out
        ('AJ abcd s', 2.10417),
        ('A23456789T acde s', 2.10417),
        ('A23456789T abde s', 2.10417),
        ('A23456789T abce s', 2.10417),
         
        # Two card straight flush draw with joker
        ('456789T ab s?', 1.38032), # to 1.45833, '3J ab s?' is lower

        # Three card straight draw with joker
        ('3456789T abc ?', 1.375),

        #############################################################
        # Here start the complicated straight flush draw with joker #
        #############################################################

        # Three card one gapper straight flush draw with joker
        #('346789T ac s?', 1.31117),

        # Two card straight flush draw with joker at the ends
        #('3J ab s?', 1.33245),

        #('uu', 0.84673), # pair
        #('sss?', 0.0), # flush draw with joker
        #('ssss', 0.0), # flush draw
        )

def handle(line):
    num, sel, win = line.split()
    num, sel, win = int(num), int(sel), float(win)
    h = num_hand(num)

    s = 0
    for i,(p,w) in enumerate(strategy):
        s = match_hand(h, p)
        if s: return (0 if s==sel else -1, num, sel, win, i)

    return ('%.2f'%win, num, sel, win, 0)

if __name__ == '__main__':
    N = 50 if len(sys.argv) < 3 else int(sys.argv[2])
    with open(sys.argv[1]) as f: lines = list(islice(f, 0, None, N))
    print('Read %.2f %% of hands' % (100.0/N))

    start = time.clock()
    p = Pool(cpu_count())
    ret = p.map(handle, lines)

    print(time.clock() - start, 's')

    right, wrong, neutral = 0, [], 0
    best, worst = [0]*len(strategy), [99]*len(strategy)
    remain = Counter()
    sample = defaultdict(list)

    for w, num, sel, win, si in ret:
        if w==0:
            best[si] = max(best[si], win)
            worst[si] = min(worst[si], win)
            right += 1
        elif w==-1:
            sel1 = match_hand(num_hand(num), strategy[si][0], 0)
            sel2 = match_hand(num_hand(num), strategy[si][0], 1)
            if sel2 == sel: # close call!
                #print('Second opinion: %d vs. %d' % (sel, sel2))
                best[si] = max(best[si], win)
                worst[si] = min(worst[si], win)
                right += 1
            else:
                print('WRONG: %s' % hand_str(num_hand(num), sel), win, w, strategy[si], hand_str(num_hand(num), sel1))
        else:
            neutral += 1
            remain[w] += 1
            if len(sample[w]) < 20: sample[w].append('%s %s %f' % (hand_str(num_hand(num), sel), w, win))

    print('%d right, %d wrong, %d neutral, %d total' % (right, len(wrong), neutral, len(lines)))
    if wrong:
        for s in wrong: print('  %s' % s)
    else:
        for w in sorted(remain, reverse=True):
            print(w, remain[w])
            for s in sample[w]: print('  %s' % s)
    #print('%d correct, est. %.2f %% coverage' % (right, float(N)*right/I))
    for i,(p,w) in enumerate(strategy): print('%8.5f .. %8.5f for %s' % (worst[i], best[i], p))
