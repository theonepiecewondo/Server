# Server
A Server developed in C++ to improve quality of life over the Client. A completely stable and usable server that was designed with Lurk Protocol  
Link to my Client to use with the server: https://github.com/theonepiecewondo/Client  
Link to Lurk Protocol developed by Seth Long: http://isoptera.lcsc.edu/~seth/cs435/lurk_2.1.html  
  
# Compile Instructions
To compile you need to use g++ compiler. Please compile with this line g++ server.cpp server_commands.cpp -pthread  
  
# Starting The Server
Ports Must be Open!!!  
./nameOfServer PortNumber  
Example:  
./a.out 5001  
Important! When connecting you most likely will use localhost for the ip on the client input else you'll need the ip of the server! Plus the port the server is open on  
