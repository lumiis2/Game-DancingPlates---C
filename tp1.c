#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <time.h>

#define N_FRAMES 7
#define N_FRAMES_P 1
#define N_FRAMES_C 1

#define BAR_WIDTH 5
#define BAR_HEIGHT 340
#define BAR_START_Y 80

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

//----------------------------------------------------
// Variaveis globais

const float FPS = 50;
const int PLATE_W = 65;
const int PLATE_H = 30;
const int FLOOR_H = 50;
const int PP_W = 80;
const int PP_H = 120;
int NUM_BARS = 11;
int NUM_PRATOS = 11;
int FASE = 3;
int i, j, k;
float anim_prato[11];
int bar_power, var_power, time_power;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_BITMAP *icon = NULL;
ALLEGRO_TIMER *timer = NULL;

ALLEGRO_BITMAP *menu = NULL;
ALLEGRO_BITMAP *margem = NULL;
ALLEGRO_BITMAP *play = NULL;
ALLEGRO_BITMAP *pag = NULL;
ALLEGRO_BITMAP *player1 = NULL;
ALLEGRO_BITMAP *player2 = NULL;
ALLEGRO_BITMAP *nome_player1 = NULL;
ALLEGRO_BITMAP *nome_player2 = NULL;
ALLEGRO_BITMAP *seta = NULL;
ALLEGRO_BITMAP *ranking = NULL;

ALLEGRO_BITMAP *fundo1 = NULL;
ALLEGRO_BITMAP *fundo2 = NULL;
ALLEGRO_BITMAP *fundo3 = NULL;
ALLEGRO_BITMAP *imagemPrato = NULL;
ALLEGRO_BITMAP *frames_pratos[5];
ALLEGRO_BITMAP *add_power1 = NULL;
ALLEGRO_BITMAP *add_power2 = NULL;
ALLEGRO_BITMAP *poder = NULL;

ALLEGRO_SAMPLE *trilha_sonora = NULL;
ALLEGRO_SAMPLE *trilha_sonora_pag1 = NULL;

ALLEGRO_SAMPLE_INSTANCE *inst_trilha_sonora = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_trilha_sonora_pag1 = NULL;

ALLEGRO_BITMAP *frames_right[N_FRAMES];
ALLEGRO_BITMAP *frames_left[N_FRAMES];
ALLEGRO_BITMAP *frame_stop[N_FRAMES_P];
ALLEGRO_BITMAP *frame_click[N_FRAMES_C];

ALLEGRO_BITMAP *frames_right2[N_FRAMES];
ALLEGRO_BITMAP *frames_left2[N_FRAMES];
ALLEGRO_BITMAP *frame_stop2[N_FRAMES_P];
ALLEGRO_BITMAP *frame_click2[N_FRAMES_C];

ALLEGRO_FONT *font_arcade = NULL;

// OBJETOS
typedef struct Bar {
    int x;
    int y;
    int batendo;
} Bar;

typedef struct Plate {
    float x;
    float y;
    float energia;
    float tempoParaAparecer;
    float aparecendo;

} Plate;

typedef struct Player {
    float x;
    float vel;
    float meio;
    int dir, esq;
    int moving;
    int espaco;
} Player;

typedef struct pow {
    float x;
    float y;
    float tempoParaAparecer;
    float aparecendo;
    float forca;
    int pegou_p;
    int c;
    // amarelo
} Pow;

typedef struct thick {
    float x;
    float y;
    float tempoParaAparecer;
    float aparecendo;
    float area;
    int pegou_t;
    int c;
    // vermelho
} Thick;

// CENARIO
void draw_scenario(ALLEGRO_BITMAP *fundo1, ALLEGRO_BITMAP *margem, ALLEGRO_TIMER *timer, ALLEGRO_FONT *font) {
    if (FASE == 1){
        al_draw_bitmap(fundo1, 0, 0, 0);
    }
    else if (FASE == 2){
        al_draw_bitmap(fundo2, 0, 0, 0);
    }
    else if (FASE == 3){
        al_draw_bitmap(fundo3, 0, 0, 0);
    }
    al_draw_bitmap(margem, 0, 0, 0);

    char texto[50];
    sprintf(texto, "%lld", al_get_timer_count(timer));
    al_draw_text(font, al_map_rgb(0, 223, 15), 40, 10, ALLEGRO_ALIGN_CENTER, texto);
}

// BARRA
void init_bar(Bar *barra, int y) {

    int x = (SCREEN_WIDTH - (NUM_BARS * BAR_WIDTH)) / (NUM_BARS + 1);
    for (i = 0; i < NUM_BARS; i++){
        barra[i].x = x;
        barra[i].y = y;
        barra[i].batendo = 0;
        x += BAR_WIDTH + (SCREEN_WIDTH - (NUM_BARS * BAR_WIDTH)) / (NUM_BARS + 1);
    }
}

void draw_bar(Bar *barra) {
    for (i = 0; i < NUM_BARS; i++){
        if (barra[i].batendo == 1){
            al_draw_filled_rectangle(barra[i].x, barra[i].y, barra[i].x + BAR_WIDTH, barra[i].y + BAR_HEIGHT, al_map_rgb(255, 0, 0));
        }
        else{
            al_draw_filled_rectangle(barra[i].x, barra[i].y, barra[i].x + BAR_WIDTH, barra[i].y + BAR_HEIGHT, al_map_rgb(255, 255, 255));
        }
    }
}

void draw_efeito_bar(Bar *array_bar, int bar_power) {
    for (k = 0; k < NUM_BARS; k++){
        if (bar_power == 1){
            if (array_bar[k].batendo == 0){
                al_draw_filled_triangle((array_bar[k].x - 10), (array_bar[k].y + BAR_HEIGHT), (array_bar[k].x + BAR_WIDTH / 2), (array_bar[k].y + BAR_HEIGHT - 20), (array_bar[k].x + BAR_WIDTH + 10), (array_bar[k].y + BAR_HEIGHT), al_map_rgb(255, 255, 255));
            }
            else{
                al_draw_filled_triangle((array_bar[k].x - 10), (array_bar[k].y + BAR_HEIGHT), (array_bar[k].x + BAR_WIDTH / 2), (array_bar[k].y + BAR_HEIGHT - 20), (array_bar[k].x + BAR_WIDTH + 10), (array_bar[k].y + BAR_HEIGHT), al_map_rgb(255, 0, 0));
            }
        }
    }
}

// PERSONA
void init_player(Player *player) {
    player->x = SCREEN_WIDTH / 2;
    player->vel = 4.5;
    player->meio = 0;
    player->dir = 0;
    player->esq = 0;
    player->moving = 0;
    player->espaco = 0;
}

void draw_player1(Player player, int currentFrame_right, int currentFrame_left, int currentFrame_stop, int currentFrame_click) {
    float base = SCREEN_HEIGHT - FLOOR_H / 2;
    int sprite_x = player.x - 84 / 2;
    int sprite_y = base - 100;
    if (player.dir == 1){
        al_draw_bitmap(frames_right[currentFrame_right], sprite_x, sprite_y, 0);
    }
    else if (player.esq == 1){
        al_draw_bitmap(frames_left[currentFrame_left], sprite_x, sprite_y, 0);
    }
    else if (player.moving == 0){
        if (player.espaco == 1){
            al_draw_bitmap(frame_click[0], sprite_x, sprite_y, 0);
            al_rest(0.002);
            player.espaco = 0;
        }
        else{
            al_draw_bitmap(frame_stop[0], sprite_x, sprite_y, 0);
        }
    }
}

void draw_player2(Player player, int currentFrame_right2, int currentFrame_left2, int currentFrame_stop2, int currentFrame_click2) {
    float base = SCREEN_HEIGHT - FLOOR_H / 2;
    int sprite_x = player.x - 84 / 2;
    int sprite_y = base - 100;
    if (player.dir == 1){
        al_draw_bitmap(frames_right2[currentFrame_right2], sprite_x, sprite_y, 0);
    }
    else if (player.esq == 1){
        al_draw_bitmap(frames_left2[currentFrame_left2], sprite_x, sprite_y, 0);
    }
    else if (player.moving == 0){
        if (player.espaco == 1){
            al_draw_bitmap(frame_click2[0], sprite_x, sprite_y, 0);
            al_rest(0.002);
            player.espaco = 0;
        }
        else{
            al_draw_bitmap(frame_stop2[0], sprite_x, sprite_y, 0);
        }
    }
}

// PRATOS
float geraTempoPrato(int i) {
    if (FASE == 3){
        int ls[] = {90, 85, 80, 80, 80, 75, 85, 80, 85, 90, 90};
        return ls[i] + rand() % 10;
    }
    else if (FASE == 2){
        int ls[] = {60, 65, 50, 45, 40, 45, 55, 60, 60};
        return ls[i] + rand() % 10;
    }
    else if (FASE == 1){
        int ls[] = {20, 10, 5, 0, 10, 15, 20};
        return ls[i] + rand() % 10;
    }
}

void init_plate(Plate *p, Bar *bar) {
    for (i = 0; i < 11; i++){
        anim_prato[i] = 0;
    }
    for (i = 0; i < NUM_PRATOS; i++){
        p[i].x = bar[i].x + 2.5;
        p[i].y = BAR_START_Y;
        p[i].tempoParaAparecer = geraTempoPrato(i);
        p[i].aparecendo = 0;
        p[i].energia = 0;
    }
}

void draw_plate(Plate *p, ALLEGRO_BITMAP *imagemPrato, ALLEGRO_BITMAP *fundo1, ALLEGRO_BITMAP *margem,
                int *paginas, ALLEGRO_TIMER *timer, ALLEGRO_FONT *font, Bar *array_bar, Player player, Player player2,
                int currentFrame_right, int currentFrame_left, int currentFrame_stop, int currentFrame_click, int choose_player1, int choose_player2) {

    float plate_pos_x;
    float plate_pos_y;
    for (i = 0; i < NUM_PRATOS; i++){
        if (p[i].aparecendo && p[i].energia <= 255){
            if (p[i].energia < 0){
                p[i].energia = 0;
            }
            if (p[i].energia < 20)
                anim_prato[i] = 0;
            else{
                anim_prato[i] += 0.05;
                for (j = 0; j < p[i].energia; j++)
                    anim_prato[i] += 0.1 / 255; // x * 255 = 0.1
            }
            if (anim_prato[i] >= 5)
                anim_prato[i] -= 4;
            al_draw_tinted_bitmap(frames_pratos[(int)anim_prato[i]], al_map_rgba_f(1, (1 - (p[i].energia / 255)), (1 - (p[i].energia / 255)), 1), p[i].x - 32.5, p[i].y - 20, 0);
        }
        else if (p[i].aparecendo && p[i].energia > 255){
            if (p[i].y < SCREEN_HEIGHT - 40){ // quando perde
                p[i].y += 15;
                al_draw_tinted_bitmap(frames_pratos[0], al_map_rgba_f(1, 0, 0, 1), p[i].x - 25, p[i].y, 0);
            }
            else{
                *paginas = 5;
                al_stop_timer(timer);
                al_rest(0.5);
                break;
            }
        }
    }
}

// PODERZINHO
float geraTempoPowerP(int i) {
    int tmp[] = {50, 70, 100};
    return tmp[i] + rand() % 10;
}

float geraTempoPowerT(int i) {
    int tmp[] = {10, 90, 130};
    return tmp[i] + rand() % 10;
}

void init_power(Pow *p, Thick *t, int *var_power) {
    for (i = 0; i < 3; i++) {
        int dir_esq = rand() % 2;
        if (dir_esq == 1){
            p[i].x = SCREEN_WIDTH - 100;
            p[i].y = 480;
            t[i].x = SCREEN_WIDTH - 100;
            t[i].y = 480;
        }
        else{
            p[i].x = 50;
            p[i].y = 480;
            t[i].x = 50;
            t[i].y = 480;
        }
        p[i].tempoParaAparecer = geraTempoPowerP(i);
        p[i].aparecendo = 0;
        p[i].forca = 0;
        p[i].pegou_p = 0;
        p[i].c = 0;
        t[i].tempoParaAparecer = geraTempoPowerT(i);
        t[i].aparecendo = 0;
        t[i].area = 0;
        t[i].pegou_t = 0;
        t[i].c = 0;
    }
    *var_power = 0;
}

void draw_power(Pow *p, Thick *t, ALLEGRO_BITMAP *poder) {
    for (i = 0; i < 3; i++){
        if (p[i].aparecendo){
            al_draw_tinted_bitmap(poder, al_map_rgba_f(1, 1, 1, 1), p[i].x, p[i].y, 0);
        }
        else if (t[i].aparecendo){
            al_draw_tinted_bitmap(poder, al_map_rgba_f(1, 0.2, 0.2, 1), t[i].x, t[i].y, 0);
        }
    }
}

void draw_popUp(ALLEGRO_BITMAP *add_power1, ALLEGRO_BITMAP *add_power2, Pow *p, Thick *t, Bar *array_bar, int bar_power) {

    for (i = 0; i < 3; i++){
        if (p[i].forca > 0.0){
            al_draw_tinted_bitmap(add_power1, al_map_rgba_f(1, 1, 1, 1), SCREEN_WIDTH - 180, 20, 0);
        }
        if (t[i].area > 0.0){
            bar_power = 1;
            draw_efeito_bar(array_bar, bar_power);

            al_draw_tinted_bitmap(add_power2, al_map_rgba_f(1, 1, 1, 1), SCREEN_WIDTH - 180, 20, 0);
        }
        else
            bar_power = 0;
    }
}

void update_player(Player *player) {
    player->moving = 0;

    if (player->dir && player->x + player->vel <= SCREEN_WIDTH){
        player->x += player->vel;
        player->moving = 1;
    }
    if (player->esq && player->x - player->vel >= 0){
        player->x -= player->vel;
        player->moving = 1;
    }
}

void update_plate(Plate *p, ALLEGRO_TIMER *timer, Player player, Player player2, Bar *bar, Pow *po, Thick *t, int *var_power) {
    int timerCount = al_get_timer_count(timer);
    int timerSeconds = timerCount / (int)FPS;
    for (i = 0; i < NUM_PRATOS; i++){
        if (!p[i].aparecendo && timerSeconds >= p[i].tempoParaAparecer){
            p[i].aparecendo = 1;
        }
        else if ((p[i].aparecendo && player.espaco && !player.moving && (player.x) >= ((bar[i].x - 4) - t[*var_power].area) && (player.x) <= ((bar[i].x + 10) + t[*var_power].area)) ||
                 (p[i].aparecendo && player2.espaco && !player2.moving && (player2.x) >= ((bar[i].x - 4) - t[*var_power].area) && (player2.x) <= ((bar[i].x + 10) + t[*var_power].area))) {
            if (p[i].energia >= 4.25 && p[i].energia < 255) {
                printf("var power %i\n", *var_power);
                printf("forca %f\n", po[*var_power].forca);
                printf("area %f\n", t[*var_power].area);
                p[i].energia = p[i].energia - (4.25 + po[*var_power].forca);
                player.espaco = 0;
                player2.espaco = 0;
            }
        }
        else if (p[i].aparecendo && timerCount % (int)FPS == 0){
            p[i].energia += 11.25;
        }
    }
}

void update_bar(Plate *p, ALLEGRO_TIMER *timer, Player player, Player player2, Bar *bar, Pow *po, Thick *t, int bar_power) {
    int timerCount = al_get_timer_count(timer);
    int timerSeconds = timerCount / (int)FPS;

    for (k = 0; k < 3; k++){
        for (i = 0; i < NUM_PRATOS; i++){
            if (!p[i].aparecendo && timerSeconds >= p[i].tempoParaAparecer) {
                p[i].aparecendo = 1;
            }
            else if ((p[i].aparecendo && player.espaco && !player.moving && (player.x) >= ((bar[i].x - 3) - t[k].area) && (player.x) <= ((bar[i].x + 9) + t[k].area)) ||
                     (p[i].aparecendo && player2.espaco && !player2.moving && (player2.x) >= ((bar[i].x - 3) - t[k].area) && (player2.x) <= ((bar[i].x + 9) + t[k].area))){
                bar[i].batendo = 1;
                draw_bar(bar);
                draw_efeito_bar(bar, bar_power);
                player.espaco = 0;
                player2.espaco = 0;
            }
            else
                bar[i].batendo = 0;
        }
    }
}

void update_power(Pow *p, Thick *t, ALLEGRO_TIMER *timer, Player player, Player player2, int *time_power, int *var_power, int bar_power) {
    int timerCount = al_get_timer_count(timer);
    int timerSeconds = timerCount / (int)FPS;

    for (i = 0; i < 3; i++){
        if (!p[i].aparecendo && timerSeconds >= p[i].tempoParaAparecer && p[i].c == 0){
            p[i].aparecendo = 1;
        }
        if (!t[i].aparecendo && timerSeconds >= t[i].tempoParaAparecer && t[i].c == 0){
            t[i].aparecendo = 1;
        }
        if (p[i].aparecendo && timerSeconds > (p[i].tempoParaAparecer + 10)){
            p[i].aparecendo = 0;
        }
        if (t[i].aparecendo && timerSeconds > (t[i].tempoParaAparecer + 10)){
            t[i].aparecendo = 0;
        }
        if ((p[i].aparecendo && (player.x) >= (p[i].x - 25) && (player.x) <= (p[i].x + 25)) ||
            (p[i].aparecendo && (player2.x) >= (p[i].x - 25) && (player2.x) <= (p[i].x + 25))){
            p[i].pegou_p = 1;
            *time_power = al_get_timer_count(timer);
            p[i].aparecendo = 0;
            p[i].c += 1;
            *var_power = i;
            printf("----i %i", i);
        }
        if ((t[i].aparecendo && (player.x) >= (t[i].x - 25) && (player.x) <= (t[i].x + 25)) ||
            (t[i].aparecendo && (player2.x) >= (t[i].x - 25) && (player2.x) <= (t[i].x + 25))){
            t[i].pegou_t = 1;
            bar_power = 1;
            *time_power = al_get_timer_count(timer);
            t[i].aparecendo = 0;
            t[i].c += 1;
            *var_power = i;
            printf("----i %i", i);
        }
    }
}

void efeito_power(Pow *p, Thick *t, ALLEGRO_TIMER *timer, int *time_power) {
    int timerCount = al_get_timer_count(timer);
    int timerSeconds = timerCount / (int)FPS;
    int inicio = *time_power / (int)FPS;

    for (i = 0; i < 3; i++){
        if (p[i].pegou_p == 1){
            if (timerSeconds < inicio + 10)
            {
                p[i].forca = 25.0;
            }
            else
                p[i].forca = 0.0;

            if (p[i].forca == 0.0)
            {
                p[i].pegou_p = 0;
            }
        }
        else if (p[i].pegou_p == 0){
            p[i].forca = 0;
        }
        if (t[i].pegou_t == 1){
            if (timerSeconds < inicio + 10)
            {
                t[i].area = 5.0;
            }
            else
                t[i].area = 0.0;

            if (t[i].area == 0.0)
            {
                t[i].pegou_t = 0;
            }
        }
        else if (t[i].pegou_t == 0){
            t[i].area = 0;
        }
    }
}

// RECORDE
long long int record(long long int points) {
    long long int record;
    FILE *fp;
    fp = fopen("recorde.txt", "r");
    fscanf(fp, "%lld", &record);
    fclose(fp);
    if (points > record){
        fp = fopen("recorde.txt", "w");
        fprintf(fp, "%lld", points);
        fclose(fp);
        printf("New Record: %lld", points);
        return points;
    }
    else{
        printf("Record: %lld\nPoints: %lld\n", record, points);
        return record;
    }
}

// TELAS
void draw_menu(ALLEGRO_BITMAP *menu, ALLEGRO_BITMAP *play, ALLEGRO_BITMAP *ranking, int no_play, int no_ranking) {
    if (!no_play){
        al_draw_bitmap(play, SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2, SCREEN_HEIGHT / 2 + 63 - al_get_bitmap_height(play) / 2, 0);
    }
    else{
        al_draw_tinted_bitmap(play, al_map_rgba_f(1, 0.5, 1, 1), (SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2), (SCREEN_HEIGHT / 2 + 63 - al_get_bitmap_height(play) / 2), 0);
    }

    if (!no_ranking){
        al_draw_bitmap(ranking, SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2, SCREEN_HEIGHT / 2 + 118 - al_get_bitmap_height(play) / 2, 0);
    }
    else {
        al_draw_tinted_bitmap(ranking, al_map_rgba_f(1, 0.5, 1, 1), SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2, SCREEN_HEIGHT / 2 + 118 - al_get_bitmap_height(play) / 2, 0);
    }
}

void draw_record_screen(long long int points, long long int record, ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(243, 243, 243));
    al_draw_text(font, al_map_rgb(0, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, ALLEGRO_ALIGN_CENTER, "Voltar  ao  inicio");
    if (points == record) {
        char texto[50];
        char texto3[50];
        sprintf(texto3, "Pontos   %lld", points);
        sprintf(texto, "Novo Recorde   %lld", record);

        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, ALLEGRO_ALIGN_CENTER, texto3);
        al_draw_text(font, al_map_rgb(0, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "Game  Over!");
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, ALLEGRO_ALIGN_CENTER, texto);
    }
    else {
        char texto1[50];
        char texto2[50];
        sprintf(texto1, "Recorde vigente   %lld", record);
        sprintf(texto2, "Pontos   %lld", points);

        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, ALLEGRO_ALIGN_CENTER, texto2);
        al_draw_text(font, al_map_rgb(0, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "Game  Over!");
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, ALLEGRO_ALIGN_CENTER, texto1);
    }
}

void draw_transicao(ALLEGRO_FONT *font) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    if (FASE == 3) {
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "FASE 3");
        al_flip_display();
    }
    else if (FASE == 2) {
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "FASE 2");
        al_flip_display();
    }
    else if (FASE == 1) {
        al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "FASE 1");
        al_flip_display();
    }
    al_rest(0.5);
}

void draw_pag(int count1, int count2) {
    if (count1 % 2 == 0) {
        al_draw_bitmap(nome_player1, SCREEN_WIDTH / 2 - al_get_bitmap_width(nome_player1) / 2, SCREEN_HEIGHT / 2 + 63 - al_get_bitmap_height(nome_player1) / 2, 0);
    }
    else if (count1 % 2 == 1 || count1 == 1) {
        al_draw_tinted_bitmap(nome_player1, al_map_rgba_f(1, 0.5, 1, 1), (SCREEN_WIDTH / 2 - al_get_bitmap_width(nome_player1) / 2), (SCREEN_HEIGHT / 2 + 63 - al_get_bitmap_height(nome_player1) / 2), 0);
    }
    if (count2 % 2 == 0) {
        al_draw_bitmap(nome_player2, SCREEN_WIDTH / 2 - al_get_bitmap_width(nome_player2) / 2, SCREEN_HEIGHT / 2 + 118 - al_get_bitmap_height(nome_player2) / 2, 0);
    }
    else if (count2 % 2 == 1 || count2 == 1) {
        al_draw_tinted_bitmap(nome_player2, al_map_rgba_f(1, 0.5, 1, 1), SCREEN_WIDTH / 2 - al_get_bitmap_width(nome_player2) / 2, SCREEN_HEIGHT / 2 + 118 - al_get_bitmap_height(nome_player2) / 2, 0);
    }

    if (count1 % 2 == 1 || count1 == 1) {
        al_draw_bitmap(player1, SCREEN_WIDTH / 2 + 300 - al_get_bitmap_width(player1) / 2, SCREEN_HEIGHT / 2 + 118 - al_get_bitmap_height(player1) / 2, 0);
    }
    if (count2 % 2 == 1 || count2 == 1) {
        al_draw_bitmap(player2, SCREEN_WIDTH / 2 - 300 - al_get_bitmap_width(player2) / 2, SCREEN_HEIGHT / 2 + 118 - al_get_bitmap_height(player2) / 2, 0);
    }
    al_draw_bitmap(seta, SCREEN_WIDTH / 2 - al_get_bitmap_width(seta) / 2, SCREEN_HEIGHT - 50 - al_get_bitmap_height(seta) / 2, 0);
}

int main() {
    srand(time(NULL));

    // INICIALIZACOES BASICAS ALLEGRO
    if (!al_init()){
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }

    if (!al_init_primitives_addon()){
        fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }

    if (!al_init_image_addon()){
        fprintf(stderr, "failed to initialize image module!\n");
        return -1;
    }

    al_init_font_addon();
    if (!al_init_ttf_addon()){
        fprintf(stderr, "failed to load tff font module!\n");
        return -1;
    }

    if (!al_install_keyboard()){
        fprintf(stderr, "failed to install keyboard!\n");
        return -1;
    }

    if (!al_install_mouse()){
        fprintf(stderr, "failed to initialize mouse!\n");
        return -1;
    }

    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(2);

    // CRIACAO
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display){
        fprintf(stderr, "failed to create display!\n");
        return -1;
    }
    al_set_window_title(display, "Harmony Plates");

    timer = al_create_timer(1.0 / FPS);
    if (!timer){
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }

    if (!al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
        fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
        return -1;
    }

    // FILA
    event_queue = al_create_event_queue();
    if (!event_queue){
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    // CARREGAMENTOS//----------------------------------------------------
    // FONTES
    font_arcade = al_load_font("arcade_font.ttf", 38, 1);
    if (font_arcade == NULL){
        fprintf(stderr, "font file does not exist or cannot be accessed!\n");
    }

    // TRILHA SONORA
    trilha_sonora_pag1 = al_load_sample("trilha_pag1.ogg");
    if (!trilha_sonora_pag1){
        fprintf(stderr, "Falha ao carregar a trilha_sonora_pag1.\n");
    }
    inst_trilha_sonora_pag1 = al_create_sample_instance(trilha_sonora_pag1);
    al_attach_sample_instance_to_mixer(inst_trilha_sonora_pag1, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_trilha_sonora_pag1, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_trilha_sonora_pag1, 0.6);

    trilha_sonora = al_load_sample("trilha_sonora1.ogg");
    if (!trilha_sonora){
        fprintf(stderr, "Falha ao carregar a trilha_sonora.\n");
    }
    inst_trilha_sonora = al_create_sample_instance(trilha_sonora);
    al_attach_sample_instance_to_mixer(inst_trilha_sonora, al_get_default_mixer());
    al_set_sample_instance_playmode(inst_trilha_sonora, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(inst_trilha_sonora, 0.6);

    // POP-UPS
    add_power1 = al_load_bitmap("add_power1.png");
    if (!add_power1){
        fprintf(stderr, "Falha ao carregar a imagem de add_power1.\n");
    }
    add_power2 = al_load_bitmap("add_power2.png");
    if (!add_power2){
        fprintf(stderr, "Falha ao carregar a imagem de add_power2.\n");
    }

    // ICONE
    icon = al_load_bitmap("icone.png");
    if (!icon){
        fprintf(stderr, "Falha ao carregar a imagem de icon.\n");
    }
    al_set_display_icon(display, icon);

    // PAGINAS
    menu = al_load_bitmap("menu1.png");
    if (!menu){
        fprintf(stderr, "Falha ao carregar a imagem de menu.\n");
    }
    play = al_load_bitmap("play1.png");
    if (!play){
        fprintf(stderr, "Falha ao carregar a imagem de play.\n");
    }
    ranking = al_load_bitmap("ranking1.png");
    if (!ranking){
        fprintf(stderr, "Falha ao carregar a imagem de ranking.\n");
    }
    nome_player1 = al_load_bitmap("nome_player1.png");
    if (!nome_player1){
        fprintf(stderr, "Falha ao carregar a imagem de nome_player1.\n");
    }
    player1 = al_load_bitmap("stop.png");
    if (!player1){
        fprintf(stderr, "Falha ao carregar a imagem de player1.\n");
    }
    player2 = al_load_bitmap("2stop.png");
    if (!player2){
        fprintf(stderr, "Falha ao carregar a imagem de player2.\n");
    }
    nome_player2 = al_load_bitmap("nome_player2.png");
    if (!nome_player2){
        fprintf(stderr, "Falha ao carregar a imagem de nome_player2.\n");
    }
    seta = al_load_bitmap("play1.png");
    if (!seta){
        fprintf(stderr, "Falha ao carregar a imagem de seta.\n");
    }
    pag = al_load_bitmap("pag.png");
    if (!pag){
        fprintf(stderr, "Falha ao carregar a imagem de pag.\n");
    }

    // FUNDOS
    fundo1 = al_load_bitmap("fundo1.png");
    if (!fundo1){
        fprintf(stderr, "Falha ao carregar a imagem de fundo1.\n");
    }
    fundo2 = al_load_bitmap("fundo2.png");
    if (!fundo2){
        fprintf(stderr, "Falha ao carregar a imagem de fundo2.\n");
    }
    fundo3 = al_load_bitmap("fundo3.png");
    if (!fundo3){
        fprintf(stderr, "Falha ao carregar a imagem de fundo3.\n");
    }
    margem = al_load_bitmap("margem.png");
    if (!margem){
        fprintf(stderr, "Falha ao carregar a imagem de margem.\n");
    }

    // PRATOS
    frames_pratos[0] = al_load_bitmap("frameprato_0.png");
    frames_pratos[1] = al_load_bitmap("frameprato_1.png");
    frames_pratos[2] = al_load_bitmap("frameprato_2.png");
    frames_pratos[3] = al_load_bitmap("frameprato_3.png");
    frames_pratos[4] = al_load_bitmap("frameprato_4.png");

    // PODER
    poder = al_load_bitmap("poder1.png");
    if (!poder){
        fprintf(stderr, "Failed to load image poder1.png\n");
        return -1;
    }

    // PLAYERS
    // PLAYER1
    for (i = 0; i < N_FRAMES; i++){
        char filename_right[20];
        sprintf(filename_right, "frame_right%d.png", i);
        frames_right[i] = al_load_bitmap(filename_right);
        if (!frames_right[i]) {
            fprintf(stderr, "Falha ao carregar o frame_right %d.\n", i);
            for (j = 0; j < i; j++)
            {
                al_destroy_bitmap(frames_right[j]);
            }
            al_destroy_display(display);
            al_destroy_timer(timer);
            al_destroy_event_queue(event_queue);
            return -1;
        }

        char filename_left[20];
        sprintf(filename_left, "frame_left%d.png", i);
        frames_left[i] = al_load_bitmap(filename_left);
        if (!frames_left[i]){
            fprintf(stderr, "Falha ao carregar o frame_left %d.\n", i);
            for (j = 0; j < i; j++)
            {
                al_destroy_bitmap(frames_left[j]);
            }
            for (j = 0; j < N_FRAMES; j++)
            {
                al_destroy_bitmap(frames_right[j]);
            }
            al_destroy_display(display);
            al_destroy_timer(timer);
            al_destroy_event_queue(event_queue);
            return -1;
        }
    }

    frame_stop[0] = al_load_bitmap("stop.png");
    if (!frame_stop[0]){
        fprintf(stderr, "Falha ao carregar o frame_stop.\n");
        for (i = 0; i < N_FRAMES; i++){
            al_destroy_bitmap(frames_right[i]);
            al_destroy_bitmap(frames_left[i]);
        }
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return -1;
    }

    frame_click[0] = al_load_bitmap("click.png");
    if (!frame_click[0]){
        fprintf(stderr, "Falha ao carregar o frame_click.\n");
        for (i = 0; i < N_FRAMES; i++){
            al_destroy_bitmap(frames_right[i]);
            al_destroy_bitmap(frames_left[i]);
        }
        al_destroy_bitmap(frame_stop[0]);
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return -1;
    }
    // PLAYER2
    for (i = 0; i < N_FRAMES; i++){
        char filename_right2[20];
        sprintf(filename_right2, "2frame_right%d.png", i);
        frames_right2[i] = al_load_bitmap(filename_right2);
        if (!frames_right2[i])
        {
            fprintf(stderr, "Falha ao carregar o 2frame_right %d.\n", i);
            for (j = 0; j < i; j++){
                al_destroy_bitmap(frames_right2[j]);
            }
            al_destroy_display(display);
            al_destroy_timer(timer);
            al_destroy_event_queue(event_queue);
            return -1;
        }

        char filename_left2[20];
        sprintf(filename_left2, "2frame_left%d.png", i);
        frames_left2[i] = al_load_bitmap(filename_left2);
        if (!frames_left2[i]){
            fprintf(stderr, "Falha ao carregar o 2frame_left %d.\n", i);
            for (j = 0; j < i; j++){
                al_destroy_bitmap(frames_left2[j]);
            }
            for (j = 0; j < N_FRAMES; j++){
                al_destroy_bitmap(frames_right2[j]);
            }
            al_destroy_display(display);
            al_destroy_timer(timer);
            al_destroy_event_queue(event_queue);
            return -1;
        }
    }

    frame_stop2[0] = al_load_bitmap("2stop.png");
    if (!frame_stop2[0]) {
        fprintf(stderr, "Falha ao carregar o 2frame_stop.\n");
        for (i = 0; i < N_FRAMES; i++) {
            al_destroy_bitmap(frames_right2[i]);
            al_destroy_bitmap(frames_left2[i]);
        }
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return -1;
    }

    frame_click2[0] = al_load_bitmap("2click.png");
    if (!frame_click2[0]) {
        fprintf(stderr, "Falha ao carregar o 2frame_click.\n");
        for (i = 0; i < N_FRAMES; i++) {
            al_destroy_bitmap(frames_right2[i]);
            al_destroy_bitmap(frames_left2[i]);
        }
        al_destroy_bitmap(frame_stop[0]);
        al_destroy_display(display);
        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        return -1;
    }

    // EVENTOS DA FILA
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    //----------------------------------------------------
    // VARIAVEIS DE PAGINAS
    int fases, principal = 1, paginas = 0;
    int no_play = 0, no_ranking = 0, no_menu = 0;
    int count1 = 1, count2 = 0;
    int choose_player1 = 0, choose_player2 = 0;

    // VARIAVEIS DO JOGO
    Player player, player2;
    Bar array_bar[NUM_BARS];
    Plate pratos[NUM_PRATOS];
    Pow power[3];
    Thick thickness[3];

    int currentFrame_right = 0, currentFrame_left = 0;
    int currentFrame_stop = 0, currentFrame_click = 0;
    int frameCount = 0;
    int currentFrame_right2 = 0, currentFrame_left2 = 0;
    int currentFrame_stop2 = 0, currentFrame_click2 = 0;
    int frameCount2 = 0;
    int frameDelay = 8;

    // VARIAVEIS RANKING
    long long int pontos = 0;
    long long int recorde;
    ALLEGRO_EVENT event;

    while (principal) { // TODO JOGO

        paginas = 1;
        // MENU
        al_start_timer(timer);
        while (paginas == 1){

            al_wait_for_event(event_queue, &event);

            al_play_sample_instance(inst_trilha_sonora_pag1);

            if (event.type == ALLEGRO_EVENT_TIMER){
                al_draw_bitmap(menu, 0, 0, 0);
                draw_menu(menu, play, ranking, no_play, no_ranking);
                al_flip_display();
            }

            if (event.type == ALLEGRO_EVENT_MOUSE_AXES){
                if (event.mouse.x >= SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2 &&
                    event.mouse.x <= SCREEN_WIDTH / 2 - 8 + al_get_bitmap_width(play) / 2 &&
                    event.mouse.y >= SCREEN_HEIGHT / 2 + 40 &&
                    event.mouse.y <= SCREEN_HEIGHT / 2 + 63 + al_get_bitmap_height(play) / 2){
                    no_play = 1;
                }
                else{
                    no_play = 0;
                }

                if (event.mouse.x >= SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2 &&
                    event.mouse.x <= SCREEN_WIDTH / 2 - 8 + al_get_bitmap_width(play) / 2 &&
                    event.mouse.y >= SCREEN_HEIGHT / 2 + 100 &&
                    event.mouse.y <= SCREEN_HEIGHT / 2 + 130 + al_get_bitmap_height(play) / 2){
                    no_ranking = 1;
                }
                else{
                    no_ranking = 0;
                }
            }
            else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if (event.mouse.x >= SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2 &&
                    event.mouse.x <= SCREEN_WIDTH / 2 - 8 + al_get_bitmap_width(play) / 2 &&
                    event.mouse.y >= SCREEN_HEIGHT / 2 + 40 &&
                    event.mouse.y <= SCREEN_HEIGHT / 2 + 63 + al_get_bitmap_height(play) / 2){
                    no_play = 0;
                    paginas = 2;
                }
                else if (event.mouse.x >= SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(play) / 2 &&
                         event.mouse.x <= SCREEN_WIDTH / 2 - 8 + al_get_bitmap_width(play) / 2 &&
                         event.mouse.y >= SCREEN_HEIGHT / 2 + 100 &&
                         event.mouse.y <= SCREEN_HEIGHT / 2 + 130 + al_get_bitmap_height(play) / 2){
                    paginas = 5;
                }
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                paginas = 0;
                principal = 0;
            }
        }

        al_flush_event_queue(event_queue);

        // ESCOLHA DE PLAYERS
        while (paginas == 2) {
            al_wait_for_event(event_queue, &event);

            if (event.type == ALLEGRO_EVENT_TIMER) {
                al_draw_bitmap(pag, 0, 0, 0);
                draw_pag(count1, count2);
                al_flip_display();
            }
            (seta, SCREEN_WIDTH / 2 - al_get_bitmap_width(seta) / 2, SCREEN_HEIGHT - 50 - al_get_bitmap_height(seta) / 2, 0);

            // ALLEGRO_EVENT event;

            if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if (event.mouse.x >= SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(nome_player1) / 2 &&
                    event.mouse.x <= SCREEN_WIDTH / 2 - 8 + al_get_bitmap_width(nome_player1) / 2 &&
                    event.mouse.y >= SCREEN_HEIGHT / 2 + 40 &&
                    event.mouse.y <= SCREEN_HEIGHT / 2 + 63 + al_get_bitmap_height(nome_player1) / 2){
                    count1 += 1;
                }
                else if (event.mouse.x >= SCREEN_WIDTH / 2 - 8 - al_get_bitmap_width(nome_player1) / 2 &&
                         event.mouse.x <= SCREEN_WIDTH / 2 - 8 + al_get_bitmap_width(nome_player1) / 2 &&
                         event.mouse.y >= SCREEN_HEIGHT / 2 + 100 &&
                         event.mouse.y <= SCREEN_HEIGHT / 2 + 130 + al_get_bitmap_height(nome_player1) / 2){
                    count2 += 1;
                }
                else if (event.mouse.x >= SCREEN_WIDTH / 2 - al_get_bitmap_width(play) / 2 &&
                         event.mouse.x <= SCREEN_WIDTH / 2 + al_get_bitmap_width(play) / 2 &&
                         event.mouse.y >= SCREEN_HEIGHT - 80 &&
                         event.mouse.y <= SCREEN_HEIGHT - 50 + al_get_bitmap_height(play)){
                    if (count1 == 1 || count1 % 2 == 1){
                        choose_player1 = 1;
                    }
                    else
                        choose_player1 = 0;
                    if (count2 % 2 == 1){
                        choose_player2 = 1;
                    }
                    else
                        choose_player2 = 0;
                    paginas = 3;
                }
            }
            if (choose_player2 == 0 && choose_player1 == 0) {
                choose_player1 = 1;
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                count1 = 1;
                count2 = 0;
                paginas = 0;
                principal = 0;
            }
        }
        al_stop_sample_instance(inst_trilha_sonora_pag1);

        // PAGINA DE TRANSICAO
        while (paginas == 3){
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_flip_display();

            al_rest(0.2);
            paginas = 4;
        }

        // INICIALIZACAO JOGO EM SI
        if (paginas == 4){
            init_bar(array_bar, BAR_START_Y);
            init_plate(pratos, array_bar);
            init_power(power, thickness, &var_power);
            if (choose_player1 == 1)
                init_player(&player);
            if (choose_player2 == 1)
                init_player(&player2);
        }
        al_flush_event_queue(event_queue);
        al_set_timer_count(timer, 0);

        al_play_sample_instance(inst_trilha_sonora);
        // JOGO EM SI
        while (paginas == 4){

            al_wait_for_event(event_queue, &event);
            fases = al_get_timer_count(timer);

            if (event.type == ALLEGRO_EVENT_TIMER){
                // FASES

                if (fases >= 0 && fases <= 2500){
                    FASE = 1;
                    draw_scenario(fundo1, margem, timer, font_arcade);
                    NUM_BARS = 7;
                    NUM_PRATOS = 7;
                }
                else if (fases > 2500 && fases <= 4800){
                    FASE = 2;
                    draw_scenario(fundo2, margem, timer, font_arcade);
                    NUM_BARS = 9;
                    NUM_PRATOS = 9;
                }
                else if (fases > 4800){
                    FASE = 3;
                    draw_scenario(fundo3, margem, timer, font_arcade);
                    NUM_BARS = 11;
                    NUM_PRATOS = 11;
                }
                if (fases <= 3 && fases >= 0 || fases < 2502 && fases >= 2501 || fases < 4802 && fases >= 4801){
                    draw_transicao(font_arcade);
                    init_bar(array_bar, BAR_START_Y);
                    init_plate(pratos, array_bar);
                    init_power(power, thickness, &var_power);
                }

                // ANIMACAO PLAYER1

                if (player.dir){
                    frameCount++;
                    if (frameCount >= frameDelay){
                        currentFrame_right = (currentFrame_right + 1) % N_FRAMES;
                        frameCount = 0;
                    }
                }
                else if (player.esq){
                    frameCount++;
                    if (frameCount >= frameDelay){
                        currentFrame_left = (currentFrame_left + 1) % N_FRAMES;
                        frameCount = 0;
                    }
                }

                else{
                    frameCount++;
                    if (frameCount >= frameDelay){
                        if (player.espaco){
                            currentFrame_click = (currentFrame_click + 1) % N_FRAMES_C;
                            player.espaco = 0;
                        }
                        else{
                            currentFrame_stop = (currentFrame_stop + 1) % N_FRAMES_P;
                        }
                        frameCount = 0;
                    }
                }

                // ANIMACAO PLAYER 2
                if (player2.dir){
                    frameCount2++;
                    if (frameCount2 >= frameDelay){
                        currentFrame_right2 = (currentFrame_right2 + 1) % N_FRAMES;
                        frameCount2 = 0;
                    }
                }
                else if (player2.esq){
                    frameCount2++;
                    if (frameCount2 >= frameDelay){
                        currentFrame_left2 = (currentFrame_left2 + 1) % N_FRAMES;
                        frameCount2 = 0;
                    }
                }

                else{
                    frameCount2++;
                    if (frameCount2 >= frameDelay){
                        if (player2.espaco){
                            currentFrame_click2 = (currentFrame_click2 + 1) % N_FRAMES_C;
                            player2.espaco = 0;
                        }
                        else{
                            currentFrame_stop2 = (currentFrame_stop2 + 1) % N_FRAMES_P;
                        }
                        frameCount2 = 0;
                    }
                }
                update_power(power, thickness, timer, player, player2, &time_power, &var_power, bar_power);
                if (choose_player1 == 1){
                    update_player(&player);
                    draw_player1(player, currentFrame_right, currentFrame_left, currentFrame_stop, currentFrame_click);
                }
                if (choose_player2 == 1){
                    update_player(&player2);
                    draw_player2(player2, currentFrame_right2, currentFrame_left2, currentFrame_stop2, currentFrame_click2);
                }
                efeito_power(power, thickness, timer, &time_power);
                update_plate(pratos, timer, player, player2, array_bar, power, thickness, &var_power);
                update_bar(pratos, timer, player, player2, array_bar, power, thickness, bar_power);
                draw_bar(array_bar);
                update_bar(pratos, timer, player, player2, array_bar, power, thickness, bar_power);
                draw_plate(pratos, imagemPrato, fundo1, margem, &paginas, timer, font_arcade, array_bar, player, player2, currentFrame_right, currentFrame_left, currentFrame_stop, currentFrame_click, choose_player1, choose_player2);
                draw_power(power, thickness, poder);
                draw_popUp(add_power1, add_power2, power, thickness, array_bar, bar_power);
            

                al_flip_display();
                
            }

            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                principal = 0;
                paginas = 0;
            }
            else if (event.type == ALLEGRO_EVENT_KEY_DOWN){
                if (event.keyboard.keycode == ALLEGRO_KEY_A){
                    player.esq = 1;
                    player.dir = 0;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_D){
                    player.dir = 1;
                    player.esq = 0;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE){
                    player.espaco = 1;
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_LEFT){
                    player2.esq = 1;
                    player2.dir = 0;
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT){
                    player2.dir = 1;
                    player2.esq = 0;
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_UP){
                    player2.espaco = 1;
                }
            }

            else if (event.type == ALLEGRO_EVENT_KEY_UP){
                switch (event.keyboard.keycode)
                {
                case ALLEGRO_KEY_A:
                    player.esq = 0;
                    break;

                case ALLEGRO_KEY_LEFT:
                    player2.esq = 0;
                    break;

                case ALLEGRO_KEY_D:
                    player.dir = 0;
                    break;

                case ALLEGRO_KEY_RIGHT:
                    player2.dir = 0;
                    break;

                case ALLEGRO_KEY_SPACE:
                    player.espaco = 0;
                    break;

                case ALLEGRO_KEY_UP:
                    player2.espaco = 0;
                    break;
                }
            }
        }
        al_stop_sample_instance(inst_trilha_sonora);

        pontos = (int)al_get_timer_count(timer);
        recorde = record(pontos);

        while (paginas == 5) {
            al_set_timer_count(timer, 0);
            draw_record_screen(pontos, recorde, font_arcade);
            al_flip_display();

            ALLEGRO_EVENT event;
            al_wait_for_event(event_queue, &event);
            if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.mouse.x >= SCREEN_WIDTH / 2 - 150 &&
                    event.mouse.x <= SCREEN_WIDTH / 2 + 150 &&
                    event.mouse.y >= SCREEN_HEIGHT - 60 &&
                    event.mouse.y <= SCREEN_HEIGHT - 20)
                {
                    paginas = 1;
                }
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                paginas = 0;
                principal = 0;
                break;
            }
        }
    }

    // DESTRUICAO
    for (i = 0; i < N_FRAMES; i++)
    {
        al_destroy_bitmap(frames_right[i]);
        al_destroy_bitmap(frames_left[i]);
    }
    for (i = 0; i < N_FRAMES; i++)
    {
        al_destroy_bitmap(frames_right2[i]);
        al_destroy_bitmap(frames_left2[i]);
    }
    for (i = 0; i < N_FRAMES_P; i++)
    {
        al_destroy_bitmap(frames_pratos[i]);
    }
    al_destroy_bitmap(frame_stop[0]);
    al_destroy_bitmap(frame_click[0]);
    al_destroy_bitmap(frame_stop2[0]);
    al_destroy_bitmap(frame_click2[0]);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font_arcade);
    al_destroy_sample(trilha_sonora);
    al_destroy_sample(trilha_sonora_pag1);
    al_destroy_sample_instance(inst_trilha_sonora_pag1);
    al_destroy_sample_instance(inst_trilha_sonora);

    return 0;
}
