# KEEP REPOSITORY PRIVATE BEFORE DDL

In the README file, please include:   
a. Your Full Name as given in the class list
Suheng Yang
b. Your Student ID
8232100684
c. What you have done in the assignment   
Phase1 : all boot up and loading files
Phase2:
        A: client prompt and tcp buffering,send
        B: serverM tcp get username and password from client
           serverM udp forward username + encrypted password to server A
Phase3:
        client create a loop to emit commands til exit or failed
        server M check auth and forward to 
                    
d. What your code files are and what each one of them does. (Please do not repeat the project description;
just name your code files and briefly mention what they do).   
common_variables.h: ruled static ports #
comon_sockets.h: create & bind tcp/udp sockets
serverM: demon server, get login data then encrypt them and forward to A
serverA: load credential from file, validate credential
serverP: load portfolio from file
serverQ: load quote from file
client: login

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



Your project grade will depend on the following:
1. Correct functionality, i.e. how well your programs fulfill the requirements of the assignment, specially
   the communications through UDP and TCP sockets.
2. Inline comments in your code. This is important as this will help in understanding what you have done.
3. Whether your programs work as you say they would in the README file.
4. Whether your programs print out the appropriate error messages and results.
5. Your code will only be tested on a fresh copy of the provided Virtual Machine (either studentVM
   (64-bit) or Ubuntu 22.04 ARM64 for M1/M2 Mac users). If your programs are not compiled or
   executed on these VM, you will receive only minimum points as described below. Be careful if you are
   going to use other environments!!! Do not update or upgrade the provided VM as well!!!
6. If your submitted codes do not even compile, you will receive 5 out of 100 for the project.
7. If your submitted codes compile using make but when executed, produce runtime errors without
   performing any tasks of the project, you will receive 10 out of 100.
8. The minimum points for compiled and executable codes is 15 out of 100.
9. If your code does not correctly assign the TCP or UDP port numbers (in any phase), you will lose
   points each.
10. We will use similar test cases to test all the programs. These test cases cover all situations including
    edge cases, referring to the on-screen messages section.
11. There are no points for the effort or the time you spend working on the project or reading the tutorial. If
    you spend about 2 weeks on this project and it doesn’t even compile, you will receive only 5 out of 100.
12. You must discuss all project related issues on the Piazza Discussion Forum. We will give extra points to  
    those who actively help others out by answering questions on Piazza. (If you want to earn the extra
    credits, do remember to leave your names visible to instructors when answering questions on Piazza.)
13. Your code will not be altered in any way for grading purposes and however it will be tested with
    different inputs. Your TA/Grader runs your project as is, according to the project description and your
    README file and then checks whether it works correctly or not. If your README is not consistent
    with the description, we will follow the description.

