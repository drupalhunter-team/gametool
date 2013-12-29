#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>
#include<linux/types.h>
#include<linux/moduleparam.h>
#include<linux/pci.h>
#include<asm/unistd.h>
#include<linux/device.h>
#include<linux/sched.h>
#include<linux/pid.h>
#include<linux/mm_types.h>
//get_user_pages
#include<linux/mm.h>
#include<linux/security.h>
#include<linux/pagemap.h>
//kmap,kunmap
#include<linux/highmem.h>
struct KVAR_ADDR
{
	unsigned int	maxd;			//上限
	unsigned int	mind;			//下限
	union{
	unsigned char  seg[2];			//段索引
	unsigned short sh;
	};
	union{
	unsigned short pg;				//页索引
	unsigned char  gch[2];			//
	};
	union{
	unsigned int   pin;				//页内偏移
	unsigned char  inch[4];
	};
};
struct KAVR_AM
{
	unsigned char sync;					//命令字段中0字节的同步标志，
	unsigned char cmd;					//命令字段中1字节的主命令标志
	union{
	int   pid;							//命令字段中的2-5字节，传入的pid
	unsigned char  pch[4];			
	};
	unsigned char proc;					//命令字段中的6字节，存放进度值1-100
	unsigned char end0;					//命令字段中的7字节，存放本次查询的结束标志，=1结束
	char vv0[2];						//命令字段中的8,9字节，暂时未用。
	union{
	unsigned long snum;					//命令字段中的10字节起的查询关键字。
	unsigned char sch[4];
	};
	char oaddr[4];						//命令字段中的14字节起的导出地址。
	char vv1[22];						//18-47字节暂时未,可以考虑传送代码段和数据段的段地址。
	union{
	unsigned int data_seg;
	unsigned char dsg[4];
	};
	union{
	unsigned int text_seg;
	unsigned char tsg[4];
	};
	struct KVAR_ADDR vadd[8];			//8个锁定（修改）数据和地址。 
};
struct KVAR_TY
{
	int		thread_lock;			//进程运行锁标志。
	unsigned char flag;//=0只允许传送pid,=1只允许首次查询和传送pid。=2全部操作允许。传送pid时该字节置1,首次查询时该字节置2。
	int		 pid;
	union{
		unsigned char  dest[4];
		int	dnum;
	};
//下面两项保存段起始地址和页序号。
	unsigned long  seg[2];//0:代码段，1：数据段
	unsigned short pin;//暂时保存，准备往buffer中写入		
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tybitsfox ([email]tybitsfox@126.com[/email])");
MODULE_DESCRIPTION("kernel memory access module.");
//
#define K_BUFFER_SIZE		8192
#define DRV_MAJOR			252
#define DRV_MINOR			0
#define drv_name			"memacc_8964_dev"
#define d_begin				192
#define d_len				8000


//{{{ 
/*定义内核与用户的互交:
  1、不再设置共享内存等，统一使用内核申请的缓冲区同步两者的操作。
  2、缓冲区前192字节始终不会包含数据信息，该区域用于传送命令数据。
  3、后8000字节用于传送数据信息，数据信息的结构为8字节一个单元，每单元
  包含段索引（0字节）内存页索引（2、3字节）和页内偏移（4-7字节）。每次
  传送的数据量为1000个地址。
  4、命令的定义：
  （1）第0字节为用户和内核操作的同步标志。用户界面操作完成后将该字节
  置0,内核操作完成后将该字节置1。因此，当用户查看内核操作是否完成，可
  轮寻该字节是否为1,不为1则进入等待。内核同理，实现两者的操作同步。
  （2）第1字节为主命令字节，
  =0为初次同步，传输待查询进程的pid。第2、3、4、5字节存储pid值。
  =1表示首次查询，此时第10字节开始的连续4字节为待查数据。
  =2表示再次查询，第10字节开始连续4字节为待查数据。并且，数据区域内传入
  前次查询匹配的地址。
  =3表示向用户传送一页的目标内存数据，第15字节开始传入8字节地址，返回该
  地址所在页的整页数据。
  =4表示修改操作，修改操作默认一次最多修改8个数据。传入格式为：上限（4字节）
  下限（4字节）8字节地址格式，一个记录16字节，其实位置：0x30
  =5表示锁定操作，该操作与修改操作基本一致，但是该操作连续监视内存，持续修改。
  （3） 第6字节存储查询进度字，有效值1-100。该字节在进行查询时由内核按
  查询进度写入。并且用户端可不受同步标志字段的限制读取该值进行显示。
  （4） 第7字节存储命令完成标志，该标志对查询命令有效，当首次查询超过1000
  地址时，由于缓冲区的限制要采取分次传送。每传送一次字节0设置并开始传送，
  查询完成后字节0设置并且该字节也设置，表示整个查询结束。同时该字节在首次查
  询时是由内核置位，用户端判断，再次查询时是由客户端置位，内核判断（需要传入
  地址）。
  （5）第30字节开始存储传出的代码段和数据段的地址。
 
 *///}}}
char *mp,*tp;
struct class *mem_class;
unsigned char suc=0;
struct KVAR_TY kv1;
struct KVAR_AM k_am;//和命令头对应的结构

static int __init reg_init(void);
static void __exit reg_exit(void);
static int mem_open(struct inode *ind,struct file *filp);
static int mem_release(struct inode *ind,struct file *filp);
static ssize_t mem_read(struct file *filp,char __user *buf,size_t size,loff_t *fpos);
static ssize_t mem_write(struct file *filp,char __user *buf,size_t size,loff_t *fpos);
static void class_create_release(struct class *cls);
//命令分析函数
static int anly_cmd(void);
//首次查询的线程函数
int first_srh(void *argc);

struct file_operations mem_fops=
{
	.owner=THIS_MODULE,
	.open=mem_open,
	.release=mem_release,
	.read=mem_read,
	.write=mem_write,
};
//{{{ int __init reg_init(void)
int __init reg_init(void)
{
	int res,retval;
	int devno;
	mem_class=NULL;
	devno=MKDEV(DRV_MAJOR,DRV_MINOR);
	mp=(char*)kmalloc(K_BUFFER_SIZE,GFP_KERNEL);
	if(mp==NULL)
		printk("<1>kmalloc error!\n");
	memset(mp,0,K_BUFFER_SIZE);
	mp[0]=1;//用户进程可以发送请求或命令了
	tp=(char*)vmalloc(K_BUFFER_SIZE);
	if(tp==NULL)
		printk("<1>vmalloc error!\n");
	memset(tp,0,K_BUFFER_SIZE);
	res=register_chrdev(DRV_MAJOR,drv_name,&mem_fops);//注册字符设备
	if(res)
	{
		printk("<1>register char dev error!\n");
		goto reg_err01;
	}
	mem_class=kzalloc(sizeof(*mem_class),GFP_KERNEL);//实体话设备类
	if(IS_ERR(mem_class))
	{
		kfree(mem_class);
		mem_class=NULL;
		printk("<1>kzalloc error!\n");
		goto reg_err02;
	}
	mem_class->name=drv_name;
	mem_class->owner=THIS_MODULE;
	mem_class->class_release=class_create_release; //初始化设备类释放处理函数。
	retval=class_register(mem_class);		//注册设备类
	if(retval)
	{
		kfree(mem_class);
		printk("<1>class_register error!\n");
		goto reg_err02;
	}
	device_create(mem_class,NULL,devno,NULL,drv_name);//注册设备文件系统，并建立节点。
	printk("<1>device create success!!!\n");
	return 0;
reg_err02:
	unregister_chrdev(DRV_MAJOR,drv_name);//删除字符设备
reg_err01:
	kfree(mp);
	suc=1;
	return -1;
}//}}}
//{{{ void __exit reg_exit(void)
void __exit reg_exit(void)
{
	if(suc!=0)
		return;
	unregister_chrdev(DRV_MAJOR,drv_name);
	device_destroy(mem_class,MKDEV(DRV_MAJOR,DRV_MINOR));
	if((mem_class!=NULL) && (!IS_ERR(mem_class)))
		class_unregister(mem_class);
	if(mp!=NULL)
		kfree(mp);
	printk("<1>module exit ok!\n");
}//}}}
//{{{ int mem_open(struct inode *ind,struct file *filp)
int mem_open(struct inode *ind,struct file *filp)
{
	try_module_get(THIS_MODULE);	//引用计数增加
	printk("<1>device open success!\n");
	return 0;
}//}}}
//{{{ int mem_release(struct inode *ind,struct file *filp)
int mem_release(struct inode *ind,struct file *filp)
{
	module_put(THIS_MODULE);	//计数器减1
	printk("<1>device release success!\n");
	return 0;
}//}}}
//{{{ ssize_t mem_read(struct file *filp,char *buf,size_t size,loff_t *fpos)
ssize_t mem_read(struct file *filp,char *buf,size_t size,loff_t *fpos)
{
	int res;
	char *tmp;
	if(mp==NULL)
	{
		printk("<1>kernel buffer error!\n");
		return 0;
	}
	if(mp[0]==0)
	{
		tmp=(char*)&k_am;
		size=sizeof(k_am);
	}
	else
	{
		tmp=mp;
		if(size>K_BUFFER_SIZE)
			size=K_BUFFER_SIZE;
	}
	res=copy_to_user(buf,tmp,size);
	if(res==0)
	{
//		printk("<1>copy data from k->u success!\n");
		return size;
	}
//	printk("<1>copy date from k->u error!\n");
	return 0;
}//}}}
//{{{ ssize_t mem_write(struct file *filp,char *buf,size_t size,loff_t *fpos)
ssize_t mem_write(struct file *filp,char *buf,size_t size,loff_t *fpos)
{
	int i,j,res;
	char *tmp;
	if(mp==NULL)
	{
		printk("<1>kernel buffer error!\n");
		return 0;
	}
	tmp=mp;
	if(size>sizeof(k_am))
		size=sizeof(k_am);
	res=copy_from_user(tmp,buf,size);
	if(res==0)
	{
//		printk("<1>copy data from u->k success!\n");
//		i=0;j=0;//这里要插入命令分析函数。
		if(kv1.thread_lock==0)
		{//需要启动内核线程,根据tmp[0]确定启动哪个线程：
			memset((void*)&k_am,0,sizeof(k_am));
			memcpy((void*)&k_am,tmp,size);//拷贝至结构，平时的操作只在结构中进行。
//为了保险，一些标志再次强制设置：
			k_am.sync=0;k_am.end0=0;			
			memset((void*)&tmp[d_begin],0,d_len);//清空数据缓冲区
			if(tmp[1]==0)
			{
				kv1.pid=k_am.pid;//命令字0的主要任务就是取得pid
				return size;	
			}
			if(tmp[1]==1)//首次查找。
			{
				kv1.thread_lock=1;
				kernel_thread(first_srh,NULL,CLONE_KERNEL);//启动首次查找线程.
				return size;
			}
			if(tmp[1]==2)//再次查询
			{
				kv1.thread_lock=1;
				kernel_thread(next_srh,NULL,CLONE_KERNEL);//启动再次查询线程
				return size;
			}
			//下面还有修改，读取整页等操作。
		}
		else
		{//此时线程已启动，本次操作仅是表示用户已经拷贝完数据，可以继续线程的查询操作
		//注意：此时tmp[0]必须是0,该操作应该是由用户设置。	
			return size;
		}
	}
	return 0;
}//}}}
//{{{ int anly_cmd(void)--2013-12-29 该函数彻底失效，其功能由mem_write函数中的代码实现
int anly_cmd(void)
{
	int i;
	i=(int)mp[1];
	switch(i)
	{
	case 0://初次同步，传送pid
		kv1.pid=0;
		memcpy((void*)&kv1.pid,(void*)&mp[2],2);//得到目标进程的pid
		kv1.flag=1;
		mp[0]=1;//本次操作完成，允许用户再进行新的指令
		break;
	case 1://首次查询，
		if(kv1.pid==0)//操作越位
		{
			printk("<1>trans pid error!\n");
			return 1;
		}
		memcpy(kv1.dest,(void*)&mp[10],4);//得到待查数据
		kv1.flag=2;
		//这里调用查询函数，开始查询。当查询的地址数量=1000时，查询函数挂起，将mp[0]置位，等待用户将地址拷贝完成后继续进行。
		break;
	case 2://再次查询
		if(kv1.flag!=2)
		{
			printk("<1>searching error!\n");
			return 1;
		}
		memcpy(kv1.dest,(void*)&mp[10],4);//得到再次查询的数据
		//这里调用再次查询函数，
		break;
	case 3://整页传送
		if(kv1.flag!=2)
		{
			printk("<1>searching error!\n");
			return 1;
		}
		memcpy(kv1.dest,(void*)&mp[10],4);//得到地址，传送该地址所在页的全部数据。
		//这里调用数据传送函数
		break;
	};
	return 0;
}//}}}
//{{{ int first_srh(void)
int first_srh(void *argc)
{
	char *c,*mc;
	int ret,len,i,j,k,m,n;
	struct pid *kpid;
	struct task_struct *t_str;
	struct mm_struct *mm_str;
	struct vm_area_struct *vadr;
	struct page **pages,*pg;
	unsigned long adr,sadr;
	wait_queue_head_t whead;
	wait_queue_t	wdata;
	pg=NULL;
	memset((void*)&mp[d_begin],0,d_len);
	mc=&mp[d_begin];
	//sleep_on_time函数的准备
	init_waitqueue_head(&whead);
	initwaitqueue_entry(&wdata,current);
	add_wait_queue(&whead,&wdata);
	kpid=find_get_pid(kv1.pid);
	if(kpid==NULL)
	{
		printk("<1>find_get_pid error!\n");
		return 1;
	}
	t_str=pid_task(kpid,PIDTYPE_PID);
	if(t_str==NULL)
	{
		printk("<1>pid_task error!\n");
		return 1;
	}
	mm_str=get_task_mm(t_str);
	if(mm_str==NULL)
	{
		printk("<1>get_task_mm error!\n");
		return 1;
	}
	printk("<1>pid: %d data addr is: 0x%lx~~~~0x%lx\n",kv1.pid,mm_str->start_data,mm_str->end_data);
	kv1.seg[0]=mm_str->start_text;
	kv1.seg[1]=mm_str->end_text;
	n=0;
	for(m=0;m<2;m++)
	{//段的循环，只查询代码段和数据段。
		if(m==0)
			vadr=mm_str->mmap;
		else
			vadr=mm_str->mmap->vm_next;
		len=vma_pages(vadr);//取得页数量
		if(pg==NULL)
		{
			pg=(struct page *)vmalloc(sizeof(void*)*(len+1));//分配足够数量的空间，保存页地址。
			if(pg==NULL)
			{
				printk("<1>vmalloc error11\n");
				return 1;
			}
		}
		memset((void*)&pg,0,sizeof(sizeof(void*)*(len+1)));
		pages=&pg;
		down_write(&mm_str->mmap_sem);
		ret=get_user_pages(t_str,mm_str,adr,len,0,0,pages,NULL);
		if(ret>0)
		{
			for(k=0;k<ret;k++)
			{
				if(pages[k]==NULL)
				{
					printk("<1>search finished\n");
					break; //查找完成
				}
				c=(char*)kmap(pages[k]);
				if(kv1.dnum<256)//按字节查询
				{
					for(j=0;j<4096;j++)
					{
						if(c[j]==kv1.dest[0])
						{
							mc[0]=(char)m;//段的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&k,2);//页的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&j,sizeof(void*));//页内偏移。
							mc+=4;
							if(mc-mp[d_begin]<8)//写满一页，需要往用户空间传送了
							{
								mp[0]=1;//调用休眠，循环查询mp[0]==0等待。
								k_am.sync=1;
								while(1)
								{
									if(mp[0]==0)
										break;
									sleep_on_timeout(&whead,200);
								}
								//重置指针
								mc=&mp[d_begin];
								memset((void*)&mp[d_begin],0,d_len);//清空数据缓冲
							}
						}
					}
				}
				if(kv1.dnum<0x10000 && kv1.dnum>0xff)
				{
					for(j=0;j<4095;j++)
					{
						if((c[j]==kv1.dest[0]) && (c[j+1]==kv1.dest[1]))
						{
							mc[0]=(char)m;//段的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&k,2);//页的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&j,sizeof(void*));//页内偏移。
							mc+=4;
							if(mc-mp[d_begin]<8)//写满一页，需要往用户空间传送了
							{
								mp[0]=1;//调用休眠，循环查询mp[0]==0等待。
								k_am.sync=1;
								while(1)
								{
									if(mp[0]==0)
										break;
									sleep_on_timeout(&whead,200);
								}
								//重置指针
								mc=&mp[d_begin];
								memset((void*)&mp[d_begin],0,d_len);//清空数据缓冲
							}
						}
					}
				}
				if(kv1.dnum<0x1000000 && kv1.dnum>0xffff)
				{
					for(j=0;j<4094;j++)
					{
						if((c[j]==kv1.dest[0]) && (c[j+1]==kv1.dest[1]) && (c[j+2]==kv1.dest[2]))
						{
							mc[0]=(char)m;//段的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&k,2);//页的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&j,sizeof(void*));//页内偏移。
							mc+=4;
							if(mc-mp[d_begin]<8)//写满一页，需要往用户空间传送了
							{
								mp[0]=1;//调用休眠，循环查询mp[0]==0等待。
								k_am.sync=1;
								while(1)
								{
									if(mp[0]==0)
										break;
									sleep_on_timeout(&whead,200);
								}
								//重置指针
								mc=&mp[d_begin];
								memset((void*)&mp[d_begin],0,d_len);//清空数据缓冲
							}
						}
					}
				}
				if(kv1.dnum<0x100000000 && kv1.dnum>0xffffff)
				{
					for(j=0;j<4093;j++)
					{
						if((c[j]==kv1.dest[0]) && (c[j+1]==kv1.dest[1]) && (c[j+2]==kv1.dest[2]) && (c[j+3]==kv1.dest[3]))
						{
							mc[0]=(char)m;//段的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&k,2);//页的索引号
							mc+=2;
							memcpy((void*)&mc,(void*)&j,sizeof(void*));//页内偏移。
							mc+=4;
							if(mc-mp[d_begin]<8)//写满一页，需要往用户空间传送了
							{
								mp[0]=1;//调用休眠，循环查询mp[0]==0等待。
								k_am.sync=1;
								while(1)
								{
									if(mp[0]==0)
										break;
									sleep_on_timeout(&whead,200);
								}
								//重置指针
								mc=&mp[d_begin];
								memset((void*)&mp[d_begin],0,d_len);//清空数据缓冲
							}
						}
					}
				}
				kunmap(pages[k]);
				pach_cache_release(pages[k]);
			}
		}
	}//至此全部结束，需要将结束标志置位、取消线程锁。
	k_am.sync=1;//用户可读
	k_am.end0=1;//通知用户全部结束的标志
	memcpy((void*)&mp,(void*)&k_am,sizeof(k_am));//重新将命令区域拷贝至缓冲区
	kv1.thread_lock=0;//取消线程锁.退出线程
	return 0;
}//}}}













