## Relay-Based-Peer2Peer-system-Application
Relay based Peer-to-Peer System using Client-Server socket programming.

In this application, we implement three C programs, namely Peer_Client, and Relay_Server
and Peer_Nodes, and they communicate with each other based on TCP sockets. The aim is to implement a Relay based Peer-to-Peer System.

![]({{site.baseurl}}/diagram.png)

**Initially**, the Peer_Nodes will connect to the Relay_Server using the TCP port already known to them. After successful connection, all the Peer_Nodes provide their information (IP address and PORT) to the Relay_Server and close the connections. The Relay_Server actively maintains all the received information with it. Now the Peer_Nodes will act as servers and wait to accept connection from Peer_Clients (refer phase three).

**In second phase**, the Peer_Client will connect to the Relay_Server using the server’s TCP port already
known to it. After successful connection; it will request the Relay_Server for active Peer_Nodes
information. The Relay_Server will response to the Peer_Client with the active
Peer_Nodes information currently having with it. On receiving the response message from the
Relay_Server, the Peer_Client closes the connection gracefully.

**In third phase**, a set of files are distributed evenly among the three Peer_Nodes. The
Peer_Client will take “file_Name” as an input from the user. Then it connects to the Peer_Nodes one at a time using the response information. After successful connection, the Peer_Client tries to fetches the file from the Peer_Node. If the file is present with the Peer_Node, it will provide the file content to the Peer_Client and the Peer_Client will print the file content in its terminal. If not, Peer_Client will connect the next Peer_Node and performs the above action. This will continue till the Peer_Client gets the file content or all the entries in the Relay_Server Response are exhausted.

After each negotiation phase, the TCP connection on both sides are closed gracefully releasing the socket resource.


## How to use this
1. compile the three codes  
	 c++ -o server server.c  
	 c++ -o client client.c  
	 c++ -o node node.c  
     (the nodes can be in other directories as well, it doesn't matter.)
2. run   
    for server:  ./server serverport  
    for peer:  ./node serverIp serverPort peerPort  
    for client:  ./client serverIp serverPort  
    server address is localhost here
3. The requested file should be with a node. The client will ask for a file name and return the file if it is found with any node.

