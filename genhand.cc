#include <iostream>
#include <set>

#include "genhand.h"
#include "util.h"

// Generate functionally mostly distinct hands of 5 with or without joker
// IF you need unique normalized hands, create a set out of these and
// iterate that
void gen_hands(function<void(int *)> f, bool joker) {
    int h[5], cards = joker ? 53 : 52;

    // funroll loops :D
    for(h[0]=0; h[0]<cards-4; h[0]+=4) { // only first suit
        for(h[1]=h[0]+1; h[1]<cards-3; h[1]++) {
            if((h[1]&3) > 1) continue;
            for(h[2]=h[1]+1; h[2]<cards-2; h[2]++) {
                if((h[2]&3) > (h[1]&3)+1) continue;
                for(h[3]=h[2]+1; h[3]<cards-1; h[3]++) {
                    int Smax=max(h[1]&3, h[2]&3);
                    if((h[3]&3) > Smax+1) continue;
                    for(h[4]=h[3]+1; h[4]<cards; h[4]++) {
                        int Smax2=max(Smax, h[3]&3);
                        if((h[4]&3) > Smax2+1) continue; // joker survives
                        f(h);
                    }
                }
            }
        }
    }
}

void gen_two_pairs(function<void(int *)> f) {
    int h[5];
    for(h[0]=0; h[0]<12*4; h[0]++) { // Smaller pair & suit
        for(h[1]=h[0]+1; h[0]/4==h[1]/4; h[1]++) { // bigger suit
            for(h[2]=(h[0]|3)+1; h[2]<52; h[2]++) { // Bigger pair, smaller suit
                for(h[3]=h[2]+1; h[3]&3; h[3]++) { // remaining bigger suits
                    for(h[4]=0; h[4]<52; h[4]++) { // Joker not possible
                        if(h[4]/4==h[0]/4 || h[4]/4==h[2]/4) continue;
                        f(h);
                    }
                }
            }
        }
    }
}

void gen_sets(function<void(int *)> f) {
    int h[5];
    int s[3] = {0,1,2}; // suits 0-3 and joker=4
    do {
        for(int c=0; c<13; c++) {
            for(int i=0; i<3; i++)
                h[i] = s[i]==4 ? 52 : c*4+s[i];
            for(h[3]=0; h[3]<52; h[3]++) { // replace first
                if(h[3]/4 == c) continue; // no quads
                for(h[4]=(h[3]|3)+1; h[4]<52; h[4]++) {
                    // no quads or full houses
                    if(h[4]/4!=c && (h[4]/4)!=(h[3]/4))
                        f(h);
                }
            }
        }
    } while(next_combi(s, 3, 4));
}

void gen_straights(function<void(int *)> f) {
    int h[5];
    for(int c=0; c<10; c++) {
        for(int s=1; s<1023; s++) { // skip 0 and 0b1111111111
            if(s==0x155 || s==0x2AA) continue; // skip other flushes
            for(int i=0; i<5; i++) h[i]=((c+i+12)%13)*4+((s>>(i*2))&3);
            f(h); // pure non-joker non-flush straight
            for(int j=0; j<5; j++) {
                // don't double count ?2345 and 2345?
                if(j==0 && c!=9) continue;
                if(h[j]&3) continue; // only replace one suit to avoid doubles
                int t=h[j]; // store
                h[j]=52; // replace joker
                int suit = h[j?0:1]&3;
                int newsuit = (s&(0x3FF-(3<<j*2))) + (suit<<j*2);
                if(suit*0x155 != newsuit) f(h); // skip flushes here as well
                h[j]=t; // restore
            }
        }
    }
}

void gen_flushes(function<void(int *)> f) {
    int h[5];
    int c[5] = {0,1,2,3,4};
    do { // with joker at the end (value 13)
        // skip simple straights and ?2345
        if((c[4]-c[0]==4 || (c[3]==3 && c[4]>=12))) continue;
        for(int s=0; s<4; s++) {
            for(int i=0; i<5; i++) h[i] = c[i]*4+s;
            if(c[4]==13) { // Joker in play
                h[4]=52; // remove suit from joker
                // TODO: Faster straight flush check?
                if(win(h) != 30) f(h);
            } else f(h); // no joker so no need to check
        }
    } while(next_combi(c, 5, 13));
}

void gen_full_houses(function<void(int *)> f) {
    int h[5];
    int s[3] = {0, 1, 2}; // set suits, 4=joker
    do {
        for(int c=0; c<13; c++) { // set value
            for(int i=0; i<3; i++) h[i] = c*4+s[i];
            if(s[2]==4) h[2] = 52; // joker

            // With a joker, select only bigger pairs to avoid duplicates
            for(h[3]= (h[2]==52 ? c*4+4 : 0); h[3]<52; h[3]++)
                if(h[3]/4 != c) // no fives
                    for(h[4]=h[3]+1; h[4]&3; h[4]++) // bigger suit
                        f(h);
        }
    } while(next_combi(s, 3, 4));
}

void gen_fours(function<void(int *)> f) {
    int h[5];
    for(int c=0; c<13; c++) {
        for(int j=0; j<5; j++) {
            for(int i=0; i<4; i++) h[i] = c*4+i;
            h[j] = 52; // replace with joker (when j=4 this will be overwritten)
            for(h[4]=0; h[4]<52; h[4]++)
                if((h[4]>>2) != c) f(h);
        }
    }
}

void gen_straight_flushes(function<void(int *)> f) {
    int h[5];
    for(int c=0; c<10; c++) {
        for(int s=0; s<4; s++) {
            for(int i=0; i<5; i++) h[i]=((c+i+12)%13)*4+s;
            f(h); // pure non-joker non-flush straight
            for(int j=0; j<5; j++) {
                // don't double count ?2345 and 2345?
                if(j==0 && c!=9) continue;
                int t=h[j]; // store
                h[j]=52; // replace joker
                f(h);
                h[j]=t; // restore
            }
        }
    }
}

void gen_fives(function<void(int *)> f) {
    int h[5];
    h[4] = 52;
    for(int c=0; c<13; c++) {
        for(int i=0; i<4; i++) h[i] = c*4+i;
        f(h);
    }
}

// Generate base cases for different hands yielding different amount of
// paired hands (five, four, three of a kind, full house, two pairs)
// Suits don't matter, nor do values, only relative numbers of values.
// Still, we return suits to support hand enumeration
void gen_base_pairs(function<void(int *)> f, bool joker) {
    int h[5];

    for(int top=1; top<=4; top++) {
        for(int i=0; i<top; i++) h[i] = i;
        for(int sec=1; sec <= top && top+sec <= 5; sec++) {
            for(int i=top; i<top+sec; i++) h[i] = 4 + (i-top);
            for(int i=top+sec; i<5; i++) h[i] = 8 + (i-top-sec)*4;
            f(h);
            if(joker && !(top==3 && sec==2)) { // 22233 and 22234
                h[4]=52;
                f(h);
            }
        }
    }
}
