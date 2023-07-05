#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <cstdlib>
#include <ctime>
#include "allegro_util.h"
#include "objetos.h"

/* -------VARIAVEIS GLOBAIS--------*/
const int largura_t=800;
const int altura_t=600;
const int FPS = 60;
const int NUM_BALAS = 5;
const int NUM_COMETAS = 4;
const int NUM_ESTRELAS = 100;
const int NUM_PLANOS = 3;
bool attCalc = false;
bool iniciaJogo = false;
bool retomaJogo = false;

ALLEGRO_SAMPLE *trilha_sonora = NULL;
ALLEGRO_SAMPLE *laser = NULL;
ALLEGRO_SAMPLE *acerto = NULL;
ALLEGRO_SAMPLE *erro = NULL;
ALLEGRO_SAMPLE *aplausos = NULL;

ALLEGRO_SAMPLE_INSTANCE *inst_trilha_sonora = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_laser = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_acerto = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_erro = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_aplausos = NULL;

enum TECLAS {CIMA,BAIXO,ESQUERDA,DIREITA,SPACE, ENTER};

/*________________________________
 -------PROTÓTIPOS--------------*/
void InitNave (NaveEspacial &nave);
void DesenhaNave(NaveEspacial &nave);

void MoveNaveCima(NaveEspacial &nave);
void MoveNaveBaixo(NaveEspacial &nave);
void MoveNaveDireita(NaveEspacial &nave);
void MoveNaveEsquerda(NaveEspacial &nave);

void InitBalas(Projeteis balas[],int tamanho);
void AtiraBalas(Projeteis balas[], int tamanho, NaveEspacial nave);
void AtualizarBalas(Projeteis balas[], int tamanho);
void DesenhaBalas(Projeteis balas[], int tamanho);
void BalaColidida(Projeteis balas[], int b_tamanho, Cometas cometas[], int c_tamanho, NaveEspacial &nave, Calculo &calc);

void InitCometas(Cometas cometas[], int tamanho, Calculo calc);
void LiberaCometas(Cometas cometas[], int tamanho);
void AtualizarCometas(Cometas cometas[], int tamanho);
void DesenhaCometas(Cometas cometas[], int tamanho, ALLEGRO_FONT *font25, Calculo calc);
void CometaColidido(Cometas cometas[],int c_tamanho, NaveEspacial &nave);

void InitPlanoFundo(Estrelas estrelas_pf[][NUM_ESTRELAS],int pf_tamanho, int e_tamanho);
void AtualizarPlanoFundo(Estrelas estrelas_pf[][NUM_ESTRELAS],int pf_tamanho, int e_tamanho);
void DesenhaPlanoFundo(Estrelas estrelas_pf[][NUM_ESTRELAS],int pf_tamanho, int e_tamanho);

void RetornaCalc(Calculo &calc);

void Options(int tamanho,Calculo &calc);

void EscreveOptions(Cometas cometas[], int tamanho, ALLEGRO_FONT *font14, Calculo calc);

/*________________________________*/


int main()
{
    /*________________________________
     -------VARIÁVEIS DO JOGO--------*/
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font14 = NULL;
    ALLEGRO_FONT *font25 = NULL;
    ALLEGRO_FONT *font50 = NULL;

    ALLEGRO_BITMAP *imagem = NULL;


    bool fim = false;
    bool desenha = true;
    bool game_over = false;
    bool tocar_aplausos = false;
    bool teclas[] = {false,false,false,false,false, false};

    int imageWidth=0;
    int imageHeight=0;


    /*________________________________________
     -------INICIALIZAÇÃO DE OBJETOS--------*/

     NaveEspacial nave;
     Projeteis balas[NUM_BALAS];
     Cometas cometas[NUM_COMETAS];

     Estrelas estrelas_fundo[NUM_PLANOS][NUM_ESTRELAS];

     Calculo calculo;
     RetornaCalc(calculo);
     Options(NUM_COMETAS,calculo);
     Mouse mouse;

    /*____________________________________________________
     -------INICIALIZAÇÃO DA ALLEGRO E DO DISPLAY--------*/
     ALLEGRO_DISPLAY *display = NULL;

     if(!al_init()){
        al_show_native_message_box(NULL,"AVISO!","ERRO!","ERRO AO INICIALIZAR O ALLEGRO!",NULL,ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
     }

     al_set_new_display_flags(ALLEGRO_FULLSCREEN);
     display = al_create_display(largura_t,altura_t);

     if(!display){
        al_show_native_message_box(NULL,"AVISO!","ERRO!","ERRO AO CRIAR O DISPLAY!",NULL,ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
     }


     /*____________________________________________________
     -------INICIALIZAÇÃO DE ADDONS E INSTALAÇÕES--------*/
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_mouse();

    al_install_audio();
    al_init_acodec_addon();

    al_reserve_samples(10);

    al_init_image_addon();

    /*____________________________________________________
     -------CRIAÇÃO DA FILA E DEMAIS DISPOSITIVOS--------*/

     fila_eventos = al_create_event_queue();
     timer = al_create_timer(1.0/FPS);
     font14 = al_load_font("C:/Windows/Fonts/Arial.ttf", 14, NULL);
     font25 = al_load_font("C:/Windows/Fonts/Arial.ttf", 25, NULL);
     font50 = al_load_font("C:/Windows/Fonts/Arial.ttf", 50, NULL);

     al_clear_to_color(al_map_rgb(0,0,0));
     al_draw_filled_rounded_rectangle(largura_t/2 - 150, altura_t/2 -25, largura_t/2+150, altura_t/2+50, 20, 20,al_map_rgb(255,0,0));
     al_draw_text(font50,al_map_rgb(255,255,255), largura_t/2, altura_t/2-15, ALLEGRO_ALIGN_CENTER, "Start");
     al_flip_display();

     trilha_sonora = al_load_sample("trilha_sonora.ogg");
     laser = al_load_sample("laser.wav");
     acerto = al_load_sample("acerto.wav");
     aplausos = al_load_sample("aplausos.wav");
     erro = al_load_sample("erro.ogg");

     inst_trilha_sonora = al_create_sample_instance(trilha_sonora);
     inst_laser = al_create_sample_instance(laser);
     inst_acerto = al_create_sample_instance(acerto);
     inst_erro = al_create_sample_instance(erro);
     inst_aplausos = al_create_sample_instance(aplausos);

     al_attach_sample_instance_to_mixer(inst_trilha_sonora,al_get_default_mixer());
     al_attach_sample_instance_to_mixer(inst_laser,al_get_default_mixer());
     al_attach_sample_instance_to_mixer(inst_acerto,al_get_default_mixer());
     al_attach_sample_instance_to_mixer(inst_erro,al_get_default_mixer());
     al_attach_sample_instance_to_mixer(inst_aplausos, al_get_default_mixer());

     al_set_sample_instance_playmode(inst_trilha_sonora, ALLEGRO_PLAYMODE_LOOP);
     al_set_sample_instance_gain(inst_trilha_sonora,0.8); /*1.0 é o volume original*/


     /*__________________________________________________*/

     /*________________________________
     -------REFISTRO DE SOURCES--------*/

    al_register_event_source(fila_eventos,al_get_display_event_source(display));
    al_register_event_source(fila_eventos,al_get_keyboard_event_source());
    al_register_event_source(fila_eventos,al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos,al_get_mouse_event_source());

    /*________________________________
     -------FUNÇÕES INICIAIS--------*/
     srand(time(NULL));

     InitNave(nave);
     InitBalas(balas, NUM_BALAS);
     InitCometas(cometas, NUM_COMETAS, calculo);

     InitPlanoFundo(estrelas_fundo, NUM_PLANOS, NUM_ESTRELAS);

    /*________________________________
     -------lOOP PRINCIPAL----------*/
     al_start_timer(timer);

    while(!fim)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

        /*-------EVENTOS E LÓGICA DO JOGO-----*/
        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            fim=true;
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(ev.keyboard.keycode){
                case ALLEGRO_KEY_ESCAPE:
                    fim = true;
                    break;
                case ALLEGRO_KEY_UP:
                    teclas[CIMA] = true;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[BAIXO] = true;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[DIREITA] = true;
                    break;
                case ALLEGRO_KEY_LEFT:
                    teclas[ESQUERDA] = true;
                    break;
                case ALLEGRO_KEY_SPACE:
                    teclas[SPACE] = true;
                    AtiraBalas(balas,NUM_BALAS,nave);
                    break;
                case ALLEGRO_KEY_ENTER:
                    teclas[ENTER] = true;
                    break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP){
            switch(ev.keyboard.keycode){
                case ALLEGRO_KEY_UP:
                    teclas[CIMA] = false;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[BAIXO] = false;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[DIREITA] = false;
                    break;
                case ALLEGRO_KEY_LEFT:
                    teclas[ESQUERDA] = false;
                    break;
                case ALLEGRO_KEY_ENTER:
                    teclas[ENTER] = false;
                    break;
            }
        }
        else if(ev.type == ALLEGRO_EVENT_MOUSE_AXES){

            mouse.pos_x = ev.mouse.x;
            mouse.pos_y = ev.mouse.y;

        }
        else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){

            if(mouse.pos_x>(largura_t/2 - 150)&&
                mouse.pos_x<(largura_t/2 + 150)&&
                mouse.pos_y>(altura_t/2 -25)&&
                mouse.pos_y<(altura_t/2+50)){

                    if(ev.mouse.button & 1){
                        iniciaJogo = true;
                        retomaJogo = true;
                    }
            }


        }
        else if(ev.type == ALLEGRO_EVENT_TIMER){
            desenha = true;

            if(teclas[CIMA])
                MoveNaveCima(nave);
            if(teclas[BAIXO])
                MoveNaveBaixo(nave);
            if(teclas[DIREITA])
                MoveNaveDireita(nave);
            if(teclas[ESQUERDA])
                MoveNaveEsquerda(nave);
            if(teclas[SPACE])
                AtualizarBalas(balas,NUM_BALAS);

            if(!game_over && iniciaJogo){

                al_play_sample_instance(inst_trilha_sonora);

                AtualizarPlanoFundo(estrelas_fundo,NUM_PLANOS,NUM_ESTRELAS);

                if(attCalc){
                    Options(NUM_COMETAS,calculo);
                }

                LiberaCometas(cometas,NUM_COMETAS);
                AtualizarCometas(cometas,NUM_COMETAS);
                BalaColidida(balas,NUM_BALAS,cometas,NUM_COMETAS,nave, calculo);
                CometaColidido(cometas, NUM_COMETAS, nave);

                if(nave.vidas <= 0){
                    retomaJogo = false;
                    game_over = true;
                    tocar_aplausos = true;
                }

            }else{
                al_stop_sample_instance(inst_trilha_sonora);
                al_stop_sample_instance(inst_acerto);
                al_stop_sample_instance(inst_laser);

                if(tocar_aplausos){
                    al_play_sample_instance(inst_aplausos);
                    tocar_aplausos = false;
                }

                if(retomaJogo){
                    InitNave(nave);
                    InitBalas(balas, NUM_BALAS);
                    InitCometas(cometas, NUM_COMETAS, calculo);
                    InitPlanoFundo(estrelas_fundo, NUM_PLANOS, NUM_ESTRELAS);

                    game_over = false;
                }

            }

        }


        /*___________________________________
        ------------DESENHO------------------*/

        char questao[50];
        strcpy(questao, calculo.quest);
        /*
        if(ev.mouse.button & 1){
            iniciaJogo = true;
            }
        */
        if(desenha && al_is_event_queue_empty(fila_eventos)&& iniciaJogo){

            desenha = false;

            if(!game_over){
                DesenhaPlanoFundo(estrelas_fundo, NUM_PLANOS, NUM_ESTRELAS);

                DesenhaNave(nave);
                DesenhaBalas(balas,NUM_BALAS);
                DesenhaCometas(cometas, NUM_COMETAS,font25,calculo);

                al_draw_textf(font14, al_map_rgb(255,255,255),0,0,NULL,"VIDAS: %d    /  PONTOS: %d",nave.vidas, nave.pontos);
                al_draw_filled_rectangle(largura_t/2-120,0, largura_t/2+130, 30, al_map_rgb(255,255,255));
                al_draw_textf(font25, al_map_rgb(0,0,0),largura_t/2-100,0,NULL,"Questao: %s",questao);

            }else{
                al_draw_textf(font25, al_map_rgb(255,255,255), largura_t/2, altura_t/2-150, ALLEGRO_ALIGN_CENTER,"Fim de jogo");
                al_draw_textf(font25, al_map_rgb(255,255,255), largura_t/2, altura_t/2-100, ALLEGRO_ALIGN_CENTER,"Seus pontos: %d", nave.pontos);
                al_draw_textf(font25, al_map_rgb(255,255,255), largura_t/2, altura_t/2+100, ALLEGRO_ALIGN_CENTER,"Tecle ESC para sair do jogo.");
                al_draw_filled_rounded_rectangle(largura_t/2 - 150, altura_t/2 -25, largura_t/2+150, altura_t/2+50, 20, 20,al_map_rgb(255,0,0));
                al_draw_text(font50,al_map_rgb(255,255,255), largura_t/2, altura_t/2-15, ALLEGRO_ALIGN_CENTER, "Restart");

            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }




        /*___________________________________*/

    }

    /*----------FINALIZAÇÕES DO PROGRAMA----------------*/

    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_font(font14);

    al_destroy_sample(trilha_sonora);
    al_destroy_sample(laser);
    al_destroy_sample(acerto);
    al_destroy_sample(aplausos);

    al_destroy_sample_instance(inst_trilha_sonora);
    al_destroy_sample_instance(inst_laser);
    al_destroy_sample_instance(inst_acerto);
    al_destroy_sample_instance(inst_aplausos);

    al_destroy_bitmap(imagem);

    return 0;
}

/*----------DEFINIÇÃO DE FUNÇÕES E PROCEDIMENTOS--------------*/
/* ---------------------NAVE-------------------*/
void InitNave(NaveEspacial &nave){
    nave.x = 20;
    nave.y = altura_t/2;
    nave.ID = JOGADOR;
    nave.vidas = 3;
    nave.velocidade = 10;
    nave.borda_x = 8;
    nave.borda_y = 8;
    nave.pontos = 0;
}

void DesenhaNave (NaveEspacial &nave){
    al_draw_filled_rectangle(nave.x,nave.y - 9, nave.x + 10, nave.y - 7,al_map_rgb(255,0,0));
    al_draw_filled_rectangle(nave.x,nave.y + 9, nave.x + 10, nave.y + 7,al_map_rgb(255,0,0));
    al_draw_filled_triangle(nave.x -12 ,nave.y - 17, nave.x + 12, nave.y, nave.x - 12, nave.y +17, al_map_rgb(0,255,0));
    al_draw_filled_rectangle(nave.x -12,nave.y -2, nave.x + 15, nave.y + 2,al_map_rgb(0,0,255));
}

void MoveNaveCima(NaveEspacial &nave){
    nave.y -= nave.velocidade; /*desce 7px*/

    if(nave.y < 20)
        nave.y = 20;
}

void MoveNaveBaixo(NaveEspacial &nave){
    nave.y += nave.velocidade;

    if(nave.y > altura_t -20)
        nave.y = altura_t -20;
}

void MoveNaveDireita(NaveEspacial &nave){
    nave.x += nave.velocidade;

    if(nave.x > (largura_t / 2) - 300)
        nave.x = (largura_t / 2) - 300;
}

void MoveNaveEsquerda(NaveEspacial &nave){
    nave.x -= nave.velocidade;

    if(nave.x < 12)
        nave.x = 12;
}

/* ---------------------PROJETEIS-------------------*/

void InitBalas(Projeteis balas[],int tamanho){

    for(int i=0; i<tamanho; i++){
        balas[i].ID = PROJETIL;
        balas[i].velocidade = 10;
        balas[i].ativo = false;


    }

}

void AtiraBalas(Projeteis balas[], int tamanho, NaveEspacial nave){

    for(int i=0; i<tamanho; i++){

        if(!balas[i].ativo){
            al_stop_sample_instance(inst_laser);
            al_play_sample_instance(inst_laser);
            balas[i].x = nave.x + 17;
            balas[i].y = nave.y;
            balas[i].ativo = true;
            break;
        }

    }
}

void AtualizarBalas(Projeteis balas[], int tamanho){

    for(int i=0; i<tamanho; i++){

        if(balas[i].ativo){
            balas[i].x += balas[i].velocidade;

            if(balas[i].x > largura_t){
                balas[i].ativo = false;
            }
        }

    }

}

void DesenhaBalas(Projeteis balas[], int tamanho){

    for(int i=0; i<tamanho; i++){

        if(balas[i].ativo){
            al_draw_filled_circle(balas[i].x, balas[i].y,2,al_map_rgb(255,255,255));

        }

    }

}

void BalaColidida(Projeteis balas[], int b_tamanho, Cometas
                  cometas[], int c_tamanho, NaveEspacial &nave, Calculo &calc){
    for(int i=0; i<b_tamanho;i++){
        if(balas[i].ativo){
            for(int j=0; j< c_tamanho;j++){
                if(cometas[j].ativo){
                    if(balas[i].x>(cometas[j].x-cometas[j].borda_x)&&
                       balas[i].x<(cometas[j].x+cometas[j].borda_x)&&
                       balas[i].y>(cometas[j].y - cometas[j].borda_y)&&
                       balas[i].y<(cometas[j].y + cometas[j].borda_y))
                       {
                           if(j == calc.correct){
                                al_stop_sample_instance(inst_acerto);
                                al_play_sample_instance(inst_acerto);
                                balas[i].ativo = false;
                                for(int p=0; p<c_tamanho;p++){
                                    cometas[p].ativo = false;
                                }

                                nave.pontos++;
                                RetornaCalc(calc);
                                Options(c_tamanho,calc);
                                for(int c=0; c<c_tamanho;c++){
                                    cometas[c].velocidade+=.125;
                                }

                           }else{
                                al_stop_sample_instance(inst_erro);
                                al_play_sample_instance(inst_erro);
                                balas[i].ativo = false;
                                for(int p=0; p<c_tamanho;p++){
                                    cometas[p].ativo = false;
                                }

                                nave.vidas--;
                                RetornaCalc(calc);
                                Options(c_tamanho,calc);
                           }

                       }
                }
            }
        }
    }
}


/* ---------------------COMETAS-------------------*/


void InitCometas(Cometas cometas[], int tamanho, Calculo calc){

    int num_rand = rand()%tamanho;

    for(int i=0; i<tamanho; i++){
        cometas[i].ID = INIMIGOS;
        cometas[i].velocidade = 1;
        cometas[i].borda_x = 30;
        cometas[i].borda_y = altura_t/16;
        if(i == calc.correct){
            cometas[i].correct = true;
        }else{
            cometas[i].correct = false;
        }
        cometas[i].ativo = false;
    }

}

void LiberaCometas(Cometas cometas[], int tamanho){

    for(int i=0; i<tamanho; i++){
        if(!cometas[i].ativo){


            /*if(rand() % 10 == 0){ garante que não desenhe os 10 de uma vez na tela (chance 1/500)*/

            cometas[i].x = largura_t;
            cometas[i].y = (altura_t/4 * i) + (altura_t/8);
            /*if(i>0 && cometas[i].y <= cometas[i-1].y+30 && cometas[i].y >= cometas[i-1].y-30){
                if(cometas[i].y<altura_t-60){
                    cometas[i].y -= 100;
                }else{
                    cometas[i].y += 100;
                }
            }*/
            cometas[i].ativo = true;
            break;
        }
    }
}

void AtualizarCometas(Cometas cometas[], int tamanho){

    for(int i=0; i<tamanho; i++){
        if(cometas[i].ativo){
            cometas[i].x -= cometas[i].velocidade;
        }
    }
    /* comentado para retirar vida apos cometa cruzar borda esquerda(f CometaColidido)
    for(int i=0; i<tamanho; i++){
        if(cometas[i].x < 0){
            cometas[i].ativo = false;
        }
    }
    */
}

void DesenhaCometas(Cometas cometas[], int tamanho, ALLEGRO_FONT *font25, Calculo calc){

    for(int i=0; i<tamanho; i++){
        if(cometas[i].ativo){
            al_draw_filled_rectangle(cometas[i].x + 30, cometas[i].y - altura_t/16, cometas[i].x - 30, cometas[i].y+altura_t/16 , al_map_rgb(255,0,0));
            /*al_draw_filled_rectangle(cometas[i].x - cometas[i].borda_x, cometas[i].y - cometas[i].borda_y,
                                     cometas[i].x +cometas[i].borda_x, cometas[i].y + cometas[i].borda_y, al_map_rgba_f(.6,.6,.6,.6));*/
            al_draw_textf(font25, al_map_rgb(255,255,255),cometas[i].x-15,cometas[i].y-15,NULL,"%d",calc.valores[i]);
        }
    }
}

void CometaColidido(Cometas cometas[],int c_tamanho, NaveEspacial &nave){
    for(int i=0; i<c_tamanho ; i++){
        if(cometas[i].ativo){
            if(cometas[i].x-cometas[i].borda_x< nave.x + nave.borda_x &&
               cometas[i].x + cometas[i].borda_x > nave.x - nave.borda_x &&
               cometas[i].y-cometas[i].borda_y< nave.y + nave.borda_y &&
               cometas[i].y + cometas[i].borda_y > nave.y - nave.borda_y)
               {
                   for(int j=0; j<c_tamanho;j++){
                        cometas[j].ativo = false;
                   }
                   nave.vidas--;
               }
            else if(cometas[i].x < 0){
                for(int j=0; j<c_tamanho;j++){
                        cometas[j].ativo = false;
                }
                nave.vidas--;
            }
        }
    }
}

/* ---------------------CALCULO RANDOM-------------------*/

void RetornaCalc(Calculo &calc){

    int random = rand()%3;
    int v1, v2;
    int limitSumSub = 30;
    int limitMult = 10;

    switch(random){
        case 0:
            v1 = rand()%limitSumSub;
            v2 = rand()%limitSumSub;
            calc.result = v1 + v2;
            sprintf(calc.quest, "%d + %d = ?", v1, v2);
            break;
        case 1:
            v1 = rand()%limitSumSub;
            v2 = rand()%limitSumSub;
            calc.result = v1 - v2;
            sprintf(calc.quest, "%d - %d = ?", v1, v2);
            break;
        case 2:
            v1 = rand()%limitMult;
            v2 = rand()%limitMult;
            calc.result = v1 * v2;
            sprintf(calc.quest, "%d * %d = ?", v1, v2);
            break;

    }
}

void Options(int tamanho, Calculo &calc){

    int a = rand()%4;
    for(int i=0; i<tamanho ; i++){
        if(i==a){
            calc.valores[i]=calc.result;
            calc.correct = i;
        }else{
            calc.valores[i]=rand()%100;
        }
    }
}


/* ---------------------PLANO DE FUNDO-------------------*/

void InitPlanoFundo(Estrelas estrelas_pf[][NUM_ESTRELAS],int pf_tamanho, int e_tamanho){

    for(int i=0; i<pf_tamanho; i++){
        for(int j=0; j<e_tamanho; j++){
            if(i==0){
                estrelas_pf[i][j].ID = ESTRELA;
                estrelas_pf[i][j].x = 5 + rand()%(largura_t -10);
                estrelas_pf[i][j].y = 5 + rand()%(largura_t -10);
                estrelas_pf[i][j].velocidade = 8;
            }else if(i==1){
                estrelas_pf[i][j].ID = ESTRELA;
                estrelas_pf[i][j].x = 5 + rand()%(largura_t -10);
                estrelas_pf[i][j].y = 5 + rand()%(largura_t -10);
                estrelas_pf[i][j].velocidade = 3;
            }else if(i==2){
                estrelas_pf[i][j].ID = ESTRELA;
                estrelas_pf[i][j].x = 5 + rand()%(largura_t -10);
                estrelas_pf[i][j].y = 5 + rand()%(largura_t -10);
                estrelas_pf[i][j].velocidade = 1;
            }
        }
    }
}

void AtualizarPlanoFundo(Estrelas estrelas_pf[][NUM_ESTRELAS],int pf_tamanho, int e_tamanho){

    for(int i=0; i<pf_tamanho; i++){
        for(int j=0; j<e_tamanho; j++){
            if(i==0){
                estrelas_pf[i][j].x -= estrelas_pf[i][j].velocidade;
                if(estrelas_pf[i][j].x<0){
                    estrelas_pf[i][j].x=largura_t;
                }
            }else if(i==1){
                estrelas_pf[i][j].x -= estrelas_pf[i][j].velocidade;
                if(estrelas_pf[i][j].x<0){
                    estrelas_pf[i][j].x=largura_t;
                }
            }else if(i==2){
                estrelas_pf[i][j].x -= estrelas_pf[i][j].velocidade;
                if(estrelas_pf[i][j].x<0){
                    estrelas_pf[i][j].x=largura_t;
                }
            }
        }
    }
}

void DesenhaPlanoFundo(Estrelas estrelas_pf[][NUM_ESTRELAS],int pf_tamanho, int e_tamanho){
    ALLEGRO_COLOR color = al_map_rgb(105, 105, 105);
    for(int i=0; i<pf_tamanho; i++){
        for(int j=0; j<e_tamanho; j++){
            if(i==0){

                al_draw_line(estrelas_pf[i][j].x,estrelas_pf[i][j].y+3, estrelas_pf[i][j].x, estrelas_pf[i][j].y-3, color,1.0);
                al_draw_line(estrelas_pf[i][j].x+3,estrelas_pf[i][j].y, estrelas_pf[i][j].x-3, estrelas_pf[i][j].y, color,1.0);

            }else if(i==1){
                al_draw_line(estrelas_pf[i][j].x+3,estrelas_pf[i][j].y, estrelas_pf[i][j].x-3, estrelas_pf[i][j].y, color,1.0);
            }else if(i==2){
                al_draw_line(estrelas_pf[i][j].x,estrelas_pf[i][j].y+3, estrelas_pf[i][j].x, estrelas_pf[i][j].y-3, color,1.0);
                al_draw_line(estrelas_pf[i][j].x+3,estrelas_pf[i][j].y, estrelas_pf[i][j].x-3, estrelas_pf[i][j].y, color,1.0);
                al_draw_line(estrelas_pf[i][j].x+3,estrelas_pf[i][j].y+3, estrelas_pf[i][j].x-3, estrelas_pf[i][j].y-3, color,1.0);
                al_draw_line(estrelas_pf[i][j].x+3,estrelas_pf[i][j].y-3, estrelas_pf[i][j].x-3, estrelas_pf[i][j].y+3, color,1.0);
            }
        }
    }
}



/* ---------------------FIM-------------------*/
