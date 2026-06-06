
#ifndef __PACKET_MEMORY_API_INC__
#define __PACKET_MEMORY_API_INC__

#ifdef __cplusplus
extern "C" {
#endif

	/*
	规则制定：
	每个HMEMORY2（CPacketList）,存放的是完整的帧。可以存1帧、2帧...N帧
	音频帧不能跨块，即使大于1024。
	*/

	typedef void* HMEMORY2;

	enum
	{
		P_FRAME = 0,
		I_FRAME =1,
		B_FRAME =2,
		AUDIO_FRAME =3,
		JPEG_FRAME = 4,
		WATER_FRAME =5,
		MOTION_FRAME = 6,
		MJPEG_FRAME = 7,
		UNDEFINE_FRAME = 8,
		METADATA_FRAME = 9,
		IVS_ASSIST_FRAME = 10,//智能分析帧
	};

	enum
	{
		NO_HEAD_TAIL = 0,
		HEAD,
		TAIL,
		HEAD_AND_TAIL,
	};

	/*
	申请内存块
	nBytes：内存块大小
	bContinue：申请连续的内存块，如果为FALSE，块会不连续，需要通过Memory2Next获取下一块数据。视频流要用非连续块。如果需要用连续的内存块，bContinue=TRUE。
			bContinue=TRUE对应应用的CPacketList，FALSE对应CPacket
			音频帧bContinue必须等于TRUE
	bSlab：除了媒体流，其他的都不用SLAB
	ret：返回内存块句柄，失败的话返回NULL
	*/
	HMEMORY2 Memory2Alloc(unsigned int nBytes, bool bContinue, bool bSlab);

	/*
	写数据，适合连续数据的写操作。写完成要调用Memory2FinishWrite，完善各个块的帧信息
	内部不会做越界检测，需要外部确保写的数据不会超出申请的空间
	Memory2WriteFinish：写完帧数据后调用，完善CPacketList里的帧信息。如果不是写帧数据，不需要调用这个接口
	Memory2Clea：清空前面写的数据，恢复成初始状态
	*/
	void Memory2Write(HMEMORY2 hMemory, void* pData, unsigned int nBytes);
	void Memory2WriteFinish(HMEMORY2 hMemory, int type);
	void Memory2Clear(HMEMORY2 hMemory);

	/*
	读缓冲里面的数据，如果Memory2Alloc时bContinue==false，就要考虑到缓存可能有多个块，需要调用Memory2Next遍历。
	Memory2Next遍历到结尾返回NULL。Memory2Next回放的句柄除了调用Memory2GetBuffer，不能进行其他操作。
	Memory2GetBuffer：获取这个块对应的内存指针和长度。
	*/
	HMEMORY2 Memory2Next(HMEMORY2 hMemory);
	void* Memory2GetBuffer(HMEMORY2 hMemory);
	unsigned int Memory2GetDataLength(HMEMORY2 hMemory);

	/*
	扩展数据块的使用接口，现有缓冲104个字节，参照老的大小。老的扩展块和头部混合使用，现在分开来。
	Memory2ClearExpandBuf:104个字节全部清0
	*/
	void* Memory2ExpandBuf(HMEMORY2 hMemory);
	unsigned int Memory2ExpandBufLen(HMEMORY2 hMemory);
	void  Memory2ClearExpandBuf(HMEMORY2 hMemory);

	/*
	hAppend如果数据为空，为缓冲区合并，hAppend里的缓冲加到hMemory结尾。
	hAppend如果有数据，为帧合并，hMemory放了两帧。视频和音频帧合并到同一个HMEMORY2，然后反入队列，可以减少上层传递的数量。
	*/
	void Memory2Append(HMEMORY2 hMemory, HMEMORY2 hAppend);

	/*
	引用索引。Memory2Alloc返回的引用索引为1，引用索引降低到0的时候，释放内存块。
	*/
	void Memory2AddRef(HMEMORY2 hMemory);
	void Memory2Release(HMEMORY2 hMemory);


#ifdef __cplusplus
}
#endif

#endif //__PACKET_MEMORY_API_INC__