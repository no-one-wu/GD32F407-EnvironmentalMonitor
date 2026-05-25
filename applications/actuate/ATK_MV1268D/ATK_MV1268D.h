/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-05-15     WJHcomputer       the first version
 */
#ifndef APPLICATIONS_ACTUATE_ATK_MV1268D_ATK_MV1268D_H_
#define APPLICATIONS_ACTUATE_ATK_MV1268D_ATK_MV1268D_H_

#ifdef __cplusplus
extern "C" {
#endif
void set_Photovoltaic_panel_mode(unsigned char ATK_MV1268D_address, unsigned charFeature);
void uart_thread_entry(void *parameter);
void ATK_MV1268D_Tx_Check(unsigned char ATK_MV1268D_address, unsigned charFeature);
void ATK_MV1268D_init(void);
void ATK_MV1268D_TxT_Check(unsigned char ATK_MV1268D_address, unsigned int charFeature,unsigned int charFeature2);

struct mv1268 {
    volatile char ID[3],setID[3];
    rt_tick_t TimID[3];
};
extern struct mv1268 mv1268Data;
#ifdef __cplusplus
}
#endif

#endif /* APPLICATIONS_ACTUATE_ATK_MV1268D_ATK_MV1268D_H_ */
