/*------ARQUIVOS OBJETOS--------*/

enum IDS {JOGADOR, PROJETIL, INIMIGOS,ESTRELA};

typedef struct{
    int ID;
    int x;
    int y;
    int vidas;
    int velocidade;
    int borda_x;
    int borda_y;
    int pontos;
}NaveEspacial;

typedef struct{
    int ID;
    int x;
    int y;
    int velocidade;
    bool ativo;
}Projeteis;

typedef struct{
    int ID;
    int x;
    int y;
    float velocidade;
    int borda_x;
    int borda_y;
    bool correct;
    bool ativo;
}Cometas;
typedef struct{
    int ID;
    int x;
    int y;
    int velocidade;

}Estrelas;

typedef struct{
    char quest[50];
    int result;
    int valores[10];
    int correct;
}Calculo;

