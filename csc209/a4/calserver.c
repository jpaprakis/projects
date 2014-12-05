#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lists.h"
#include <unistd.h>
#include <arpa/inet.h>

#define INPUT_BUFFER_SIZE 256
#define INPUT_ARG_MAX_NUM 8
#define DELIM " \n"

#ifndef PORT
  #define PORT 50472
#endif 
 
//Declaring our server's socket fd
static int listenfd;

//Declaring the client struct, taken from muffinman code but changed to 
//also include the username and buffer of the client
struct client {
    int fd;
    struct in_addr ipaddr;
    char *username;
    char cbuf [INPUT_BUFFER_SIZE];
    struct client *next;
} *top = NULL;

void bindandlisten(void);
int find_network_newline(char *buf, int inbuf);
static void addclient(int fd, struct in_addr addr);
void newconnection(void);
static int getClientInput(struct client *p);
void client_interaction(struct client *p);
static void removeclient(int fd);

//declaring the client command variable as global so that it can be accessed by 
//multiple functions
static char client_command [INPUT_BUFFER_SIZE];

//Declaring calendar and user head pointers as global so that they can be
//accessed by the whole program and every client
static Calendar *calendar_list = NULL;
static User *user_list = NULL;


/* 
 * Print a formatted error message to the client. Only for simple strings
 * with no extra variable arguments
 */
void error(char *msg, int fd) {
    char buf[INPUT_BUFFER_SIZE];
    sprintf(buf, "%s\r\n", msg);
    write(fd, buf, strlen(buf));
}

/* 
 * Return a calendar time representation of the time specified
 *  in local_args. 
 *    local_argv[0] must contain hour. 
 *    local_argv[1] may contain day, otherwise use current day
 *    local_argv[2] may contain month, otherwise use current month
 *    local_argv[3] may contain day, otherwise use current year
 */
time_t convert_time(int local_argc, char** local_argv) {

   time_t rawtime;
   struct tm * info;  

   // Instead of expicitly setting the time, use the current time and then
   // change some parts of it.
   time(&rawtime);            // get the time_t represention of the current time 
   info = localtime(&rawtime);   

   // The user must set the hour.
   info->tm_hour = atof(local_argv[0]);
   
   // We don't want the current minute or second. 
   info->tm_min = 0;
   info->tm_sec = 0;

   if (local_argc > 1) {
       info->tm_mday = atof(local_argv[1]);
   }
   if (local_argc > 2) {
       // months are represented counting from 0 but most people count from 1
       info->tm_mon = atof(local_argv[2])-1;
   }
   if (local_argc > 3) {
       // tm_year is the number of years after the epoch but users enter the year AD
       info->tm_year = atof(local_argv[3])-1900;
   }

   // start off by assuming we won't be in daylight savings time
   info->tm_isdst = 0;
   mktime(info);
   // need to correct if we guessed daylight savings time incorrect since
   // mktime changes info as well as returning a value. Check it out in gdb.
   if (info->tm_isdst == 1) {
       // this means we guessed wrong about daylight savings time so correct the hour
       info->tm_hour--;
   }
   return  mktime(info);
}


/* 
 * Read and process calendar commands
 * Return:  -1 for quit command
 *          0 otherwise
 */
int process_args(int cmd_argc, char **cmd_argv, Calendar **calendar_list_ptr, 
  User **user_list_ptr, int fd) {

    Calendar *calendar_list = *calendar_list_ptr; 
    User *user_list = *user_list_ptr;

    //buf will hold the malloced strings, while temp will hold the rest
    char *buf;
    char temp[INPUT_BUFFER_SIZE];

    if (cmd_argc <= 0) {
        return 0;
    } else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
        return -1;
        
    } else if (strcmp(cmd_argv[0], "add_calendar") == 0 && cmd_argc == 2) {
        if (add_calendar(calendar_list_ptr, cmd_argv[1]) == -1) {
            error("Calendar by this name already exists", fd);
        }
        
    } else if (strcmp(cmd_argv[0], "list_calendars") == 0 && cmd_argc == 1) {
        buf = list_calendars(calendar_list);
        write(fd, buf, strlen(buf));
        free(buf);

    } else if (strcmp(cmd_argv[0], "add_event") == 0 && cmd_argc >= 4) {
        // Parameters for convert_time are the values in the array
        // cmd_argv but starting at cmd_argv[3]. The first 3 are not
        // part of the time.
        // So subtract these 3 from the count and pass the pointer 
        // to element 3 (where the first is element 0).
        time_t time = convert_time(cmd_argc-3, &cmd_argv[3]);

        if (add_event(calendar_list, cmd_argv[1], cmd_argv[2], time) == -1) {
           error("Calendar by this name does not exist", fd);
        }

    } else if (strcmp(cmd_argv[0], "list_events") == 0 && cmd_argc == 2) {
        buf = list_events(calendar_list, cmd_argv[1]);
        write(fd, buf, strlen(buf));
        free(buf);

    } else if (strcmp(cmd_argv[0], "add_user") == 0 && cmd_argc == 2) {
        if (add_user(user_list_ptr, cmd_argv[1]) == -1) {
                sprintf(temp, "User %s already exists\r\n", cmd_argv[1]);
                write(fd, temp, strlen(temp));
        }
        else
        {
          sprintf(temp, "User %s added\r\n", cmd_argv[1]);
          write(fd, temp, strlen(temp)); 
        }

    } else if (strcmp(cmd_argv[0], "list_users") == 0 && cmd_argc == 1) {
        buf = list_users(user_list);
        write(fd, buf, strlen(buf));
        free(buf);

    } else if (strcmp(cmd_argv[0], "subscribe") == 0 && cmd_argc == 3) {
        int return_code = subscribe(user_list, calendar_list, cmd_argv[1], cmd_argv[2]);
        if (return_code == -1) {
           error("User by this name does not exist", fd);
        } else if (return_code == -2) {
           error("Calendar by this name does not exist", fd);
        } else if (return_code == -3) {
           error("This user is already subscribed to this calendar", fd);
        } else if (return_code == 0) {

          //This means there was a successful subscribe - will now look through
          //the usernames of all connected clients and let them know that they
          //were subscribed to the calendar if their username matches the username
          //subscribed here
          char buf[INPUT_BUFFER_SIZE];
          sprintf(buf, "You have been subscribed to calendar %s\r\n", cmd_argv[2]);
          struct client *p;
          
          for(p=top; p; p=p->next)
          {
            //If the username matches, write the message in buf to them comparing
            //from the length of the username plus the length of the arg1, since
            //we don't want usernames that contain argv1 to get subscribed, only
            //those which are fully equal to it.
            if (strncmp(p->username, cmd_argv[1], strlen(p->username) 
              + strlen(cmd_argv[1])) == 0)
            {
              write(p->fd, buf, strlen(buf));
            }
          }
        }
      
    } else {
        error("Incorrect syntax", fd);
    }
    return 0;
}


int main(int argc, char* argv[]) {

    //calling bindandlisten so that the socket will be ready to
    //accept connections
    bindandlisten();

    //Can only exit the server by killing it
    while(1) {

      //The rest of main (which is mostly the select functions) was based off of
      //muffinman and adapted to work with calserver

      //p contains a pointer to a client struct
      struct client *p;
      //The list of fd's that have been added at some point
      fd_set fdlist;
      //maxfd contains the largest (most recently added) fd, starts
      //by initializing it to the socket fd
      int maxfd=listenfd;
      //zeroing out the fd set to reset it
      FD_ZERO(&fdlist);
      //Setting the socket fd, since we know it exists still
      FD_SET(listenfd, &fdlist);

      //Now iterating through all the clients that exist in the
      //client struct - if reaching them, sets its fd in the fdlist
      //to indicate that it still exists
      for (p = top; p; p = p->next) 
      {
      FD_SET(p->fd, &fdlist);
      if (p->fd > maxfd)
        maxfd = p->fd;
      }

      //Calling select to set the fdlist for all the fds that are ready
      //to write
      if (select(maxfd + 1, &fdlist, NULL, NULL, NULL) < 0) 
      {
        perror("select");
      }

      else
      {
        //Iterating through the clients again and checking if their
        //fds were set in the fdlist - if so, select has indicated they
        //are ready to write. 
        for (p=top; p; p=p->next)
        {
          if(FD_ISSET(p->fd, &fdlist))
          {
            break;
          }
        }

        //After breaking from the for loop, will call client_interaction
        //on the client (p) that's ready, which will get the client's 
        //input and process it. This is not within the for loop, since
        //calling client_interaction on p might actually remove p from
        //the client list, which would cause problems with the for loop.
        if (p)
        {
          client_interaction(p);
        }

        //Checks if the server socket, listenfd, was set by select - if
        //so, a new client was added, and newconnection() will be called to
        //add the client.
        if (FD_ISSET(listenfd, &fdlist))
        {
          newconnection();
        }
      }
    }
    return 0;
 }

//This function was taken from the sample code and had parts added to it 
 //- bindandlisten establishes the connection on the server side
void bindandlisten(void)
{
    int on = 1;
    struct sockaddr_in r;

    //listenfd contains a socket in AF_INET protocol family, of type SOCK_STREAM,
    //with protocol of 0 since it is not a RAW socket 
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server process terminates
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, 
      sizeof(on)) == -1) 
    {
      perror("setsockopt -- REUSEADDR");
    }

    //padding r with null terminators
    memset(&r, '\0', sizeof r);
    //setting appropriate values of the sockaddr_in struct, r
    r.sin_family = AF_INET;
    //set to INADDR_ANY so that it can connect on any network interface
    r.sin_addr.s_addr = INADDR_ANY;
    //seting r's port
    r.sin_port = htons(PORT);

    //binding to a name
    if (bind(listenfd, (struct sockaddr *)&r, sizeof r)) {
        perror("bind");
        exit(1);
    }

    //calling listen to prepare a queue in the kernal where partially
    //completed connections wait to be accepted - will be able to accept
    //a maximum of 5 partially completed connections that the kernal will
    //queue
    if (listen(listenfd, 5)) {
        perror("listen");
        exit(1);
    }
}

//newconnection function taken from sample code, but changed to incorporate calendar
//functionality instead of muffinman. Will describe in more detail below
void newconnection(void)
{
    int fd; //file descriptor
    struct sockaddr_in r; //same as "peer" in lab8
    socklen_t socklen = sizeof r;

    //Accepting the connection, with listenfd being the static socket declared
    //at the top of the program and initialized in bindandlisten function (the
    //listening socket) While r is the address of the client 
    if ((fd = accept(listenfd, (struct sockaddr *)&r, &socklen)) < 0) 
    {
        perror("accept");
    } 

    else 
    {
        printf("connection from %s\n", inet_ntoa(r.sin_addr));

        //Calling addclient to create the client's struct
        addclient(fd, r.sin_addr);

        //Ask for the client's username - their answer will be handled in a
        //different function once select indicates they they have written something
        write(fd, "What is your user name?\r\n", 25);
    }
}

//Taken from muffinman, but changed slightly to get rid of the howmany line
//(since not necessary to track number of clients) and to include a username
//initialization to NULL, as a way to keep track of the client's state. Also
//memsets the client's buffer to null terminators to begin with. Finally, 
//changed to add new clients to the bottom instead of the top.
static void addclient(int fd, struct in_addr addr)
{
    //creating a new client struct for the client, and mallocing for its space
    struct client *p = malloc(sizeof(struct client));
    if (!p) {
        fprintf(stderr, "out of memory!\n");  /* highly unlikely to happen */
        exit(1);
    }
    printf("Adding client %s\n", inet_ntoa(addr));

    //forcing any unwritten data to be written to stdout 
    fflush(stdout);

    //setting the client's file descriptor
    p->fd = fd;
    //setting client's ip address
    p->ipaddr = addr;
    //setting the client's username to an initial NULL until we get it from them
    p->username = NULL;
    //setting the client's buffer to all null terminators to begin with
    memset(&(p->cbuf), '\0', sizeof(p->cbuf));
    
    //Case where top != NULL, iterating through all clients in the struct and
    //adding the newest client to the end
    if (top)
    {
      struct client *client_ptr; 
      for (client_ptr = top; client_ptr->next !=NULL; client_ptr 
        = client_ptr->next)
        ;
      client_ptr->next = p;
      p->next = NULL;
    }

    //Case where top == NULL, set the new client as top
    else
    {
      top = p;
      top->next = NULL;
    }
}

/*Taken from lab 8 - 
  Search the first inbuf characters of buf for a network newline ("\r\n").
  Return the location of the '\r' if the network newline is found,
  or -1 otherwise.
*/

int find_network_newline(char *buf, int inbuf) {
  int count = 0;
  while((buf[count] != '\r') && ((buf[count+1]) != '\n') && (count < inbuf))
  {
    count ++;
  }
  if (count < inbuf)
  {
    return count; // return the location of '\r' if found 
  }
  else
  {
    return -1; 
  }
}

/*
Gets client input after reading from client and breaks down input by network
newlines /r/n. Stores them in the global variable array client_command. Returns 
1 if something was read from the client, 0 if not.
*/
static int getClientInput(struct client *p)
{
  int inbuf, room, nbytes;
  char* after;
  int where = -1;

  //client_command is a global variable, will get the first full argument up until
  //the network newline put into it if one exists, otherwise will stay nulled out 
  memset(&client_command, '\0', sizeof(client_command));

    //inbuf is the size of the client's current buffer
    inbuf = strlen(p->cbuf);

    //room is the total size of the client's current buffer, less the amount
    //currently inside of it
    room = sizeof(p->cbuf)-inbuf;

    //Pointer after is pointing to the end of the client's buf,
    //so things are getting read into cbuf after all the things already in there
    after = (p->cbuf)+inbuf;
    //printf("in the more_args 1st while loop\n");

    //Reading the client's input into their buffer up to the amount of room left
    if ((nbytes = read(p->fd,after, room)) < 0)
    {
      perror("read");
      exit(1);
    }
    //If nothing was read in, return 0 - Otherwise will continue to execute
    if (nbytes == 0) 
    {
      return 0;
    }
    else
    {
      //printf("just read\n");
      //Add the amount that was just read to inbuf
      inbuf += nbytes; //the amount of bytes read
      //printf("inbuf is %d\n", inbuf);
      where = find_network_newline(p->cbuf, inbuf);
      //printf("where is %d\n", where);

      //Once a network newline is found, sets the username to the full thing in
      //the buffer before the network newline. 
      if (where >= 0)
      {
        //copy the stuff in buf up to "where" bytes (which is everything before 
        //the null terminator) to client_command
        strncpy(client_command, p->cbuf, where);
        //Now add in the null terminator to the end
        *(client_command + where) = '\0';
        printf("client_command is %s\n", client_command);

        inbuf -= strlen(client_command);

        //If the strlen of buf is greater than where+2 (which is the string length 
        //+ the \r\n) will shift over the rest of the buffer to the front
        if (strlen(p->cbuf) > where+2)
        {
          //create a temp variable to hold the rest of the buffer. Then pad the 
          //client buffer with nulls, then bring the rest of the buffer back to 
          //the 1st spot in the client buffer
          char temp[INPUT_BUFFER_SIZE];
          memset(&temp, '\0', sizeof(temp));
          strncpy(temp, ((p->cbuf)+where+2), inbuf);
          memset(&p->cbuf, '\0', sizeof(p->cbuf));
          strncpy(p->cbuf, temp, inbuf);
        }

        //Otherwise there is nothing else in the buffer so will clear it all out
        else
        {
          //Should take everything out of the buffer
          memset(&p->cbuf, '\0', sizeof(p->cbuf));
        }
      }
      return 1;
    }
}

//client_interaction is called when the select() says that there is client activity
void client_interaction(struct client *p)
{

  //First get the client's input and check if it's 0 - if so, the client
  //has disconnected. This is because if there is client activity but no actual
  //write, the only other possible thing the client could have done was disconnect
  if (getClientInput(p) == 0)
  {
    printf("Client ctrl+c'd\n");
    fflush(stdout);
    removeclient(p->fd); 
  }

  //Otherwise the client hasn't disconnected, continue to process their input
  else
  {
    //Checks if the client has given their username yet - if not, will
    //take only the first full thing entered before the network newline
    //and set that as their username
    if(p->username == NULL)
    {

      //Will set the client's username to client_command as long as client_command
      //is not empty (ie. a full line was read by getClientInput)
      if (client_command[0])
      {
        if ((p-> username = malloc(sizeof(client_command))) == NULL)
        {
          perror("malloc");
          exit(1);
        }
        strncpy(p->username, client_command, strlen(client_command)+1);
        write(p->fd, "Go ahead and enter calendar commands>\r\n", 39);
        printf("their username is %s\n", p->username);
      }
    }

    //Otherwise the client has already given their username, and the program
    //will take and process their arguments
    else
    {

      //Will process the client_command by running it through process_args as long
      //as client_command is not null (ie. a full line was written to it by 
      //getClientInput)
      if (client_command[0])
      {
        //tokenizing arguments
        char *cmd_argv[INPUT_BUFFER_SIZE];
        int cmd_argc;
        char *next_token = strtok(client_command, DELIM);
        cmd_argc = 0;
        while (next_token != NULL) {
            if (cmd_argc >= INPUT_ARG_MAX_NUM - 1) {
                error("Too many arguments!", p->fd);
                cmd_argc = 0;
                break;
            }
            cmd_argv[cmd_argc] = next_token;
            cmd_argc++;
            next_token = strtok(NULL, DELIM);
        }

        //Will return -1 on quit - will then send a message and remove the client
        if (cmd_argc > 0 && process_args(cmd_argc, cmd_argv, &calendar_list, 
          &user_list, p->fd) == -1) {
            printf("Quit - Disconnect from %s\n", inet_ntoa(p->ipaddr));
            fflush(stdout);
            removeclient(p->fd);
        }
      }
    }
  }
}

//Taken from muffinman and modified for the purposes of this code
static void removeclient(int fd)
{ 

  //Iterate through and find the client that matches the given fd
  struct client **p;
  struct client *pprev = NULL;
  for (p = &top; *p && (*p)->fd != fd; p = &(*p)->next)
  {
    pprev = *p;
  }
  
  //If the client is found, set t to be the client's->next, and remove the client
  if (*p) 
  {
    struct client *t = (*p)->next;
    printf("Removing client %s\n", inet_ntoa((*p)->ipaddr));
    fflush(stdout);
    memset(&((*p)->cbuf), '\0', sizeof((*p)->cbuf));
    free((*p)->username);
    free(*p);
    
    //Then set the previous pointer's next to t if pprev is not null
    if (pprev)
    {
      pprev->next = t;
    }
    else
    {
      top = t;
    }
  } 
  else 
  {
    fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n", fd);
    fflush(stderr);
  }

}