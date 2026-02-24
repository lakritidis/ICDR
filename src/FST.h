#ifndef FST_H
#define FST_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ALPHABET_SIZE 256

typedef struct State State;

typedef struct {
    unsigned char symbol;
    State *target;
} Transition;

struct State {
    int is_final;
    uint32_t output;

    Transition *transitions;
    size_t num_transitions;
    size_t capacity;
};

typedef struct {
    State *start;
} FST;


/* ---------------- State Utilities ---------------- */

State *create_state() {
    State *s = (State *)malloc(sizeof(State));
    s->is_final = 0;
    s->output = 0;
    s->transitions = NULL;
    s->num_transitions = 0;
    s->capacity = 0;
    return s;
}

void add_transition(State *from, unsigned char symbol, State *to) {
    // Expand array if needed
    if (from->num_transitions == from->capacity) {
        from->capacity = from->capacity == 0 ? 4 : from->capacity * 2;
        from->transitions = (Transition *)realloc(from->transitions,
                                    from->capacity * sizeof(Transition));
    }

    from->transitions[from->num_transitions].symbol = symbol;
    from->transitions[from->num_transitions].target = to;
    from->num_transitions++;
}

State *find_transition(State *from, unsigned char symbol) {
    for (size_t i = 0; i < from->num_transitions; i++) {
        if (from->transitions[i].symbol == symbol) {
            return from->transitions[i].target;
        }
    }
    return NULL;
}


/* ---------------- FST Operations ---------------- */

FST *fst_create() {
    FST *fst = (FST *)malloc(sizeof(FST));
    fst->start = create_state();
    return fst;
}

void fst_insert(FST *fst, const char *key, uint32_t value) {
    State *current = fst->start;

    for (size_t i = 0; i < strlen(key); i++) {
        unsigned char c = (unsigned char)key[i];
        State *next = find_transition(current, c);

        if (!next) {
            next = create_state();
            add_transition(current, c, next);
        }

        current = next;
    }

    current->is_final = 1;
    current->output = value;
}

int fst_lookup(FST *fst, const char *key, uint32_t *out_value) {
    State *current = fst->start;

    for (size_t i = 0; i < strlen(key); i++) {
        unsigned char c = (unsigned char)key[i];
        current = find_transition(current, c);
        if (!current)
            return 0; // not found
    }

    if (current->is_final) {
        *out_value = current->output;
        return 1;
    }

    return 0;
}


/* ---------------- Prefix Traversal ---------------- */

void dfs_print(State *state, char *buffer, int depth) {
    if (state->is_final) {
        buffer[depth] = '\0';
        printf("%s -> %u\n", buffer, state->output);
    }

    for (size_t i = 0; i < state->num_transitions; i++) {
        buffer[depth] = state->transitions[i].symbol;
        dfs_print(state->transitions[i].target, buffer, depth + 1);
    }
}

void fst_print_all(FST *fst) {
    char buffer[1024];
    dfs_print(fst->start, buffer, 0);
}


/* ---------------- Cleanup ---------------- */

void free_state(State *state) {
    for (size_t i = 0; i < state->num_transitions; i++) {
        free_state(state->transitions[i].target);
    }
    free(state->transitions);
    free(state);
}

void fst_free(FST *fst) {
    free_state(fst->start);
    free(fst);
}


#endif // FST_H
