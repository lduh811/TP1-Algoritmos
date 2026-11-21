#include "poligonos.h"

float vector_producto_escalar(vector_t a, vector_t b) {
    return a.x * b.x + a.y * b.y;   
}

float vector_producto_vectorial(vector_t a, vector_t b) {
    return a.x * b.y - a.y * b.x;    
}

vector_t vector_restar(vector_t a, vector_t b) {
    vector_t resta;
    resta.x = a.x - b.x;
    resta.y = a.y - b.y;
    return resta;
}

vector_t vector_perpendicular(vector_t a) {
    vector_t v = {-a.y,a.x};
    return v;
}

float vector_norma(vector_t a) {
    return sqrt(a.x * a.x + a.y*a.y);
}

void vector_rotar(vector_t v[], size_t n, double a) {
    float cos_a = cos(a); 
    float sin_a = sin(a);
    for (size_t i = 0; i < n; i++) {
        float x_orig = v[i].x;
        float y_orig = v[i].y;  
        v[i].x = x_orig * cos_a - y_orig * sin_a;
        v[i].y = x_orig * sin_a + y_orig * cos_a;
    }
}

void vector_trasladar(vector_t v[], size_t n, vector_t t) {
    for (size_t i = 0; i < n; i++) {
        v[i].x += t.x;  
        v[i].y += t.y;   
    }    
}

vector_t vector_centro_de_masa(const vector_t v[], size_t n) {
    float area = 0, cmx = 0, cmy = 0;
    for (size_t i = 0; i < n; i++) {
        float prod = vector_producto_vectorial(v[i], v[(i + 1) % n]);
        area += prod;
        cmx += (v[i].x + v[(i + 1) % n].x) * prod;
        cmy += (v[i].y + v[(i + 1) % n].y) * prod;
    }
    area *= 0.5;
    cmx *= 1 / (6 * area);
    cmy *= 1 / (6 * area);
    vector_t cm = {cmx, cmy};
    return cm;
}

float vector_radio(const vector_t coordenadas[], size_t n) {
    vector_t cm = vector_centro_de_masa(coordenadas, n);
    float k = 0;
    for(size_t i = 0; i < n; i++) {
        vector_t resta = vector_restar(coordenadas[i],cm);
        float d = vector_norma(resta);
        if(d > k) 
            k = d;
    }
    return k;
}

void poligono_destruir(poligono_t *poligono) {
    free(poligono->cs);
    free(poligono->coordenadas);
    free(poligono);
}

poligono_t *poligono_crear(const vector_t coordenadas[], size_t n) {
    poligono_t *poligono = malloc(sizeof(poligono_t));
    if(poligono == NULL)
        return NULL;
    poligono->coordenadas = malloc(n * sizeof(*coordenadas));
    poligono->cs = malloc(n * sizeof(*poligono->cs));
    if(poligono->coordenadas == NULL || poligono->cs == NULL){
        poligono_destruir(poligono);
        return NULL;
    }
    memmove(poligono->coordenadas, coordenadas, n*sizeof(*coordenadas));

    vector_t cm = vector_centro_de_masa(coordenadas, n);
    vector_t t = {-cm.x,-cm.y};
    vector_trasladar(poligono->coordenadas, n, t);
    memmove(poligono->cs, poligono->coordenadas, n*sizeof(*poligono->cs));

    poligono->n = n;
    poligono->a = 0;
    poligono->cm.x = 0; poligono->cm.y = 0;
    poligono->radio = vector_radio(coordenadas,n);
    
    return poligono;
}

poligono_t *poligono_clonar(const poligono_t *poligono) {
    poligono_t *poligono2 = malloc(sizeof(poligono_t));
    if (poligono2 == NULL) 
        return NULL;

    poligono2->a = poligono->a; poligono2->cm = poligono->cm;
    poligono2->radio = poligono->radio; poligono2->n = poligono->n;

    poligono2->coordenadas = malloc(poligono->n * sizeof(vector_t));
    if (poligono2->coordenadas == NULL) {
        free(poligono2);
        return NULL;
    }
    memcpy(poligono2->coordenadas, poligono->coordenadas, poligono->n * sizeof(vector_t));

    poligono2->cs = malloc(poligono->n * sizeof(vector_t));
    if (poligono2->cs == NULL) {
        free(poligono2->coordenadas);
        free(poligono2);
        return NULL;
    }
    memcpy(poligono2->cs, poligono->cs, poligono->n * sizeof(vector_t));

    return poligono2;
}

size_t poligono_get_largo(const poligono_t *poligono) {
    return poligono->n;
}

vector_t poligono_get_coordenada(const poligono_t *poligono, size_t i) {
    return poligono->cs[i];
}

vector_t poligono_get_cm(const poligono_t *poligono) {
    return poligono->cm;
}

double poligono_get_a(const poligono_t *poligono) {
    return poligono->a;
}

void poligono_transformacion(const poligono_t *poligono, vector_t *cm, double *a) {
    *a = poligono->a;
    *cm = poligono->cm;
}

void poligono_set_cm(poligono_t *poligono, vector_t cm) {
    poligono->cm = cm;
    memcpy(poligono->cs, poligono->coordenadas,
           poligono->n * sizeof(vector_t));

    vector_rotar(poligono->cs, poligono->n, poligono->a);
    vector_trasladar(poligono->cs, poligono->n, poligono->cm);
}

void poligono_set_a(poligono_t *poligono, double a) {
    poligono->a = a;
    memcpy(poligono->cs, poligono->coordenadas,
           poligono->n * sizeof(vector_t));

    vector_rotar(poligono->cs, poligono->n, poligono->a);

    vector_trasladar(poligono->cs, poligono->n, poligono->cm);
}

void poligono_transformar(poligono_t *poligono, vector_t cm, double a) {
    poligono_set_cm(poligono, cm);
    poligono_set_a(poligono,a);
}

vector_t proyectar(vector_t v[], size_t n, vector_t eje) { 
    float min = vector_producto_escalar(v[0],eje);
    float max = vector_producto_escalar(v[0],eje);
    for(size_t j = 1; j < n; j++) {
        float proyeccion = vector_producto_escalar(v[j],eje);
        if(proyeccion < min)
            min = proyeccion;
        if(proyeccion > max)
            max = proyeccion;
    }
    vector_t ab = {min,max};
    return ab;
}

bool poligono_hay_interseccion(const poligono_t *a, const poligono_t *b) {
    double distancia = vector_norma(vector_restar(a->cm, b->cm));
    if(distancia > a->radio + b->radio)
        return false;

    for(size_t i = 0; i < a->n; i++) {
        vector_t lado = vector_restar(a->cs[(i+1)%a->n], a->cs[i]); 
        vector_t normal = vector_perpendicular(lado);

        vector_t proyeccion_a = proyectar(a->cs, a->n, normal);
        vector_t proyeccion_b = proyectar(b->cs, b->n, normal);
        
        if(proyeccion_a.y < proyeccion_b.x || proyeccion_b.y < proyeccion_a.x)
            return false;
    }

    for(size_t i = 0; i < b->n; i++) { 
        vector_t lado = vector_restar(b->cs[(i+1)%b->n], b->cs[i]);
        vector_t normal = vector_perpendicular(lado);

        vector_t proyeccion_a = proyectar(a->cs, a->n, normal);
        vector_t proyeccion_b = proyectar(b->cs, b->n, normal);

        if(proyeccion_a.y < proyeccion_b.x || proyeccion_b.y < proyeccion_a.x)
            return false;
    }

    return true;
}