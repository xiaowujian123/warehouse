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


#include "public.h"



//管理员查询考勤记录
void do_admin_attend(int sfd,MSG *msg)
{
     int n;
     msg->messagetype = ADMIN_ATTEND;
     while(1)
     {
        printf("***********************************************\n");
        printf("*******1.按工号查找     2：查找所有    3：退出******\n");
        printf("***********************************************\n");
        printf("请输入要选择的选项>>>>>");
        scanf("%d",&n);
        getchar();
        switch (n)
        {
        case 1:
            msg->flag =1;
            break;
        case 2:
            msg->flag =0;
            break;
        case 3:
            return;    
        }
        if(msg->flag == 1)
        {
            printf("请输入要查询的工号:");
            scanf("%d",&msg->info.no);
            getchar();
            send(sfd,msg,sizeof(MSG),0);
         /**recv(sfd,msg,sizeof(MSG),0);
            printf("%s__%d\n",msg->recvmsg,__LINE__);
            printf("===============\n");
            printf("工号\t考勤记录n");
            printf("===============\n");
            printf("%s\n",msg->recvmsg);**/
            while (1)
            {
                //循环接收服务器发送的用户数据
                recv(sfd,msg,sizeof(MSG),0);
                if(strncmp(msg->recvmsg,"finish",6)==0)
                {
                    break;
                }
            printf("======================\n");
            printf("%s\n",msg->recvmsg);
            }
        }
        if(msg->flag == 0)
        {
            send(sfd,msg,sizeof(MSG),0);
            printf("============\n");
            printf("工号\t考勤记录\n");
            while (1)
            {
                //循环接收服务器发送的用户数据
                recv(sfd,msg,sizeof(MSG),0);
                if(strncmp(msg->recvmsg,"finish",6)==0)
                {
                    break;
                }
            printf("======================\n");
            printf("%s\n",msg->recvmsg);
            }
            

        }
     }
     printf("查询结束\n");
}


//管理员查询信息
void do_admin_query(int sfd,MSG *msg)
 {
     int n;
     msg->messagetype = ADMIN_QUERY;
     while(1)
     {
        printf("***********************************************\n");
        printf("*******1.按人名查找     2：查找所有    3：退出******\n");
        printf("***********************************************\n");
        printf("请输入要选择的选项>>>>>");
        scanf("%d",&n);
        getchar();
        switch (n)
        {
        case 1:
            msg->flag =1;
            break;
        case 2:
            msg->flag =0;
            break;
        case 3:
            return;    
        }
        if(msg->flag == 1)
        {
            printf("请输入要查询的人名:");
            scanf("%s",msg->info.name);
            getchar();
            send(sfd,msg,sizeof(MSG),0);
            recv(sfd,msg,sizeof(MSG),0);
            printf("====================================================================\n");
            printf("工号\t用户类型\t 姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t工资\n");
            printf("====================================================================\n");
            printf("%s\n",msg->recvmsg);
        }
        if(msg->flag == 0)
        {
            send(sfd,msg,sizeof(MSG),0);
            printf("====================================================================\n");
            printf("工号\t用户类型\t 姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t工资\n");
            while (1)
            {
                //循环接收服务器发送的用户数据
                recv(sfd,msg,sizeof(MSG),0);
                if(strncmp(msg->recvmsg,"finish",6)==0)
                {
                    break;
                }
            printf("====================================================================\n");
            printf("%s\n",msg->recvmsg);
            }
            

        }
     }
     printf("查询结束\n");
 }



//管理员删除员工信息
void do_admin_deluser(int sfd,MSG *msg)
{
    msg->messagetype = ADMIN_DELUSER;

    printf("请输入要删除的用户ID：");
    scanf("%d",&msg->info.no);
    getchar();
    send(sfd,msg,sizeof(MSG),0);
    recv(sfd,msg,sizeof(MSG),0);

    if(strncmp(msg->recvmsg,"OK",2)==0)
    {
        printf("\n");
        printf("删除成功\n");
        printf("删除工号为:%d的用户\n",msg->info.no);
    }
    else
    {
        printf("%s",msg->recvmsg);
    }
    

}





//管理员查看历史记录
void do_admin_history(int sfd,MSG *msg)
{
    msg->messagetype=ADMIN_HISTORY;
    send(sfd,msg,sizeof(MSG),0);

    while(1)
    {
        recv(sfd,msg,sizeof(MSG),0);
        if(strncmp(msg->recvmsg,"finish",6) == 0)
         {  
              break;
         }
            printf("===========================================================\n");
            printf("%s",msg->recvmsg);
    }
}


//管理员增加用户
void do_admin_adduser(int sfd,MSG *msg)
 {
     msg->messagetype = ADMIN_ADDUSER;
     msg->usetype = ADMIN;
     char temp;

     while(1)
     {
         printf("======================\n");

         printf("是否为管理员(Y/N)");
         scanf("%c",&temp);
         getchar();

         if(temp == 'Y'||temp =='y')
            msg->info.usertype = ADMIN;
         else if(temp == 'N'||temp =='n')
            msg->info.usertype = USER;

         printf("请输入工号:");
         scanf("%d",&msg->info.no);
         getchar();

         printf("请输入用户名:");
         scanf("%s",msg->info.name);
         getchar();

         printf("请输入用户密码:");
         scanf("%s",msg->info.passwd);
         getchar();

         printf("请输入年龄:");
         scanf("%d",&msg->info.age);
         getchar();

         printf("请输入电话:");
         scanf("%s",msg->info.phone);
         getchar();

         printf("请输入家庭住址:");
         scanf("%s",msg->info.addr);
         getchar();

         printf("请输入职位:");
         scanf("%s",msg->info.work);
         getchar();

         printf("请输入入职日期(格式：xx-xx-xx):");
         scanf("%s",msg->info.date);
         getchar();

         printf("请输入工资:");
         scanf("%lf",&msg->info.salary);
         getchar();
         printf("type:%d __%d__\n", msg->messagetype, __LINE__);
        send(sfd,msg,sizeof(MSG),0);
        recv(sfd,msg,sizeof(MSG),0);

       if(strncmp(msg->recvmsg,"OK",2)==0)
       {
           printf("添加成功\n");
       }else
       {
           printf("%s",msg->recvmsg);
       }
       	printf("是否继续添加员工:(Y/N)");
		scanf("%c",&temp);
		getchar();
		if(temp == 'N' || temp == 'n')
			break;
         
     }

     
 }




//管理员修改员工参数
void do_admin_modify(int sfd,MSG *msg)
{
    int n=0;
    msg->messagetype = ADMIN_MODIFY;
    printf("请输入要修改信息的工号为:>>");
    scanf("%d",&msg->info.no);
    getchar();

    printf("\n");
    printf("*******************请输入要修改的选项********************\n");
	printf("******	1：姓名	  2：年龄	3：家庭住址   4：电话  ******\n");
	printf("******	5：职位	   6：工资  7：入职年月   8：密码  ******\n");
	printf("******          	9：退出	            	  *******\n");
	printf("*************************************************************\n");

    printf("请输入您的选择>>>");
    scanf("%d",&n);
    getchar();
    switch (n)
    {
        case 1:
                printf("请输入用户名:");
                msg->recvmsg[0]='N';
                scanf("%s",msg->info.name);
                getchar();
                break;
        case 2:
                printf("请输入年龄:");
                msg->recvmsg[0]='A';
                scanf("%d",&msg->info.age);
                getchar();
                break;
        case 3:
                printf("请输入家庭住址:");
                msg->recvmsg[0]='D';
                scanf("%s",msg->info.addr);
                getchar();
                break;
        case 4:
                printf("请输入电话:");
                msg->recvmsg[0]='P';
                scanf("%s",msg->info.phone);
                getchar();
                break;
        case 5:
                printf("请输入职位:");
                msg->recvmsg[0]='W';
                scanf("%s",msg->info.work);
                getchar();
                break;
        case 6:
                printf("请输入工资:");
                msg->recvmsg[0]='S';
                scanf("%lf",&msg->info.salary);
                getchar();
                break;
        case 7:
                printf("请输入入职年月(格式:xx-xx-xx):");
                msg->recvmsg[0]='T';
                scanf("%s",msg->info.date);
                getchar();
                break;
        case 8:
                printf("请输入密码:");
                msg->recvmsg[0]='M';
                scanf("%s",msg->info.passwd);
                getchar();
                break;
        case 9:
                return;

    }
    send(sfd,msg,sizeof(MSG),0);
    recv(sfd,msg,sizeof(MSG),0);
    printf("%s",msg->recvmsg);
    printf("\n");

}

//管理员退出并打开
void do_admin_quit(int sfd,MSG *msg)
{
   msg->messagetype = QUIT;
   send(sfd,msg,sizeof(MSG),0);
   recv(sfd,msg,sizeof(MSG),0);
   printf("出勤打开:%s\n",msg->workon);
   return;
}


//管理员菜单
void admin_menu(int sfd,MSG *msg)
{
    int n;
    while(1)
    {
        printf("************************************************\n");
        printf("******1:查询  2：修改  3：添加用户 4:删除用户********\n");
        printf("*********5:查询历史记录  6:查询考勤记录 *************\n");
        printf("****************7：退出***************************\n");
        printf("************************************************\n");

        printf("请输入您的选择>>>>");
        scanf("%d",&n);
        getchar();

        switch (n)
        {
        case 1:
                do_admin_query(sfd,msg);
                 break;
        case 2:
                do_admin_modify(sfd,msg);
                 break;
        case 3:
                do_admin_adduser(sfd,msg);
                 break;
        case 4:
                do_admin_deluser(sfd,msg);
                 break;
        case 5:
                do_admin_history(sfd,msg);
                 break;
        case 6:
                do_admin_attend(sfd,msg);
                break;
        case 7:
                do_admin_quit(sfd,msg);
                 return;
                
        default:
                printf("您输入有误，请重新输入!\n");
        }
    }
}









//员工查询
void do_user_query(int sfd,MSG *msg)
{
    msg->messagetype = USER_QUERY;
    send(sfd,msg,sizeof(MSG),0);
    recv(sfd,msg,sizeof(MSG),0);
    printf("工号\t用户类型\t姓名\t密码\t年龄\t电话\t地址\t职位\t入职时间\t工资\n");
    printf("======================================================================================\n");
    printf("%s\n",msg->recvmsg);

}






//修改记录
void do_user_modify(int sfd,MSG *msg)
{
    int n=0;
    msg->messagetype =USER_MODIFY;
    printf("修改信息的工号:%d\n",msg->id);
    printf("type:%d __%d__\n", msg->recvmsg[0], __LINE__);

    printf("***************请输入要修改的以下选项**********************\n");
    printf("************1:家庭住址   2:电话  3:密码  4:退出************\n");
    printf("********************************************************\n");
    printf("请输入您的选择>>>>");
    scanf("%d",&n);
    getchar();

    switch(n)
    {
        case 1:
                printf("请输入您的家庭住址:");
                msg->recvmsg[0]='D';
    
                scanf("%s",msg->info.addr);
                getchar();
                break;
        case 2:
                printf("请输入您的电话:");
                msg->recvmsg[0]='P';
                scanf("%s",msg->info.phone);
                getchar();
                break;
        case 3:
                printf("请输入您的新密码:");
                msg->recvmsg[0]='M';
                scanf("%s",msg->info.passwd);
                getchar();
                break;
        case 4:
                return ;

    }
    printf("type:%d __%d__\n", msg->recvmsg[0], __LINE__);
    send(sfd,msg,sizeof(MSG),0);


    recv(sfd,msg,sizeof(MSG),0);

    printf("%s",msg->recvmsg);

    printf("修改结束\n");

    
}



//员工考勤记录查询
void do_user_attend(int sfd,MSG *msg)
{
    msg->messagetype = USER_ATTEND;
    send(sfd,msg,sizeof(MSG),0);
    recv(sfd,msg,sizeof(MSG),0);
    printf("考勤时间\t工号\n");
    printf("=====================\n");
    printf("%s\n",msg->recvmsg);
}

//员工菜单
void user_menu(int sfd,MSG *msg)
{
    int n;
    while (1)
    {
        
        printf("********************************************************\n");
        printf("**********1:查询   2：修改   3：考勤记录  4:退出 ***********\n");
        printf("********************************************************\n");

        printf("请输入您的选择>>>");
        scanf("%d",&n);
        getchar();

        switch(n)
        {
            case 1:
                    do_user_query(sfd,msg);
                    break;
            case 2:
                    do_user_modify(sfd,msg);
                    break;
            case 3: 
                    do_user_attend(sfd,msg);
                    break;
            case 4:
                    do_admin_quit(sfd,msg);
                    return ;
            default:
                    printf("您输入有错误,请重新输入\n");
        }
        printf("请输入任意字符清除>>");
        getchar();

    }
    

}


//管理员登录还是用户登录
int admin_or_usr_login(int sfd,MSG *msg)
{

    //输入账户
   // memset(&msg->id,0,NAMELEN);
    printf("请输入账户ID>>>:");
    scanf("%d",&msg->id);
    getchar();

    //输入密码
    memset(msg->passwd,0,NAMELEN);
    printf("请输入账户密码>>>:");
    scanf("%s",msg->passwd);
    getchar();

    //发送登录请求
    if(send(sfd,msg,sizeof(MSG),0)<0)
        ERR_LOG("send");

    //接收服务器响应
    if(recv(sfd,msg,sizeof(MSG),0)<0)
        ERR_LOG("recv");
    //printf("msg->recvmsg :%s\n",msg->recvmsg);
    printf("考勤登录时间 :%s\n",msg->workon);
    //判断是否登录成功
    if(strncmp(msg->recvmsg,"OK",2) == 0)
    {
        if(msg->usetype == ADMIN)
        {
            printf("\n");
            printf("******欢迎%d登录员工管理系统！******\n",msg->id);
            admin_menu(sfd,msg);
        }
        else if (msg->usetype == USER)
        {
            printf("\n");
            printf("******欢迎%d登录员工管理系统！******\n",msg->id);
            user_menu(sfd,msg);
        }
       
        
    }else
    {
        printf("\n");
        printf("登录失败!%s\n",msg->recvmsg);
        return -1;
    }
    
    return 0;
}







//用户登录窗口
int do_login(int sfd)
{
    int i;
    MSG msg;

    while(1)
    {
        system("clear");
        printf("********************************************\n");
        printf("********1:管理员模式  2.员工模式  3.退出********\n");
        printf("*********************************************\n");
        printf("请输入您的选项>>>");
        scanf("%d",&i);
        getchar();

        switch(i)
        {
      
            case 1:
                    msg.messagetype = ADMIN_LOGIN;
                    msg.usetype     = ADMIN;    
                    break;
            case 2:
                    msg.messagetype = USER_LOGIN;
                    msg.usetype     = USER;
                    break;
            case 3:
                    msg.messagetype = QUIT;
                    if(send(sfd,&msg,sizeof(MSG),0)<0)
                         ERR_LOG("send");
                    close(sfd);
                    exit(0);
            default:
                    printf("您的输入有误，请重新输入\n");
                    

        }
        admin_or_usr_login(sfd,&msg);
        printf("输入任意字符清除>>>>");
        getchar();

        
       
    }

}





























int main(int argc, char const *argv[])
{
    //创建socket
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd<0)
    {
        ERR_LOG("socket");
    }

    //端口快速重用
    int reuse=1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<0)
    {
        ERR_LOG("setsocket");
    }

    //绑定服务器IP和端口号
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(atoi(argv[2]));
    sin.sin_addr.s_addr = inet_addr(argv[1]);

    //链接服务器
    if(connect(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
    {
        ERR_LOG("connect");
    }

    do_login(sfd);

    close(sfd);    
    return 0;
}
