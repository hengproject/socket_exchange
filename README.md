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
                    
d. What your code files are and what each one of them does. (Please do not repeat the project description;
just name your code files and briefly mention what they do).   
common_variables.h: ruled static ports #
comon_sockets.h: create & bind tcp/udp sockets
serverM: demon server, 
serverA: load credential from file,
serverP: load portfolio from file
serverQ: load quote from file
client: login

e. The format of all the messages exchanged, e.g., username and password are concatenated and delimited
by a comma, etc.   
login message data: `${username},${password}`


f. Any idiosyncrasy of your project. It should specify under what conditions the project fails, if any.
1. servers are not started as assigned order
2. during services, any of the services is down(serverM will wait forever if serverA is down)


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
