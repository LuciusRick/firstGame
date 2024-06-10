#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define WIDTH 1280
#define HEIGHT 720
#define PLAYER_WIDTH 120
#define PLAYER_HEIGHT 120
#define GROUND_HEIGHT 64

// Estrutura para representar o jogador
typedef struct {
    int x, y; // posição do jogador
    int on_ground; // flag para indicar se o jogador está no chão
} Player;

// Função para configurar o terminal para entrada não bloqueante
void set_nonblock(int fd) {
    struct termios ttystate;

    tcgetattr(fd, &ttystate);
    ttystate.c_lflag &= ~(ICANON | ECHO); // Desativa o modo canônico e eco
    ttystate.c_cc[VMIN] = 0;
    tcsetattr(fd, TCSANOW, &ttystate);
}

// Função para detectar colisões entre o jogador e o chão
int detect_collision(Player player) {
    // Verifica se a parte inferior do jogador está acima ou no nível do chão
    return player.y + PLAYER_HEIGHT >= HEIGHT - GROUND_HEIGHT;
}

// Função para desenhar o jogador na tela
void draw_player(Player player, char *screen) {
    int i, j;
    for (i = 0; i < PLAYER_HEIGHT; i++) {
        for (j = 0; j < PLAYER_WIDTH; j++) {
            if (i == 0 || i == PLAYER_HEIGHT - 1 || j == 0 || j == PLAYER_WIDTH - 1)
                screen[(player.y + i) * WIDTH + player.x + j] = 'R'; // Cor vermelha para o jogador
        }
    }
}

// Função para desenhar o chão na tela
void draw_ground(char *screen) {
    int i, j;
    for (i = 0; i < GROUND_HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            screen[(HEIGHT - GROUND_HEIGHT + i) * WIDTH + j] = 'B'; // Cor azul para o chão
        }
    }
}

// Função para desenhar o background na tela
void draw_background(char *screen) {
    int i;
    for (i = 0; i < WIDTH * HEIGHT; i++) {
        screen[i] = 'G'; // Cor verde para o background
    }
}

// Função principal do jogo
int main() {
    char screen[WIDTH * HEIGHT]; // Tela do jogo
    Player player = {WIDTH / 2 - PLAYER_WIDTH / 2, HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT, 0}; // Inicializa o jogador
    int fd = fileno(stdin); // Obtém o descritor de arquivo para o stdin
    set_nonblock(fd); // Configura o terminal para entrada não bloqueante

    char input;
    int running = 1;

    // Loop principal do jogo
    while (running) {
        draw_background(screen); // Desenha o background
        draw_ground(screen); // Desenha o chão
        draw_player(player, screen); // Desenha o jogador

        system("clear"); // Limpa a tela
        int i;
        for (i = 0; i < WIDTH * HEIGHT; i++) {
            putchar(screen[i]); // Exibe a tela
            if ((i + 1) % WIDTH == 0)
                putchar('\n');
        }

        usleep(50000); // Pequena pausa para controlar a taxa de atualização da tela

        // Leitura da entrada do jogador
        input = getchar();
        if (input != EOF) {
            if (input == 'a' && player.x > 0) // Move o jogador para a esquerda
                player.x--;
            else if (input == 'd' && player.x < WIDTH - PLAYER_WIDTH) // Move o jogador para a direita
                player.x++;
            else if (input == 's') // Faz o jogador abaixar (não implementado neste exemplo)
                continue;
            else if (input == '\n') // Inicia o jogo ao pressionar Enter
                running = 0;
        }

        // Verifica se o jogador está no chão
        player.on_ground = detect_collision(player);
    }

    return 0;
}
