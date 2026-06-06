
#ifndef __PACKET_PIPELINE_API_INC__
#define __PACKET_PIPELINE_API_INC__

#include "memory_api.h"

#ifdef __cplusplus
extern "C" {
#endif


	enum
	{
		PACKET_USE_LOCAL_CAPTURE = 0,
		PACKET_USE_MIXED_CAPTURE,
		PACKET_USE_EX_CAPTURE,
		PACKET_USE_AUDIO_IN,
	};

	struct PIPELINE_ITEM
	{
		HMEMORY2 hMemory;
		unsigned char chn;		//通道
		char stream_type;		//码流
		char use;				//用途
		char reserve;			//备用
	};

	/*
	把数据通过管道传给应用，如果管道满则堵塞
	*/
	void PipelinePush(PIPELINE_ITEM* item);

	/*
	获取管道数据，如果没有数据，堵塞。只能单个线程调用
	*/
	void PipelinePop(PIPELINE_ITEM* item);

	/*
	获取管道的当前长度和最大长度
	*/
	unsigned int PipelineSize();
	unsigned int PipelineMaxSize();


#ifdef __cplusplus
}
#endif

#endif //__PACKET_PIPELINE_API_INC__