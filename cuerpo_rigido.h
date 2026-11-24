#ifndef CUERPO_RIGIDO_H
#include "poligonos.h"
typedef struct {
    poligono_t geometria;
    vector_t v;
    float w;
    const float m_inv; // 1/m (masa infinita , cuerpo fijo)
    const float i_inv; // 1/I (0=> inercia infinita, no rota por fuerzas)
} cuerpo_rigido_t;

cuerpo_rigido_t *crear_cr_vacio(); //reserva memoria cr con todo en cero
void destruir_cr(cuerpo_rigido_t *cr); //libera la geometria y el cuerpo
void inicializar_cr(cuerpo_rigido_t *cr, poligono_t geom, vector_t v, float w, float densidad); //"carga" el cuerpo. calcula m_inv y i_inv a partir de la geometria y densidad
void actualizar_cr(cuerpo_rigido_t *cr, float dt);
#endif
