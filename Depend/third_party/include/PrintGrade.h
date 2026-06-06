
#ifndef __PRINTGRADE_H__
#define __PRINTGRADE_H__

#ifdef __cplusplus 
extern "C" 
{
#endif

/// 打印等级
typedef enum
{
	LEVEL_PRINT = 0, //IMPORT
	LEVEL_ERROR = 1,
	LEVEL_WARN = 2,
	LEVEL_INFO = 3,
	LEVEL_TRACE = 4, //白色
	LEVEL_DEBUG = 5,
	LEVEL_CLOUD = 6, //云端日志级别
}PRINT_LEVLE;

/// 模块名宏
#ifdef PRINT_MODULE_NAME 
#define PRINT_MODULE_NAME2	PRINT_MODULE_NAME
#else
#define PRINT_MODULE_NAME2  ""
#endif


/// 打印接口
/// 
/// \param [in] libName 模块名
/// \param [in] file 源文件名，__FILE__
/// \param [in] line 源文件行号，__LINE__
/// \param [in] nLevel 打印等级，参考PRINT_LEVLE
/// \param [in] fmt 打印格式，同printf
/// \retval 0  获取成功。
/// \retval <0  获取失败。
int Print(const char *libName, const char *file, int line, int nLevel, const char *fmt, ...);


/// 代替原有的printf，增加颜色显示和保存文件
/// 使用场景：
/// 1. 暂时只再控制台的输入输出交互上，其他模块不允许使用
#define PRINT(fmt, ...)				Print("", __FILE__, __LINE__, LEVEL_PRINT, fmt, ##__VA_ARGS__)

/// 严重错误打印
/// 使用场景：
/// 1. 出现的错误会导致程序无法运行，或者重要功能失效。
/// 2. 程序BUG导致不应该出现的状态或变量值。
#define PRINT_ERROR(fmt, ...)		Print(PRINT_MODULE_NAME2, __FILE__, __LINE__, LEVEL_ERROR , fmt, ##__VA_ARGS__)

/// 警告打印
/// 使用场景：
/// 1. 出现错误分支，例如网络连接失败、函数错误返回。
#define PRINT_WARN(fmt, ...)		Print(PRINT_MODULE_NAME2, __FILE__, __LINE__, LEVEL_WARN , fmt, ##__VA_ARGS__)


/// 信息打印
/// 使用场景：
/// 1. 模块或者类的信息打印
#define PRINT_INFO(fmt, ...)		Print(PRINT_MODULE_NAME2, __FILE__, __LINE__, LEVEL_INFO , fmt, ##__VA_ARGS__)


/// 流程跟踪打印
#define PRINT_TRACE(fmt, ...)		Print(PRINT_MODULE_NAME2, __FILE__, __LINE__, LEVEL_TRACE, fmt, ##__VA_ARGS__)

/// 调试打印
#define PRINT_DEBUG(fmt, ...)		Print(PRINT_MODULE_NAME2, __FILE__, __LINE__, LEVEL_DEBUG, fmt, ##__VA_ARGS__)

/// 云端日志打印
/// 使用场景：
/// 1. 需要上报到云端的关键错误或重要事件
/// 2. 用于远程诊断和问题分析
#define PRINT_CLOUD(fmt, ...)		Print(PRINT_MODULE_NAME2, __FILE__, __LINE__, LEVEL_CLOUD, fmt, ##__VA_ARGS__)




#ifdef __cplusplus
}
#endif

#endif	//__PRINTGRADE_H__










