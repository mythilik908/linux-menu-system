/* References:
 * https://jbwyatt.com/ncurses.html
 * https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
 * https://man7.org/linux/man-pages/index.html
 * https://invisible-island.net/ncurses/ncurses-intro.html
 * run code with using command-  gcc -o Code2_1002143211 Code2_1002143211.c -lncurses
*/

#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	// initialising curses
	initscr();
	start_color();
	raw();
	cbreak();

	// intialising the colors
	init_pair(1,COLOR_BLUE,COLOR_BLACK);
	init_pair(2,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_RED,COLOR_BLACK);


    char cwd[1024];
    int ch = 0 ;	
    char initialcwd[1024];
    strcpy(initialcwd,getcwd(cwd,sizeof(cwd)));	
    
    while(true) {

		
		  // displaying time
			time_t T = time(NULL);
			struct tm *time ;
			time = localtime(&T);
			char s[100];
		    attron(COLOR_PAIR(2));
			strftime(s,sizeof(s),"It is now: %d %B %Y, %I:%M %p", time);
			printw("%s\n",s);
			
	attron(COLOR_PAIR(1));
	printw("\nSelect your choice from the list of operations below: \n"
			"1 List 10 file names\n"
			"2 List 10 directory names\n"
			"3 Change directory\n"
			"4 Run file\n"
			"5 List next 10 file entries\n"
			"6 List next 10 directory entries\n"
			"7 Restart/Rerun\n"
			"8 Exit\n");
        refresh();
	   
    // creating a new window     
    int x,y;
    getmaxyx(stdscr,y,x);
    WINDOW * inputwin;
    inputwin= newwin(3,23,15,0); 
    box(inputwin,0,0);
    refresh();
    wrefresh(inputwin);
    keypad(inputwin,true);
    attron(COLOR_PAIR(1));
    mvwprintw(inputwin,1,1,"Enter input here :");
    attron(COLOR_PAIR(3));
    ch = wgetch(inputwin) - '0';	
    wrefresh(inputwin);        
    clear();
 
    // creating a new window to execute the functionionalities
     WINDOW * datawin;
     datawin = newwin(y,x,0,0);
     wrefresh(datawin);
     scrollok(datawin,true);
  
  switch(ch) {
	
    case 1 :
	{ 
		int count = 0;
		char *filenames[100];
		struct stat st;
		struct dirent *files;
		DIR *dir = opendir(".");
	if(dir != NULL) {
		        attron(COLOR_PAIR(1));
			printw("List of 10 files \n");
		 	printw("Name        Size       Last modified \n");
			while((files= readdir(dir)) != NULL)
				if(files->d_type == DT_REG) {
					count ++;
				if(stat(files->d_name,&st) != -1 && count <=10)
					{
			attron(COLOR_PAIR(2));
			printw("%s    %ld bytes ", files->d_name,st.st_size);
			printw("   %s", ctime(&st.st_mtime));				
					}	     
				}
			closedir(dir);
		} else {
	      attron(COLOR_PAIR(4));
	      printw("Error!! Could not open directory \n");
		}	
	    break; 
	}
    case 2:
	{ 
		// displaying first 10 directories
		int count = 0;
		struct stat s;
		struct dirent *file;
		DIR *dir = opendir(".");
		if(dir != NULL) {
			attron(COLOR_PAIR(1));
			printw("List of 10 directories \n");
			 printw("Name        Size       Last modified \n");
			while((file= readdir(dir)) != NULL){
				if(file->d_type == DT_DIR) {
				count++; 
				if(stat(file->d_name,&s) != -1 && count <= 10)
				{
				attron(COLOR_PAIR(2));
				printw("%s   %ld bytes ", file->d_name,s.st_size);
				printw("    %s", ctime(&s.st_mtime));					
				}
				}
			}
			closedir(dir);
		} else {
		 attron(COLOR_PAIR(4));
              printw("Error!! Could not open directory\n");
		}
		break;
	}
    case 3:
	{
		//changing directories
		char newdir[256];
		attron(COLOR_PAIR(1));
		printw("Changing directory\n");
		printw("Please enter the directory path\n");
		wrefresh(datawin);
		attron(COLOR_PAIR(3));
		getstr(newdir);
		attron(COLOR_PAIR(2));
		if(chdir(newdir) == 0)
			printw("Directory changed to %s \n",newdir);
		else {
			attron(COLOR_PAIR(4));
			printw("Error!! Please try with a valid directory\n");
		}
			break;
	}


    case 4: { 
    	// running a file
		char path[256];
		int status;
	        attron(COLOR_PAIR(1));
		printw("Running a file..\n");
		printw("Please enter the file path\n");
		wrefresh(datawin);
		attron(COLOR_PAIR(3));
		getstr(path);
	       
		char *ext = strstr(path,".");
		char command[1024];
		int cmp = strcmp(ext,".c");
		if(cmp == 0) {
			// Checking if file is .c type 
			snprintf(command,sizeof(command), "gcc %s -o my_program",path);  
			status =  system(command);     
			if(status == 0) {
		 		// file compiled successfully
				attron(COLOR_PAIR(2));
				int runstatus = system("./my_program");
				if(WIFEXITED(runstatus))
				printw("Success! File ran successfully!\n");
				else{
				wrefresh(datawin);
				attron(COLOR_PAIR(4));
				printw("Failure! Could not run the file!\n");
			        }
		         	}
			else{
		         	attron(COLOR_PAIR(4));
				printw("Failure! Could not compile the file!\n");
	         	}
	        	}
		else {
			status = system(path);
			attron(COLOR_PAIR(2));
			if(status == 0)
				printw("Success! File ran successfully!\n");
			else {
				attron(COLOR_PAIR(4));
				printw("Failure! Could not run the file,check for permissions!\n");
		         	}
		}
		break;
	}

	case 5: {
		// displaying next 10 files
		int count = 0;
		struct stat s;
		struct dirent *file;
		DIR *dir = opendir(".");
		attron(COLOR_PAIR(1));
		printw("List of next 10 files\n");
		if(dir != NULL) {
			printw("Name     Size     Last modified \n");
			while((file= readdir(dir)) != NULL){
			 if(file->d_type == DT_REG) {
				count++;
	        	if(stat(file->d_name,&s) != -1 && (count > 10 && count <= 20))
					{
			attron(COLOR_PAIR(2));			
	               printw("%s %ld bytes ", file->d_name,s.st_size); 						
                   printw("%s", ctime(&s.st_mtime));								}
				}
			}
		}
		else {
		 attron(COLOR_PAIR(4));
               printw("Error!! Could not open files\n");
          	}
		closedir(dir);
		break;
	}

	case 6 : {
		//displaying next 10 directories
		int count = 0;
		struct stat s;
		struct dirent *file;
		attron(COLOR_PAIR(1));
		printw("List of next 10 directories\n");
		 printw("Name     Size     Last modified File Mode \n");
		DIR *dir = opendir(".");
		if(dir != NULL) {
			while((file= readdir(dir)) != NULL){
				if(file->d_type == DT_DIR) {
					count++;
			if(stat(file->d_name,&s) != -1 && (count > 10 && count <= 20)){         
				     attron(COLOR_PAIR(1));
    				printw("%s %ld bytes ", file->d_name,s.st_size);
                                            printw("%s", ctime(&s.st_mtime));  
						}
					}
				}
			} else {
		 attron(COLOR_PAIR(4));
              printw("Error!! Could not open directory\n");
		}
			closedir(dir);
			
			break;
		}
 

	case 7: { 
		    // resetting directory to initial state
			endwin();
			attron(COLOR_PAIR(1));
			printw("Resetting the program..\n");
			char *argv[] ={"./a.out",NULL};
			execve(argv[0],argv,NULL);
			chdir(initialcwd);
			printw("\nDirectory reset done successfully to %s\n"
				,getcwd(cwd,sizeof(cwd)));
				break;
        	}	   

	case 8: {
			printw("Exiting program..\n");
			endwin();
			exit(0);
			break;
		    }
    default: 
	attron(COLOR_PAIR(4));
    	printw("Invalid entry, try with valid entries between 1-8\n");
    	break;
    }
 
  attron(COLOR_PAIR(3));
  printw("\nPress enter to go back to main menu..\n");
  getch();
  clear();
  endwin();
	}
  return 0;
	}   
