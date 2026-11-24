#ifndef POLIGONOS_H
#define POLIGONOS_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define PI 3.14159265358979323846

typedef struct {
    float x, y;
} vector_t;
                                                        
typedef struct {
    vector_t *coordenadas; // Arreglo dinámico de n coordenadas, con centro de masa en el origen
    size_t n; // Cantidad de coordenadas.
    vector_t cm; // Translación
    double a; // Rotación
    vector_t *cs; // Arreglo dinámico con las n coordenadas rotadas a y transladadas cm
    float radio; // Radio del polígono
} poligono_t;

//Devuelve el producto escalar entre los vectores a y b
float vector_producto_escalar(vector_t a, vector_t b);

//Devuelve el producto vectorial entre los vectores a y b
float vector_producto_vectorial(vector_t a, vector_t b);

vector_t vector_restar(vector_t a, vector_t b);

vector_t vector_perpendicular(vector_t a);

float vector_norma(vector_t a);

void vector_rotar(vector_t v[], size_t n, double a);

void vector_trasladar(vector_t v[], size_t n, vector_t t);

float area_poligono(poligono_t *p);

vector_t vector_centro_de_masa(const vector_t v[], size_t n);

float vector_radio(const vector_t coordenadas[], size_t n);

void poligono_destruir(poligono_t *poligono);

poligono_t *poligono_crear(const vector_t coordenadas[], size_t n);

poligono_t *poligono_clonar(const poligono_t *poligono);

size_t poligono_get_largo(const poligono_t *poligono);

vector_t poligono_get_coordenada(const poligono_t *poligono, size_t i);

vector_t poligono_get_cm(const poligono_t *poligono);

double poligono_get_a(const poligono_t *poligono);

void poligono_set_cm(poligono_t *poligono, vector_t cm);

void poligono_set_a(poligono_t *poligono, double a);

void poligono_transformar(poligono_t *poligono, vector_t cm, double a);

bool poligono_hay_interseccion(const poligono_t *a, const poligono_t *b, vector_t *n, float *d);

#endif
