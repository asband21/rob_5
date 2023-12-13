#ifndef ROLLING_AVERAGE_H
#define ROLLING_AVERAGE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    double *buffer;
    int size;
    int index;
    int count;
    double sum;
} rolling_average;

// Function prototypes
rolling_average* init_rolling_average(int size);
double add_value_get_average(rolling_average *ra, double new_value);
void free_rolling_average(rolling_average *ra);

// Implementations
rolling_average* init_rolling_average(int size) {
    rolling_average *ra = (rolling_average *)malloc(sizeof(rolling_average));
    if (ra == NULL) {
        perror("Failed to allocate rolling_average");
        return NULL;
    }

    ra->buffer = (double *)malloc(size * sizeof(double));
    if (ra->buffer == NULL) {
        perror("Failed to allocate buffer");
        free(ra);
        return NULL;
    }

    ra->size = size;
    ra->index = 0;
    ra->count = 0;
    ra->sum = 0.0;

    for (int i = 0; i < size; i++) {
        ra->buffer[i] = 0.0;
    }

    return ra;
}

double add_value_get_average(rolling_average *ra, double new_value) {
    if (ra == NULL) return 0.0;

    ra->sum -= ra->buffer[ra->index];
    ra->buffer[ra->index] = new_value;
    ra->sum += new_value;

    ra->index = (ra->index + 1) % ra->size;
    ra->count = ra->count < ra->size ? ra->count + 1 : ra->size;

    return ra->sum / ra->count;
}

void free_rolling_average(rolling_average *ra) {
    if (ra) {
        free(ra->buffer);
        free(ra);
    }
}

#endif // ROLLING_AVERAGE_H
