任务1：调试视频编码参数设置接口（帧率、码率、分辨率、QP等）
实施计划
梳理现有接口
检查libmedia已有的参数设置接口（SetBitrate, SetFrameRate等）
对比SDK接口（AX_VENC_SetRcParam, AX_VENC_SetChnAttr等）
确认哪些参数可以动态修改，哪些只能创建时设置
实现/完善接口
帧率设置：SetFrameRate(chn_id, fps) → AX_VENC_SetChnAttr
码率设置：SetBitrate(chn_id, kbps) → AX_VENC_SetRcParam
分辨率设置：需要重建通道（Stop → Destroy → Create → Start）
QP设置：SetQpRange(chn_id, min_qp, max_qp) → AX_VENC_SetRcParam
编写测试代码
在libmedia_demo中添加参数调整测试
运行时动态修改参数
抓取码流验证参数生效
验收标准
 帧率调整：能从30fps动态切换到15fps/10fps，码流metadata显示正确帧率
 码率调整：能从2Mbps动态调整到1Mbps/4Mbps，实际码率在目标±10%范围内
 分辨率调整：能从1920x1080切换到1280x720，码流解码后分辨率正确
 QP调整：能设置QP范围（如25-35），编码后I/P帧QP在设置范围内
 无crash：所有参数调整过程不崩溃，不丢帧超过2帧
 日志完整：每次参数修改有日志输出，显示旧值→新值
任务2：调试智能检测模块（SKEL接口）
实施计划
理解SKEL模块
阅读SDK sample中SKEL相关代码（sample_skel或sample_vin_ivps_skel）
了解SKEL初始化流程：AX_SKEL_Init → Create → Start
了解数据流：IVPS输出 → SKEL检测 → 结果回调
检查libmedia现有实现
查看detect_service相关代码
确认SKEL适配器实现情况（ax_skel_adapter.cpp）
检查是否正确加载AI模型
集成测试
启动IVPS → SKEL pipeline
验证检测结果回调
OSD叠加检测框（如果需要）
验收标准
 模块初始化：SKEL模块能正常Init、Create、Start，无错误日志
 模型加载：AI检测模型（人形/人脸/车辆等）加载成功
 检测功能：能检测到目标物体（人/车），回调函数正常触发
 检测精度：检测框位置准确（与实际目标位置偏差<5%）
 性能达标：检测延迟<100ms，CPU占用<30%
 结果输出：检测结果JSON/结构体包含：目标类别、坐标、置信度
 稳定运行：连续运行30分钟不crash，检测率稳定
任务3：调试AOV休眠唤醒流程
实施计划
研究AOV机制
阅读SDK文档中AOV（Always-On Vision）说明
理解低功耗模式：正常模式 ↔ AOV模式切换
确认唤醒条件：运动检测/人形检测/定时唤醒
实现状态机
正常模式：全pipeline运行（30fps高质量编码）
AOV模式：低功耗模式（降帧率/降分辨率，检测运行）
唤醒逻辑：检测到目标 → 切换回正常模式
Pipeline动态切换
休眠流程：降低VENC帧率 → 停止部分模块 → 进入检测循环
唤醒流程：恢复模块 → 恢复VENC参数 → 正常录像
处理切换过程中的同步问题
验收标准
 进入休眠：无检测目标5秒后自动进入AOV模式
 功耗降低：AOV模式功耗比正常模式降低>50%
 检测保持：休眠状态下检测功能仍在运行（1-5fps检测帧率）
 快速唤醒：检测到目标后1秒内唤醒，恢复正常录像
 无丢帧：唤醒过程前后码流连续，时间戳无跳变
 稳定切换：休眠↔唤醒循环50次不crash
 事件上报：休眠/唤醒事件能通过回调通知上层应用
任务4：调试音频模块
实施计划
音频采集调试
检查audio_capture适配器（ax_audio_adapter.cpp）
验证ACODEC/I2S配置：采样率、位深、声道数
测试AI模块：AX_AI_EnableChn → AX_AI_GetFrame
音频编码调试
验证音频编码器配置（AAC/G711等）
测试AENC模块：创建编码通道 → 送入PCM → 获取码流
音视频时间戳同步
音频播放调试
验证AO模块配置
测试音频播放：解码 → AX_AO_SendFrame → 播放输出
验收标准
 采集功能：能正常采集音频，PCM数据无杂音、无明显噪声
 编码功能：音频能正常编码为AAC/G711，码流文件可被ffplay播放
 音质达标：播放音频清晰，无破音、无卡顿、无明显失真
 音视频同步：视频和音频时间戳差异<50ms，播放不出现AV不同步
 采样参数：支持常见配置（16kHz/48kHz采样率，16bit，单/双声道）
 播放功能：能播放音频文件，音量可调，播放流畅
 长时间稳定：音频采集编码连续运行2小时无异常
总体时间规划建议
任务	预计工作量	优先级
任务1：视频编码参数	2-3天	P0（基础功能）
任务2：智能检测SKEL	3-4天	P0（核心功能）
任务3：AOV休眠唤醒	4-5天	P1（高级功能）
任务4：音频模块	3-4天	P1（完整性）
建议按优先级顺序逐个完成，每个任务完成后进行充分测试再进入下一个。