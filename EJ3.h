#ifndef EJ3_H
#define EJ3_H

#include "poligonos.h"
#include <stdint.h>
#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

enum materiales_t {MADERA, VIDRIO, PIEDRA, FIJO, PERSONAJE, LETRA, FONDO};

struct geometria {
    char nombre[15];
    float ancho;
    float alto;
    int16_t n_coords;
    vector_t *cs;

};

typedef struct geometria geometria_t;

struct asset {
    char geometria[15];
    int8_t material;
    int8_t version;
    char archivo[50];
    int16_t posicion[2];
    int16_t tamano[2];
};
typedef struct asset asset_t;

struct bloque {
    char geometria[15];
    int8_t material;
    vector_t cm;
    double angulo;
};
typedef struct bloque bloque_t;

struct nivel {
    int16_t n_bloques;
    bloque_t *bloques;
};
typedef struct nivel nivel_t;


struct geometria *leer_geometrias(FILE *f, size_t *n);
struct asset *leer_assets(FILE *f, size_t *n);
struct nivel *leer_nivel(FILE *f);
#endif
