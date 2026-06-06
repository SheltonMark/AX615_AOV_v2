 //
//  "$Id: dhfs.h 5995 2009-06-09 01:06:42Z yao_guoqin $"
//
//  Copyright (c)1992-2007, ZheJiang Dahua Technology Stock CO.LTD.
//  All Rights Reserved.
//
//	Description:	
//	Revisions:		Year-Month-Day  SVN-Author  Modification
//

#ifndef __DHFS_H_
#define __DHFS_H_

#include "APIs/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

// 大华文件系统句柄
typedef int DHFILE;

//外部调用错误号
#define FS_NO_ERROR				0			//无错误
#define FS_OK					FS_NO_ERROR	//无错误
#define FS_NO_HANLE				1			//没有句柄
#define FS_EOF					2			//文件结束
#define FS_READ_ERROR			3			//底层读错误
#define FS_WRITE_ERROR			4			//底层写错误
#define FS_NO_SPACE				5			//没有空间
#define FS_NO_EXIST_FILE		6			//文件不存在
#define FS_ERROR				7			//没有硬盘
#define FS_OVERLAY				8			//单个硬盘或硬盘之间时间冲突
#define FS_MUST_REINIT			9			//必须从新初始化
#define FS_UD_NOT_VALID			10			//用户数据无效
#define FS_INVALID_HANDLE		11			//句柄无效
#define FS_INVALID_FILENAME		12			//文件名无效
#define FS_ALREADY_EXISTS		13			//文件已经存在
#define FS_TIME_ERROR			14			//文件系统最后时间与当前时间冲突
#define FS_BAD_DISK				15			//有坏盘
#define FS_NO_EXIST_ALARM		16			//查不到报警
#define FS_NO_RELATED_VIDEO		17			//无关联视频

#define FS_DISK_HOST_ERROR		18			//硬盘控制器错误 ZhengYi 2012-8-23
#define FS_DISK_SECTOR_ERROR    19			//硬盘扇区错误 ZhengYi 2012-8-23
#define FS_DISK_DATA_ERROR		20			//硬盘数据错误ZhengYi 2014-10-14

//读文件方式
#define FILE_READ_SINGLE		1			//读单个文件

//驱动器类型
#define DRIVER_TYPE_BASE		0			//驱动器类型起始值
#define DRIVER_READ_WRITE		0			//读写驱动器
#define DRIVER_READ_ONLY		1			//只读驱动器
#define DRIVER_BACKUP 			2			//事件驱动器
#define DRIVER_REDUNDANT		3			//冗余驱动器
#define DRIVER_SNAPSHOT			4			//快照驱动器
#define DRIVER_TYPE_NR			5			//驱动器类型个数
#define DRIVER_UNUSED			0xff		//没有使用的驱动器结构

//文件类型， 共256种
#define F_COMMON				0x00		//普通录像
#define F_ALERT					0x01		//外部报警
#define F_DYNAMIC				0x02		//动态检测
#define F_CARD					0x03		//卡号录像
#define F_HAND					0x04		//手动录像
#define F_JPG					0x05		//图片

//文件访问类型
#define FILE_ACCESS_READ_ONLY	0		//文件访问方式(只读)
#define FILE_ACCESS_NORMAL		1		//文件访问方式(普通)

//全部通道
#define ALL_CHANNELS			8192
//全部报警
#define ALL_ALARM_TYPE			255

typedef enum
{
	INITED = 0,
	NOT_FAT32,
	CHECK_ERROR,
	DEVICE_UNUSABLE,
	MOUNT_FAILED,
	UNMOUNT_FAILED,
	AVAILABLE_CAPACITY_LITTLE,
	FILE_LOST,
	FORMAT_FAILED,
    FALLOCATE_FAILED,
}INIT_STATE;

typedef struct _PARTITION_INFO
{
	uint64 start_sector;
	uint64 total_sector;
}PARTITION_INFO;


//驱动器信息结构
typedef struct _DRIVER_INFO{
	uint	driver_type;		//驱动器类型
	uint	is_current;			//是否为当前工作盘
	uint	section_count;		//时间段数
	DHTIME  start_time1;		//录像时间段1开始时间
	DHTIME	end_time1;			//录像时间段1结束时间
	uint	two_part;			//是否有第二段
	DHTIME	start_time2;		//录像时间段2开始时间
	DHTIME	end_time2;			//录像时间段2结束时间
	uint	total_space;		//总容量，MB为单位
	uint	remain_space;		//剩余容量，MB为单位
	uint	error_flag;			//错误标志，文件系统初始化时被设置
	uint	index;				//硬盘物理序号
}DRIVER_INFO,*pDRIVER_INFO;

//驱动器时间段信息
typedef struct _DRIVER_SECTION{
	DHTIME start_time;			//开始时间
	DHTIME end_time;			//结束时间
}DRIVER_SECTION,*pDRIVER_SECTION;

//文件系统信息结构
typedef struct _SYS_USE_INFO
{
	uchar	drive_num;					//工作驱动器数量
	uchar	work_drive ;				//当前工作驱动器所在盘
	uchar   not_work_num;				//非工作驱动器数量
	uchar   not_work_driver;			//当前非工作驱动器所在盘
	uchar   part_num;	                //分区数
	uint	drive_space;				//工作驱动器总共空间,MB为单位
	uint	RemainSpace ;				//硬盘保留空间,MB为单位
	uint	drive_free_space;			//工作驱动器剩余空间,MB为单位

	DHTIME	start_time;					//开始录像时间
	DHTIME	end_time;					//结束录像时间
	uint	cur_work_free_space;		//当前工作驱动器剩余空间(盘不满),MB为单位
	uint	cur_work_overlay_space;		//当前工作驱动器剩余空间(盘己满，循环录),MB为单位
	uint	not_work_space;				//非工作驱动器总共空间,MB为单位
	uint	not_work_free_space;		//非工作驱动器剩余空间,MB为单位
	uint	error_space;				//坏空间,MB为单位
}SYS_USE_INFO,*pSYS_USE_INFO;

//增加文件访问模式标记 ZhengYi 2011-9-20
//将channel的类型修改为ushort ZhengYi 2011-9-2
//文件信息结构,本地使用
typedef struct _FILE_INFO
{
	ushort		channel;				//通道号
	uchar		alarm;					//报警类型
	uchar		rev;					//保留
	uchar		video_audio;			//视频或音频
	uchar		image;					//图像质量
	uchar		hint;					//查找文件时使用的暗示参数
	DHTIME		start_time ;			//开始时间
	DHTIME		end_time ;				//结束时间
	uint		file_length ;			//文件长度，单位为KB
	uint		disk_no;				//所在的硬盘号
	uint		disk_part;				//所在的分区
	uint		first_clus_no ;			//文件首簇号
	uint		ud_no;					//用户数据号
	char		base_name[24];			//文件名
	char		ext_name[4];			//扩展名
	uint		checked;				//是否选中，用于文件备份
	uchar		access_mode;			//文件访问类型（只读、非只读）
}FILE_INFO,*pFILE_INFO;

//只读索引文件结构信息 ZhengYi 2011-9-15
typedef struct _INDEX_FILE_INFO
{
	ushort		channel;				//通道号
	ushort		time_length;			//文件时间长度(单位:秒)
	DHTIME		start_time;				//开始时间
	uint		start_cluster_no;		//只读文件首簇
	uint		item_id;				//只读文件id
	uint		driver_no;				//驱动器编号
	uchar		flag;					//索引项标志
	
}INDEX_FILE_INFO, *pINDEX_FILE_INFO;

//智能报警信息结构 ZhengYi 2011-11-18 增加报警项号
typedef struct _ALARM_INDEX_INFO
{
	uchar		flag;				//标志字段
	uchar		alarm_type;			//报警类型
	ushort		channel;			//通道号
	DHTIME		alarm_time;			//报警产生时间
	uint		video_clus_no;		//报警关联视频的簇号
	uint		data_clus_no;		//附加数据所在簇号
	uint		data_clus_offset;	//附加数据簇内偏移
	uint		data_length;		//附加数据长度（扇区）
	uint		alarm_id;			//报警id
	uint		driver_no;			//报警所在驱动器编号
}ALARM_INDEX_INFO, *pALARM_INDEX_INFO;

// 将channel的类型修改为ushort ZhengYi 2011-9-2
//用户数据结构
typedef struct _USER_DATA{
	union{
		uchar b_ud[8];
		ushort w_ud[4];
		uint dw_ud[2];
	}ud8;
	uchar b8;
	uchar b9;
	ushort channel;
	uchar flag;  //loop num
}USER_DATA,*pUSER_DATA;

typedef struct _DISK_ERROR_INFO64
{
	uint readmask;		//读错误
	uint writemask;		//写错误
	uint64 offset[32];	//错误偏移
	uint removemask;	//已移除的硬盘掩码 WHF 2006-10-4
	uchar errortype[32];	//硬盘错误类型	ZhengYi 2012-8-23
}DISK_ERROR_INFO64, *pDISK_ERROR_INFO64;

//自定义比特数组，存放通道掩码
typedef struct _BITARRAY
{
	uchar element[32];

}BITARRAY, *pBITARRAY;


//智能报警高级检索条件结构体 ZhengYi 2013-03-04
typedef struct _ALARM_QUERY_CONDITION
{
	uint*		p_alarm_types;			//指向存放实际需要检索的报警类型编号的数组
	pBITARRAY	p_query_conditions;		//指向检索数据的数组
	uint		alarm_type_size;		//报警类型编号数组的大小
	uint		condition_size;			//检索数据数组的大小
	DHTIME		start_time;				//检索开始时间
	DHTIME		end_time;				//检索结束时间

}ALARM_QUERY_CONDITION;

typedef struct FS_IOVEC_t
{
 void  *iov_base;			/* Starting address */
 unsigned int iov_len;      /* Number of bytes to transfer */
}FS_IOVEC;

typedef struct _DRIVER_MT_HOOK		//多任务有关的回调函数
{
	void (*wakup)(uint disk_no);	//唤醒回调函数
	void (*lock)();					//锁回调函数
	void (*unlock)();				//解锁回调函数
}DRIVER_MT_HOOK;

typedef int (*RECOVER_HOOK)(uchar *, uint *, int);

#define UD_BCD_NR 20 //用户数据结构能表示的BCD数字位数

//公共缓冲使用
void dev_set_opt(int dev_type, int (*read_opt)(ushort ,ushort ,uint64 ,void * ), int (*write_opt)(ushort ,ushort ,uint64 ,void * ));
int buffer_dev_init(uint dev);

//时间结构操作
void clear_time(DHTIME *ptime);
uint iszero_time(DHTIME t);
int compare_time(DHTIME t1,DHTIME t2);//t1>t2,返回1，t1<t2,返回-1，相等返回0；容易用错，特加注释BY LXQ 2016-07-07
uint is_leap_year(int year);
int time_to_day(DHTIME t);
int time_to_second(DHTIME t);
DHTIME second_to_time(int second);
int changeto_second(DHTIME end_time, DHTIME start_time);
int compare_in_time(DHTIME t,DHTIME t1,DHTIME t2);
void display_time(DHTIME t);
DHTIME add_time(DHTIME time, int second);

//文件系统操作
int dh_file_sys_init(uint *pwork_num, DHTIME *shutdown_time);
uint dh_get_fs_info(pSYS_USE_INFO pinfo);
void dh_get_kit_area64(uint disk_no,uint part,uint64 *offset,uint *size);
void dh_get_log_area64(uint disk_no,uint part,uint64 *offset,uint *size);
void dh_get_audio_area(uint disk_no, uint part, uint64 *offset, uint *size);	//音频提示文件存放区操作接口 ZhengYi 2012-4-28
uint dh_disk_full(uint driver_type);
uint dh_file_set_max_len(uint second);
uint dh_file_set_hold_time(int second);											//设置文件保留时间
void dh_set_recover_hook(RECOVER_HOOK hook);
int  dh_fs_init();
int  dh_fs_mount(uint disk_no, uint part);
int  dh_fs_umount(uint disk_no, uint part);
void dh_array_driver();															// 重新排列驱动器
INIT_STATE dh_get_file_init_state(uint disk_no,uint part);
uint dh_format_file(uint disk_no,uint part,uint* progress);
uint dh_check_index_file(uint disk_no,uint part,float* rec_del_rate,float* ala_del_rate);
uint dh_insert_storage_device();
uint dh_remove_storage_device();
uint dh_free_mem();

//硬盘分区接口
int  dh_read_partitions(uint disk_no,PARTITION_INFO* info, uint num);
int  dh_write_partitions(uint disk_no,PARTITION_INFO* info, uint num);

//分区格式化接口
uint dh_format_disk(uint disk_no,uint* progress);
uint dh_format_driver(uint disk_no, uint part, PARTITION_INFO* info, uint num ,uint sec_per_clus, uint* progress);

//底层操作接口
uint dh_get_driver_type(uint disk_no,uint part,uint *pdriver_type);
uint dh_set_driver_type(uint disk_no,uint part,uint driver_type);
uint dh_get_driver_info(uint disk_no,uint part,DRIVER_INFO *pdriver_info);
uint dh_get_driver_section(uint disk_no,uint part, uint index, DRIVER_SECTION *pdriver_section);
void dh_wakeup_curr_driver();
void dh_wakeup_driver(uint disk_no);
void dh_set_driver_standy(uint disk_no, int minutes);
uint dh_scan_disk64(DISK_ERROR_INFO64 *pError);
void dh_set_driver_mt_hook(DRIVER_MT_HOOK *hook);
void dh_set_driver_standy_multi(int minutes);
uint dh_parole_driver(uint disk_no,uint part);

//文件常规操作
uint dh_file_open(DHFILE* p_handle,uchar read_mode,uint sys_use);
uint dh_file_open_near(DHFILE handle,ushort channel,DHFILE *p_handle);
uint dh_file_sync(DHFILE handle,ushort channel,DHFILE handle1);
uint dh_file_create(ushort channel,uchar video,uchar alarm,uchar image, DHTIME time, uint redu, uint driver_type, DHFILE *p_handle);
uint dh_file_read(DHFILE handle,uchar *pbuf,uint sec_num);
uint dh_file_write(DHFILE handle,uchar *pbuf, uint sec_num, uint changeFile, DHTIME* time);
uint dh_file_close(DHFILE handle,DHTIME* time);
uint dh_file_close_ex(DHFILE handle,DHTIME end_time);		//针对NVR需求新增关闭文件接口 2014-04-21
uint dh_file_close_all();
uint dh_file_break(DHFILE handle);

//文件查询与定位
uint dh_file_list(ushort channel,DHTIME start_time,DHTIME end_time,uint *pmax_ret_num,pFILE_INFO pfile_info,uchar type, uchar hint, uint driver_type);
uint dh_file_goto_prev(DHFILE handle, uint loop, uchar type, uint driver_type);
uint dh_file_goto_next(DHFILE handle, uint loop, uchar type, uint driver_type);
uint dh_file_goto_last(DHFILE handle, uchar type, uint driver_type);
uint dh_file_goto_chan_last(DHFILE handle, ushort channel, uchar type, uint driver_type);
uint dh_file_goto_first(DHFILE handle, uchar type, uint driver_type);
uint dh_file_goto(DHFILE handle,ushort channel,DHTIME time, uchar type, uint driver_type);
uint dh_file_goto_list(DHFILE handle,pFILE_INFO pfile_list);
uint dh_file_goto_clus(DHFILE handle,uint disk, uint part, uint clus);
uint dh_file_moveto_len(DHFILE handle,uint len);
uint dh_file_moveto_time(DHFILE handle,DHTIME time);
uint dh_file_move_second(DHFILE handle,uint bback,uint second);
uint dh_file_move_size(DHFILE handle,uint bback,uint sector);

//文件详细信息
uint dh_get_file_info(DHFILE handle,ushort *channel,uchar *alarm, uchar *image_quality,uchar *video_audio);
uint dh_get_file_info2(DHFILE handle,DHTIME * pstart_time,DHTIME * pend_time,uint *plen);
uint dh_get_cur_time(DHFILE handle,DHTIME *pstart_time,uint *ms,uint *curr_offset);
uint dh_get_clus_size(DHFILE handle,uint *clus_size);
uint dh_get_file_pos(DHFILE handle,uint *pposition);

//用户数据操作
uint dh_set_file_ud(DHFILE handle,pUSER_DATA pud);
uint dh_get_file_ud(DHFILE handle,pUSER_DATA pud);
uint dh_get_file_ud_ex(DHFILE handle,pUSER_DATA pud);
uint dh_file_ud_list_ex(DHTIME start_time, DHTIME end_time,pUSER_DATA pud,uchar number, uint *pmax_ret_num,pFILE_INFO pfile_info);
// 控制台接口
int  dh_console(int argc, char * argv[]);

/**********************************只读文件操作接口 ZhengYi 2012-2-8*****************************************/

//根据通道、时间等设置文件为只读
uint dh_file_set_read_only(ushort channel, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num, uchar type, uchar hint, uint driver_type);
//根据通道、时间等设置文件为普通
uint dh_file_set_normal(ushort channel, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num);
//根据只读索引文件设置文件为普通
uint dh_file_set_normal_by_item(pINDEX_FILE_INFO pIndex_file_info);

//根据编号和驱动器编号查询单个只读文件
uint dh_get_read_only_file_by_id(uint item_no, uint driver_no, pINDEX_FILE_INFO pIndex_file_info);	
//根据时间和通道查询单个只读文件
uint dh_get_read_only_file_by_time(ushort channel, DHTIME time, pINDEX_FILE_INFO pIndex_file_info);
//查询时间段内的只读文件列表(查找所有驱动器)
uint dh_get_read_only_file_list(ushort channel, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num, pINDEX_FILE_INFO pIndex_file_info);
//定位到只读文件的接口
uint dh_goto_read_only_file(DHFILE handle, pINDEX_FILE_INFO pIndex_file_info);
/**********************************智能报警操作接口 ZhengYi 2012-2-8****************************************/

//智能报警写入接口
uint dh_write_alarm(DHFILE handle, uchar flag, uchar alarm_type, DHTIME alarm_time, uchar *pbuf, uint data_length);
//智能报警聚集写接口
uint dh_write_alarm_gather(DHFILE handle, uchar flag, uchar alarm_type, DHTIME alarm_time, FS_IOVEC* iov, uint iov_num);
//智能报警查询接口
uint dh_get_alarm(ushort channel, DHTIME time, uchar alarm_type, pALARM_INDEX_INFO palarm_index_info, uint driver_type);
//查询智能报警列表
uint dh_get_alarm_list(ushort channel, uchar alarm_type, DHTIME start_time, DHTIME end_time, uint *pmax_ret_num, pALARM_INDEX_INFO palarm_index_info, uint driver_type);
//根据报警ID查询报警信息
uint dh_get_alarm_by_id(uint alarm_id, uint driver_no, pALARM_INDEX_INFO palarm_index_info);
//根据视频文件查询出相关报警的列表
uint dh_get_related_alarm(DHFILE handle, uint *pmax_ret_num, pALARM_INDEX_INFO palarm_index_info);
//根据报警ID读取附加数据
uint dh_read_alarm_data_by_id(uint alarm_id, uint driver_no, uchar *pbuf, uint length);
//读出智能报警附加数据
uint dh_read_alarm_data(pALARM_INDEX_INFO palarm_index_info, uchar *pbuf, uint length);
//定位到报警对应的视频文件并同时定位到回放时间点
uint dh_goto_alarm_video_file(DHFILE handle, uint second, pALARM_INDEX_INFO palarm_index_info);
//智能报警列表高级检索接口
uint dh_get_alarm_list_advanced(ALARM_QUERY_CONDITION aqc, uint *pmax_ret_num, pALARM_INDEX_INFO palarm_index_info, uint driver_type);
//获取高级检索条件下报警总数
uint dh_get_alarm_numbers(ALARM_QUERY_CONDITION aqc, uint *pret_num, uint driver_type);

//设置硬盘出错信息 ZhengYi 2012-8-24
uint dh_set_disk_error(DISK_ERROR_INFO64 *pError);
void dh_break_restore_data(uint need);

//新增多缓存聚集写数据的接口 ZhengYi 2015-05-19
uint dh_file_multi_write(DHFILE handle, FS_IOVEC* iov, uint iov_num, uint sec_num, DHTIME* time);
uint dh_get_fallocate_file_left_len(DHFILE handle,uint64* len);

#ifdef __cplusplus
}
#endif

#endif //__DHFS_H_

