#include <winsock2.h>
#include <stdio.h>
#include "slre.h"
#include "cJSON.h"
#include <string.h>
#include <windows.h>
/* including libraries */                  /*please note you need to be able to run ANSI ESCAPE CODES for
                                            SO clion terminal or cmd doesnt work on windows please
                                            test the program on WINDOWS10 NEW TERMINAL or any unix based
                                            operating system */

// PROJECT  CWITTER (C TWITTER)
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define CYAN    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define BLUE    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
/* including color macros*/
void buffer();
void Logout();
void twitter();
void refresh();
void timeline();
void liking(int );
void sendtweet();
void setPersonalArea();
void setbio();
void TweetProfile();
void changePass();
int start(int );
void end(int , int);
char *senddata(char *data);
char *sendBigdata(char *data);
int menu();
void search();
void Register();
char user_token[200] = {'\0'};
int port;
/*including prototyes*/
int main() {
    scanf("%d",&port);
    buffer();
    Register();
}
void Register(){
    int a = 0;
    int b = 1;
    while(b != -1) {
        do {
            a = menu();
            b = a;
        } while (a != 1 && a != -1);
        if(a != -1) {
            twitter();
        }
    }
}
int menu(){
    printf(WHITE "welcome to Cwitter!\n"GREEN"1-signup:\n"CYAN"2-log in:\n"RED"3-Exit\n" RESET);
    int choice;
    char username[50] = "";
    char password[50] ="";
    char a[50] = "";
    scanf("%d",&choice);
    switch(choice){
        case 1:
            printf(BLUE"username :"RESET);
            scanf("%s",username);
            getchar();
            printf(BLUE"password :"RESET);
            scanf("%s",password);
            printf(RESET);
            getchar();
            sprintf(a ,"signup %s,%s\n",username,password);
            break;
        case 2:
            printf(BLUE"username :"RESET);
            scanf("%s",username);
            printf(BLUE"password :"RESET);
            scanf("%s",password);
            sprintf(a ,"login %s,%s\n",username,password);
            break ;
        case 3:
            return -1;
    }
    char *reply = senddata(a);
    int numcaps = 2;
    char caps[2][100000] = {0};
    char regex[200] = ":\"([A-Za-z]+)\",\"message\":\"([A-Za-z0-9-_ ]*)";
    slre_match(regex,reply,100000,caps, numcaps, 0);
    if(!strcmp(caps[0],"Error")){
        printf(RED"%s\n"RESET,caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Successful")){
        printf(GREEN"You have successfully signed up, please loggin to your account\n"RESET);
        return 0;
    }
    else if(!strcmp(caps[0],"Token")){
        memset(user_token,0,200);
        strcpy(user_token,caps[1]);
        return 1;
    }

}
void twitter(){
    int choice = 0;
    while(choice != 5) {
        printf(BLUE"1-Timeline\n2-Search\n3-Tweet Profile\n4-Personal area\n5-Log out\n"RESET);
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                timeline();
                break;
            case 2:
                search();
                break;
            case 3:
                TweetProfile();
                break;
            case 4:
                setPersonalArea();
                break;
            case 5:
                Logout();
        }
    }
    return ;
}
void timeline(){
    int choice = 0;
    while(choice != 3) {
        printf(BLUE"1-send tweet\n2-refresh\n3-Back\n"RESET);
        scanf("%d",&choice);
        getchar();
        switch (choice) {
            case 1:
                sendtweet();
                break;
            case 2:
                refresh();
                break;
            case 3:
                break;
        }
    }
}
void TweetProfile(){
    char a[150];
    char *reply;
    sprintf(a ,"profile %s\n",user_token);
    reply = sendBigdata(a);
    //printf("%s",reply);
    cJSON *root = cJSON_Parse(reply);
    cJSON *message = cJSON_GetObjectItem(root,"message");
    cJSON *username = cJSON_GetObjectItem(message, "username");
    cJSON *bio = cJSON_GetObjectItem(message,"bio");
    cJSON *numberOfFollowers = cJSON_GetObjectItem(message , "numberOfFollowers");
    cJSON *numberOfFollowings = cJSON_GetObjectItem(message ,"numberOfFollowings");
    printf(CYAN"*******************************\n"
           "Username : %s \n"
           "bio : %s      \n"
           "followers : %d   following : %d\n"
           "*******************************\n"RESET,username ->valuestring,bio ->valuestring,
           numberOfFollowers->valueint,numberOfFollowings ->valueint);
    cJSON *tweets = NULL;
    cJSON *alltweets = cJSON_GetObjectItem(message ,"allTweets");
    cJSON_ArrayForEach(tweets ,alltweets){
        cJSON *content = cJSON_GetObjectItem(tweets ,"content");
        cJSON *comments = cJSON_GetObjectItem(tweets , "comments");
        cJSON *likes = cJSON_GetObjectItem(tweets ,"likes");
        printf("%s\ncomments : %d   Likes : %d\n",content ->valuestring,cJSON_GetArraySize(comments),
               likes -> valueint);
    }
    cJSON_Delete(root);
    buffer();
    puts("Press any key to go back");
    getchar();
}
void setPersonalArea(){
    int choice = 0;
    while(choice != 3) {
        printf(BLUE"1-set bio:\n2-change password\n3-Back\n"RESET);
        scanf("%d",&choice);
        switch (choice) {
            case 1:
                setbio();
                break;
            case 2:
                changePass();
                break;
            case 3:
                break;
        }
    }
    return;
}
void setbio(){
    puts(BLUE"write something about yourself"RESET);
    char a[200] = {'\0'};
    char bio[200] = {'\0'};
    getchar();
    gets(bio);
    sprintf(a ,"set bio %s, %s\n",user_token,bio);
    char*reply = senddata(a);
    cJSON *root = cJSON_Parse(reply);
    cJSON *type = cJSON_GetObjectItem(root,"type");
    cJSON *message = cJSON_GetObjectItem(root,"message");
    if(!strcmp((type ->valuestring),"Error")) {
        printf(RED"%s\n"RESET, message->valuestring);
    }
    else{
        printf(GREEN"%s\n"RESET, message->valuestring);
    }
}
void changePass(){
    char a[50];
    char b[50];
    char c[200];
    printf(YELLOW"Enter you current password:");
    scanf("%s", a);
    printf("Enter New password:");
    scanf("%s", b);
    sprintf(c, "change password %s, %s, %s\n"RESET, user_token, a, b);
    char *reply = senddata(c);
    cJSON *root = cJSON_Parse(reply);
    cJSON *type = cJSON_GetObjectItem(root,"type");
    cJSON *message = cJSON_GetObjectItem(root , "message");
    if(!strcmp((type ->valuestring),"Error")) {
        printf(RED"%s\n"RESET, message->valuestring);
    }
    else{
        printf(GREEN"%s\n"RESET, message->valuestring);
    }
}
void search(){
    char *note;
    puts(BLUE"whos on your mind?"RESET);
    char user[20];
    scanf("%s",user);
    char search[100] = "";
    sprintf(search ,"search %s,%s\n",user_token,user);
    char *reply = senddata(search);
    cJSON *root = cJSON_Parse(reply);
    cJSON *type = cJSON_GetObjectItem(root,"type");
    if(strcmp((type ->valuestring),"Error")) {
        cJSON *message = cJSON_GetObjectItem(root, "message");
        cJSON *username = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *numberOfFollowers = cJSON_GetObjectItem(message, "numberOfFollowers");
        cJSON *numberOfFollowings = cJSON_GetObjectItem(message, "numberOfFollowings");
        cJSON *status = cJSON_GetObjectItem(message, "followStatus");
        printf(CYAN"*******************************\n"
               "Username : %s\n"
               "bio : %s\n"
               "followers : %d   following : %d\n"
               "%s \n"
               "*******************************\n"RESET, username->valuestring, bio->valuestring,
               numberOfFollowers->valueint, numberOfFollowings->valueint, status->valuestring);
        cJSON *tweets = NULL;
        cJSON *alltweets = cJSON_GetObjectItem(message, "allTweets");
        cJSON_ArrayForEach(tweets, alltweets) {
            cJSON *content = cJSON_GetObjectItem(tweets, "content");
            cJSON *comments = cJSON_GetObjectItem(tweets, "comments");
            cJSON *likes = cJSON_GetObjectItem(tweets, "likes");
            printf(BLUE"%s\ncomments : %d   Likes : %d\n-------------------------\n"RESET
                    , content->valuestring, cJSON_GetArraySize(comments),
                   likes->valueint);
        }
        printf(RED"You have %s %s \n"RESET
               BLUE"in order to follow this user type follow"
               "\nin order to unfollow this user type unfollow"
               "\notherwise type back\n"RESET, status->valuestring, username->valuestring);
        char changeStatus[50];
        buffer();
        gets(changeStatus);
        if (!strcmp(changeStatus, "follow")) {
            char sending[150] = "";
            sprintf(sending, "follow %s, %s\n", user_token, username->valuestring);
            note = sendBigdata(sending);
        } else if (!strcmp(changeStatus, "unfollow")) {
            char sending[150] = "";
            sprintf(sending, "unfollow %s, %s\n", user_token, username->valuestring);
            note = sendBigdata(sending);
        }
        else{
            return;;
        }
    }
    else{
        printf(RED"Username is not valid\n"RED);
        return;
    }
    cJSON *root2 = cJSON_Parse(note);
    cJSON *type2 = cJSON_GetObjectItem(root2,"type");
    cJSON *message2 = cJSON_GetObjectItem(root2,"message");
    if(!strcmp((type2 ->valuestring),"Error")) {
        printf(RED"%s\n"RESET, message2->valuestring);
    }
    else{
        printf(GREEN"%s\n"RESET, message2->valuestring);
    }
    cJSON_Delete(root);
    cJSON_Delete(root2);
}
void refresh(){
    char a[150] = "";
    char *reply;
    sprintf(a, "refresh %s\n", user_token);
    reply = sendBigdata(a);
    cJSON *root = cJSON_Parse(reply);
    cJSON *message = cJSON_GetObjectItem(root, "message");
    cJSON *tweet = NULL;
    cJSON_ArrayForEach(tweet, message) {
        cJSON *subtweet = NULL;
        cJSON *id = cJSON_GetObjectItem(tweet, "id");
        cJSON *content = cJSON_GetObjectItem(tweet, "content");
        cJSON *author = cJSON_GetObjectItem(tweet, "author");
        cJSON *like = cJSON_GetObjectItem(tweet, "likes");
        cJSON *comments = cJSON_GetObjectItem(tweet, "comments");
        printf(BLUE"____________________________\n");
        printf("%s   id:%d\n", author->valuestring, id->valueint);
        printf("%s\n", content->valuestring);
        printf("likes : %d  Comments : %d\n"RESET, like->valueint, cJSON_GetArraySize(comments));
    }
    cJSON_Delete(root);
    while(1) {
        printf(BLUE"1-like or comment\n2-see comments\n3-Back\n"RESET);
        int choice, userid;
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                puts(BLUE"Enter desired comments id"RESET);
                scanf("%d", &userid);
                liking(userid);
                break;
            case 2:
                puts(BLUE"Enter desired comments id"RESET);
                scanf("%d", &userid);
                cJSON *root2 = cJSON_Parse(reply);
                cJSON *message2 = cJSON_GetObjectItem(root2, "message");
                cJSON *tweet2 = NULL;
                cJSON_ArrayForEach(tweet2, message2) {
                    cJSON *subtweet = NULL;
                    cJSON *id = cJSON_GetObjectItem(tweet2, "id");
                    cJSON *comments = cJSON_GetObjectItem(tweet2, "comments");
                    if (userid == (id->valueint)) {
                        for (int i = 0; i < cJSON_GetArraySize(comments); i++) {
                            cJSON *sub = cJSON_GetArrayItem(comments, i);
                            printf("%s : %s\n", sub->string, sub->valuestring);
                        }
                    }
                }
                cJSON_Delete(root2);
                break;
            case 3:
                return;

        }
    }
}
void liking(int userid){
    char a[200] = {'\0'};
    printf(BLUE"1-like\n2-comment\n3-Back\n"RESET);
    int choice; scanf("%d",&choice);
    if(choice == 1){
        sprintf(a,"like %s, %d\n",user_token,userid);
        senddata(a);
    }
    else if(choice == 2){
        char input[200] = {'\0'};
        printf(BLUE"Your comment :"RESET);
        scanf("%*c%[^\n]s",input);
        sprintf(a,"comment %s, %d, %s\n",user_token,userid,input);
        senddata(a);
    }
    else if(choice == 3){
        return;
    }
}
void sendtweet(){
    printf(BLUE"whats on your mind ?\n"RESET);
    char str[100000] ={'\0'};
    gets(str);
    char text[100000] ={'\0'};
    sprintf(text,"send tweet %s, %s\n",user_token,str);
    char *reply = senddata(text);
    return;
}
void Logout(){
    char a[100] = "logout ";
    char* reply;
    strcat(a,user_token);
    strcat(a,"\n");
    reply = senddata(a);
}
int start(int port){
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in server;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket failed\n");
        WSACleanup();
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    server.sin_port = htons( port );
    if (connect( ConnectSocket, (SOCKADDR*) &server, sizeof(server) )) {
        printf("connection failed \n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    return ConnectSocket;
}
void end(int port, int ConnectSocket) {
    if (closesocket(ConnectSocket)== SOCKET_ERROR) {
        printf("close failed\n");
        WSACleanup();
    }
    WSACleanup();
    return;
}
char *senddata(char *data){
    int socket = start(port);
    char *buffer = malloc(1000);
    memset(buffer , 0, 1000);
    send(socket , data,strlen(data), 0);
    recv(socket , buffer, 999, 0);
    end(port ,socket);
    return buffer;
}
char *sendBigdata(char *data){
    int socket = start(port);
    char *buffer = malloc(10000);
    memset(buffer , 0, 10000);
    send(socket , data,strlen(data), 0);
    recv(socket , buffer, 9999, 0);
    end(port ,socket);
    return buffer;
}
void buffer(){
    char c;
    while((c = getchar()) != '\n' && c != EOF){
    }
}