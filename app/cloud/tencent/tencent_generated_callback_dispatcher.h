#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*aov_tencent_property_callback)(const char* property_id,
                                              const char* value,
                                              uint64_t cloud_version,
                                              uint64_t recv_ts_ms,
                                              void* user);

typedef void (*aov_tencent_action_callback)(const char* action_id,
                                            const char* params,
                                            const char* request_id,
                                            uint64_t recv_ts_ms,
                                            void* user);

int aov_tencent_generated_push_property(const char* property_id,
                                        const char* value,
                                        uint64_t cloud_version,
                                        uint64_t recv_ts_ms);

int aov_tencent_generated_push_action(const char* action_id,
                                      const char* params,
                                      const char* request_id,
                                      uint64_t recv_ts_ms);

int aov_tencent_generated_drain(aov_tencent_property_callback property_callback,
                                aov_tencent_action_callback action_callback,
                                void* user);

void aov_tencent_generated_clear(void);

#ifdef __cplusplus
}
#endif
