#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

pid_t pidB; // Глобальная переменная для хранения PID процесса B

void handle_SIGUSR1(int sig) {
    // Обработчик сигнала SIGUSR1
}

void handle_SIGUSR2(int sig) {
    // Обработчик сигнала SIGUSR2
}

int main() {
    signal(SIGUSR1, handle_SIGUSR1);
    signal(SIGUSR2, handle_SIGUSR2);

    pidB = fork(); // Создаем процесс B

    if (pidB < 0) {
        perror("Ошибка fork");
        exit(EXIT_FAILURE);
    }

    if (pidB == 0) { // Процесс B
        int fb = open("Fb.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fb < 0) {
            perror("Ошибка открытия файла Fb");
            exit(EXIT_FAILURE);
        }

        const char *Sb = "Строка из процесса B\n";
        write(fb, Sb, strlen(Sb));
        close(fb);

        kill(getppid(), SIGUSR1); // Уведомляем процесс A о завершении записи
        pause(); // Ожидаем сигнал от A

        // Чтение файла Fa
        char buffer[100];
        int fa = open("Fa.txt", O_RDONLY);
        if (fa >= 0) {
            while (read(fa, buffer, sizeof(buffer)) > 0) {
                write(STDOUT_FILENO, buffer, strlen(buffer));
            }
            close(fa);
        }
        exit(EXIT_SUCCESS);
    } else { // Процесс A
        int fa = open("Fa.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fa < 0) {
            perror("Ошибка открытия файла Fa");
            exit(EXIT_FAILURE);
        }

        const char *Sa = "Строка из процесса A\n";
        write(fa, Sa, strlen(Sa));
        close(fa);

        pause(); // Ожидаем сигнал от B

        kill(pidB, SIGUSR2); // Уведомляем процесс B о завершении записи

        // Чтение файла Fb
        char buffer[100];
        int fb = open("Fb.txt", O_RDONLY);
        if (fb >= 0) {
            while (read(fb, buffer, sizeof(buffer)) > 0) {
                write(STDOUT_FILENO, buffer, strlen(buffer));
            }
            close(fb);
        }
        
        wait(NULL); // Ожидание завершения процесса B
    }

    return EXIT_SUCCESS;
}