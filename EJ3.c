#include "EJ3.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


struct geometria *leer_geometrias(FILE *f, size_t *n) {
    if (f == NULL) {
        fprintf(stderr, "Error: Archivo inválido en leer_geometrias.\n");
        return NULL;
    }

    int16_t n_geom_leido = 0;
    if (fread(&n_geom_leido, sizeof(int16_t), 1, f) != 1) {
        fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
        return NULL;
    }
    *n = (size_t)n_geom_leido;

    geometria_t *geoms = malloc(sizeof(geometria_t) * (*n));
    if (geoms == NULL) return NULL;

    for (size_t i = 0; i < *n; i++) {
        if (fread(geoms[i].nombre, sizeof(char), 15, f) != 15) {
            free(geoms); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
            }
        if (fread(&geoms[i].ancho, sizeof(float), 1, f) != 1) {
            free(geoms); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if (fread(&geoms[i].alto, sizeof(float), 1, f) != 1) {
            free(geoms); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if (fread(&geoms[i].n_coords, sizeof(int16_t), 1, f) != 1) {
            free(geoms); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }

        geoms[i].cs = malloc(sizeof(vector_t) * geoms[i].n_coords);
        if (geoms[i].cs == NULL) {
            for (size_t j = 0; j < i; j++) {
                free(geoms[j].cs);
            }
            free(geoms);
            return NULL;
        }

        if (fread(geoms[i].cs, sizeof(vector_t), geoms[i].n_coords, f) != geoms[i].n_coords) {
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            free(geoms[i].cs);
            for (size_t j = 0; j < i; j++) {
                free(geoms[j].cs);
            }
            free(geoms);
            return NULL;
        }
    }
    return geoms;
}

struct asset *leer_assets(FILE *f, size_t *n) {
    if (f == NULL) {
        fprintf(stderr, "Error: Archivo inválido en leer_assets.\n");
        return NULL;
    }

    int16_t n_assets = 0;
    if (fread(&n_assets, sizeof(int16_t), 1, f) != 1) {
        fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
        return NULL;
    }
    *n = (size_t)n_assets;

    asset_t *assets = malloc(sizeof(asset_t) * (*n));
    for(size_t i = 0; i < *n; i++) {
        if(fread(assets[i].geometria, sizeof(char), 15, f) != 15) {
            free(assets); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(&assets[i].material, sizeof(int8_t), 1, f) != 1) {
            free(assets); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(&assets[i].version, sizeof(int8_t), 1, f) != 1) {
            free(assets); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(assets[i].archivo, sizeof(char), 50, f) != 50) {
            free(assets); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(assets[i].posicion, sizeof(int16_t), 2, f) != 2){
            free(assets); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(assets[i].tamano, sizeof(int16_t), 2, f) != 2) {
            free(assets); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
    }
    return assets;
}

cuerpo_rigido_t *crear_cr_vacio(void) {
    return calloc(1, sizeof(cuerpo_rigido_t));
}

void destruir_cr(cuerpo_rigido_t *cr) {
    if (cr == NULL)
        return;
    poligono_destruir(&cr->geometria);
    free(cr);}

static float momento_inercia(const poligono_t *p, float masa) {
    size_t n = poligono_cantidad_vertices(p);
    float num = 0, den = 0;

    for (size_t i = 0; i < n; i++) {
        vector_t a = poligono_vertice(p, i);
        vector_t b = poligono_vertice(p, (i + 1) % n);  //el último se une con el primero

        float cruz = producto_vectorial_modulo(a, b);
        float q = producto_interno(a, a) + producto_interno(a, b) + producto_interno(b, b);

        num += q * cruz;
        den += cruz;
    }

    if (den == 0)
        return 0;  // polígono degenerado
    return masa / 6.0f * num / den;
}

bool inicializar_cr(cuerpo_rigido_t *cr, poligono_t geom, vector_t v, float w, float densidad) {
    if (cr == NULL)
        return false;

    float m_inv = 0, i_inv = 0; 

    if (densidad > 0) {
        float masa = densidad * poligono_area(&geom);
        float inercia = momento_inercia(&geom, masa);
        if (masa > 0 && inercia > 0) {
            m_inv = 1.0f / masa;
            i_inv = 1.0f / inercia;
        }
    }
    cuerpo_rigido_t tmp = {
        .geometria = geom,
        .v = v,
        .w = w,
        .m_inv = m_inv,
        .i_inv = i_inv,
    };
    memcpy(cr, &tmp, sizeof(tmp));
    return true;
}

void actualizar_cr(cuerpo_rigido_t *cr, float dt) {
    poligono_trasladar(&cr->geometria, cr->v.x * dt, cr->v.y * dt);  // cm' 
    poligono_rotar(&cr->geometria, cr->w * dt);                      // a' 
}


struct nivel *leer_nivel(FILE *f) {
    if (f == NULL) {
        fprintf(stderr,"Error: archivo corrupto o incompleto.");
        return NULL;
    }

    int16_t n_bloques = 0;
    if(fread(&n_bloques, sizeof(int16_t), 1, f) != 1) {
        fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
        return NULL;
    }

    struct nivel *nivel = malloc(sizeof(nivel_t));
    if(nivel == NULL) return NULL;
    nivel->bloques = malloc(32 * (n_bloques));
    if(nivel->bloques == NULL) {free(nivel); return NULL;};
    nivel->n_bloques = n_bloques;

    for(size_t i = 0; i < n_bloques; i++) {
        if(fread(nivel->bloques[i].geometria, sizeof(char), 15, f) != 15) {
            free(nivel->bloques); free(nivel); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(&nivel->bloques[i].material, sizeof(int8_t), 1, f) != 1) {
            free(nivel->bloques); free(nivel); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(&nivel->bloques[i].cm, sizeof(vector_t), 1, f) != 1) {
            free(nivel->bloques); free(nivel); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
        if(fread(&nivel->bloques[i].angulo, sizeof(double), 1, f) != 1) {
            free(nivel->bloques); free(nivel); 
            fprintf(stderr,"Error: archivo corrupto o incompleto. \n");
            return NULL;
        }
    }
    return nivel;
}
