NAME=wd_process
NAME_1=tools
NAME_2=scheduler_heap
NAME_3=pq_heap
NAME_4=task
NAME_5=heap
NAME_6=uid
NAME_7=vector
NAME_8=watchdog

CC=gcc
CFLAGS_GC=-ansi -pedantic-errors -Wall -Wextra -DNDEBUG -O3 -pthread
CFLAGS_GD=-ansi -pedantic-errors -Wall -Wextra -g -pthread
SRCS= ./$(NAME).c ./$(NAME_1).c ./$(NAME_2).c ./$(NAME_3).c ./$(NAME_4).c ./$(NAME_5).c ./$(NAME_6).c ./$(NAME_7).c ./$(NAME_8).c
HEADER= ./$(NAME_1).h

.PHONY: all                                                                                         
all:				 
	@make release
	@make debug

.PHONY: open                                                                                         
open:
	@gedit ./source/$(NAME).c ./test/$(NAME)_test.c $(HEADER) &

.PHONY: create
create:
	@make create_source
	@make create_test
	@make create_header

.PHONY: create_source
create_source:
	@touch ./source/$(NAME).c

.PHONY: create_test
create_test:
	@touch ./test/$(NAME)_test.c

.PHONY: create_header
create_header:
	@touch $(HEADER)

.PHONY: objects
objects: $(SRCS)
	@$(CC) -c $(SRCS)
		
.PHONY: object_source
object_source: ./source/$(NAME).c ./source/$(NAME_1).c ./$(NAME_2).c ./$(NAME_3).c ./$(NAME_4).c ./$(NAME_5).c ./$(NAME_6).c ./$(NAME_7).c ./$(NAME_8).c
	@$(CC) -c ./source/$(NAME).c ./source/$(NAME_1).c ./$(NAME_2).c ./$(NAME_3).c ./$(NAME_4).c ./$(NAME_5).c ./$(NAME_6).c ./$(NAME_7).c ./$(NAME_8).c
	
	
.PHONY: clean
clean:
	@rm -f ./*/*.o *.out *.ar *.o

.PHONY: library
library: ./source/$(NAME).c
	@$(CC) -c ./source/$(NAME).c -o ./source/$(NAME).o 
	@ar rcs $(NAME)_lib.ar ./source/$(NAME).o
	@rm -f ./source/$(NAME).o

.PHONY: run
run: 
	make release
	./$(NAME)_release.out
		
.PHONY: release
release: $(SRCS) $(HEADER)
	@$(CC) $(CFLAGS_GC) -o $(NAME)_release.out $(SRCS) $(INCLUDE)

.PHONY: vlg
vlg: 
	make debug
	valgrind --leak-check=yes --track-origins=yes ./$(NAME)_debug.out
	
.PHONY: gdb
gdb: 
	make debug
	cgdb ./$(NAME)_debug.out

.PHONY: debug
debug: $(SRCS) $(HEADER)
	@$(CC) $(CFLAGS_GD) -o wd_process.out $(SRCS) $(INCLUDE)

git:
	git add .
	git commit -m "new commit"
	git push -u origin master


