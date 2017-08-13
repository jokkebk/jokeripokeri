import sys
from random import randint
from collections import Counter
from itertools import product, combinations
from util import *
from jokeri import win

# Match hands to patterns and return matching selection or 0 if no match
# Following patterns are supported:
#
# Pair-type patterns:
# uuuu and uuu? = fours (u is the most_common value)
# uuuvv and uuvv? = full house (v is the 2nd most_common value)
#
# Flush-type patterns:
# sssss and ssss? = flush (s is the most common suit)
# ssss and sss? = flush draw
#
# Shortcuts
# !s = straight flush with or without joker
# ! = straight with or without joker
#
# Straight-type patterns:
# A23456789T abcde = all straights
# 3456789T abc s? = open-ended straight flush draw with joker
#
# Joker goes always last.
def match_hand(h, p, skip=0):
    h = sorted(h)
    if h[4]==52:
        jmask = 16
        del h[4] # take joker out
    else: jmask = 0
    if not jmask and p[-1] == '?': return 0 # no required joker

    if p[0] == '!': # Straight flush finder
        if p[-1]=='s' and any((c&3) != (h[0]&3) for c in h[1:]):
            return 0 # not suited
        hmask = sum(1<<c//4 for c in h)
        if h[0]<8 and h[-1]//4==12:
            hmask = (hmask&0xFFF)*2+1 # ROL the ace to bottom
        # Binary search a shift that brings lowermost bit to bottom
        step = 8
        shift = 0
        while step:
            if not (hmask & (1<<shift+step)-1): shift += step
            step //= 2
        if bin((hmask>>shift)&31).count('1') + (1 if jmask else 0) == 5:
            return 31
    elif p[0] == 'u': # Pair-type pattern
        vc = Counter(c>>2 for c in h).most_common(2)
        if p.count('u') != vc[0][1]: return 0
        v = p.count('v')
        if v == 0: return jmask + val_mask(h, vc[0][0])
        elif p.count('v') == vc[1][1]:
            return jmask + val_mask(h, vc[0][0]) + val_mask(h, vc[1][0])
    elif p[0] == 's': # Flush-type pattern
        suit, sc = Counter(c&3 for c in h).most_common(1)[0]
        #if jmask: sc += 1 # Count joker in all suits
        if p.count('s') == sc:
            return jmask + sum(1<<i for i,c in enumerate(h) if (c&3)==suit)
    else: # Straight-type pattern
        val_pat = p.split()
        hmasks = [] # Do it with arrays to support suited
        if len(val_pat)>2 and val_pat[2][0]=='s': # Suited
            suited = True
            for s in set(c&3 for c in h):
                hmask = sum(1<<c//4 for c in h if (c&3)==s)
                if bin(hmask).count('1') >= len(val_pat[1]):
                    hmasks.append((hmask, s))
        else:
            suited = False
            hmasks.append((sum(1<<c//4 for c in h), 0))

        mask = sum(1<<ord(c)-ord('a') for c in val_pat[1])

        match = False
        for v in val_pat[0]:
            v = valS.index(v)
            for hmask, s in hmasks:
                #if debug: print(v, 'vs', bin(hmask)[2:])
                if v==12: # Ace
                    if not (hmask&0x1000): continue
                    if ((hmask&0xF) & mask//2) != mask//2: continue
                else:
                    if ((hmask>>v) & mask) != mask: continue
                if skip: # skip=1 can be used to return second match
                    skip -= 1
                    continue
                if suited:
                    #if debug: print('YAY!')
                    return sum(card_mask(h, ((ord(c)-ord('a')+v)%13)*4+s) \
                            for c in val_pat[1]) + jmask
                else:
                    return sum(val_mask(h, (ord(c)-ord('a')+v)%13) \
                            for c in val_pat[1]) + jmask

    return 0

if __name__ == "__main__":
    pat = [
            'A23456789T abcde s', # straight flush
            'A23456789T acde s?', # straight flush
            'A23456789T abde s?', # straight flush
            'A23456789T abce s?', # straight flush
            'A23456789TJ abcd s?', # straight flush

            'A23456789T abcde', # straight flush
            'A23456789T acde ?', # straight flush
            'A23456789T abde ?', # straight flush
            'A23456789T abce ?', # straight flush
            'A23456789TJ abcd ?', # straight flush
    #'uuuu?', # fives
    #'uuuu', 'uuu?', # fours
    #'uuuvv', 'uuvv?', # full house
    #'uuu', 'uu?', 'uuvv', # set or two pair
    #'3456789T abc s?', # super straight flush draw
    ]

    for h in combinations(range(0,53), 5):
        #if any(c&3 for c in h): continue # TODO: remove
        #if win(h)==30: print('xxx', hand_str(h))
        #else: continue
        for p in pat:
            sel = match_hand(h, p)
            if sel:
                print(hand_str(h, sel))
                break
