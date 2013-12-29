#include"clsscr.h"
#include<gtk/gtk.h>

struct win_struct
{
	GtkWidget *window;
	GtkWidget *fixed;
	GtkWidget *bnt[10];
	GtkWidget *entry[4];
	GtkStatusIcon *sicon;
	GtkWidget *menu;
	GtkWidget *menu_restore;
	GtkWidget *menu_set;
	GtkWidget *menu_exit;
	GtkWidget *label;
};
struct win_struct ws;

#define	wintitle			"修改大师V1.0"
#define win_x				600
#define win_y				480
#define iconfile			"/root/Desktop/20131113114643935_easyicon_net_48.png"

#define lab1				"获取的进程："
#define lab2				"当前指定的进程："
#define lab3				"待查询数据："
#define lab4				"锁定摘要："
#define lab5				"锁定地址："
#define lab6				"锁定下限："
#define lab7				"锁定上限："

//主窗口的建立函数
void crt_window(int a,char **b);
//pixbuf的生成函数
GdkPixbuf *crt_pixbuf(gchar *fname);
//











