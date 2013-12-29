#include"clsscr.h"
#include<gtk/gtk.h>

struct win_struct
{
	GtkWidget *window;
	GtkWidget *fixed;
	GtkWidget *scroll[2];
	GtkWidget *list[3];
	GtkListStore *store[3];
	GtkWidget *bnt[10];
	GtkWidget *entry[5];
	GtkWidget *radio[2];
	GtkWidget *prog;
	GtkStatusIcon *sicon;
	GtkWidget *menu;
	GtkWidget *menu_restore;
	GtkWidget *menu_set;
	GtkWidget *menu_exit;
	GtkWidget *label;
};
struct win_struct ws;

//{{{ define

#define	wintitle			"修改大师V1.0"
//window's width & height
#define win_x				640
#define win_y				492
//lab1's width & height
#define lab1_w				70
#define lab1_h				25
#define lab1_px				10
#define lab1_py				lab1_px
//treeview1's width & height
#define t1_w				160
#define t1_h				275
//treeview's position
#define t1_px				10
#define t1_py				40
//scrollbar1's width
#define srl_w				16
#define srl_h				t1_h
//scrollbar1's position
#define srl_px				11+t1_w
#define srl_py				t1_py
//bnt1~2's pos and wid
#define bnt1_w				75
#define bnt1_h				22
#define bnt1_px				t1_px
#define bnt1_py				t1_h+t1_py+10
#define bnt2_w				bnt1_w
#define	bnt2_h				bnt1_h
#define bnt2_px				bnt1_px+bnt1_w+srl_w+10
#define bnt2_py				bnt1_py
//part2 label
#define lab2_w				100
#define lab2_h				lab1_h
#define lab2_px				srl_w+srl_px+5
#define lab2_py				lab1_py
//输入项的位置
#define labi_w				140
#define labi_h				lab2_h
#define labi_px				lab2_px
#define labi_py				lab2_py+lab2_h+5
//part2 label2
#define lab3_w				lab2_w
#define lab3_h				lab2_h
#define lab3_px				lab2_px
#define lab3_py				labi_py+55
//part2 entry1
#define entry1_w			140
#define entry1_h			lab3_h-3
#define entry1_px			lab3_px
#define entry1_py			lab3_py+lab3_h+3
//part2 radiobutton
#define rd1_px				entry1_px
#define rd1_py				entry1_py+entry1_h+5
#define rd2_px				rd1_px+70
#define rd2_py				rd1_py
//part2 progressbar
#define pro_w				entry1_w
#define pro_h				entry1_h-6
#define pro_px				entry1_px
#define pro_py				rd1_py+27
//part2 button
#define bnt3_w				120
#define bnt3_h				bnt2_h
#define bnt3_px				pro_px+10
#define bnt3_py				pro_py+pro_h+18
#define bnt4_w				bnt3_w
#define bnt4_h				bnt3_h
#define bnt4_px				bnt3_px
#define bnt4_py				bnt3_py+bnt3_h+18
#define bnt5_w				bnt4_w
#define bnt5_h				bnt4_h
#define bnt5_px				bnt4_px
#define bnt5_py				bnt4_py+bnt4_h+18
//part3 label
#define lab4_px				lab2_px+150
#define lab4_py				lab2_py
#define lab5_px				lab4_px+150
#define lab5_py				lab4_py
//part3 entry
#define entry2_w			140
#define entry2_h			20
#define entry2_px			lab4_px
#define entry2_py			labi_py
#define entry3_w			entry2_w
#define entry3_h			entry2_h
#define entry3_px			lab5_px
#define entry3_py			entry2_py
//part3 label
#define lab6_px				lab4_px
#define lab6_py				entry2_py+entry2_h+5
#define lab7_px				lab5_px
#define lab7_py				lab6_py
//part3 entry
#define entry4_w			entry3_w
#define entry4_h			entry3_h
#define entry4_px			entry2_px
#define entry4_py			lab6_py+27
#define entry5_w			entry4_w
#define entry5_h			entry4_h
#define entry5_px			lab7_px
#define entry5_py			entry4_py
//part3 button
#define bnt6_w				entry4_w
#define bnt6_h				bnt5_h
#define bnt6_px				entry4_px
#define bnt6_py				entry4_py+entry4_h+10
#define bnt7_w				bnt6_w
#define bnt7_h				bnt6_h
#define bnt7_px				entry5_px
#define bnt7_py				bnt6_py
//part3 treeview 185
#define t2_w				290
#define t2_h				160
#define t2_px				bnt6_px
#define t2_py				bnt6_py+bnt6_h+10
//part3 button
#define bnt8_w				bnt1_w
#define bnt8_h				bnt1_h
#define bnt8_px				t2_px+12
#define bnt8_py				t2_h+t2_py+10
#define bnt9_w				bnt8_w
#define bnt9_h				bnt8_h
#define bnt9_px				bnt8_px+bnt8_w+20
#define bnt9_py				bnt8_py
#define bnt10_w				bnt9_w
#define bnt10_h				bnt9_h
#define bnt10_px			bnt9_px+bnt9_w+20
#define bnt10_py			bnt9_py
//part3 treeview
#define t3_w				win_x-srl_w-20
#define t3_h				130
#define t3_px				t1_px
#define t3_py				bnt10_py+bnt10_h+10
//part3 scroll
#define srl2_w				srl_w
#define srl2_h				t3_h
#define srl2_px				t3_px+t3_w
#define srl2_py				t3_py


#define tip_statusicon		"修改大师V1.0\ntybitsfox\n2013-12-25"
#define iconfile			"/root/Desktop/20131113114643935_easyicon_net_48.png"
#define myphoto				"/root/Desktop/08888.jpg"

#define ps_cmd				"ps -Ao pid,fname >/tmp/uuuu_mod_tybitsfox.txt"
#define proc_file			"/tmp/uuuu_mod_tybitsfox.txt"

#define labi				"<span style=\"oblique\" color=\"red\" font=\"10\">%s</span>"
#define lab1				"<span style=\"oblique\" color=\"blue\" font=\"10\">获取的进程：</span>"
#define lab2				"<span style=\"oblique\" color=\"blue\" font=\"10\">当前指定的进程：</span>"
#define lab3				"<span style=\"oblique\" color=\"blue\" font=\"10\">待查询数据：</span>"
#define lab4				"<span style=\"oblique\" color=\"blue\" font=\"10\">锁定摘要：</span>"
#define lab5				"<span style=\"oblique\" color=\"blue\" font=\"10\">锁定地址：</span>"
#define lab6				"<span style=\"oblique\" color=\"blue\" font=\"10\">锁定下限：</span>"
#define lab7				"<span style=\"oblique\" color=\"blue\" font=\"10\">锁定上限：</span>"
#define rad0				"<span color=\"#ff00ff\" font=\"10\">十进制</span>"
#define rad1				"<span color=\"#ff00ff\" font=\"10\">十六进制</span>"
//}}}
//{{{
//主窗口的建立函数
void crt_window(int a,char **b);
//pixbuf的生成函数
GdkPixbuf *crt_pixbuf(gchar *fname);
//尝试建立treeview
void crt_treeview1();
//建立操作treeview1的两个按钮
void crt_tv1_bnt();
//建立第二列的控件
void crt_part2();
//建立第三列的控件
void crt_part3();
//按钮1 "取得进程"的响应函数
void on_getproc(GtkWidget *widget,gpointer gp);
//按钮2 "重置"的响应函数
void on_reset(GtkWidget *widget,gpointer pg);
//消息提示框
void messagebox(char *gc);
//树形列表1的双击响应函数
void on_tree1_dblclk(GtkTreeView *treeview,GtkTreePath *path,GtkTreeViewColumn *col,gpointer userdata);
//建立托盘
void crt_statusicon();
//托盘菜单中：恢复窗口的响应函数。
void restore_window(GtkWidget *widget,gpointer gp);
//托盘菜单中：关于窗口的响应函数
void about_window(GtkWidget *widget,gpointer gp);
//托盘菜单中：退出的响应函数
void exit_window(GtkWidget *widget,gpointer gp);
//托盘图标的右键菜单弹出响应函数
void show_menu(GtkWidget *widget,guint button,guint32 act_time,gpointer gp);
//主窗口隐藏，切换至托盘形式
gint hide_window(GtkWidget *widget,GdkEvent *event,gpointer gp);




//}}}


