from collections import Counter
from itertools import product, combinations
import sys
from match import match_hand

#I = 0
#N = 1000 if len(sys.argv) < 3 else int(sys.argv[2])

deckS = [''.join(p) for p in product('23456789TJQKA', 'CDHS')]+['??']

odds = {} # category -> (min, max) pairs 

for line in open(sys.argv[1]):
    if len(line) < 40: continue
    line = line.strip()
    handStr, handRet, handSel = line[0:14], line[14:24], line[24:]
    hand = [deckS.index(s.upper()) for s in handStr.split()]
    handRet = float(handRet)

    valC = Counter(c//4 for c in hand).most_common(2)
    suitC = list(Counter(c%4 for c in hand if c!=52).most_common(1)[0])
    
    selected = 'sel%d' % ((len(handSel) - len(handStr)) // 2);
    # Categorize hands
    joker = (52 in hand)
    if valC[0][1]==4:
        cat = 'fives' if joker else 'fours'
    elif valC[0][1]==3:
        if joker: cat = 'fours'
        else: cat = 'full_house' if valC[1][1]==2 else 'set'
    elif valC[0][1]==2:
        if valC[1][1]==2: cat = 'full_house' if joker else 'two_pair'
        else: cat = 'set' if joker else 'one_pair'
    else:
        cat = 'no_pair'

    if cat in odds:
        oddMin, oddMax = odds[cat]
        odds[cat] = (min(oddMin, handRet), max(oddMax, handRet))
    else:
        odds[cat] = (handRet, handRet)

    if joker: suitC[1] += 1
    flush = 'flush%d' % suitC[1]

    if flush in odds:
        oddMin, oddMax = odds[flush]
        odds[flush] = (min(oddMin, handRet), max(oddMax, handRet))
    else:
        odds[flush] = (handRet, handRet)

    if flush == 'flush5':
        guess = ' '.join('[%s]'%deckS[c] for c in hand)
        guess = 'CORRECT' if guess==handSel else 'WRONG'
    elif cat=='no_pair' or cat=='one_pair':
        guess = 'NO GUESS'
    else:
        guess = []
        for c in hand:
            if c==52 or c//4==valC[0][0] or (c//4==valC[1][0] and valC[1][1]>1):
                guess.append('[%s]' % deckS[c])
            else:
                guess.append(deckS[c])
        guess = 'CORRECT' if ' '.join(guess)==handSel else 'WRONG'

    print(selected, cat, flush, handStr, '[%s]'%handSel, guess)
    #print('   ', hand, valC, suitC)

for cat in odds:
    oddMin, oddMax = odds[cat]
    print(cat, oddMin, oddMax)
