REPOSITORY WAS KEPT PRIVATE BEFORE DDL

In the README file, please include:   
a. Your Full Name as given in the class list
--------------

b. Your Student ID
-----------

c. What you have done in the assignment   
- Phase 1:
    - Implemented server boot-up procedures for serverM, serverA, serverP, and serverQ.
    - Each backend server loads its corresponding file (members.txt, portfolios.txt, quotes.txt) into in-memory data structures at startup.

- Phase 2:
    - Client prompts the user to enter username and password.
    - Client sends login credentials via TCP to serverM.
    - ServerM encrypts the password and forwards the (username, encrypted password) pair via UDP to serverA.
    - ServerA validates the credentials against its loaded database and sends the result back to serverM, which then responds to the client.

- Phase 3:
    - After successful authentication, the client enters a command loop, allowing users to issue commands (quote, buy, sell, position, exit) until they choose to exit or encounter a failure.
    - ServerM processes client commands, forwards them to the appropriate backend servers (serverQ for quotes, serverP for portfolio operations), and relays responses back to the client over TCP.
  
d. What your code files are and what each one of them does. (Please do not repeat the project description;
just name your code files and briefly mention what they do).   
serverM.cpp : Receives login information from clients, encrypts the password, and forwards authentication/commands to backend servers.

serverA.cpp : Loads credentials from file and verifies login.

serverP.cpp : Loads portfolios from file, handles buy/sell/position updates.

serverQ.cpp : Loads stock quotes from file and provides current prices.

client.cpp : Provides user interface for login and stock trading.

common_variables.h : Defines static port numbers and constant parameters.

common_sockets.h : Provides utility functions to create and bind TCP/UDP sockets.

e. The format of all the messages exchanged, e.g., username and password are concatenated and delimited
by a comma, etc.   

##### login
```
Client
  ↓ TCP: username,password
ServerM
  ↓ UDP: username,encrypted_password
ServerA
  ↓ UDP: GRANTED / DENIED
ServerM
  ↓ TCP: GRANTED / DENIED
Client
```
##### quote
```
Client
  ↓ TCP: quote S1
ServerM
  ↓ UDP: quote S1
ServerQ
  ↓ UDP: S1 102.5
ServerM
  ↓ TCP: S1 102.5
Client
```
```
QUOTE,OK,<DATA>
QUOTE,ERROR,<stock name> 
```


##### BUY
```
Client->>ServerM: buy S1 10
ServerM->>ServerQ: quote S1
ServerQ-->>ServerM: S1 102.5
ServerM-->>Client: CONFIRM buy S1 10 at 102.5
Client-->>ServerM: Y   or   N

alt User confirms (Y)
    ServerM->>ServerP: buy James S1 10 102.5
    ServerP-->>ServerM: OK buy S1 <new_qty> at avg <new_avg>
    ServerM->>ServerQ: advance S1
    ServerM-->>Client: OK buy S1 10 at 102.5
else User denies (N)
    ServerM-->>Client: ERR buy denied by user
    end

```
```
Input not complete:client locally ret
Quote fail->stock not found:BUY,ERR
stock found & confirm: BUY,CONFIRM,stock,price
stock confirmed:BUY,OK,stock,price,share
```

-------------
##### SELL
```
Client->>ServerM: sell S1 10
ServerM->>ServerQ: quote S1
ServerQ-->>ServerM: S1 103.0
ServerM->>ServerP: check James S1 10
ServerP-->>ServerM: OK
ServerM-->>Client: CONFIRM sell S1 10 at 103.0
Client-->>ServerM: Y   or   N

alt User confirms (Y)
    ServerM->>ServerP: sell James S1 10 103.0 Y
    ServerP-->>ServerM: OK sell S1 <remaining> at avg <avg_price>
    ServerM->>ServerQ: advance S1
    ServerQ-->>ServerM: (no reply)
    ServerM-->>Client: OK sell S1 <remaining> at avg <avg_price>
else User denies (N)
    ServerM-->>Client: ERR sell denied by user
end

```

```
stock not exist: SELL,ERROR,stock name does not exist
confirm: SELL,CONFIRM,stock_name,current_price
not enough share: SELL,ERROR,username does not have enough shares of ,stock_name
sell finished: SELL,OK,username,number_of_shares,stock_name
denied: SELL,ERROR
```

###### postion
```
Client   -> ServerM : position
ServerM  -> ServerP : position <username>
ServerP  -> ServerM : OK position\nS1 10 @ avg 100.0\nS2 5 @ avg 50.0
ServerM  -> ServerQ : quote S1
ServerQ  -> ServerM : S1 105.0
ServerM  -> ServerQ : quote S2
ServerQ  -> ServerM : S2 55.0
ServerM  -> Client  : OK S1 10 @ avg 100.0 | P/L = 50.00\nS2 5 @ avg 50.0 | P/L = 25.00\n<username>’s current profit is 75.00
```

```
ServerM  -> Client: POSITION,OK,DATA / POSITION,ERR,DATA
```

f. Any idiosyncrasy of your project. It should specify under what conditions the project fails, if any.
1. servers are not started as assigned order
2. during services, any of the services is down(serverM will wait forever if serverA is down)
3. over ${MAX_PENDING_CLIENTS} attempt to connect serverM during a time period (default set to 20).Defined in common variables.h, mutable


g. Reused Code: Did you use code from anywhere for your project? If not, say so. If so, state what
functions and where they're from. (Also identify this with a comment in the source code).   
1. C++11 does not support optional,so I used ChatGPT prompt:'can you implement a simple plugin class/template for C++11 to support optional'


h. Which version of Ubuntu (only the Ubuntu versions that we provided to you) are you using? 

studentVM


