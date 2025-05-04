DESCRIPTION:
1.headers.h :- For all the required headers and defining all the required functions and structs.

2.main.c :- For scanning the input,and prompting the user infinitely.
To exit just press ctrl + C.

3.prompt.c :- For prompting the user and hop

4.reveal.c :- For the reveal specification.
 for -a all files along with hidden files are printed.executable files in green, directories in blue

 for -l file info is printed file/direcctory name is coloured according to blue for directories green for executable file,default for non executable files

 At the starting total no of directories are printed

 5.log.c :- for the log specification,stores all commands(valid/Invalid) to log except log functions.


 6.input_output.c :-for the foreground and background processes.
  if it is invalid command in the background process it prints the pid and prints invalid command exited normally with status 1.
  if it is valid command in the background after execution is done process name with pid exit status is printed
  if it is foreground command and the process takes more than 2 sec.
  suppose input is sleep 3;sleep 5
  the next prompt is printed as:
  <tharanimahathi@TharanimahathiSivarathri:~/test [sleep 3 (3sec), sleep 5 (5sec)] (8s)> 
  i.e, if more than one process takes more than 2 seconds total time taken by all such processses is printed.
  no custom command is run in background.

7.proclore.c :- for proclore specification

8.seek.c :- for seek specifiation

9.test direcory,project directory,.pp.c,sample.c,test.txt,test.c all these files are for checking if the code is working properly.


ASSUMPTIONS:

1.Maximum of 256 processes at a time.
2.maximum length of home,prev_direc,input_command,systemname is 4096
3.Store all commands (valid/invalid) to log

