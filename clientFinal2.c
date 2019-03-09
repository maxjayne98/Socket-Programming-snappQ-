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
char* concat(const char* s1, const char* s2)
{
    char* result = (char*)malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
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
	for(int i = 0; i < second_size; i++){
		if (first[i] != second[i]){
			return 0;
		}
	}
	return 1;
}
char* get_string()
{
    char* buffer = (char*)malloc(30 * sizeof(char));
    char c = '\0';
    read(STDIN_FILENO, &c, 1);
    int i = 0;
    while(c != '\n')
    {
        buffer[i] = c;
        read(STDIN_FILENO, &c, 1);
        i++;
    }

    if (i == 0)
    {
        free(buffer);
        return NULL;
    }

    buffer[i] = '\0';
    return buffer;
}

void chat(int client_or_server,int server_port,int broad_caster,char* myname){
    char server_response[256];
    int client_socket;
    int server_socket;
    char server_message[256] = "\nHello my friedn\n";
    printf("my name is %s\n",myname);
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;  
    struct timeval timeout;
    timeout.tv_usec = timeout.tv_sec = 0;
    if (client_or_server == 1) {
        timeout.tv_usec = timeout.tv_sec = 1;
    }
    
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    int count =0;
    char buf[256];
    int nbytes;
    int i;
    fdmax = 5;

    char* teemp;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    printf("client_or : %d       port : %d\n",client_or_server,server_port);
    clock_t now = clock();
    if (client_or_server == 1) {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        // write(1,"your chat as client socket :", sizeof"your chat as client socket :");
        //  write(1,client_socket,sizeof client_socket);
        // printf("client socket is : %d \n",client_socket);

        while(1){
            if((clock() - now)/CLOCKS_PER_SEC >= 2)
                break;
        }
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(server_port);
        server_address.sin_addr.s_addr = INADDR_ANY;
        bind(client_socket,(struct sockaddr*) &server_address, sizeof(server_address));
        int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
        // printf("connection status : %d \n ",connection_status);
        FD_SET(client_socket,&master);
    }
    if (client_or_server == 2) {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        // write(1,"your chat as server socket :", sizeof"your chat as server socket :");
        // write(1,server_socket,sizeof server_socket);
        // printf("server socket is : %d \n",server_socket);
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(server_port);
        server_address.sin_addr.s_addr = INADDR_ANY;
        bind(server_socket,(struct sockaddr*) &server_address, sizeof(server_address));
         if (listen(server_socket, 5) < 0) {
            perror("listenr failed");
            exit(3);
         }
         FD_SET(server_socket,&master);
     }
        
     FD_SET(STDIN_FILENO, &master);

     for(;;) {
        // printf("\ndodoli\n");
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, &timeout) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                // printf("\nsecy\n");
                   if (client_or_server == 2 && i == server_socket) {
                    // handle new connections
                    // write(1,"\nlolo\n",sizeof"\nlolo\n");

                    addrlen = sizeof remoteaddr;
                    // printf("\n ghable accpet\n");
                    newfd = accept(server_socket,(struct sockaddr *)&remoteaddr,&addrlen);
                    // printf("\n baed az accept\n");
                    count = 1;
                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        // printf("Soghra\n");
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from NAGHI on socket %d\n",newfd);
                        char welcome_message[256] = "welcome to my chat\n";
                        int lenn = strlen("welcome to my chat\n");
                            // printf("\n%d\n",lenn);
                        write(newfd,"welcome to my chat\n",strlen("welcome to my chat\n"));  
                        write(newfd,'\n',sizeof'\n');
                      }
                }
                else if(i == STDIN_FILENO){
                    // printf("siskisisisisis");
                    if ((nbytes = read(i, buf, sizeof buf))){
                        if (is_same(buf,"exitchat",strlen(buf),strlen("exitchat")) == 1) {

                            if (client_or_server == 1) {
                                write(server_socket,"Bye Bye in chat\n",sizeof "Bye Bye\n");
                                // printf("\nsik in client\n");
                            // write(client_socket,buf,sizeof buf);
                                close(client_socket);
                            }
                            else if (client_or_server == 2)            
                            {
                                write(newfd,"Bye Bye in chat\n",sizeof "\nBye Bye\n");
                                close(server_socket);
                            }
                            
                            return;
                        }
                        if (client_or_server == 1) {
                            // printf("\nsik in client\n");
                            // printf("buf  is in client : %s\n",buf);
                            teemp = concat(myname," : ");
                            teemp = concat(teemp, buf);
                            write(client_socket,teemp,strlen(teemp));
                           
                        }
                        else if (client_or_server == 2){
                            // printf("\nsik in server\n ");
                            // printf("buf is : %s\n",buf);
                            teemp = concat(myname," : ");
                            teemp = concat(teemp, buf);
                            write(newfd,teemp,strlen(teemp));
                        }
                         memset(&teemp,0,sizeof teemp);
                        memset(&buf,0,sizeof buf);
                    }
                }
               
                else  {
                    // printf("yki gozid\n");
                    // handle data from a client
                    memset(&buf,0,sizeof buf);
                    if ((nbytes = read(i, buf, sizeof buf)) <= 0) {
                        // got error or connection closed by client

                        if (is_same(buf,"Bye Bye in chat\n",strlen(buf),strlen("Bye Bye in chat\n"))) {
                            // connection closed
                            // printf("selectserver: socket %d hung up\n", i);
                            write(1,"\nchat ended\n",sizeof "\nchat ended\n");
                            // close(server_socket);
                            // return;
                        } else {
                            // printf("salam");
                            // perror("recv");
                        }
                        // memset(&token,0,sizeof(token));
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        return;
                     }
                    //  strcat(buf," :client sent__\n");
                    //   write(0,buf,nbytes);

                    if (nbytes != 0) {
                    // printf("\n1:%s\n",buf);
                    buf[strlen(buf)] = '\0';
                    // printf("\n from other : %s \n",buf);
                    write(1,buf,strlen(buf));
                    // printf("\n2:%s\n",buf);
                    //  parser(buf,"@",words);
                    //  words = str_split(buf,'@');
                    //  printf("clieeent:%s\n",buf);
                    //  free(words);
                    //  printf("parsed:%s\n",words[0]);
                    //  printf("parsed2:%s\n",words[1]);
                    // //  printf("lili:%ld",strlen(words[1]));
                    //  printf("(parsed3:%s)\n",words[2]);
                    //  printf("(parsed4:%s)\n",words[3]);
                    // Handle_command(i,words, users,&number_of_users,is_competition_started);   
                    // words[0] = "";
                    // memset(&words,0,sizeof words);
                    // printf("cleand:%s\n",words[0]);
                    memset(&buf,0,sizeof buf);
                    }
                    //  memset(&token,0,sizeof(token));
                    
                    
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
}
int main(int argc, char const *argv[]){
	//create socket
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	//specify an address for the socket 
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(argv[1]));
	server_address.sin_addr.s_addr = INADDR_ANY;

    bind(network_socket,(struct sockaddr*) &server_address, sizeof(server_address));

	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    printf("connection status in main : %d \n ",connection_status);
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    char* result;
    // char* buf = (char*)malloc(256 * sizeof(char));

    int nbytes;

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct timeval timeout;
    int n;

    //broadcast start
    int udp_socket;
    struct sockaddr_in udp_address,message_address;
    socklen_t messageAddrLen;
    int udpFlag = 1;

    if((udp_socket = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        write(1,"UDP Socket Failed\n",strlen("UDP Socket Failed\n"));
        exit(0);
    }

    setsockopt(udp_socket,SOL_SOCKET,SO_REUSEADDR,&udpFlag,sizeof(udpFlag));
    udp_address.sin_family = AF_INET;
    udp_address.sin_addr.s_addr = INADDR_ANY;
    udp_address.sin_port = htons(UDP_PORT);
    if(bind(udp_socket,(struct sockaddr*)&udp_address,sizeof(udp_address)) < 0){
        write(1,"Could not bind for broadcasting by client\n",strlen("Could not bind for broadcasting by client\n"));
        exit(0);
    }
    messageAddrLen = sizeof(message_address);
    //broadcast end

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    FD_SET(udp_socket,&master);
    FD_SET(network_socket,&master);
    FD_SET(STDIN_FILENO, &master);

    fdmax = udp_socket;

    timeout.tv_usec = timeout.tv_sec = 0;
	//check for error with the connection

	if (connection_status == -1) {
		printf("There was an error making a connection to the remote socket\n\n");
	}

	//recieve data from the server
	char server_response[256];
	read(network_socket,&server_response, sizeof(server_response));
    server_response[sizeof server_response] = '\0';
	write(1,server_response,cal_string_size(server_response));

	memset(&server_response,0,sizeof(server_response));
	char client_message[256];
    int count;
    int flag = 0;
    int exit_flag = 0;
    char** words;
    char** tmp;
    for(;;){
        read_fds = master;
        if (select(fdmax+1,&read_fds,NULL,NULL,&timeout) == -1) {
            perror("select\n");
            exit(4);
        }
        for( i = 0; i < fdmax; i++)
        {
            for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                // if (i == server_socket) {
                //     // handle new connections
                //  lent   addrlen = sizeof remoteaddr;
                //     newfd = accept(network_socket,(struct sockaddr *)&remoteaddr,&addrlen);
                //     count =1;
                //     if (newfd == -1) {
                //         perror("accept");
                //     } else {
                //         FD_SET(newfd, &master); // add to master set
                //         if (newfd > fdmax) {    // keep track of the max
                //             fdmax = newfd;
                //         }
                //         printf("selectserver: new connection from NAGHI on socket %d\n",newfd);
                //         char welcome_message[256] = "welcome to server\n";
                //         int lenn = cal_string_size(welcome_message);
                //             // printf("\n%d\n",lenn);
                //         sendall(newfd,welcome_message,&lenn);     
                //     }
                // }
                if(i == STDIN_FILENO){
                   
                    if ((nbytes = read(STDIN_FILENO, buf, sizeof buf))){
                        if (is_same(buf,"exit",strlen(buf),strlen("exit")) == 1) {
                            exit_flag = 1;
                            close(network_socket);
                            buf[nbytes-1] = '\0';
                            write(1,"\nBye Bye\n",sizeof "\nBye Bye\n");
                            return 0;
                        }
                        memset(&result,0,sizeof(result));
                        // printf("bytes:%d",nbytes);
                        buf[nbytes-1] = '\0';
                        // printf("1:%s",buf);
                        result = concat(buf,"@");
                        result = concat(result,argv[2]);
                        // read(0,client_message,sizeof(client_message));
                        // words = str_split(buf,'@');
                        // write(1,buf,sizeof buf);
                        write(network_socket,result,strlen(result));

                        // printf("-READ FROM STDIN %s\n", buf);
                    }
                }
                else if(i == udp_socket){
                    memset(buf,0,sizeof(buf));
                    nbytes = recvfrom(i,buf,sizeof(buf),0,(struct sockaddr*)&message_address,&messageAddrLen);
                    if(nbytes == 0){
                        write(1,"Broadcast Server went down\n",strlen("Broadcast Server went down\n"));
                        exit(0);
                    }
                    buf[nbytes] = '\n';
                    write(1,buf,strlen(buf));
                }
                else {
                    // handle data from a client
                    memset(&buf,0,sizeof buf);
                    if ((nbytes = read(i, buf, sizeof buf)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0 && i == network_socket) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        // memset(&token,0,sizeof(token));
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                     }
                    //  strcat(buf," :client sent__\n");
                    //   write(0,buf,nbytes);
                    if (nbytes != 0) {
                    buf[nbytes] = '\0';
                    // printf("buuuf is : %s",buf);
                    //  parser(buf,"@",words);
                    // printf("buf 0 is %c",buf[0]);

                        tmp = str_split(buf,'@');
                        // printf("parsed:%s\n",tmp[0]);
                        // printf("parsed2:%s\n",tmp[1]);
                        // // write(1,"\n ",sizeof "\n ");
                        if (!strcmp(tmp[0],"1") || !strcmp(tmp[0],"2")) {
                            // write(network_socket,"whoami",sizeof "whoami");
                            // read(network_socket,tmp[2],sizeof tmp[2]);
                            // printf(" tmp0 : %s \n",tmp[0]);
                            // printf(" tmp1 : %s \n",tmp[1]);
                            // printf(" tmp2 : %s \n",tmp[2]);
                            chat(atoi(tmp[0]),atoi(tmp[1]),udp_socket,tmp[2]);

                            memset(&buf,0,sizeof buf);
                            memset(&words,0,sizeof words);

                        }
                        
                        // memset(&buf,0,sizeof buf);
                    
                    write(1,buf,sizeof buf);
                    //  printf("clieeent:%s\n",buf);
                    //  free(words);
                    //  printf("parsed:%s\n",words[0]);
                    //  printf("parsed2:%s\n",words[1]);
                    //  printf("lili:%ld",strlen(words[1]));
                    //  printf("parsed3:%s\n",words[2]);
                    //  Handle_command(i,words, users,&number_of_users,is_competition_started);   
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
                close(network_socket);
                break;
            }
        }
        
        
    }
    	
	// while(1){
	// 	// write(network_socket,"salam jende ",sizeof"salam jende ");
	// 	// memset(client_message,0,sizeof(client_message));
	// 	read(0,client_message,sizeof(client_message));
	// 	// client_message = get_string();
	// 	// scanf("%s",&client_message);
	// 	// printf("sik");
	// 	client_message[sizeof client_message] = '\0';
	// 	printf("%s\n",client_message);
	// 	write(network_socket,client_message,cal_string_size(client_message));
		

	// 	if (cal_string_size(client_message) && is_same(client_message,"exit",cal_string_size(client_message),
	// 		cal_string_size("exit"))) {
			
	// 		printf("bye bye\n");
	// 		write(1,client_message,sizeof client_message);
	// 		memset(&client_message,0,sizeof(client_message));
	// 		break;
	// 	}
	// 	memset(&client_message,0,sizeof(client_message));
	// 	// if (sizeof(server_response)) {
	// 	read(network_socket,&server_response, sizeof(server_response));
	// 	write(1,server_response,cal_string_size(server_response));
	// 	memset(&server_response,0,sizeof(server_response));
		
		
		
	// }

	

	return 0;
}