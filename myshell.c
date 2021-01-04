#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> //信号库
#include <unistd.h> //linux标准库
#include <pwd.h> //系统密码库
#include <libgen.h> 
#include <sys/types.h>
#include <readline/readline.h> //语句解析readline库
#include <readline/history.h>

char buf[BUFSIZ];
char* myptr;
char* mylim;
char lastdir[100];
char NULCHAR = 0;

extern void yylex();//处理语句解析的函数
void set_info(char* info);
int execute_shell(char* line, char* info);

//以下两个是处理输入历史的函数，就是命令行里按上键的那个功能，使用的是readline中history库的函数，不用深究
void history_setup()
{
	using_history();
	stifle_history(50);
	read_history("/tmp/msh_history");	
}

void history_finish() 
{
	append_history(history_length, "/tmp/msh_history");
	history_truncate_file("/tmp/msh_history", history_max_entries);
} 

void display_history_list()
{
	HIST_ENTRY** h = history_list();
	if(h) {
		int i = 0;
		while(h[i]) {
			printf("%d: %s\n", i, h[i]->line);
			i++;
		}
	}
}

int main(int argc, char** argv)
{
	char* line;
	char* info = (char*)malloc(200 * sizeof(char));
	
	getcwd(lastdir, 99);
	signal(SIGINT, SIG_IGN); 
	//信号量函数，作用是忽略Ctrl+C等等的快捷键，避免和shell里面的快捷键起冲突
	history_setup();	

	while(!execute_shell(line, info)) 

	history_finish();
	return 0;
}


int execute_shell(char* line, char* info) 
{
	//1.打印出Linux账户信息
	set_info(info);
	//2.读取你的输入
	line = readline(info);
	if(!line) return 1;
	else if (*line) add_history(line);

	strcpy(buf, line);strcat(buf, "\n");

	//3.myptr和mylim是parsecmd.l中的外部变量
	myptr = buf;
	mylim = buf+strlen(buf);
	//4.执行解析功能
	yylex();
	return 0;
}

//这个函数用来获取你的linux账户信息并打印出来
void set_info(char* info)
{	
	//身份认证
	char* host = (char*)malloc(125);
	//当前路径，shell一开始的初始路径
	char* current_wd = (char*)malloc(125);
	
	//host名称默认unknown
	if(gethostname(host, 99) == -1) strcpy(host, "unknown");
	else {
		//去掉'.'
		char* p = strchr(host, '.');
		if(p) *p = NULCHAR;
	}
	//如果找不到初始路径
	if(!getcwd(current_wd, 99)) strcpy(current_wd, "unknown"); 
	//basename()返回最后一个‘/’后的内容，比如basename("usr/root")返回"root"
	else if(strcmp(current_wd, "/") != 0) strcpy(current_wd, basename(current_wd));
	
	struct passwd* passwd;
	passwd = getpwuid(getuid());

	sprintf(info, "[%s@%s %s]# ", (passwd == NULL ? passwd->pw_name : "unknown"), host, current_wd);
}