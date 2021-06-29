#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include "public.h"

typedef void(*sighandler_t)(int);


sqlite3 *db=NULL;
int flag=0;
void handler(int sig)
{
	printf("回收子进程成功\n");
	while(waitpid(-1, NULL, WNOHANG) > 0);
}




//管理员考勤回调函数
int admin_attend_query_callback(void *arg, int ncolumn, char **f_value, char **f_name)
{
	int i = 0;
	MSG *msg= (MSG *)arg;
	int newsfd = msg->flag;

	if(flag == 0){
		for(i = 0; i < ncolumn; i++){
			
			printf("%-11s", f_name[i]);

		}
		putchar(10);
		flag = 1;
	}

	for(i = 0; i < ncolumn; i+=2)
	{
		puts("========================================================\n");
		printf("%-8s %-8s.\n",f_value[i],f_value[i+1]);
		sprintf(msg->recvmsg,"%-8s %-8s .\n",f_value[i],f_value[i+1]);
		send(newsfd,msg,sizeof(MSG),0);
	}
	printf("\n");


	return 0;
}


//管理员考勤查询入口
int process_admin_attend_request(int newsfd,MSG *msg)
{
    int i=0,j=0;
    char *errmsg=NULL;
    char **resultp=NULL;
    int nrow,ncolumn;
    char sql[DATALEN]={0};
    printf("%d___%d\n",msg->flag,__LINE__);
    if(msg->flag == 1)
    {
        sprintf(sql,"select * from workoninfo where staffno='%d';",msg->info.no);
    }
    if(msg->flag==0)
    {
        sprintf(sql,"select * from workoninfo;");
    }
  /**  msg->flag == newsfd;
    if(sqlite3_exec(db,sql,admin_attend_query_callback,(void *)msg,&errmsg)!=SQLITE_OK)
    {
         printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
    }else
    {

        printf("管理员查询结束\n");
    }
    strcpy(msg->recvmsg,"finish");
    send(newsfd,msg,sizeof(MSG),0);
    flag=0;**/

    if(sqlite3_get_table(db, sql, &resultp,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("%s.\n",errmsg);
	 }else{
		printf("ncolumn :%d\tnrow :%d.\n",ncolumn,nrow);
		
	 	for(i = 0; i < ncolumn; i ++){
			printf("%-8s ",resultp[i]);
	 	}
	 	puts("");
		flag=1;
	 	int index = ncolumn;
	 	for(i = 0; i < nrow; i ++){
	 		printf("%s    %s     .\n",resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
				
	 		sprintf(msg->recvmsg,"%s,    %s,  ;",resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
			send(newsfd,msg,sizeof(MSG),0);

	 		index += ncolumn;
	 	}
		
	 	if(msg->flag != 1){  //全部查询的时候不知道何时结束，需要手动发送结束标志位，但是按人名查找不需要
	 		//通知对方查询结束了
			strcpy(msg->recvmsg,"finish");
	 		send(newsfd,msg,sizeof(MSG),0);
	 	}
        flag=0;
		
	 	sqlite3_free_table(resultp);
	 	printf("sqlite3_get_table successfully.\n");
	 }
    
    return 0;
    
}









//回调函数
int user_workon_query_callback(void *arg, int ncolumn, char **f_value, char **f_name)
{
	int i = 0;
	MSG *msg= (MSG *)arg;
	int newsfd = msg->flag;
	if(flag == 0){
		for(i = 0; i < ncolumn; i++){
			
			printf("%-11s", f_name[i]);

		}
		putchar(10);
		flag = 1;
	}

	for(i = 0; i < ncolumn; i+=3)
	{
		puts("=======================================================");
		printf("%-8s %-8s %-8s .\n",f_value[i],f_value[i+1],f_value[i+2]);
		sprintf(msg->recvmsg,"%-8s %-8s %-8s .\n",f_value[i],f_value[i+1],f_value[i+2]);
 
		send(newsfd,msg,sizeof(MSG),0);
       
	}
	printf("\n");
	return 0;
}



//员工出勤消息查询处理入口
int process_user_attend_request(int newsfd,MSG *msg)
{
    int i=0,j=0;
    char **resultp=NULL;
    int nrow,ncolumn;
    char sql[DATALEN]={0};
    char *errmsg=NULL;
    sprintf(sql,"select * from workoninfo where staffno=%d;",msg->id);
   
  /**  msg->flag=newsfd;
    if(sqlite3_exec(db,sql,user_workon_query_callback,(void*)msg,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
    }else
    {
        printf("回调成功\n");
    }

    flag=0;**/

    	

    if(sqlite3_get_table(db, sql, &resultp,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
	 	printf("%s.\n",errmsg);
	 }else{
	 	printf("searching.....\n");	
		for(i = 0; i < ncolumn; i ++){
	 		printf("%-8s ",resultp[i]);
	 	}
	 	puts("");
	 	puts("======================================================================================");
				
	 	int index = ncolumn;
	 	for(i = 0; i < nrow; i ++){
	 		printf("%s    %s     .\n",resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
				
	 		sprintf(msg->recvmsg,"%s,    %s    ;",resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
	 		send(newsfd,msg,sizeof(MSG),0);
			//}
			puts("======================================================================================");
	 		index += ncolumn;
		}
    }
	 	sqlite3_free_table(resultp);
	 	printf("sqlite3_get_table successfully.\n");


    return 0;
}




//处理出勤表信息
void process_workoninfo_add(MSG *msg)
{
    char *errmsg=NULL;
    char **resultp=NULL;
	char sqlworkon[DATALEN] = {0};
	char timedata[DATALEN] = {0};

	get_system_time(timedata);

	sprintf(sqlworkon,"insert into workoninfo values ('%s',%d);",timedata,msg->info.no);
	if(sqlite3_exec(db,sqlworkon,NULL,NULL,&errmsg)!= SQLITE_OK)
    {
      	printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
	}else{
		printf("插入出勤数据成功\n");
	}
}




//管理员回调函数
int admin_query_callback(void *arg, int ncolumn, char **f_value, char **f_name)
{
	int i = 0;
	MSG *msg= (MSG *)arg;
	int newsfd = msg->flag;

	if(flag == 0){
		for(i = 0; i < ncolumn; i++){
			
			printf("%-11s", f_name[i]);

		}
		putchar(10);
		flag = 1;
	}

	for(i = 0; i < ncolumn; i+=10)
	{
		puts("=============================================================================================================");
		printf("%-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s .\n",f_value[i],f_value[i+1],f_value[i+2],f_value[i+3],f_value[i+4],
			      f_value[i+5],f_value[i+6],f_value[i+7],f_value[i+8],f_value[i+9]);
		sprintf(msg->recvmsg,"%-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s .\n",f_value[i],f_value[i+1],f_value[i+2],f_value[i+3],f_value[i+4],
			      f_value[i+5],f_value[i+6],f_value[i+7],f_value[i+8],f_value[i+9]); 
		send(newsfd,msg,sizeof(MSG),0);
	}
	printf("\n");


	return 0;
}


//管理员查询入口
int process_admin_query_request(int newsfd,MSG *msg)
{
    int i=0,j=0;
    char **resultp=NULL;
    int nrow,ncolumn;
    char *errmsg=NULL;
    char sql[DATALEN]={0};
    printf("%d___%d\n",msg->flag,__LINE__);
    if(msg->flag == 1)
    {
        sprintf(sql,"select * from usrinfo where name='%s';",msg->info.name);
    }
    if(msg->flag==0)
    {
        sprintf(sql,"select * from usrinfo;");
    }
   /** msg->flag == newsfd;
    if(sqlite3_exec(db,sql,admin_query_callback,(void *)msg,&errmsg)!=SQLITE_OK)
    {
         printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
    }else
    {
        printf("管理员查询结束\n");
    }
    strcpy(msg->recvmsg,"finish");
    send(newsfd,msg,sizeof(MSG),0);
    flag=0;**/
 
 
	
	 if(sqlite3_get_table(db, sql, &resultp,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("%s.\n",errmsg);
	 }else{
		printf("ncolumn :%d\tnrow :%d.\n",ncolumn,nrow);
		
	 	for(i = 0; i < ncolumn; i ++){
			printf("%-8s ",resultp[i]);
	 	}
	 	puts("");
		
	 	int index = ncolumn;
	 	for(i = 0; i < nrow; i ++){
	 		printf("%s    %s     %s     %s     %s     %s     %s     %s     %s     %s  .\n",resultp[index+ncolumn-10],\
	 			resultp[index+ncolumn-9],resultp[index+ncolumn-8],resultp[index+ncolumn-7],resultp[index+ncolumn-6],resultp[index+ncolumn-5],\
	 			resultp[index+ncolumn-4],resultp[index+ncolumn-3],resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
				
	 		sprintf(msg->recvmsg,"%s,    %s,    %s,    %s,    %s,    %s,    %s,    %s,    %s,    %s;",resultp[index+ncolumn-10],\
	 			resultp[index+ncolumn-9],resultp[index+ncolumn-8],resultp[index+ncolumn-7],resultp[index+ncolumn-6],resultp[index+ncolumn-5],\
				resultp[index+ncolumn-4],resultp[index+ncolumn-3],resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
			send(newsfd,msg,sizeof(MSG),0);

	 		index += ncolumn;
	 	}
		
	 	//if(msg->flag != 1){  //全部查询的时候不知道何时结束，需要手动发送结束标志位，但是按人名查找不需要
	 		//通知对方查询结束了

	    //	}
        	strcpy(msg->recvmsg,"finish");
	 		send(newsfd,msg,sizeof(MSG),0);
		
	 	sqlite3_free_table(resultp);
	 	printf("sqlite3_get_table successfully.\n");
	 }




    return 0;
    
}






//管理员删除入口
int process_admin_deluser_request(int newsfd,MSG *msg)
{
    char *errmsg=NULL;
    char sql[DATALEN]={0};
    char buf[DATALEN]={0};

    sprintf(sql,"delete from usrinfo where staffno=%d;",msg->info.no);
    if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
        strcpy(msg->recvmsg,"failed");
        send(newsfd,msg,sizeof(MSG),0); 
        return -1; 
    }else
    {
        strcpy(msg->recvmsg,"OK");
        send(newsfd,msg,sizeof(MSG),0);
        printf("%d delete success.\n",msg->info.no);
    }
    sprintf(buf,"管理员%d删除了%d用户",msg->id,msg->info.no);
	process_historyinfo_add(msg,buf);
    
    return 0;
}











//管理员添加信息入口
int process_admin_adduser_request(int newsfd,MSG *msg)
{
    char* errmsg=NULL;
    char sql[DATALEN]={0};
    char buf[DATALEN]={0};

    printf("%d\t %d\t %s\t %s\t %d\n %s\t %s\t %s\t %s\t  %f.\n",msg->info.no,msg->info.usertype,msg->info.name,msg->info.passwd,\
		msg->info.age,msg->info.phone,msg->info.addr,msg->info.work,\
		msg->info.date,msg->info.salary);

    sprintf(sql,"insert into usrinfo values(%d,%d,'%s','%s',%d,'%s','%s','%s','%s',%f);",\
            msg->info.no,msg->info.usertype,msg->info.name,msg->info.passwd,msg->info.age,\
            msg->info.phone,msg->info.addr,msg->info.work,msg->info.date,msg->info.salary);
    if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
        strcpy(msg->recvmsg,"failed");
        send(newsfd,msg,sizeof(MSG),0);
        return -1;
    }else
    {
        strcpy(msg->recvmsg,"OK");
        send(newsfd,msg,sizeof(MSG),0);
        printf("%s register success.\n",msg->info.name);
    }
    sprintf(buf,"管理员%d添加了%s用户",msg->id,msg->info.name);
	process_historyinfo_add(msg,buf);

	return 0;
}


//管理员修改信息入口
 int process_admin_modify_request(int newsfd,MSG *msg)
 {
     char *errmsg=NULL;
     char sql[DATALEN]={0};
     char historybuf[DATALEN]={0};

     switch (msg->recvmsg[0])
     {
        case 'N':
                sprintf(sql,"update usrinfo set name='%s' where staffno=%d;",msg->info.name,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的用户名为%s",msg->id,msg->info.no,msg->info.name);
                break;
        case 'A':
                sprintf(sql,"update usrinfo set age=%d where staffno=%d;",msg->info.age,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的年龄为%d",msg->id,msg->info.no,msg->info.age);
                break;
        case 'D':
                sprintf(sql,"update usrinfo set addr='%s' where staffno=%d;",msg->info.addr,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的地址为%s",msg->id,msg->info.no,msg->info.addr);
                break;
        case 'P':
                sprintf(sql,"update usrinfo set phone='%s' where staffno=%d;",msg->info.phone,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的电话为%s",msg->id,msg->info.no,msg->info.phone);
                break;
        case 'W':
                sprintf(sql,"update usrinfo set work='%s' where staffno=%d;",msg->info.work,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的职位为%s",msg->id,msg->info.no,msg->info.work);
                break;
        case 'S':
                sprintf(sql,"update usrinfo set salary='%.2f' where staffno=%d;",msg->info.salary,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的薪水为%.2f",msg->id,msg->info.no,msg->info.salary);
                break;
        case 'T':
                sprintf(sql,"update usrinfo set date='%s' where staffno=%d;",msg->info.date,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的入职日期为%s",msg->id,msg->info.no,msg->info.date);
                break;
        case 'M':
                sprintf(sql,"update usrinfo set passwd='%s' where staffno=%d;",msg->info.passwd,msg->info.no);
                sprintf(historybuf,"%d修改工号为%d的密码为%s",msg->id,msg->info.no,msg->info.passwd);
                break;
     }
     if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
     {
            printf("sqlite3_exec:%s %d\n",errmsg,__LINE__);
     }else
     {
         printf("数据库修改成功\n");
         sprintf(msg->recvmsg,"数据库修改成功");
         process_historyinfo_add(msg,historybuf);
     }
     if(send(newsfd,msg,sizeof(MSG),0)<0)
            ERR_LOG("send");
     printf("------%s\n",historybuf);
     return 0;
     
 }




//历史记录回调入口
int history_callback(void *arg, int ncolumn, char **f_value, char **f_name)
{
	int i = 0;
	MSG *msg= (MSG *)arg;
	int newsfd = msg->flag;

	if(flag == 0){
		for(i = 0; i < ncolumn; i++){
			puts("======================================================================================");
			printf("%-11s", f_name[i]);
		}
		putchar(10);
		flag = 1;
	}

	for(i = 0; i < ncolumn; i+=3)
	{
		puts("======================================================================================");
		printf("%-8s %-8s %-8s",f_value[i],f_value[i+1],f_value[i+2]);
		sprintf(msg->recvmsg,"%-8s %-8s %-8s.\n",f_value[i],f_value[i+1],f_value[i+2]);
		send(newsfd,msg,sizeof(MSG),0);
	}
	puts("\n");

	return 0;
}

//处理管理员历史记录入口
void process_admin_history_request(int newsfd,MSG *msg)
{
    char sql[DATALEN]={0};
    char *errmsg=NULL;
    msg->flag=newsfd;

    sprintf(sql,"select * from histroyinfo;");
    if(sqlite3_exec(db,sql,history_callback,(void *)msg,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
    }else
    {
        printf("历史记录查询成功\n");
    }


    strcpy(msg->recvmsg,"finish");
    send(newsfd,msg,sizeof(MSG),0);
    flag=0;
    
}




//查询系统时间
void get_system_time(char *timedata)
{
    time_t t;
    struct tm *tp;
    time(&t);
    tp=localtime(&t);
	sprintf(timedata,"%d-%d-%d %d:%d:%d",tp->tm_year+1900,tp->tm_mon+1,\
			tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
	return ;
}
//回调函数
int user_query_callback(void *arg, int ncolumn, char **f_value, char **f_name)
{
	int i = 0;
	MSG *msg= (MSG *)arg;
	int newsfd = msg->flag;
	if(flag == 0){
		for(i = 0; i < ncolumn; i++){
			
			printf("%-11s", f_name[i]);

		}
		putchar(10);
		flag = 1;
	}

	for(i = 0; i < ncolumn; i+=10)
	{
		puts("=================================================================================================");
		printf("%-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s .\n",f_value[i],f_value[i+1],f_value[i+2],f_value[i+3],f_value[i+4],
			      f_value[i+5],f_value[i+6],f_value[i+7],f_value[i+8],f_value[i+9]);
		sprintf(msg->recvmsg,"%-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s %-8s .\n",f_value[i],f_value[i+1],f_value[i+2],f_value[i+3],f_value[i+4],
			      f_value[i+5],f_value[i+6],f_value[i+7],f_value[i+8],f_value[i+9]);
 
		send(newsfd,msg,sizeof(MSG),0);
       
	}
	printf("\n");
	return 0;
}
//员工查询入口
int process_user_query_request(int newsfd,MSG *msg)
{
    int i=0,j=0;
    char sql[DATALEN]={0};
    char **resultp=NULL;
    char *errmsg=NULL;
    int nrow,ncolumn;

    sprintf(sql,"select * from usrinfo where staffno=%d;",msg->id);
   
   /** msg->flag=newsfd;
    if(sqlite3_exec(db,sql,user_query_callback,(void*)msg,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
    }else
    {
        printf("回调成功\n");
    }

    flag=0;**/
    if(sqlite3_get_table(db, sql, &resultp,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
	 	printf("%s.\n",errmsg);
	 }else{
	 	printf("searching.....\n");	
		for(i = 0; i < ncolumn; i ++){
	 		printf("%-8s ",resultp[i]);
	 	}
	 	puts("");
	 	puts("======================================================================================");
				
	 	int index = ncolumn;
	 	for(i = 0; i < nrow; i ++){
	 		printf("%s    %s     %s     %s     %s     %s     %s     %s     %s     %s  .\n",resultp[index+ncolumn-10],\
	 			resultp[index+ncolumn-9],resultp[index+ncolumn-8],resultp[index+ncolumn-7],resultp[index+ncolumn-6],resultp[index+ncolumn-5],\
	 			resultp[index+ncolumn-4],resultp[index+ncolumn-3],resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
				
	 		sprintf(msg->recvmsg,"%s,    %s,    %s,    %s,    %s,    %s,    %s,    %s,    %s,    %s;",resultp[index+ncolumn-10],\
	 			resultp[index+ncolumn-9],resultp[index+ncolumn-8],resultp[index+ncolumn-7],resultp[index+ncolumn-6],resultp[index+ncolumn-5],\
	 			resultp[index+ncolumn-4],resultp[index+ncolumn-3],resultp[index+ncolumn-2],resultp[index+ncolumn-1]);
	 		send(newsfd,msg,sizeof(MSG),0);
			//}
			puts("======================================================================================");
	 		index += ncolumn;
		}
    }
	 	sqlite3_free_table(resultp);
	 	printf("sqlite3_get_table successfully.\n");
    return 0;

}

//处理历史信息
void process_historyinfo_add(MSG *msg,char *buf)
{
	char *errmsg=NULL;
    char **resultp=NULL;
	char sqlhistory[DATALEN] = {0};
	char timedata[DATALEN] = {0};

	get_system_time(timedata);

	sprintf(sqlhistory,"insert into histroyinfo values ('%s',%d,'%s');",timedata,msg->id,buf);
	if(sqlite3_exec(db,sqlhistory,NULL,NULL,&errmsg)!= SQLITE_OK)
    {
      	printf("sqlite3_exec:%s %d\n",errmsg,__LINE__); 
	}else{
		printf("插入历史数据成功\n");
	}
}

//员工修改入口
int process_user_modify_request(int newsfd,MSG *msg)
{



  char *errmsg=NULL;

  char **resultp=NULL;
  char sql[DATALEN]={0};
  char historybuf[DATALEN]={0};

  switch (msg->recvmsg[0])
  {
        case 'P':
                    sprintf(sql,"update usrinfo set phone='%s' where staffno=%d;",msg->info.phone,msg->info.no);
                    sprintf(historybuf,"%d修改工号为%d的电话为%s",msg->id,msg->info.no,msg->info.phone);
                    break;
        case 'D':
                    sprintf(sql,"update usrinfo set addr='%s' where staffno=%d;",msg->info.addr,msg->info.no);
                    sprintf(historybuf,"%d修改工号为%d的地址为%s",msg->id,msg->info.no,msg->info.addr);
                    break;
        case 'M':
                    sprintf(sql,"update usrinfo set passwd='%s' where staffno=%d;",msg->info.passwd,msg->info.no);
                    sprintf(historybuf,"%d修改工号为%d的密码为%s",msg->id,msg->info.no,msg->info.passwd);
                    break;
        
  }
  	printf("msgtype :%#x--usrtype: %#x--usrname: %d-passwd: %s.\n",msg->messagetype,msg->info.usertype,msg->info.no,msg->info.passwd);
	printf("msg->info.no :%d\t msg->info.addr %s\t msg->info.phone: %s.\n",msg->info.no,msg->info.addr,msg->info.phone);

    printf("%s__%d\n",sql,__LINE__);

    printf("%p __%d__\n", db, __LINE__);

  if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
  {
      printf("err_code:%d\n", sqlite3_errcode(db));
      	printf("sqlite3_exec:%s %d\n",errmsg,__LINE__);
        sprintf(msg->recvmsg,"数据库修改失败 %s\n",errmsg); 
  }else
  {
      printf("数据更新成功\n");
      sprintf(msg->recvmsg,"数据库修改成成功\n");
      process_historyinfo_add(msg,historybuf);
  }

  if(send(newsfd,msg,sizeof(MSG),0)<0)
        ERR_LOG("send");
    
    printf("------%s.\n",historybuf);
    return 0;

}

//员工或管理员入口
int process_user_or_admin_login_request(int newsfd,MSG *msg)
{
    char sql[DATALEN]={0};
    char *errmsg=NULL;
    char **result=NULL;
    char timedata[DATALEN] = {0};

	get_system_time(timedata);
    int row,column;
    
    msg->info.usertype=msg->usetype;
    msg->info.no=msg->id;
    strcpy(msg->info.passwd,msg->passwd);
    printf("usrtype: %#x-----usrid: %d---passwd: %s.\n",msg->info.usertype,msg->info.no,msg->info.passwd);

    sprintf(sql,"select * from usrinfo where usertype=%d and staffno=%d and passwd='%s';",msg->info.usertype,msg->info.no,msg->info.passwd);
    if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_get_table:%s %d\n",errmsg,__LINE__);
        return -1;
    }else
    {
        if(row == 0)
        {
            strcpy(msg->recvmsg,"帐号或密码错误\n");
            if(send(newsfd,msg,sizeof(MSG),0)<0)
                ERR_LOG("send");
        }else
        {
            printf("%d___%d\n",newsfd,__LINE__);
            strcpy(msg->recvmsg,"OK");
            strcpy(msg->workon,timedata);
            process_workoninfo_add(msg);
            if(send(newsfd,msg,sizeof(MSG),0)<0)
                ERR_LOG("send");

        }
        
    }

    return 0;
    
}
//退出考勤打开
int process_client_quiy_request(int newsfd,MSG *msg)
{
    char timedata[DATALEN] = {0};
	get_system_time(timedata);
	process_workoninfo_add(msg);
	strcpy(msg->workon,timedata);
    send(newsfd,msg,sizeof(MSG),0);
    return 0;
}



 //功能类型选项入口
int process_client_request(int newsfd,MSG *msg)
{
   // printf("%d __%d__\n", msg->messagetype, __LINE__);

 

    switch(msg->messagetype)
    {
        //用户或管理员登录
        case USER_LOGIN:
        case ADMIN_LOGIN:
                process_user_or_admin_login_request(newsfd,msg);
                break;
        //用户修改入口
        case USER_MODIFY:
                process_user_modify_request(newsfd,msg);
                break;
        //用户查询入口
        case USER_QUERY:
                process_user_query_request(newsfd,msg);
                break;
        //管理员查询历史记录入口
        case ADMIN_HISTORY:
                process_admin_history_request(newsfd,msg);
                break;
        //管理员修改入口
        case ADMIN_MODIFY:
                process_admin_modify_request(newsfd,msg);
                break;
        //管理员添加用户入口
        case ADMIN_ADDUSER:
                process_admin_adduser_request(newsfd,msg);
                break;
        //管理员删除用户入口
        case ADMIN_DELUSER:
                process_admin_deluser_request(newsfd,msg);
                break;
        //管理员查询入口
        case ADMIN_QUERY:
                process_admin_query_request(newsfd,msg);
                break;
       //用户考勤查询入口
        case USER_ATTEND:
                process_user_attend_request(newsfd,msg);
                break;
        //管理员考勤查询入口
        case ADMIN_ATTEND:
                process_admin_attend_request(newsfd,msg);
                break;
        //退出入口
        case QUIT:
                process_client_quiy_request(newsfd,msg);
                break; 
        default:
              //  goto END;
                break;
                  
    }
//END:
    //sqlite3_close(db);
    return 0;
}





int main(int argc, char const *argv[])
{
    sighandler_t s = signal(SIGCHLD, handler);
	if(s == SIG_ERR)
		ERR_LOG("signal");

    //打开数据库
    char *errmsg=NULL;
    MSG msg;
    if(sqlite3_open(STAFF_DATABASE,&db)!=SQLITE_OK)
    {
        //打印错误信息
        fprintf(stderr,"%s\n",sqlite3_errmsg(db));
        return -1;
    }
    printf("词典打开成功\n");

    //创建员工信息表
  //  char sql[DATALEN]="create table if not exists usrinfo(staffno int,usertype int,name char,passwd char,\
                        age int,phone char,addr char,work char,date char,salary REAL);";
  
    if(sqlite3_exec(db,"create table if not exists usrinfo(staffno int,usertype int,name char,passwd char,\
                        age int,phone char,addr char,work char,date char,salary REAL);",NULL,NULL,&errmsg)!=0)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__);
        return -1;
    }
    printf("员工信息表创建成功\n");

    //创建历史帐号信息表
    char sqlhis[DATALEN]="create table if not exists histroyinfo(time char,name char,words char);";
    if(sqlite3_exec(db,sqlhis,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__);
        return -1;
    }
    printf("历史记录表创建成功\n");

     
    if(sqlite3_exec(db,"create table if not exists workoninfo(time char,staffno int);",NULL,NULL,&errmsg)!=0)
    {
        printf("sqlite3_exec:%s %d\n",errmsg,__LINE__);
        return -1;
    }
    printf("考勤表创建成功\n");
    

    
    //创建套接字链接
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd<0)
    {
        ERR_LOG("socket");
    }

    //允许绑定地址快速重复利用
    int reuse=1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse,sizeof(int)) < 0)
    {
        ERR_LOG("setsockopt");
    }
    //绑定服务器和IP地址
    struct sockaddr_in sin;
    sin.sin_family          =   AF_INET;
    sin.sin_port            =   htons(atoi(argv[2]));
    sin.sin_addr.s_addr     =   inet_addr(argv[1]);

    if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
        ERR_LOG("bind");
    
    //将套接字设置成监听模式
    if(listen(sfd,5)<0)
        ERR_LOG("listen");
    printf("监听套接字成功\n");


    //获取连接后的套接字
    struct sockaddr_in cin;
    socklen_t clen=sizeof(cin);
    int newsfd=0;


    while(1)
    {

        //创建连接后的套接字
        newsfd  =accept(sfd,(struct sockaddr*)&cin,&clen);
        printf("newsfd=%d\n",newsfd);
        pid_t pid=fork();

        if(pid > 0)
        {
            //父进程
            close(newsfd);
        }
        else if(pid == 0)
        {
            //子进程
            close(sfd);
            int res=0;
            while (1)
            {
                memset(&msg,0,sizeof(msg));
                printf("返回阻塞点，等待重新输入\n");
                if((res=recv(newsfd,&msg,sizeof(MSG),0))<0)
                    ERR_LOG("recv");
                if(res==0)
                {
                    printf("客户端关闭\n");
                    break;
                }
                //功能处理函数
                printf("type:%d __%d__\n", msg.messagetype, __LINE__);
             //   printf("%d\t %d\t %s\t %s\t %d\n %s\t %s\t %s\t %s\t  %f.\n",msg.info.no,msg.info.usertype,msg.info.name,msg.info.passwd,\
		                msg.info.age,msg.info.phone,msg.info.addr,msg.info.work,\
		                    msg.info.date,msg.info.salary);
                printf("flags=%d_____%d\n",msg.flag,__LINE__);
                process_client_request(newsfd,&msg);
               // sqlite3_close(db);

            }
            printf("跳出循环\n");
            close(newsfd);
            exit(1);
        }

    }
    return 0;
}
