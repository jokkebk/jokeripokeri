from collections import Counter
import sys

I = 0
cnt = Counter()
for line in open(sys.argv[1]):
    line = line.strip()
    if line.count('[') != 1: continue
    a, b = line.split('[')
    c = b[0]
    cnt[c] += 1
    I += 1

for i,(c,v) in enumerate(sorted(((c,v) for v,c in cnt.items()), reverse=True)):
    print('%d. %s %.2f %% (%d / %d)' % (i+1, v, c/I, c, I))
