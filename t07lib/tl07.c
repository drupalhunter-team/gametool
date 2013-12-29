#include"tl07.h"

//{{{ void crt_window(int a,char **b)
void crt_window(int a,char **b)
{
	GtkWidget *item;
	GtkTreeIter iter;
	int i,j,k;
	char ch[36];
	gtk_init(&a,&b);
	ws.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(ws.window),wintitle);
	gtk_widget_set_size_request(ws.window,win_x,win_y);
	gtk_window_set_resizable(GTK_WINDOW(ws.window),FALSE);
	gtk_window_set_position(GTK_WINDOW(ws.window),GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(ws.window),crt_pixbuf(iconfile));
	ws.fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(ws.window),ws.fixed);
	crt_treeview1();
	crt_tv1_bnt();
	crt_part2();
	crt_part3();
	memset(ch,0,36);
	gtk_list_store_append(ws.store[1],&iter);
	gtk_list_store_set(ws.store[1],&iter,0,"生   命   值",1,"0x884893823",2,155,3,245,-1);
	gtk_list_store_append(ws.store[1],&iter);
	gtk_list_store_set(ws.store[1],&iter,0,"武器1",1,"0x884920230",2,16,-1);
	crt_statusicon();
	gtk_widget_show_all(ws.window);
	g_signal_connect(G_OBJECT(ws.window),"delete-event",G_CALLBACK(hide_window),NULL);
//	g_signal_connect_swapped(G_OBJECT(ws.window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_main();
}//}}}
//{{{ GdkPixbuf *crt_pixbuf(gchar *fname)
GdkPixbuf *crt_pixbuf(gchar *fname)
{
	GdkPixbuf *pix;
	GError *error=NULL;
	pix=gdk_pixbuf_new_from_file(fname,&error);
	if(!pix)
	{
		g_print("%s\n",error->message);
		g_error_free(error);
	}
	return pix;
}//}}}
//{{{ void crt_treeview1()
void crt_treeview1()
{
	GtkTreeModel *model;
	GtkTreeViewColumn *column;
	GtkCellRenderer	*render;
//把label1的建立代码移至该函数中，该函数建立的三个控件：label1,treeview,scroll1	
	ws.label=gtk_label_new("");
	//gtk_widget_set_size_request(ws.label,lab1_w,lab1_h);
	gtk_label_set_markup(GTK_LABEL(ws.label),lab1);
	ws.store[0]=gtk_list_store_new(2,G_TYPE_INT,G_TYPE_STRING);
	model=GTK_TREE_MODEL(ws.store[0]);
	ws.list[0]=gtk_tree_view_new_with_model(model);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("PID",render,"text",0,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[0]),column);
	render=gtk_cell_renderer_text_new();
//	g_object_set(G_OBJECT(render),"editable",TRUE);  //可编辑
	column=gtk_tree_view_column_new_with_attributes("进程",render,"text",1,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[0]),column);
	gtk_widget_set_size_request(ws.list[0],t1_w,t1_h);
	ws.scroll[0]=gtk_vscrollbar_new(gtk_tree_view_get_vadjustment(GTK_TREE_VIEW(ws.list[0])));
	gtk_widget_set_size_request(ws.scroll[0],srl_w,srl_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.label,lab1_px,lab1_py);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.list[0],t1_px,t1_py);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.scroll[0],srl_px,srl_py);
	g_signal_connect(G_OBJECT(ws.list[0]),"row-activated",G_CALLBACK(on_tree1_dblclk),NULL);
}//}}}
//{{{ void crt_tv1_bnt()
void crt_tv1_bnt()
{
	ws.bnt[0]=gtk_button_new_with_label("取得进程");
	gtk_widget_set_size_request(ws.bnt[0],bnt1_w,bnt1_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[0],bnt1_px,bnt1_py);
	ws.bnt[1]=gtk_button_new_with_label("重    置");
	gtk_widget_set_size_request(ws.bnt[1],bnt2_w,bnt2_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[1],bnt2_px,bnt2_py);
	g_signal_connect(G_OBJECT(ws.bnt[0]),"clicked",G_CALLBACK(on_getproc),NULL);
	g_signal_connect(G_OBJECT(ws.bnt[1]),"clicked",G_CALLBACK(on_reset),NULL);
}//}}}
//{{{ void crt_part2()
void crt_part2()
{
	GtkWidget *label;
	GSList *l;
	label=gtk_label_new("");
	//gtk_widget_set_size_request(label,lab2_w,lab2_h);
	gtk_label_set_markup(GTK_LABEL(label),lab2);
	gtk_fixed_put(GTK_FIXED(ws.fixed),label,lab2_px,lab2_py);
	ws.label=gtk_label_new("");//从现在起，该指针将保留用于输入label的使用
	//gtk_widget_set_size_request(ws.label,labi_w,labi_h);
	//gtk_label_set_markup(GTK_LABEL(ws.label),lab5);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.label,labi_px,labi_py);
	label=gtk_label_new("");
	//gtk_widget_set_size_request(label,lab3_w,lab3_h);
	gtk_label_set_markup(GTK_LABEL(label),lab3);
	gtk_fixed_put(GTK_FIXED(ws.fixed),label,lab3_px,lab3_py);
	ws.entry[0]=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(ws.entry[0]),60);
	gtk_widget_set_size_request(ws.entry[0],entry1_w,entry1_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.entry[0],entry1_px,entry1_py);
	ws.radio[0]=gtk_radio_button_new(NULL);
	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),rad0);
	gtk_container_add(GTK_CONTAINER(ws.radio[0]),label);
	l=gtk_radio_button_get_group(GTK_RADIO_BUTTON(ws.radio[0]));
	ws.radio[1]=gtk_radio_button_new(l);
	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),rad1);
	gtk_container_add(GTK_CONTAINER(ws.radio[1]),label);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.radio[0],rd1_px,rd1_py);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.radio[1],rd2_px,rd2_py);
	ws.prog=gtk_progress_bar_new();
	gtk_widget_set_size_request(ws.prog,pro_w,pro_h);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(ws.prog),"进度%");
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.prog,pro_px,pro_py);
	ws.bnt[2]=gtk_button_new_with_label("首次查找");
	gtk_widget_set_size_request(ws.bnt[2],bnt3_w,bnt3_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[2],bnt3_px,bnt3_py);
	ws.bnt[3]=gtk_button_new_with_label("再次查找");
	gtk_widget_set_size_request(ws.bnt[3],bnt4_w,bnt4_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[3],bnt4_px,bnt4_py);
	ws.bnt[4]=gtk_button_new_with_label("保存至文件");
	gtk_widget_set_size_request(ws.bnt[4],bnt5_w,bnt5_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[4],bnt5_px,bnt5_py);
}//}}}
//{{{ void crt_part3()
void crt_part3()
{
	GtkTreeModel *model;
	GtkTreeViewColumn *column;
	GtkCellRenderer	*render;
	GtkWidget *label;
	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),lab4);
	gtk_fixed_put(GTK_FIXED(ws.fixed),label,lab4_px,lab4_py);
	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),lab5);
	gtk_fixed_put(GTK_FIXED(ws.fixed),label,lab5_px,lab5_py);
	ws.entry[1]=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(ws.entry[1]),60);
	gtk_widget_set_size_request(ws.entry[1],entry2_w,entry2_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.entry[1],entry2_px,entry2_py);
	ws.entry[2]=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(ws.entry[2]),60);
	gtk_widget_set_size_request(ws.entry[2],entry3_w,entry3_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.entry[2],entry3_px,entry3_py);
	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),lab6);
	gtk_fixed_put(GTK_FIXED(ws.fixed),label,lab6_px,lab6_py);
	label=gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),lab7);
	gtk_fixed_put(GTK_FIXED(ws.fixed),label,lab7_px,lab7_py);
	ws.entry[3]=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(ws.entry[3]),60);
	gtk_widget_set_size_request(ws.entry[3],entry4_w,entry4_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.entry[3],entry4_px,entry4_py);
	ws.entry[4]=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(ws.entry[4]),60);
	gtk_widget_set_size_request(ws.entry[4],entry5_w,entry5_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.entry[4],entry5_px,entry5_py);
	ws.bnt[5]=gtk_button_new_with_label("添加锁定");
	gtk_widget_set_size_request(ws.bnt[5],bnt6_w,bnt6_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[5],bnt6_px,bnt6_py);
	ws.bnt[6]=gtk_button_new_with_label("删除锁定");
	gtk_widget_set_size_request(ws.bnt[6],bnt7_w,bnt7_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[6],bnt7_px,bnt7_py);
	ws.store[1]=gtk_list_store_new(4,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT,G_TYPE_INT);
	model=GTK_TREE_MODEL(ws.store[1]);
	ws.list[1]=gtk_tree_view_new_with_model(model);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("摘  要",render,"text",0,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[1]),column);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("锁定地址",render,"text",1,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[1]),column);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("下限",render,"text",2,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[1]),column);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("上限",render,"text",3,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[1]),column);
	gtk_widget_set_size_request(ws.list[1],t2_w,t2_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.list[1],t2_px,t2_py);
	ws.bnt[7]=gtk_button_new_with_label("保存锁定");
	gtk_widget_set_size_request(ws.bnt[7],bnt8_w,bnt8_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[7],bnt8_px,bnt8_py);
	ws.bnt[8]=gtk_button_new_with_label("锁    定");
	gtk_widget_set_size_request(ws.bnt[8],bnt9_w,bnt9_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[8],bnt9_px,bnt9_py);
	ws.bnt[9]=gtk_button_new_with_label("装载锁定");
	gtk_widget_set_size_request(ws.bnt[9],bnt10_w,bnt10_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.bnt[9],bnt10_px,bnt10_py);
	ws.store[2]=gtk_list_store_new(2,G_TYPE_INT,G_TYPE_STRING);
	model=GTK_TREE_MODEL(ws.store[2]);
	ws.list[2]=gtk_tree_view_new_with_model(model);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("页序号",render,"text",0,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[2]),column);
	render=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes("扫描地址",render,"text",1,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(ws.list[2]),column);
	gtk_widget_set_size_request(ws.list[2],t3_w,t3_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.list[2],t3_px,t3_py);
	ws.scroll[1]=gtk_vscrollbar_new(gtk_tree_view_get_vadjustment(GTK_TREE_VIEW(ws.list[2])));
	gtk_widget_set_size_request(ws.scroll[1],srl2_w,srl2_h);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.scroll[1],srl2_px,srl2_py);

}//}}}
//{{{ void on_getproc(GtkWidget *widget,gpointer gp)
void on_getproc(GtkWidget *widget,gpointer gp)
{
	FILE *file;
	int i,j;
	char ch[128],c[128];
	GtkTreeIter iter;
	//先调用清空函数
	gtk_list_store_clear(ws.store[0]);
	system(ps_cmd);
	file=fopen(proc_file,"r");
	if(file==NULL)
	{
		messagebox("打开文件失败！");
		return;
	}
	memset(ch,0,sizeof(ch));i=0;
	while(fgets(ch,sizeof(ch),file)!=NULL)
	{
		if(i==0)
		{//分割空格都是对齐的，这里确定分割空格的位置：
			for(i=0;i<strlen(ch);i++)
			{
				if(ch[i]=='D')
				{i++;break;}
			}//i保存了空格的位置
			memset(ch,0,sizeof(ch));
			continue;
		}
		memset(c,0,sizeof(c));
		memcpy(c,ch,i);
		j=atoi(c);//得到pid
		gtk_list_store_append(ws.store[0],&iter);
		gtk_list_store_set(ws.store[0],&iter,0,j,1,&ch[i+1],-1);
		memset(ch,0,sizeof(ch));	
	}
	fclose(file);
}//}}}
//{{{ void on_reset(GtkWidget *widget,gpointer pg)
void on_reset(GtkWidget *widget,gpointer pg)
{
	gtk_list_store_clear(ws.store[0]);
}//}}}
//{{{ void messagebox(char *gc)
void messagebox(char *gc)
{
	GtkWidget *dialog;
	GtkMessageType type;
	type=GTK_MESSAGE_INFO;
	dialog=gtk_message_dialog_new(NULL,GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,type,GTK_BUTTONS_OK,(gchar *)gc);
	gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}//}}}
//{{{ void on_tree1_dblclk(GtkTreeView *treeview,GtkTreePath *path,GtkTreeViewColumn *col,gpointer userdata)
void on_tree1_dblclk(GtkTreeView *treeview,GtkTreePath *path,GtkTreeViewColumn *col,gpointer userdata)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	GValue value;
	char ch[200];
	memset((void*)&value,0,sizeof(value));
	model=gtk_tree_view_get_model(treeview);
	if(gtk_tree_model_get_iter(model,&iter,path))
	{
		gtk_tree_model_get_value(model,&iter,1,&value);
		memset(ch,0,sizeof(ch));
		snprintf(ch,sizeof(ch),labi,g_value_get_string(&value));
		//g_value_unset(&value);
		gtk_label_set_markup(GTK_LABEL(ws.label),ch);
	}
}//}}}
//{{{ void crt_statusicon()
void crt_statusicon()
{
	ws.sicon=gtk_status_icon_new_from_file(iconfile);
	ws.menu=gtk_menu_new();
	ws.menu_restore=gtk_menu_item_new_with_label("恢复窗口");
	ws.menu_set=gtk_menu_item_new_with_label("关    于");
	ws.menu_exit=gtk_menu_item_new_with_label("退    出");
	gtk_menu_shell_append(GTK_MENU_SHELL(ws.menu),ws.menu_restore);
	gtk_menu_shell_append(GTK_MENU_SHELL(ws.menu),ws.menu_set);
	gtk_menu_shell_append(GTK_MENU_SHELL(ws.menu),ws.menu_exit);
	g_signal_connect(G_OBJECT(ws.menu_restore),"activate",G_CALLBACK(restore_window),NULL);
	g_signal_connect(G_OBJECT(ws.menu_set),"activate",G_CALLBACK(about_window),NULL);
	g_signal_connect(G_OBJECT(ws.menu_exit),"activate",G_CALLBACK(exit_window),NULL);
	gtk_widget_show_all(ws.menu);
	gtk_status_icon_set_tooltip(ws.sicon,tip_statusicon);
	g_signal_connect(GTK_STATUS_ICON(ws.sicon),"activate",GTK_SIGNAL_FUNC(restore_window),NULL);
	g_signal_connect(GTK_STATUS_ICON(ws.sicon),"popup_menu",GTK_SIGNAL_FUNC(show_menu),NULL);
	gtk_status_icon_set_visible(ws.sicon,FALSE);
}//}}}
//{{{ void restore_window(GtkWidget *widget,gpointer gp)
void restore_window(GtkWidget *widget,gpointer gp)
{
	gtk_widget_show(ws.window);
	gtk_window_deiconify(GTK_WINDOW(ws.window));
	gtk_status_icon_set_visible(ws.sicon,FALSE);
}//}}}
//{{{ void about_window(GtkWidget *widget,gpointer gp)
void about_window(GtkWidget *widget,gpointer gp)
{
//	messagebox("hello world");
	char ch[512];
	GtkWidget *dlg,*fixed,*label,*img;
	dlg=gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dlg),"关于本软件");
	gtk_window_set_position(GTK_WINDOW(dlg),GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request(dlg,450,300);
	gtk_window_set_resizable(GTK_WINDOW(dlg),FALSE);
	fixed=gtk_fixed_new();
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox),fixed,FALSE,FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(dlg),2);
	img=gtk_image_new_from_file(myphoto);
	gtk_widget_set_size_request(img,99,110);
	gtk_fixed_put(GTK_FIXED(fixed),img,5,5);
	label=gtk_label_new("");
	memset(ch,0,sizeof(ch));
	snprintf(ch,sizeof(ch),"<span style=\"italic\" face=\"YaHei Consolas Hybrid\" font=\"16\" color=\"#ff00ff\">程式设计：tybitsfox</span>");
	gtk_label_set_markup(GTK_LABEL(label),ch);
	gtk_widget_set_size_request(label,300,30);
	gtk_fixed_put(GTK_FIXED(fixed),label,120,40);
	gtk_widget_show_all(dlg);
	gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);
}//}}}
//{{{ void exit_window(GtkWidget *widget,gpointer gp)
void exit_window(GtkWidget *widget,gpointer gp)
{
	gtk_main_quit();
}//}}}
//{{{ void show_menu(GtkWidget *widget,guint button,guint32 act_time,gpointer gp)
void show_menu(GtkWidget *widget,guint button,guint32 act_time,gpointer gp)
{
	gtk_menu_popup(GTK_MENU(ws.menu),NULL,NULL,gtk_status_icon_position_menu,widget,button,act_time);

}//}}}
//{{{ gint hide_window(GtkWidget *widget,GdkEvent *event,gpointer gp)
gint hide_window(GtkWidget *widget,GdkEvent *event,gpointer gp)
{
	gtk_widget_hide(ws.window);
	gtk_status_icon_set_visible(ws.sicon,TRUE);
	return TRUE;
}//}}}














//{{{
/*
 ws.scroll[0]=gtk_scrolled_window_new(NULL,NULL);
	gtk_widget_set_size_request(ws.scroll[0],164,304);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ws.scroll[0]),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	gtk_fixed_put(GTK_FIXED(ws.fixed),ws.scroll[0],10,40);
	ws.list[0]=gtk_list_new();
	gtk_list_set_selection_mode(GTK_LIST(ws.list[0]),GTK_SELECTION_SINGLE);
	gtk_list_scroll_vertical(GTK_LIST(ws.list[0]),GTK_SCROLL_STEP_FORWARD,0);
	gtk_widget_set_size_request(ws.list[0],160,300);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(ws.scroll[0]),ws.list[0]);
	memset(ch,0,6);
	for(j=0;j<5;j++)
	{
		ch[j]='a';
	}
	for(i=0;i<20;i++)
	{
		item=gtk_list_item_new_with_label(ch);
		for(j=0;j<5;j++)
		{
			ch[j]++;
		}
		gtk_container_add(GTK_CONTAINER(ws.list[0]),item);
	}
//下列代码是treeview的双击事件响应，待测试！
g_signal_connect(view, "row-activated", (GCallback) view_onRowActivated, NULL);
void view_onRowActivated (GtkTreeView *treeview,
                      GtkTreePath *path,
                      GtkTreeViewColumn *col,
                      gpointer userdata)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    g_print ("A row has been double-clicked!\n");

    model = gtk_tree_view_get_model(treeview);

    if (gtk_tree_model_get_iter(model, &iter, path))
    {
         gchar *name;

         gtk_tree_model_get(model, &iter, COLUMN, &name, -1);

         g_print ("Double-clicked row contains name %s\n", name);

         g_free(name);
    }
}


 *///}}}











