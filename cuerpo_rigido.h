#ifndef CUERPO_RIGIDO_H
#include "poligonos.h"
typedef struct {
    poligono_t geometria;
    vector_t v;
    float w;
    const float m;
    const float i;
} cuerpo_rigido_t;

cuerpo_rigido_t *crear_cr_vacio();
void destruir_cr;
void inicializar_cr(poligono_t geom, vector_t v, float w);

#endif
