First run the server
 

To run the server use terminal in ubuntu   /* The file u run must be in the current directory*/

Run the command: gcc -o server server.c
Then run the command : ./server portno  /*The portno is of 4 random digits u can choose */


Second run the client

To run the client use another terminal in ubuntu /* The file u run must be in the current directory*/

Run the command : gcc -o client1 client.c
Then run the command: ./client IP Address portno /* The portno should be same as the u used in server terminal*/

Then u can choose according to the data which is available in the terminal printed.

For multiple clients open the new terminals and run the above same commands with different file names like client1, client2