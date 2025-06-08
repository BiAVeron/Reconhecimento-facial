#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<string.h>
#include<assert.h>
#include <math.h>

/*Definições desenvolvedor usuario*/
typedef struct _reg{
    float embedding[128];
    char id[100];
}treg;

void *aloca_reg(float val0, float val1, const char id[]) {
    treg *reg = malloc(sizeof(treg));
    for (int i = 0; i < 128; i++)
        reg->embedding[i] = 0.0f;
    reg->embedding[0] = val0;
    reg->embedding[1] = val1;
    strncpy(reg->id, id, 99);
    reg->id[99] = '\0';
    return reg;
}


int comparador(void *va, void *vb, int pos) {
    treg *a = (treg*)va;
    treg *b = (treg*)vb;
    if (a->embedding[pos] < b->embedding[pos]) return -1;
    if (a->embedding[pos] > b->embedding[pos]) return  1;
    return 0;
}

double distancia(void *a, void *b) {
    treg *ra = (treg *)a;
    treg *rb = (treg *)b;
    double soma = 0.0;
    for (int i = 0; i < 128; i++) {
        double diff = (double)(ra->embedding[i] - rb->embedding[i]);
        soma += diff * diff;
    }
    return sqrt(soma);
}


/*Definições desenvolvedor da biblioteca*/
typedef struct _node{
    void * key;
    struct _node * esq;
    struct _node * dir;
}tnode;

typedef struct _arv{
    tnode * raiz;
    int (*cmp)(void *, void *, int);
    double (*dist) (void *, void *);
    int k;
}tarv;



/*funções desenvolvedor da biblioteca*/

void kdtree_constroi(tarv * arv, int (*cmp)(void *a, void *b, int ),double (*dist) (void *, void *),int k){
    arv->raiz = NULL;
    arv->cmp = cmp;
    arv->dist = dist;
    arv->k = k;
}

/*teste*/
void test_constroi(){
    /* declaracao de variaveis */
    tarv arv;
    tnode node1;
    tnode node2;

    node1.key = aloca_reg(2,3,"Dourados");
    node2.key = aloca_reg(1,1,"Campo Grande");


    /* chamada de funções */
    kdtree_constroi(&arv,comparador,distancia,128);
    
    /* testes */
    assert(arv.raiz == NULL);
    assert(arv.k == 2);
    assert(arv.cmp(node1.key,node2.key,0) == 1);
    assert(arv.cmp(node1.key,node2.key,1) == 1);
    assert(strcpy(((treg *)node1.key)->id,"Dourados"));
    assert(strcpy(((treg *)node2.key)->id,"Campo Grande"));
    free(node1.key);
    free(node2.key);
}

void _kdtree_insere(tnode **raiz, void * key, int (*cmp)(void *a, void *b, int),int profund, int k){
    if(*raiz == NULL){
        *raiz = malloc(sizeof(tnode));
        (*raiz)->key = key;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
    }else{
        int pos = profund % k;
        if (cmp( (*(*raiz)).key , key ,pos) <0){
            _kdtree_insere( &((*(*raiz)).dir),key,cmp,profund + 1,k);
        }else{
            _kdtree_insere( &((*raiz)->esq),key,cmp,profund +1,k);
        }
    }
}

void kdtree_insere(tarv *arv, void *key){
    _kdtree_insere(&(arv->raiz),key,arv->cmp,0,arv->k);
}


void _kdtree_destroi(tnode * node){
    if (node!=NULL){
        _kdtree_destroi(node->esq);
        _kdtree_destroi(node->dir);
        free(node->key);
        free(node);
    }
}

void kdtree_destroi(tarv *arv){
    _kdtree_destroi(arv->raiz);
}


void _kdtree_busca(tarv *arv, tnode ** atual, void * key, int profund, double *menor_dist, tnode **menor){
    tnode ** lado_principal; 
    tnode ** lado_oposto;    
    if (*atual != NULL){
        double dist_atual = arv->dist((*atual)->key, key);
        if (dist_atual < *menor_dist){
            *menor_dist = dist_atual;
            *menor = *atual;
        }
        int pos = profund % arv->k;
        int comp = arv->cmp(key, (*atual)->key, pos);

        printf("%s dist %4.3f menor_dist %4.3f comp %d\n", ((treg *)((tnode *)*atual)->key)->id,dist_atual,*menor_dist,comp);

        /* define lado principal para buscar */
        if (comp < 0){
            lado_principal =  &((*atual)->esq);
            lado_oposto    =  &((*atual)->dir); 
        }else{
            lado_principal =  &((*atual)->dir);
            lado_oposto    =  &((*atual)->esq); 
        }

        _kdtree_busca(arv, lado_principal, key, profund + 1, menor_dist, menor);

        /* Verifica se deve buscar também no outro lado*/

        double diff = ((treg *)((*atual)->key))->embedding[pos] - ((treg *)(key))->embedding[pos];
        double dist_hyper = diff * diff;
        if (dist_hyper < *menor_dist) {
            _kdtree_busca(arv, lado_oposto, key, profund + 1, menor_dist, menor);
        }
    }
}


tnode * kdtree_busca(tarv *arv, void * key){
    tnode * menor = NULL;
    double menor_dist = DBL_MAX;
    _kdtree_busca(arv,&(arv->raiz),key,0,&menor_dist,&menor);
    return menor;
}

treg buscar_mais_proximo(tarv *arv, treg *query) {
    double menor_dist = 1e20;
    tnode *menor = NULL;
    _kdtree_busca(arv, &(arv->raiz), query, 0, &menor_dist, &menor);
    
    if (menor != NULL) {
        printf("Mais próximo: %s com distância %.3f\n", ((treg *)menor->key)->id, menor_dist);
        return *((treg *)(menor->key));
    } else {
        treg vazio;
        memset(&vazio, 0, sizeof(treg));
        strcpy(vazio.id, "NENHUM_ENCONTRADO");
        return vazio;
    }
}



tarv arvore_global;

tarv* get_tree() {
    return &arvore_global;
}

void inserir_ponto(treg p) {
    treg *novo = malloc(sizeof(treg));
    *novo = p;  // cópia de estrutura
    kdtree_insere(&arvore_global,novo);
}

void kdtree_construir() {
    arvore_global.k = 128;
    arvore_global.dist = distancia;
    arvore_global.cmp = comparador;
    arvore_global.raiz = NULL;
}

void test_busca(){
    tarv arv;
    kdtree_constroi(&arv,comparador,distancia,128);
    kdtree_insere(&arv,aloca_reg(10,10,"a"));
    kdtree_insere(&arv,aloca_reg(20,20,"b"));
    kdtree_insere(&arv,aloca_reg(1,10,"c"));
    kdtree_insere(&arv,aloca_reg(3,5,"d"));
    kdtree_insere(&arv,aloca_reg(7,15,"e"));
    kdtree_insere(&arv,aloca_reg(4,11,"f"));
    tnode * raiz = arv.raiz;
    assert(strcmp(((treg *)raiz->dir->key)->id, "b")==0);
    assert(strcmp(((treg *)raiz->esq->key)->id, "c")==0);
    assert(strcmp(((treg *)raiz->esq->esq->key)->id, "d")==0);
    assert(strcmp(((treg *)raiz->esq->dir->key)->id, "e")==0);

    printf("\n");
    treg  * atual = aloca_reg(7,14,"x");
    tnode * mais_proximo = kdtree_busca(&arv,atual);
    assert(strcmp(((treg *)mais_proximo->key)->id,"e") == 0);

    printf("\n");
    atual->embedding[0] = 9;
    atual->embedding[1] = 9;
    mais_proximo = kdtree_busca(&arv,atual);
    assert(strcmp(((treg *)mais_proximo->key)->id,"a") == 0);

    printf("\n");
    atual->embedding[0] = 4;
    atual->embedding[1] = 5;
    mais_proximo = kdtree_busca(&arv,atual);
    assert(strcmp(((treg *)mais_proximo->key)->id,"d") == 0);

    printf("\n");
    atual->embedding[0] = 4;
    atual->embedding[1] = 9;
    mais_proximo = kdtree_busca(&arv,atual);
    assert(strcmp(((treg *)mais_proximo->key)->id,"f") == 0);



    free(atual);
    kdtree_destroi(&arv);
}



int main(void){
    test_constroi();
    test_busca();
    printf("SUCCESS!!\n");
    return EXIT_SUCCESS;
}