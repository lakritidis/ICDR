#ifndef FSTPACKED_H
#define FSTPACKED_H


typedef struct {
    unsigned char label;
    uint32_t target;
} Arc;

typedef struct {
    uint32_t first_arc;
    uint32_t num_arcs;
    uint32_t output;
    uint8_t  is_final;
} State;

typedef struct {
    State *states;
    Arc   *arcs;
    uint32_t num_states;
    uint32_t num_arcs;
} FST;

static int find_arc(FST *fst, State *state, unsigned char label) {
    int left = 0;
    int right = state->num_arcs - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        Arc *arc = &fst->arcs[state->first_arc + mid];

        if (arc->label == label)
            return arc->target;
        else if (arc->label < label)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return -1;
}


/* ---------------- Lookup ---------------- */

int fst_lookup(FST *fst, const char *key, uint32_t *out) {
    uint32_t state_index = 0; // start state

    for (size_t i = 0; i < strlen(key); i++) {
        State *s = &fst->states[state_index];
        int next = find_arc(fst, s, (unsigned char)key[i]);
        if (next < 0)
            return 0;
        state_index = (uint32_t)next;
    }

    State *final = &fst->states[state_index];
    if (final->is_final) {
        *out = final->output;
        return 1;
    }

    return 0;
}


/* ---------------- Example Builder ---------------- */
/*
   For simplicity, we manually build this FST for:
   cat -> 1
   car -> 2
   dog -> 3
*/

FST *build_example() {
    FST *fst = malloc(sizeof(FST));

    fst->num_states = 8;
    fst->num_arcs   = 7;

    fst->states = calloc(fst->num_states, sizeof(State));
    fst->arcs   = calloc(fst->num_arcs, sizeof(Arc));

    /*
        State layout:

        0 - root
        1 - c
        2 - ca
        3 - cat (final)
        4 - car (final)
        5 - d
        6 - do
        7 - dog (final)
    */

    // ----- Root -----
    fst->states[0].first_arc = 0;
    fst->states[0].num_arcs  = 2;

    fst->arcs[0] = (Arc){'c', 1};
    fst->arcs[1] = (Arc){'d', 5};

    // ----- c -----
    fst->states[1].first_arc = 2;
    fst->states[1].num_arcs  = 1;
    fst->arcs[2] = (Arc){'a', 2};

    // ----- ca -----
    fst->states[2].first_arc = 3;
    fst->states[2].num_arcs  = 2;
    fst->arcs[3] = (Arc){'r', 4};
    fst->arcs[4] = (Arc){'t', 3};

    // ----- cat -----
    fst->states[3].is_final = 1;
    fst->states[3].output = 1;

    // ----- car -----
    fst->states[4].is_final = 1;
    fst->states[4].output = 2;

    // ----- d -----
    fst->states[5].first_arc = 5;
    fst->states[5].num_arcs  = 1;
    fst->arcs[5] = (Arc){'o', 6};

    // ----- do -----
    fst->states[6].first_arc = 6;
    fst->states[6].num_arcs  = 1;
    fst->arcs[6] = (Arc){'g', 7};

    // ----- dog -----
    fst->states[7].is_final = 1;
    fst->states[7].output = 3;

    return fst;
}


/* ---------------- Cleanup ---------------- */

void fst_free(FST *fst) {
    free(fst->states);
    free(fst->arcs);
    free(fst);
}
#endif // FSTPACKED_H
