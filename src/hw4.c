#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT1 2201
#define PORT2 2202 // Port for client 2
#define BUFFER_SIZE 1024

int opt = 1;

//I encoded the letter of the direction they go. So r moves right and in the matrix i+1; 
    
/* Key:
r -> i +1
l -> i -1
u -> j -1
d -> j +1*/
char ***shapes() {
    char ***tetris_shapes = malloc(sizeof(char**) * 7); // 7 shapes in total
    
    for (int i = 0; i < 7; i++) {
        tetris_shapes[i] = malloc(sizeof(char*) * 4); // Each shape has up to 4 orientations
        for (int j = 0; j < 4; j++) {
            tetris_shapes[i][j] = malloc(sizeof(char) * 4); // Allocate space for each direction string
        }
    }

    // Initialize shapes and orientations
    strcpy(tetris_shapes[0][0], "rdl"); // Square shape
    strcpy(tetris_shapes[0][1], "rdl"); // Same orientation for square
    strcpy(tetris_shapes[0][2], "rdl");
    strcpy(tetris_shapes[0][3], "rdl");

    strcpy(tetris_shapes[1][0], "ddd"); // Vertical line
    strcpy(tetris_shapes[1][1], "rrr"); // Horizontal line
    strcpy(tetris_shapes[1][2], "ddd"); 
    strcpy(tetris_shapes[1][3], "rrr");

    // Continue for the remaining shapes
    strcpy(tetris_shapes[2][0], "rur");
    strcpy(tetris_shapes[2][1], "drd");
    strcpy(tetris_shapes[2][2], "rur");
    strcpy(tetris_shapes[2][3], "drd");

    strcpy(tetris_shapes[3][0], "ddr");
    strcpy(tetris_shapes[3][1], "durr");
    strcpy(tetris_shapes[3][2], "rdd");
    strcpy(tetris_shapes[3][3], "rru");

    strcpy(tetris_shapes[4][0], "rdr");
    strcpy(tetris_shapes[4][1], "duru");
    strcpy(tetris_shapes[4][2], "rdr");
    strcpy(tetris_shapes[4][3], "duru");

    strcpy(tetris_shapes[5][0], "ruu");
    strcpy(tetris_shapes[5][1], "drr");
    strcpy(tetris_shapes[5][2], "rldd");
    strcpy(tetris_shapes[5][3], "rrd");

    strcpy(tetris_shapes[6][0], "rdur");
    strcpy(tetris_shapes[6][1], "rudd");
    strcpy(tetris_shapes[6][2], "rudr");
    strcpy(tetris_shapes[6][3], "drld");

    return tetris_shapes;
}



int** begin(int row, int col){
    int** board = malloc(sizeof(int*) * row);
    for(int i = 0; i < row; i++){
        board[i] = malloc(sizeof(int) * col);
    }

    // Set all elements to zero
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            board[i][j] = 0;
        }
    }

    return board;
}

int initialize(char* buffer, int** board, int max_row, int max_col){ // Need to add cases where number is greater than what the dimension, not a number, etc
    int value[20]; //In total 5 pieces and each has 4 numbers to represent them
    int count = 0;
    int index = 2; //The I and space takes up 0 and 1
    while(sscanf(&buffer[index], "%d", &value[count]) == 1){
        count++;
        index += 2;
        if(index > 41){ //Max can have is 21 characters + 20 spaces.
            break;
        }
        
    }

    char*** tetris_shapes = shapes();
    int k = 0;
    for(int i = 0; i < 20; i+=4){
        
        int shapes_row = value[i];
        int shapes_col = value[i+1];
        int row = value[i+2];
        int col = value[i+3];
        board[row][col] = 1; //Make it to one meaning the cell is occupied
        char* word = tetris_shapes[shapes_row - 1][shapes_col - 1]; //Subtract 1 because my tetris_shapes is 0 indexing. 
        for(int j = 0; j < strlen(tetris_shapes[shapes_row][shapes_col]); j++){
            if(word[j] == 'd'){
                row++;
                if(row > max_row) {
                    printf("Error.This goes above beyond the max_row dimensions.");
                    exit(EXIT_FAILURE);
                }
                board[row][col] = 1;
            }
            if(word[j] == 'u'){
                row--;
                if(row < 0) {
                    printf("Error.This goes below beyond the max_row dimensions.");
                    exit(EXIT_FAILURE);
                }
                board[row][col] = 1;
            }
            if(word[j] == 'r'){
                col++;
                if(col > max_col) {
                    printf("Error.This goes above beyond the max_col dimensions.");
                    exit(EXIT_FAILURE);
                }
                board[row][col] = 1;
            }
            if(word[j] == 'l'){
                col--;
                if(col < 0) {
                    printf("Error.This goes below beyond the max_row dimensions.");
                    exit(EXIT_FAILURE);
                }
                board[row][col] = 1;
            }
            
        }
    }

}

int main(){
    int listen_fd1, conn_fd1, listen_fd2, conn_fd2;
    struct sockaddr_in address1, address2;
    int addrlen1 = sizeof(address1), addrlen2 = sizeof(address2);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket for client 1
    if((listen_fd1 = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket 1 failed");
        exit(EXIT_FAILURE);
    }
    if((listen_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket 2 failed");
        close(listen_fd1); // Close other socket on failure
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listen_fd1, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(listen_fd1, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        exit(EXIT_FAILURE);
    }


    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = INADDR_ANY;
    address1.sin_port = htons(PORT1);

    address2.sin_family = AF_INET;
    address2.sin_addr.s_addr = INADDR_ANY;
    address2.sin_port = htons(PORT2);

    // Bind sockets to ports
    if (bind(listen_fd1, (struct sockaddr *)&address1, addrlen1) < 0){
        perror("[Server] bind() failed for client 1.");
        close(listen_fd1); close(listen_fd2);
        exit(EXIT_FAILURE);
    }

    if (bind(listen_fd2, (struct sockaddr *)&address2, addrlen2) < 0){
        perror("[Server] bind() failed for client 2.");
        close(listen_fd1); close(listen_fd2);
        exit(EXIT_FAILURE);
    }

    // Start listening on both sockets
    if (listen(listen_fd1, 3) < 0){
        perror("[Server] listen() failed.");
        close(listen_fd1); close(listen_fd2);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd2, 3) < 0){
        perror("[Server] listen() failed.");
        close(listen_fd1); close(listen_fd2);
        exit(EXIT_FAILURE);
    }

    printf("[Server] Running on ports %d and %d\n", PORT1, PORT2);

    // Accept clients
    if ((conn_fd1 = accept(listen_fd1, (struct sockaddr *)&address1, (socklen_t *)&addrlen1)) < 0){
        perror("[Server] accept() failed for client 1.");
        close(listen_fd1); close(listen_fd2);
        exit(EXIT_FAILURE);
    }
    printf("[Server] Client 1 connected\n");

    if ((conn_fd2 = accept(listen_fd2, (struct sockaddr *)&address2, (socklen_t *)&addrlen2)) < 0){
        perror("[Server] accept() failed for client 2.");
        close(conn_fd1); close(listen_fd1); close(listen_fd2);
        exit(EXIT_FAILURE);
    }
    printf("[Server] Client 2 connected\n");

    // Implementing game logic here
    int join_game = 0;
    int max_row, max_col;
    int **board1;
    int **board2;
    int nbytes1 = read(conn_fd1, buffer, BUFFER_SIZE);
    if (nbytes1 > 0 && buffer[0] == 'B') {
        printf("[Server] Client 1 wants to begin.\n");
        int row;
        int col;
        if (sscanf(buffer, "B %d %d", &row, &col) == 2) { //Kinda like scanf but it can read integers from characters.
            printf("[Server] Received dimensions: row=%d, col=%d\n", row, col);
            board1 = begin(row, col);  // Call the begin function with extracted row and col
            board2 = begin(row, col); //Just realized I need two boards for two players to play.
            max_row = row - 1; //Keep track of the dimensions for edge cases
            max_col = col - 1;
            send(conn_fd1, "Waiting for Client 2 to join...\n", 32, 0);
        } else {
            printf("[Server] Failed to parse dimensions from the string.\n");
        }
        send(conn_fd1, "Waiting for Client 2 to join...\n", 32, 0);
    }

    int nbytes2 = read(conn_fd2, buffer, BUFFER_SIZE);
    if (nbytes2 > 0 && buffer[0] == 'B') {
        printf("[Server] Client 2 wants to begin.\n");
        send(conn_fd2, "Waiting for Client 1 to join...\n", 32, 0);
    }

    // Confirm both players are in the game
    if (nbytes1 > 0 && nbytes2 > 0 && buffer[0] == 'B') {
        join_game = 1;
        send(conn_fd1, "Both players joined. Game start!\n", 33, 0);
        send(conn_fd2, "Both players joined. Game start!\n", 33, 0);
    } else {
        printf("[Server] One or both clients did not initiate the game properly.\n");
    }

    // Game loop
    while(join_game == 1){
        memset(buffer, 0, BUFFER_SIZE);

        // Read from client 1 and send to client 2
        nbytes1 = read(conn_fd1, buffer, BUFFER_SIZE);
        if (nbytes1 <= 0){
            perror("[Server] read() failed.");
            break;
        }
        if(buffer[0] == 'I'){
            printf("Initializing Board");
            initialize(buffer, board1, max_row, max_col); //Player 1 initialize board
            send(conn_fd2, "Successfully initialized\n", 25, 0);
        }
        send(conn_fd2, buffer, nbytes1, 0);

        // Check for "quit" command
        if (strcmp(buffer, "quit") == 0){
            printf("[Server] Client 1 quit.\n");
            send(conn_fd2, buffer, strlen(buffer), 0);
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);

        // Read from client 2 and send to client 1
        nbytes2 = read(conn_fd2, buffer, BUFFER_SIZE);
        if (nbytes2 <= 0){
            perror("[Server] read() failed.");
            break;
        }
        if(buffer[0] == 'I'){
            printf("Intializing Board");
            initialize(buffer, board2, max_row, max_col); //Player 2 initialize board
            send(conn_fd1, "Successfully initialized\n", 25, 0);
        }
        
        send(conn_fd1, buffer, nbytes2, 0);

        // Check for "quit" command
        if (strcmp(buffer, "quit") == 0){
            printf("[Server] Client 2 quit.\n");
            send(conn_fd1, buffer, strlen(buffer), 0);
            break;
        }
    }

    // Clean up
    close(conn_fd1);
    close(conn_fd2);
    close(listen_fd1);
    close(listen_fd2);

    printf("[Server] Connection ends\n");
    return 0;
}
