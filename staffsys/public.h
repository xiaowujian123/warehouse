#ifndef __PUBLIC_H__
#define __PUBLIC_H__


#define STAFF_DATABASE   "staff_manage.db"//数据库

#define NAMELEN  16
#define DATALEN  128

#define  ADMIN      0   //管理员
#define  USER       1   //用户

//用户功能
#define  USER_LOGIN         0X00000001
#define  USER_MODIFY        0X00000002
#define  USER_ATTEND        0X00000003
#define  USER_QUERY         0X00000004

//管理员功能
#define  ADMIN_LOGIN         0X00000010
#define  ADMIN_MODIFY        0X00000012
#define  ADMIN_ADDUSER       0X00000014
#define  ADMIN_DELUSER       0X00000016
#define  ADMIN_QUERY         0X00000020
#define  ADMIN_HISTORY       0X00000022
#define  ADMIN_ATTEND        0X00000024

//退出
#define QUIT        0X11111111


//员工基本信息结构体
typedef struct staff_message{
    int no;                     //编号
    int usertype;               //管理员0   员工1
    char name[NAMELEN];               //姓名
    char passwd[8];               //密码
    int age;                    //年龄
    char phone[NAMELEN];              //电话
    char addr[DATALEN];                //地址
    char work[DATALEN];                 //职位
    char date[DATALEN];               //入职日期
    double salary;              //薪资
}staff_message_t;


//通信结构体信息
typedef struct{
    int messagetype;               //请求消息类型
    int id;                         //编号
    int usetype;                    //管理员0  用户1
    char username[NAMELEN];         //姓名
    char passwd[6];                   //密码
    char workon[DATALEN];              //出勤通信的消息
    char recvmsg[DATALEN];           //通信的消息
    int flag;                       //标致位
    staff_message_t info;           //员工信息

}MSG;

//错误信息判断
#define ERR_LOG(msg) do{\
	perror(msg);\
	printf("%d %s %s\n",__LINE__,__func__,__FILE__);\
	return -1;\
}while(0)





//*********服务器的函数声明***********//
//查询系统时间
void get_system_time(char *timedata);
//处理历史信息
void process_historyinfo_add(MSG *msg,char *buf);
//员工修改入口
int process_user_modify_request(int newsfd,MSG *msg);
//用户还是管理员处理入口
int process_user_or_admin_login_request(int newsfd,MSG *msg);
//功能类型选项入口
int process_client_request(int newsfd,MSG *msg);
//用户回调函数
int user_query_callback(void *arg, int ncolumn, char **f_value, char **f_name);
//员工查询入口
int process_user_query_request(int newsfd,MSG *msg);
//历史记录回调入口
int history_callback(void *arg, int ncolumn, char **f_value, char **f_name);
//处理管理员历史记录入口
void process_admin_history_request(int newsfd,MSG *msg);
//管理员修改信息入口
int process_admin_modify_request(int newsfd,MSG *msg);
//管理员添加信息入口
int process_admin_adduser_request(int newsfd,MSG *msg);
//管理员查询入口
int process_admin_query_request(int newsfd,MSG *msg);
//管理员回调函数
int admin_query_callback(void *arg, int ncolumn, char **f_value, char **f_name);
//处理出勤表信息
void process_workoninfo_add(MSG *msg);
//管理员考勤查询入口
int process_admin_attend_request(int newsfd,MSG *msg);
//管理员考勤回调函数
int admin_attend_query_callback(void *arg, int ncolumn, char **f_value, char **f_name);
//退出考勤打开
int process_client_quiy_request(int newsfd,MSG *msg);


//*********客户端函数申明****************//
//员工查询
void do_user_query(int sfd,MSG *msg);
//修改记录
void do_user_modify(int sfd,MSG *msg);
//考勤记录
void do_user_attend(int sfd,MSG *msg);
//员工菜单
void user_menu(int sfd,MSG *msg);
//管理员登录还是用户登录
int admin_or_usr_login(int sfd,MSG *msg);
//用户登录窗口
int do_login(int sfd);
//管理员修改员工参数
void do_admin_modify(int sfd,MSG *msg);
//管理员增加用户
void do_admin_adduser(int sfd,MSG *msg);
//管理员删除员工信息
void do_admin_deluser(int sfd,MSG *msg);
//员工考勤记录查询
void do_user_attend(int sfd,MSG *msg);
//管理员退出并打开
void do_admin_quit(int sfd,MSG *msg);

#endif // !1