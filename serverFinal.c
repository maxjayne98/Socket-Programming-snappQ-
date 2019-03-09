#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <string.h>
#include<netinet/in.h>
#include<time.h>

#define UDP_PORT 9999

int cal_string_size(char* st){
	int i = 0;
	while(st[i] != NULL)
		i+=1;
	return i;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;
    result = malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        *(result + idx) = 0;
    }
    return result;
}
int is_same(char* first,char* second,int first_size,int second_size){
    // printf("first : %d_____second : %d",first_size,second_size);
	for(int i = 0; i < second_size; i++){
		if (first[i] != second[i]){
			return 0;
		}
	}
	return 1;
}
void parser(char* str,char* deli, char** words)
{
    char* token;
    token = strtok(str, deli);
    //printf("%s\n",words[0]);
   /* walk through other tokens */
   
   int i = 0;
   
   while( token != NULL ) {
    //   printf("%d %s",i,token);
      words[i]= token;
      token = strtok(NULL, deli);
      i++;
   }
}
int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = write(s, buf+total, bytesleft);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

struct User
{
    char* id ;
    char* password;
    char code[10];
    int gem;
    int online_or_not;
    int current_socket;
    char* port;

};
struct Question
{
    char* question_text;
    char answer[3][20];
    int right_answer;
};

char* concat(const char* s1, const char* s2)
{
    char* result = (char*)malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
int is_duplicate_id(struct User users[],char* id,int number_of_users){
    
    for(int i = 0; i < number_of_users; i++){
        if (is_same((users[i].id),id,sizeof (users[i].id),sizeof( id))){
            write(1,users[i].id,sizeof users[i].id);
            return 0;
          }
        }
    return 1;
}
int is_socket_using(struct User users[],int this_socket,int *number_of_users){
    // printf("\nslaam jendegan\n");
    for(int i = 0; i < *number_of_users; i++){
        if (users[i].current_socket == this_socket ){
            return 0;
          }
        }
    return 1;
}
int sign_up(int user_socket,char* id,char* password, struct User users[],int *number_of_users){
 
    if (!is_duplicate_id(users,concat(id,""),*number_of_users)){
        write(user_socket,"your id is used\ntry agian\n",sizeof "your id is used\ntry agian\n"); 
        return 0;
         }
    users[*number_of_users].id = concat(id,"");
    users[*number_of_users].password = concat(password,"");
    users[*number_of_users].online_or_not = 0;
    //
    // printf("\n size not seted for id in signup : %d",strlen(id)); 
    // printf("\n size seted for id in signup : %d",strlen (users[*number_of_users].id));
    *(number_of_users)+=1;

    return 1;
    // write(1,number_of_users,sizeof *number_of_users);
}
void who_am_i(struct User users[],int this,int* number_of_users){
    int flag = 0;
    // printf("\nsalam2\n");
     for(int i = 0; i < *number_of_users; i++){
         if (users[i].current_socket == this) {
            //  printf("\n sooo %d\n",users[i].current_socket);
            //  write(this,'\n',sizeof '\n');
             write(this,users[i].current_socket,sizeof users[i].current_socket);
            

             write(this,users[i].id,sizeof users[i].id);
             write(this, " : is your id\n",sizeof " : is your id\n");
            
            return;
            
         }
         
     }
    if (flag == 0) {
        write(this,'\n',sizeof '\n');
        
        write(this,"You are not login\n",sizeof "You are not login\n");
        return;
            
    }
    
}
int log_in(int user_want_logged_socket,struct User users[],char* id,char*password,char* port,int *number_of_users){
    int logged = -1;
    
    printf("\n%d\n",strlen( id));
    for(int i = 0; i < *number_of_users; i++){
           
        // printf("\nresult : %d\n",strcmp(id,"mahdi"));

        if (!strcmp(users[i].id,id) && !strcmp(users[i].password,password)) {
            if ( users[i].online_or_not ){
                // printf("\nsiiikiii678");
                write(user_want_logged_socket,"\nThis user is online\n",sizeof"\nThis user is online\n" );
                return 0;
            }
            // write(1,"\nfinded\n","\nfinded\n");
            logged = i;
            // printf("\ngigi:%d\n",logged);
            break;

        }  
    }
    if (logged == -1) {
                //user doesn't exist
                write(user_want_logged_socket,"\nThis user doesn't exist\n",sizeof"\nThis user doesn't exist\n" );
                return 0;
            }
            // printf("\nport is : %s\n",port);
    users[logged].online_or_not = 1;
    users[logged].current_socket = user_want_logged_socket;
    users[logged].port = port;
    // printf("\nclient port is :%s\n",users[logged].port);
    return 1;
}
void log_out(struct User  users[],int want_log_out,int *number_of_users){
    printf("\ndidilo\n");
    for(int i = 0; i < *number_of_users; i++){
        if (users[i].current_socket == want_log_out ) {
            write(users[i].current_socket,"\nYou loged Out\n",sizeof "\nYou loged Out\n");
            users[i].online_or_not = 0;
            users[i].current_socket = 0;
            return;
            
        }
        
    }
    write(want_log_out,"\nYou are not logged in\n",sizeof "\nYou are not logged in\n");
    return 0;
}
void prepare_chat(struct User  users[],int want_chat,char* id,int *number_of_users,int is_compettion_start){
    char* temp;
    char* temp2;
    int want_chat_index = 0;
    // printf("fifi socket is : %d\n",want_chat);
    if (!is_compettion_start) {
        
        for(int j = 0; j < *number_of_users; j++){
            if (users[j].current_socket == want_chat ){
                want_chat_index = j;
            }
        }

        // printf("here1 socket is :%d\n",want_chat_index);
        for(int i = 0; i < *number_of_users; i++){
            // printf("\nin need this pepole : %s :online :%d size: %d",users[i].id,users[i].online_or_not,strlen(users[i].id));
            // printf(" compare : %d\n",strcmp(users[i].id, id));
            // printf("chat want id: %s :size :%d\n",id,strlen(id));

            if (is_same(users[i].id,id,cal_string_size(users[i].id),cal_string_size(id)) == 1) {
                    
                if(users[i].online_or_not){
                    // printf("here2\n");
                    temp = concat("@1@",users[i].port);
                    temp = concat(temp,"@");
                    temp = concat(temp,users[want_chat_index].id);
                    printf("temp want is : %s\n",temp);
                    write(want_chat,temp,strlen(temp));
                    // printf("what send to want : %s\n",concat(temp,users[want_chat_index].id));
                    // printf("here3\n");
                    temp2 = concat("@2@",users[i].port);
                    temp2 = concat(temp2,"@");
                    temp2 = concat(temp2,users[i].id);
                    write(users[i].current_socket,temp2,strlen(temp2));
                    // printf("what send to wanted  : %s\n",concat(temp,users[i].id));
                    // printf("here4\n");
                    return ;
                }
                else
                {
                    write(want_chat,"\nYour friend is not online\n",sizeof "\nYour friend is not online\n");
                    return ;
                }
                
            }
        }
        write(want_chat,"\nYour friend id is wrong\n",sizeof "\nYour friend id is wrong\n");
        return ;
    }
    else{
        write(want_chat,"\nCan't chat now\n",sizeof "\nCan't chat now\n");
    }
    

}
void Handle_command(int user_socket,char** words, struct User  users[], int *number_of_users,int is_competition_start){
        
      if (is_same(words[0],"whoami",cal_string_size(words[0]),cal_string_size("whoami"))) {
            // printf("\nsalam\n");
            // write(user_socket,"000salam0000",sizeof "000salam0000");
            who_am_i(users,user_socket,number_of_users);
         }
     else if (is_same(words[0],"signup",cal_string_size(words[0]),cal_string_size("signup"))){
        //  printf("\nbefore call signup : %d\n",strlen(words[1]));
            if (is_competition_start == 0) {
                if(is_socket_using(users,user_socket,number_of_users)) {
                    if(sign_up(user_socket,words[1],words[2],users,number_of_users)){
                        write(user_socket,"\nYou signedup successfully\n",sizeof "\nYou signedup successfully\n");
                    }
                }
                else
                {   
                     write(user_socket,"\nYour socket is using\n",sizeof "\nYour cocket is using\n");
                }
                
            }
            else
            {
                 write(user_socket,"\nYou can't signup now\n",sizeof "\nYou can't signup now\n");
            }
            
        }
     else if (is_same(words[0],"login",cal_string_size(words[0]),cal_string_size("login"))){
            if (is_competition_start == 0) {
                // printf("\nbefore call login : %d\n",strlen(words[1]));
                 if (is_socket_using(users,user_socket,number_of_users)) {
                     if(log_in(user_socket,users,words[1],words[2],words[3],number_of_users)){
                        write(user_socket,"\nYou loggedin successfully\n",sizeof "\nYou loggedin successfully\n");
                     }
                 }
                 else
                 {
                     write(user_socket,"\nYour socket is using\n",sizeof "\nYour cocket is using\n");
                 }  
            }
             else
            {
                 write(user_socket,"\nYou can't login now\n",sizeof "\nYou can't login now\n");
            }
        }
     else if (is_same(words[0],"logout",cal_string_size(words[0]),cal_string_size("logout"))) {
            log_out(users,user_socket,number_of_users);
        }
     else if (is_same(words[0],"chat",cal_string_size(words[0]),cal_string_size("chat"))) {
            
            prepare_chat(users,user_socket,words[1],number_of_users,is_competition_start);
     }
     else
     {
         write(user_socket,"\nWrong Command\n",sizeof "\nWrong Command\n");
     }
     
     

    memset(&words,0,sizeof words);
}
int cal_time(clock_t start,int endtime){
    // printf("\nwhy\n");
    if((clock() - start)/CLOCKS_PER_SEC >= endtime){
        return 1 ;
    }
    return 0;
    
}
int main(int argc, char const *argv[])
{
    char server_message[256] = "You have reached the server\n";

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr = INADDR_ANY;

    // struct sockaddr_in clientAddress;
    // clientAddress.sin_family = AF_INET;
    // clientAddress.sin_addr.s_addr = inet_addr("255.255.255.255");
    // clientAddress.sin_port = htons(PORT);

    bind(server_socket,(struct sockaddr*) &server_address, sizeof(server_address));

    if (listen(server_socket, 5) < 0) {
        perror("listenr failed");
        exit(3);
    }

    //broadcast start
    int udp_socket;
    struct sockaddr_in message_address;
    int udpFlag = 1;
    if((udp_socket = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        write(1,"Could not open a socket for broadcast\n",strlen("Could not open a socket for broadcast\n"));
        exit(0);
    }
    setsockopt(udp_socket,SOL_SOCKET,SO_BROADCAST,&udpFlag,sizeof(udpFlag));
    message_address.sin_family = AF_INET;
    message_address.sin_addr.s_addr = INADDR_BROADCAST;
    message_address.sin_port = htons(UDP_PORT);
    //broadcast end

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    int broadcast =1;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    FD_SET(server_socket,&master);
    FD_SET(STDIN_FILENO, &master);
    fdmax = server_socket;
    int count =0;
    char* token;
    char** words;
    struct User users[100];
    int *number_of_users = 0;
    int is_competition_started =0;
    clock_t comp_start_time = clock();
    struct timeval timeout;
    int flag = 0;
    int counted = 0;
    timeout.tv_usec = timeout.tv_sec = 0;
    int exit_flag = 0;
    int broad_cast_Question = 0;
     for(;;) {   

        if(cal_time(comp_start_time , 100) == 1){
            is_competition_started = 1;
            if(counted == 0) {
                flag = 1;
            }
        }
        if (flag == 1) {
            write(1,"\nCompetition is started\n",sizeof "\nCompetition is started\n");
            sendto(udp_socket,"\nCompetition is started\n",strlen("\nCompetition is started\n"),
            0,(struct sockaddr*)&message_address,sizeof(message_address));
            flag = 0;
            counted++;
        }

        memset(&token,0,sizeof(token));

        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, &timeout) == -1) {
            perror("select");
            exit(4);
        }
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == server_socket) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(server_socket,(struct sockaddr *)&remoteaddr,&addrlen);
                    count =1;
                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from NAGHI on socket %d\n",newfd);
                        char welcome_message[256] = "welcome to server\n";
                        int lenn = cal_string_size(welcome_message);
                            // printf("\n%d\n",lenn);
                        sendall(newfd,welcome_message,&lenn);     
                    }
                }
                else if(i == STDIN_FILENO){
                    if ((nbytes = read(i, buf, sizeof buf))){
                        if (is_same(buf,"exit",strlen(buf),strlen("exit")) == 1) {
                            if (is_socket_using(users,i,&number_of_users) == 0) {
                                log_out(users,i,&number_of_users);
                                    exit_flag = 1;
                                    write(1,"\nBye Bye\n",sizeof "\nBye Bye\n");
                            }
                            

                        }
                        sendto(udp_socket,buf,strlen(buf),0,(struct sockaddr*)&message_address,sizeof(message_address));
                        memset(&buf,0,sizeof buf);
                        // write(1,"I send folan\n",strlen("I send folan\n"));
                    }
                } else {
                    // handle data from a client
                    memset(&buf,0,sizeof buf);
                    if ((nbytes = read(i, buf, sizeof buf)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("\nselectserver: socket %d hung up\n", i);
                            // log_out(users,i,number_of_users);
                        } else {
                            perror("recv");
                        }
                        memset(&token,0,sizeof(token));
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                     }
                    //  strcat(buf," :client sent__\n");
                    //   write(0,buf,nbytes);

                    if (nbytes != 0) {
                    // printf("\n1:%s\n",buf);
                    buf[strlen(buf)] = '\0';
                    // printf("\n2:%s\n",buf);
                    //  parser(buf,"@",words);
                     words = str_split(buf,'@');
                    //  printf("clieeent:%s\n",buf);
                    //  free(words);
                    //  printf("parsed:%s\n",words[0]);
                    //  printf("parsed2:%s\n",words[1]);
                    //  printf("lili:%ld",strlen(words[1]));
                    write(1,"\nsocket : ",sizeof"\nsocket : ");
                    write(1,i,sizeof(i));
                    write(1," said : ",sizeof " said : ");
                    write(1,buf,sizeof buf);

                    //  printf("(parsed3:%s)\n",words[2]);
                    //  printf("(parsed4:%s)\n",words[3]);
                    Handle_command(i,words, users,&number_of_users,is_competition_started);   
                    // words[0] = "";
                    memset(&words,0,sizeof words);
                    // printf("cleand:%s\n",words[0]);
                    memset(&buf,0,sizeof buf);
                    }
                    //  memset(&token,0,sizeof(token));
                    
                    
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
        if (exit_flag == 1) {
            break;
        }
    } // END for(;;)--and you thought it would never end!
    close(server_socket);
    return 0;
}
    