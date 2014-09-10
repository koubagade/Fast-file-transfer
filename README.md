Fast-file-transfer
==================

New application level protocol for file transfer over lossy link

Compile client.c and server.c
Run server with "time ./server <port>"
Run client.c with "./client <host IP> <port> <path-to-file>" 

Command to create new file system on Deter: sudo /usr/testbed/bin/mkextrafs /mnt
Command to generate binary file: sudo dd if=/dev/urandom of=data.bin bs=1M count= 200


git commands:

1. Pull latest code : git pull

2. Once you changed some code and tested it works

--> git status
---check the file you modified is highlighed in modified list.

--> git add filename (or names)
----filename is the file (or files) which you want to commit (save chanfges to repository)

--> git commit -m "message"
----message is one line description of changes

--> git push origin master
----sends code to internet repositry where it will be made available to other users

3. compare your changes with online repo
----git diff

4. discard changes made to file and get the previously commited code 
----git checkout filename

	

