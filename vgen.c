#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <signal.h>
#include <math.h>

#include "ed25519/ed25519.h"
#include "hashset/hashset.h"
#include "stellar.h"
#include "seed.h"


volatile sig_atomic_t sigint_received = 0;
int minlen = INT32_MAX;
int maxlen = 0;
const int nbench = 10000;


void sigint_handler(int s)
{
    sigint_received = 1;
}


double exp_avg(double x, int idx)
{
    const double alpha = 0.05;
    static double xhist[2] = { -1.0, -1.0 };

    xhist[idx] = xhist[idx] < 0 ? x : alpha * x + (1 - alpha) * xhist[idx];

    return(xhist[idx]);
}


int read_keywords(const char* filename, set* hs)
{
    FILE *infile = fopen(filename, "r");
    assert(infile != NULL);

    size_t linesiz = 0;
    char* linebuf = NULL;
    int linelen = 0;
    int count = 0;

    // getline will (re)alloc if necessary..
    while ((linelen = getline(&linebuf, &linesiz, infile)) != -1) {
        // skip empty or short lines
        if (linelen <= 4) continue;
        const int ll = linelen - 1;
        assert(!set_add(hs, linebuf, ll));
        if (ll > maxlen)
            maxlen = ll;
        if (ll < minlen)
            minlen = ll;
        count++;
    }

    free(linebuf);
    fclose(infile);
    return(count);
}


int main()
{
    uint8_t public_key[32], private_key[64];
    uint8_t address[57] = { 0 };
    uint8_t secret[57] = { 0 };
    uint64_t seed[4];

    clock_t oldtime;
    clock_t newtime;
    int i = 0;
    int l = 0;
    int count = 0;
    int hit = 0;
    int oldhit = 0;

    signal(SIGINT, sigint_handler);

    /* LOAD KEYWORDS INTO A HASHSET */
    set *hs = NULL;
    assert(!set_alloc(&hs, 125000*2, 1024*1024*2, hasher));
    count = read_keywords("names/NAMES.txt", hs);
    fprintf(stderr, "loaded %d keywords, minlen = %d, maxlen = %d\n", count, minlen, maxlen);

    /* ADDRESS GENERATION */
    fprintf(stderr, "generating keys...\n");
    init_seed(seed);
    oldtime = clock();
    while (!sigint_received) {
        ed25519_create_keypair(public_key, private_key, (uint8_t *)seed);
        stellar_address(address, public_key);
        for (l = maxlen; l >= minlen; l--) {
            if (!set_find(hs, address + 56 - l, l)) {
                stellar_secret(secret, (uint8_t *)seed);
                printf("{\"suffix\": \"%s\", \"address\": \"%s\", \"secret\": \"%s\"}\n", address + 56 - l, address, secret);
                hit++;
                break;
            }
        }
        next_seed(seed);
        i++;
        if (i % nbench == 0) {
            newtime = clock();
            double dt = 1.0 * (newtime - oldtime) / CLOCKS_PER_SEC;
            fprintf(stderr, "generation rate: %g addr/sec\thit rate: %g addr/sec\n",
                    round(exp_avg(1.0 * nbench / dt, 0)),
                    round(10.0 * exp_avg(1.0 * (hit - oldhit) / dt, 1)) / 10.0);
            oldtime = newtime;
            oldhit = hit;
        }
    }

    assert(!set_free(&hs));

    return 0;
}
