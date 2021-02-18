#include <stdio.h>
#include <dir.h>
#include <winsock2.h>
#include <time.h>
#include "cJSON.h"
typedef struct {
    char token[50];
    char username[50];
    char password[100];
    int TweetId[100];
}User;
void startup();
char* create_user(char *);
int initialize(int );
void control(int );
char* token_generator();
int StartWith(const char *, const char *);
int exists(const char *);
char* getuntil(const char *,int,char);
char* login_auth(char *, User * user[]);
char* sendtweet(char *,User*);
char* search(char *,User* );
int existInJsonArr(cJSON* , int );
char* follow(char *,User* );
char* unfollow(char *,User*);
char* comment(char* , User* );
void upddateRefreshor(char *, char*);
void truncate(char* );
int tweet_count();
void refresh(User* user);
char* refresh_reframe(User* user);
char* BadFormat();
char* like(char* ,User*);
char* TweetProfile(char* ,User* );
char* SetBio(char *,User* user);
char* Logout(User*);
char* ChangePass(char* ,User*);
int FindUserbyToken(char *token,User* Online[]);
char* TokFail();
char* getTok(char *,char *);
char* TokFail2();
User* online[100];
int Index = 0;
int main( )
{
    int port = 12345;
    int server_socket = initialize(port);
    if(server_socket != INVALID_SOCKET){
        startup();
        control(server_socket);
    }
}
void control(int server_socket){
    User* user = NULL;
    int len = sizeof(struct sockaddr_in);
    int client_socket;
    struct sockaddr_in client;
    char buffer[1000]; int temp_index = 0;
    char *response;
    while((client_socket = accept(server_socket,(struct sockaddr*)&client,&len)) != INVALID_SOCKET){
        printf("Client connected\n");
        memset(buffer,0,1000);
        recv(client_socket,buffer,999,0);

        printf("%s\n",buffer);
        if(StartWith("signup",buffer)) {
            response = create_user(buffer);
        }
        else if(StartWith("login",buffer)){
            response = login_auth(buffer,online);
        }
        else if(StartWith("send tweet",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"send tweet"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = sendtweet(buffer, user);
            }
        }
        else if(StartWith("search",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"search"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = search(buffer, user);
            }
        }
        else if(StartWith("follow",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"follow"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = follow(buffer, user);
            }
        }
        else if(StartWith("unfollow",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"unfollow"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = unfollow(buffer, user);
            }
        }
        else if(StartWith("comment",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"comment"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = comment(buffer, user);
            }
        }
        else if(StartWith("refresh",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"refresh"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                refresh(user);
                response = refresh_reframe(user);
            }
        }
        else if(StartWith("like",buffer)){
                temp_index = FindUserbyToken(getTok(buffer, "like"), online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = like(buffer, user);
            }
        }
        else if(StartWith("profile",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"profile"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = TweetProfile(buffer, user);
            }
        }
        else if(StartWith("set bio",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"set bio"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = SetBio(buffer, user);
            }
        }
        else if(StartWith("logout",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"logout"),online);
            if(temp_index == -1){
                response = TokFail2();
            }else {
                user = online[temp_index];
                response = Logout(user);
            }
        }
        else if(StartWith("change password",buffer)){
            temp_index = FindUserbyToken(getTok(buffer,"change password"),online);
            if(temp_index == -1){
                response = TokFail();
            }else {
                user = online[temp_index];
                response = ChangePass(buffer, user);
            }
        }
        else{
            response = BadFormat();
        }
        send(client_socket,response,strlen(response),0);
        if(!StartWith("signup",buffer) && !StartWith("login",buffer)) {
            printf("\nClient: %s response: %s\n", user->username, response);
        }
        free(response);
        closesocket(client_socket);
    }
}
char* TokFail(){
    char* response = malloc(100);
    memset(response,0,100);
    strcpy(response,"{\"type\":\"Error\",\"message\":\"Token auth failed!\"}");
    return response;
}
char* TokFail2(){
    char* response = malloc(100);
    memset(response,0,100);
    strcpy(response,"{\"type\":\"Error\",\"message\":\"User already logged out or Token isnt valid\"}");
    return response;
}
int FindUserbyToken(char *token,User* Online[]){
    for(int i = 0; i < Index; i++){
        if(strcmp(token , (Online[i])->token) == 0){
            return i;
        }
    }
    return -1;
}
char* getTok(char *buffer, char* startingDefiner){
    int i = strlen(startingDefiner);
    char *tok = calloc(35,1);
    strncpy(tok,&buffer[i+1],30);
    return tok;
}
char* ChangePass(char* buffer,User* user){
    char* response = calloc(1000,1);
    char* token = getuntil(buffer,strlen("change pass")+1,',');
    char *currpass = getuntil(buffer, strlen("change pass")+1+strlen(token)+2,',');
    char *newpass = getuntil(buffer,strlen("change pass")+1+strlen(token)+2+strlen(currpass)+2,'\n');
    printf("current pass : %s\nnew pass : %s\n",currpass,newpass);
    printf("%d",strcmp(currpass,user->password));
    if(strcmp(currpass,user->password)!=0){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Password is wrong!\"}");
        free(token); free(currpass); free(newpass);
        return response;
    }
    if(strlen(newpass) > 16){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Password is longer than 16 byte!\"}");
        free(token); free(currpass); free(newpass);
        return response;
    }
    char dir[500] = {0};    char BUFFER[10000] = {0};
    sprintf(dir,"users\\%s.user.json",user->username);
    FILE *fp = fopen(dir,"r");  fgets(BUFFER,10000,fp); fclose(fp);
    cJSON*root = cJSON_Parse(BUFFER);
    cJSON_DeleteItemFromObject(root,"password");
    cJSON_AddStringToObject(root,"password",newpass);
    char *str = cJSON_PrintUnformatted(root);
    fp = fopen(dir,"w"); fprintf(fp,"%s",str);  fclose(fp);
    cJSON_Delete(root);
    free(str);
    free(token);
    free(currpass);
    memset(user->password,0,100); strcpy(user->password,newpass);
    free(newpass);
    strcpy(response,"{\"type\":\"Successful\",\"message\":\"Password Changed sucessfully\"}");
    return response;
}
char* Logout(User* user){
    char* response = malloc(1000);
    memset(user->token,0,50);
    memset(user->username,0,50);
    memset(user->password,0,100);
    strcpy(response,"{\"type\":\"Successful\",\"message\":\"Logged out successfully\"}");
    return response;
}
char* like(char* buffer ,User* user) {
    char *response = malloc(1000);
    char *token = getuntil(buffer, strlen("like") + 1, ',');
    char *id = getuntil(buffer, strlen("like") + 1 + strlen(token) + 2, '\n');
    int ID = atoi(id);
    char dir[100] = {0};
    sprintf(dir, "tweets\\%d.tweet.json", ID);
    if (exists(dir)) {
        char Buff[1000] = {0};
        FILE *fp = fopen(dir, "r+");
        fgets(Buff, 1000, fp);
        truncate(dir);
        rewind(fp);
        cJSON *root = cJSON_Parse(Buff);
        cJSON *likeCount = cJSON_GetObjectItem(root, "likes");
        int likes = likeCount->valueint;
        cJSON_DeleteItemFromObject(root, "likes");
        cJSON_AddNumberToObject(root, "likes", likes + 1);
        char *str = cJSON_PrintUnformatted(root);
        printf("%s", str);
        fprintf(fp, "%s", str);
        cJSON_Delete(root);
        free(str);
        fclose(fp);
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"Liked succesfully\"}");
    } else {
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Id is invalid\"}");
    }
    free(token);
    free(id);
    return response;
}
char* TweetProfile(char* buffer,User* user){
    char* response = malloc(1000);
    char *token = getuntil(buffer,strlen("profile")+1,'\n');
    char *searched_user = user->username;
    char dir[100] = {0};
    sprintf(dir,"users\\%s.user.json",searched_user);
    if(exists(dir)) {
        if (!strcmp(token, user->token)) {
            char reply[1000];
            char* myusername = user->username;
            FILE* file2pointer; file2pointer = fopen(dir,"r"); fgets(reply,1000,file2pointer);
            fclose(file2pointer);
            cJSON* parse = cJSON_Parse(reply);
            cJSON* username =cJSON_GetObjectItem(parse,"username");
            cJSON* bio =cJSON_GetObjectItem(parse,"bio");
            cJSON* followers =cJSON_GetObjectItem(parse,"followers");
            cJSON* followings =cJSON_GetObjectItem(parse,"followings");
            cJSON* personalTweets =cJSON_GetObjectItem(parse,"personalTweets");
            int num_followers =cJSON_GetArraySize(followers);
            int num_followings =cJSON_GetArraySize(followings);
            cJSON* root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "type","Profile");
            cJSON* message = cJSON_CreateObject();
            cJSON_AddStringToObject(message,"username",username->valuestring);
            cJSON_AddStringToObject(message,"bio",bio->valuestring);
            cJSON_AddNumberToObject(message,"numberOfFollowers",num_followers);
            cJSON_AddNumberToObject(message,"numberOfFollowings",num_followings);
            cJSON*allTweets = cJSON_CreateArray();
            FILE *filepointer;
            cJSON*TweetParser = NULL;
            char buff[1000] = {0};
            for(int i = 0; i < cJSON_GetArraySize(personalTweets); i++){
                memset(dir,0,100);
                sprintf(dir,"tweets\\%d.tweet.json",cJSON_GetArrayItem(personalTweets,i)->valueint);
                filepointer = fopen(dir,"r");
                printf("opening %s\n",dir);
                fgets(buff,999,filepointer);
                fclose(filepointer);
                TweetParser = cJSON_Parse(buff);
                cJSON_AddItemToArray(allTweets,TweetParser);
            }

            cJSON_AddItemToObject(message,"allTweets",allTweets);
            cJSON_AddItemToObject(root,"message",message);
            char* str = cJSON_PrintUnformatted(root);
            printf("%s",str);
            strcpy(response,str);
            cJSON_Delete(parse);
            cJSON_Delete(root);
            free(str);
        } else {
            strcpy(response, "{\"type\":\"Error\",\"message\":\"Token failed\"}");
            printf("%s\n", response);
        }
    }else{
        strcpy(response,"{\"type\":\"Error\",\"message\":\"The User doesnt exist!\"}");
    }
    free(token);
    return response;
}
char* SetBio(char* buffer, User* user){
    char* response = calloc(1000,sizeof(char));
    char* token = getuntil(buffer,strlen("set bio")+1,',');
    char* bio = getuntil(buffer,strlen("set bio ")+strlen(token)+2,'\n');
    if(strcmp(user->token,token)){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Token Authentication failed\"}");
        free(token);
        free(bio);
        return response;
    }
    char dir[100] = {0};
    char Buff[1000] = {0};
    sprintf(dir,"users\\%s.user.json",user->username);
    FILE* filepointer = fopen(dir,"r+");
    fgets(Buff,1000,filepointer);
    truncate(dir);
    rewind(filepointer);
    cJSON* root = cJSON_Parse(Buff);
    cJSON_DeleteItemFromObject(root,"bio");
    cJSON_AddStringToObject(root,"bio",bio);
    char* str = cJSON_PrintUnformatted(root);
    fprintf(filepointer,"%s",str);
    fclose(filepointer);
    cJSON_Delete(root);
    free(str);
    free(token);
    free(bio);
    strcpy(response,"{\"type\":\"Successful\",\"message\":\"Bio updated successfully!\"}");
    return response;
}
char* refresh_reframe(User* user){
    char *response = calloc(10000,1);
    cJSON*boot = cJSON_CreateObject();
    cJSON_AddStringToObject(boot,"type","List");
    cJSON*message = cJSON_CreateArray();
    char dir[100] = {0}; sprintf(dir,"history\\%s.user.json",user->username);
    char buffer[10000] = {0};
    char BigBuff[10000] = {0};
    FILE* filepointer = fopen(dir,"r");fgets(buffer,10000,filepointer); fclose(filepointer);
    cJSON* root = cJSON_Parse(buffer);
    cJSON* Tweets = cJSON_GetObjectItem(root,"rData");
    int minIndex = cJSON_GetObjectItem(root,"MinIndex")->valueint;
    int maxIndex = cJSON_GetObjectItem(root,"MaxIndex")->valueint;
    int tweet_index;
    cJSON*BigBuffTweet = NULL;
    for(int i = minIndex;i < maxIndex; i++){
        tweet_index = cJSON_GetArrayItem(Tweets,i)->valueint;
        memset(dir,0,100); sprintf(dir,"tweets\\%d.tweet.json",tweet_index);
        filepointer = fopen(dir,"r");
        memset(BigBuff,0,10000);
        fgets(BigBuff,10000,filepointer);
        BigBuffTweet = cJSON_Parse(BigBuff);
        fclose(filepointer);
        cJSON_AddItemToArray(message,BigBuffTweet);
    }
    cJSON_AddItemToObject(boot,"message",message);
    char *string = cJSON_PrintUnformatted(boot);
    strcpy(response,string);
    cJSON_Delete(root);
    cJSON_Delete(boot);
    free(string);
    return response;
}
void refresh(User* user){
    char jimi[100] = {0}; sprintf(jimi,"users\\%s.user.json",user->username);
    FILE* myhisto = fopen(jimi,"r");
    char data[1000] = {0};
    fgets(data,1000,myhisto); fclose(myhisto);
    char *fan;
    char* username = user->username;
    char dir[100] = {0}; sprintf(dir,"history\\%s.user.json",username);
    if (!exists(dir)){
        cJSON*rase = cJSON_CreateObject();
        cJSON_AddArrayToObject(rase,"rData");
        cJSON_AddNumberToObject(rase,"MaxIndex",0);
        cJSON_AddNumberToObject(rase,"MinIndex",0);
        char*str = cJSON_PrintUnformatted(rase);
        FILE* createfile = fopen(dir,"w+");
        fprintf(createfile,"%s",str);
        fclose(createfile);
        free(str);
        cJSON_Delete(rase);
    }
    char str[1000] = {0};
    FILE *fpeon = fopen(dir,"r"); fgets(str,1000,fpeon); fclose(fpeon);
    cJSON*boot =cJSON_Parse(str);
    cJSON_DeleteItemFromObject(boot,"MinIndex");
    cJSON_AddNumberToObject(boot,"MinIndex",cJSON_GetObjectItem(boot,"MaxIndex")->valueint);
    char*fw = cJSON_PrintUnformatted(boot);
    fpeon = fopen(dir,"w"); fprintf(fpeon,"%s",fw); fclose(fpeon);
    cJSON_Delete(boot);
    free(fw);
    cJSON* root = cJSON_Parse(data);
    cJSON* following = cJSON_GetObjectItem(root,"followings");
    for(int i = 0; i < cJSON_GetArraySize(following); i++){
        fan = cJSON_GetArrayItem(following,i)->valuestring;
        upddateRefreshor(fan,username);
    }
}
void upddateRefreshor(char *fan, char*username){
    char dir[100] = {0}; sprintf(dir,"history\\%s.user.json",username);
    char buff[1000] = {0};
    FILE *fp;
    fp = fopen(dir,"r");
    fscanf(fp,"%s",buff);
    fclose(fp);
    cJSON*base = cJSON_Parse(buff);
    cJSON*rData = cJSON_GetObjectItem(base,"rData");
    memset(dir,0,100); sprintf(dir,"users\\%s.user.json",fan);
    fp = fopen(dir,"r"); fgets(buff,1000,fp); fclose(fp);
    cJSON*Root = cJSON_Parse(buff);
    int index = 0;
    cJSON*personalTweets = cJSON_GetObjectItem(Root,"personalTweets");
    for(int i = 0; i < cJSON_GetArraySize(personalTweets); i++) {
        if(!existInJsonArr(rData,cJSON_GetArrayItem(personalTweets,i)->valueint)) {
            cJSON_AddNumberToObject(rData, "dummy", cJSON_GetArrayItem(personalTweets, i)->valueint);
            index++;
        }
    }
    int temp =cJSON_GetObjectItem(base,"MaxIndex")->valueint;
    cJSON_DeleteItemFromObject(base,"MaxIndex");
    cJSON_AddNumberToObject(base,"MaxIndex",cJSON_GetArraySize(rData));
    memset(dir,0,100); sprintf(dir,"history\\%s.user.json",username);
    fp = fopen(dir,"w");
    fprintf(fp,"%s",cJSON_PrintUnformatted(base));
    fclose(fp);
    cJSON_Delete(base);
    cJSON_Delete(Root);
}
char* comment(char* data, User* user){
    char* response = malloc(1000);
    char* my_user_name = user->username;
    char tweet[1000] = {0};
    FILE* filepointer = NULL;
    char* token = getuntil(data,strlen("comment "),',');
    char* id = getuntil(data,strlen("comment ")+strlen(token)+2,',');
    int ID = atoi(id);
    char dir[100]; sprintf(dir,"tweets\\%d.tweet.json",ID);
    filepointer = fopen(dir,"r");
    fgets(tweet,1000,filepointer);
    fclose(filepointer);
    char* comment = getuntil(data,strlen("comment ")+strlen(token)+strlen(id)+4,'\n');
    cJSON *root = cJSON_Parse(tweet);
    cJSON* comments = cJSON_GetObjectItem(root,"comments");
    cJSON_AddStringToObject(comments,my_user_name,comment);
    char *str = cJSON_PrintUnformatted(root);
    filepointer = fopen(dir,"w");
    fprintf(filepointer,"%s",str);
    fclose(filepointer);
    cJSON_Delete(root);
    free(str);
    free(id);
    free(comment);
    free(token);
    strcpy(response,"{\"type\":\"Successful\",\"message\":\"Comment successful\"}");
    return response;
}
char* follow(char *buffer,User* user){
    char dir[100] = {0};
    char *response = calloc(1000, sizeof(char));
    char *token = getuntil(buffer,strlen("follow")+1,',');
    char *searched_user;
    char str[1000];
    char*temp_test;
    if(!strcmp(token,user->token)){
        //---------------------------------------------------------------------//
        searched_user = getuntil(buffer,strlen("follow")+strlen(token)+3,'\n');
        sprintf(dir,"users\\%s.user.json",user->username);
        FILE* filepointer = fopen(dir,"r");
        fgets(str,999,filepointer); fclose(filepointer);
        cJSON* root = cJSON_Parse(str);
        cJSON* following = cJSON_GetObjectItem(root,"followings");
        int trigger = 1;
        for(int i = 0; i < cJSON_GetArraySize(following); i++){
            if(!strcmp(cJSON_GetArrayItem(following,i)->valuestring,searched_user)){
                strcpy(response,"{\"type\":\"Error\",\"message\":\"You have already followed this user!\"}");
                cJSON_Delete(root);
                return response;
            }
        }
        cJSON_AddStringToObject(following,"dummy",searched_user);
        temp_test = cJSON_PrintUnformatted(root);
        filepointer = fopen(dir,"w");
        fprintf(filepointer,"%s",temp_test); fclose(filepointer);
        printf("%s\n",temp_test);
        cJSON_Delete(root);
        free(temp_test);
        //------------------------------------------------------------------------//
        memset(dir,0,100);
        memset(str,0,1000);
        sprintf(dir,"users\\%s.user.json",searched_user);
        filepointer = fopen(dir,"r");
        fgets(str,999,filepointer); fclose(filepointer);
        root = cJSON_Parse(str);
        following = cJSON_GetObjectItem(root,"followers");
        cJSON_AddStringToObject(following,"dummy",user->username);
        temp_test = cJSON_PrintUnformatted(root);
        filepointer = fopen(dir,"w");
        fprintf(filepointer,"%s",temp_test); fclose(filepointer);
        printf("%s\n",temp_test);
        cJSON_Delete(root);
        free(temp_test);
        //------------------------------------------------------------------------//
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"followed successfully\"}");
    }
    else{
        strcpy(response, "{\"type\":\"Error\",\"message\":\"Token failed\"}");
    }
    free(searched_user);
    free(token);
    return response;
}
char* search(char *buffer, User* user){
    char* response = malloc(1000);
    char *token = getuntil(buffer,strlen("search")+1,',');
    char *searched_user = getuntil(buffer,strlen("search")+1+strlen(token)+1,'\n');
    if(!strcmp(searched_user,user->username)){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Searched User is your username\"}");
        free(token); free(searched_user);
        return response;
    }
    char dir[100] = {0};
    sprintf(dir,"users\\%s.user.json",searched_user);
    if(exists(dir)) {
        if (!strcmp(token, user->token)) {
            char reply[1000];
            char* myusername = user->username;
            FILE* file2pointer; file2pointer = fopen(dir,"r"); fgets(reply,1000,file2pointer);
            fclose(file2pointer);
            cJSON* parse = cJSON_Parse(reply);
            cJSON* username =cJSON_GetObjectItem(parse,"username");
            cJSON* bio =cJSON_GetObjectItem(parse,"bio");
            cJSON* followers =cJSON_GetObjectItem(parse,"followers");
            cJSON* followings =cJSON_GetObjectItem(parse,"followings");
            cJSON* personalTweets =cJSON_GetObjectItem(parse,"personalTweets");
            int num_followers =cJSON_GetArraySize(followers);
            int num_followings =cJSON_GetArraySize(followings);
            char follow_status[15] ="NotFollowed";
            for(int i = 0; i < num_followers; i++){
                if (!strcmp(cJSON_GetArrayItem(followers,i)->valuestring,myusername)){
                    memset(follow_status,0,15);
                    strcpy(follow_status,"Followed");
                }
            }

            cJSON* root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "type","Profile");
            cJSON* message = cJSON_CreateObject();
            cJSON_AddStringToObject(message,"username",username->valuestring);
            cJSON_AddStringToObject(message,"bio",bio->valuestring);
            cJSON_AddNumberToObject(message,"numberOfFollowers",num_followers);
            cJSON_AddNumberToObject(message,"numberOfFollowings",num_followings);
            cJSON_AddStringToObject(message,"followStatus",follow_status);
            cJSON*allTweets = cJSON_CreateArray();
            FILE *filepointer;
            cJSON*TweetParser = NULL;
            char buff[1000] = {0};
            for(int i = 0; i < cJSON_GetArraySize(personalTweets); i++){
                memset(dir,0,100);
                sprintf(dir,"tweets\\%d.tweet.json",cJSON_GetArrayItem(personalTweets,i)->valueint);
                filepointer = fopen(dir,"r");
                printf("opening %s\n",dir);
                fgets(buff,999,filepointer);
                fclose(filepointer);
                TweetParser = cJSON_Parse(buff);
                cJSON_AddItemToArray(allTweets,TweetParser);
            }

            cJSON_AddItemToObject(message,"allTweets",allTweets);
            cJSON_AddItemToObject(root,"message",message);
            char* str = cJSON_PrintUnformatted(root);
            printf("%s",str);
            strcpy(response,str);
            cJSON_Delete(parse);
            cJSON_Delete(root);
            free(str);
        } else {
            strcpy(response, "{\"type\":\"Error\",\"message\":\"Token failed\"}");
            printf("%s\n", response);
        }
    }else{
        strcpy(response,"{\"type\":\"Error\",\"message\":\"The User doesnt exist!\"}");
    }
    free(token);
    free(searched_user);
    return response;


}
char* unfollow(char *buffer,User* user){
    char dir[100] = {0};
    char *response = calloc(1000, sizeof(char));
    char *token = getuntil(buffer,strlen("unfollow")+1,',');
    char *searched_user;
    char str[1000];
    char*temp_test;
    if(!strcmp(token,user->token)){
        //---------------------------------------------------------------------//
        searched_user = getuntil(buffer,strlen("unfollow")+strlen(token)+3,'\n');
        sprintf(dir,"users\\%s.user.json",user->username);
        FILE* filepointer = fopen(dir,"r");
        fgets(str,999,filepointer); fclose(filepointer);
        cJSON* root = cJSON_Parse(str);
        cJSON* following = cJSON_GetObjectItem(root,"followings");
        int trigger = -1;
        for(int i = 0; i < cJSON_GetArraySize(following); i++){
            if(!strcmp(cJSON_GetArrayItem(following,i)->valuestring,searched_user)){
                trigger = i;
            }
        }
        if(trigger == -1){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"You havent followed this user!\"}");
            cJSON_Delete(root);
            return response;
        }
        cJSON_DeleteItemFromArray(following,trigger);
        temp_test = cJSON_PrintUnformatted(root);
        filepointer = fopen(dir,"w");
        fprintf(filepointer,"%s",temp_test); fclose(filepointer);
        printf("%s\n",temp_test);
        cJSON_Delete(root);
        free(temp_test);
        //------------------------------------------------------------------------//
        cJSON* followers = NULL;
        memset(dir,0,100);
        memset(str,0,1000);
        sprintf(dir,"users\\%s.user.json",searched_user);
        filepointer = fopen(dir,"r");
        fgets(str,999,filepointer); fclose(filepointer);
        root = cJSON_Parse(str);
        followers = cJSON_GetObjectItem(root,"followers");
        trigger = -1;
        for(int i = 0; i < cJSON_GetArraySize(followers); i++){
            if(!strcmp(cJSON_GetArrayItem(followers,i)->valuestring,user->username)){
                trigger = i;
            }
        }
        if(trigger == -1){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"Unknown Error in database\"}");
            cJSON_Delete(root);
            return response;
        }
        cJSON_DeleteItemFromArray(followers,trigger);
        temp_test = cJSON_PrintUnformatted(root);
        filepointer = fopen(dir,"w");
        fprintf(filepointer,"%s",temp_test); fclose(filepointer);
        printf("%s\n",temp_test);
        cJSON_Delete(root);
        free(temp_test);
        //------------------------------------------------------------------------//
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"unfollowed successfully\"}");
    }
    else{
        strcpy(response, "{\"type\":\"Error\",\"message\":\"Token failed\"}");
    }
    free(searched_user);
    free(token);
    return response;
}
char* sendtweet(char *buffer,User* user){
    char dir[100] = {0};
    char token[100] = {0};
    char content[1000] = {0};
    char *response = calloc(500, sizeof(char));
    strcpy(token,getuntil(buffer,strlen("send tweet")+1,','));
    int content_index = strlen("send tweet ")+strlen(user->token)+2;
    strcpy(content,getuntil(buffer,content_index,'\n'));
    if(!strcmp(token,user->token)) {
        int startingIndex = tweet_count();
        sprintf(dir, "tweets\\%d.tweet.json", startingIndex);
        FILE *filepointer = fopen(dir, "w");
        cJSON *tweet = cJSON_CreateObject();
        cJSON_AddNumberToObject(tweet,"id",startingIndex);
        cJSON_AddStringToObject(tweet,"author",user->username);
        cJSON_AddStringToObject(tweet,"content",content);
        cJSON_AddObjectToObject(tweet,"comments");
        cJSON_AddNumberToObject(tweet,"likes",0);
        char* string = cJSON_PrintUnformatted(tweet);
        printf("%s\n",string);
        fprintf(filepointer,"%s",string);
        fclose(filepointer);
        cJSON_Delete(tweet);
        free(string);
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"Tweet sent successfully\"}");
        sprintf(dir, "users\\%s.user.json",user->username);
        filepointer = fopen(dir,"r+");
        memset(content,0,1000);
        fscanf(filepointer,"%s",content);
        fclose(filepointer);
        cJSON*root = cJSON_Parse(content);
        cJSON*username = cJSON_GetObjectItem(root,"username");
        cJSON*password = cJSON_GetObjectItem(root,"password");
        cJSON*bio = cJSON_GetObjectItem(root,"bio");
        cJSON*followers = cJSON_GetObjectItem(root,"followers");
        cJSON*followings = cJSON_GetObjectItem(root,"followings");
        cJSON*personaltweets = cJSON_GetObjectItem(root,"personalTweets");
        cJSON_AddNumberToObject(personaltweets,"dummy",startingIndex);
        char*str = cJSON_PrintUnformatted(root);
        filepointer = fopen(dir,"w");
        fprintf(filepointer,"%s",str);
        fclose(filepointer);
        cJSON_Delete(root);
        free(str);
    }
    else{
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Token failed\"}");
        printf("%s\n",response);
    }
    return response;
}
int tweet_count(){
    char dir[100] = {0};
    int count = 1;
    strcpy(dir,"Tweets\\1.tweet.json");
    while (exists(dir)){
        count++;
        memset(dir,0,100);
        sprintf(dir,"Tweets\\%d.tweet.json",count);
    }
    return (count);
}
char* login_auth(char *buffer ,User * user[]){
    char *response = malloc(1000);
    memset(response,0,1000);
    char *username = getuntil(buffer,6,',');
    for(int i = 0; i < Index; i++){
        if(!strcmp(username,(user[i])->username)){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"This User is already logged in!\"}");
            free(username);
            return response;
        }
    }
    char *password = getuntil(buffer,7+(int)strlen(username),'\n');
    char dir[100] = {0};
    sprintf(dir,"users\\%s.user.json",username);
    if(exists(dir)){
        FILE *filepointer;
        filepointer = fopen(dir,"r");
        char str[1000] = {0};
        fgets(str,1000,filepointer);
        fclose(filepointer);
        printf("%s",str);
        cJSON *root = cJSON_Parse(str);
        cJSON *passs = cJSON_GetObjectItem(root,"password");
        cJSON* Itweets = cJSON_GetObjectItem(root,"personalTweets");
        if(!strcmp(password,passs->valuestring)){
            user[Index] = malloc(sizeof(User));
            strcpy(user[Index]->token,token_generator());
            strcpy(user[Index]->username,username);
            strcpy(user[Index]->password,password);
            int id[100] = {0};
            for(int i = 0; i < cJSON_GetArraySize(Itweets); i++){
                id[i] = cJSON_GetArrayItem(Itweets,i)->valueint;
                (user[Index]->TweetId)[i] = id[i];
            }
            sprintf(response,"{\"type\":\"Token\",\"message\":\"%s\"}",user[Index]->token);
            Index++;
            cJSON_Delete(root);
        }else{
            sprintf(response,"{\"type\":\"Error\",\"message\":\"incorrect password\"}");
        }
    }
    else{
        strcpy(response,"{\"type\":\"Error\",\"message\":\"This username doesnt exist.\"}");
    }
    free(username);
    free(password);
    return response;
}
char* create_user(char *buffer){
    char *username = getuntil(buffer,7,',');
    char *password = getuntil(buffer,(8+(int)strlen(username)),'\n');
    char *response = malloc(1000);
    memset(response,0,1000);
    char dir[100] = {0};
    sprintf(dir,"users\\%s.user.json",username);
    if(exists(dir)){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"This username is already taken.\"}");
    }
    else{
        FILE *filepointer;
        filepointer = fopen(dir,"w");
        cJSON* user = cJSON_CreateObject();
        cJSON_AddStringToObject(user,"username",username);
        cJSON_AddStringToObject(user,"password",password);
        cJSON_AddStringToObject(user,"bio","");
        cJSON* followers = cJSON_AddArrayToObject(user,"followers");
        cJSON* following = cJSON_AddArrayToObject(user,"followings");
        cJSON* personaltweet = cJSON_AddArrayToObject(user,"personalTweets");
        char *string = cJSON_PrintUnformatted(user);
        printf("%s\n",string);
        fprintf(filepointer,"%s",string);
        fclose(filepointer);
        cJSON_Delete(user);
        free(string);
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"UserCreated\"}");
    }
    free(username);
    free(password);
    return response;
}
char* getuntil(const char *string,int start,char tokenizer){
    char *username = malloc(100);
    memset(username,0,100);
    int t = 0;
    for(int i = start; string[i] !=tokenizer;i++){
        username[t] = string[i];
        t++;
    }
    return username;
}
char* token_generator(){
    srand(time(NULL));
    char*token = calloc(31, sizeof(char ));
    char temp;
    for(int i = 0; i < 30 ;i++){
        temp = (rand() % 75) + 48;
        if(temp >= 91 && temp <= 96){
            temp = ( rand()%10 + 48);
        }
        else if(temp >= 58 && temp <= 64){
            temp = ( rand()%10 + 48);
        }
        token[i] = temp;
    }
    return token;
}
int initialize(int port){
    WSADATA wsadata;
    int wsa_res = WSAStartup(MAKEWORD(2,2),&wsadata);
    if(wsa_res != 0){
        puts("wsa failed!");
        return 0;
    }
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == INVALID_SOCKET){
        puts("Socket creation failed!");
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    int bind_res = bind(server_socket, (struct sockaddr*)&server, sizeof(server));
    if (bind_res == SOCKET_ERROR){
        puts("bind failed");
        return 0;
    }
    listen(server_socket,SOMAXCONN);
    return server_socket;
}

void startup(){
    mkdir("users");
    mkdir("tweets");
    mkdir("history");
}
int StartWith(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}
int exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    fclose(file);
    return 0;
}
void truncate(char* dir){
    FILE* filepointer = fopen(dir,"w");
    fclose(filepointer);
}
int existInJsonArr(cJSON* array, int x){
    for(int i = 0; i < cJSON_GetArraySize(array); i++){
        if(cJSON_GetArrayItem(array,i)->valueint == x){
            return 1;
        }
    }
    return 0;
}
char* BadFormat(){
    char *response = malloc(100);
    strcpy(response,"{\"type\":\"Error\",\"message\":\"Bad Format!\"}");
    return response;
}
