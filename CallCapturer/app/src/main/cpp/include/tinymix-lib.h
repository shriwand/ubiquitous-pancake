//
// Created by ivan on 13.07.2024.
//

#pragma once


#include <tinyalsa/asoundlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif

void tinymix_list_controls(struct mixer *mixer);

int tinymix_detail_control(struct mixer *mixer, const char *control,
                           int prefix, int print_all);

int tinymix_set_value(struct mixer *mixer, const char *control,
                      char **values, unsigned int num_values);

void tinymix_print_enum(struct mixer_ctl *ctl, const char *space,
                        int print_all);

#ifdef __cplusplus
}
#endif