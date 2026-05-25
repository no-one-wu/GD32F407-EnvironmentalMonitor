/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-03-29     wcj       the first version
 */
#ifndef APPLICATIONS_ACTUATE_ESP_12F_WIFI_H_
#define APPLICATIONS_ACTUATE_ESP_12F_WIFI_H_

void esp12f_thread_entry(void *parameter);
void wifi_init(void);
void wifi_tx_check(unsigned char address, unsigned int data1, unsigned int data2);


#endif /* APPLICATIONS_ACTUATE_ESP_12F_WIFI_H_ */
