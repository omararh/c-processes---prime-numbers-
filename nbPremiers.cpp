#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>

int isPrime(int nombre)
{
    if (nombre <= 1) {
        return 0;
    }
    for (int i = 2; i * i <= nombre; ++i) {
        if (nombre % i == 0) {
            return 0;
        }
    }
    return 1;
}

void verify(int e, int p) {
    int fd[2];
    if (pipe(fd) == -1) { // Tube creation
        std::cerr << "Erreur de création du tube: " << strerror(errno) << std::endl;
        return;
    }


    int ep = e / p;

    for(int i = 0 ; i < p ; i ++) {
        int start = i * ep + 1;
        int end = (i + 1) * ep;

        pid_t pid;
        pid = fork();
        if(pid == 0) {
            for(int j = start ; j <= end; j++) {
                if(isPrime(j)) {
                    if(write(fd[1], &j, sizeof(j)) == -1) {
                        std::cerr << "Erreur d'écriture: " << strerror(errno) << std::endl;
                        break;
                    }
                }
            }
            exit(0);
        }
        start += ep;
    }

    for(int k = 0 ; k < p  ; k++) {
        wait(NULL);
    }

    std::vector<int> ordredPrimaryNumbers;
    close(fd[1]);
    fcntl(fd[0], F_SETFL, O_NONBLOCK);

    int i;
    while (read(fd[0], &i, sizeof(i)) > 0) {
        ordredPrimaryNumbers.push_back(i);
    }

    std::sort(ordredPrimaryNumbers.begin(), ordredPrimaryNumbers.end());
    for(auto& element : ordredPrimaryNumbers) { std::cout << element << " "; }
}

int main(int argc, char* argv[]) {

    if(argc != 3) {
        std::cout << " agruments needed <e (int)> <p (int)> ";
        return 1;
    }

    int e = atoi(argv[1]);
    int p = atoi(argv[2]);

    verify(e, p);

    return 0;
}