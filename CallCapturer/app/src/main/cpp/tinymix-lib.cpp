//
// Created by ivan on 13.07.2024.
//

#include "tinymix-lib.h"



static int g_tabs_only = 0;
static int g_all_values = 0;

 int isnumber(const char *str) {
    char *end;

    if (str == NULL || strlen(str) == 0)
        return 0;

    strtol(str, &end, 0);
    return strlen(end) == 0;
}

 void tinymix_list_controls(struct mixer *mixer)
{
    struct mixer_ctl *ctl;
    const char *name, *type;
    unsigned int num_ctls, num_values;
    unsigned int i;

    num_ctls = mixer_get_num_ctls(mixer);

    printf("Number of controls: %u\n", num_ctls);

    if (g_tabs_only)
        printf("ctl\ttype\tnum\tname\tvalue");
    else
        printf("ctl\ttype\tnum\t%-40s value\n", "name");
    if (g_all_values)
        printf("\trange/values\n");
    else
        printf("\n");
    for (i = 0; i < num_ctls; i++) {
        ctl = mixer_get_ctl(mixer, i);

        name = mixer_ctl_get_name(ctl);
        type = mixer_ctl_get_type_string(ctl);
        num_values = mixer_ctl_get_num_values(ctl);
        if (g_tabs_only)
            printf("%d\t%s\t%d\t%s\t", i, type, num_values, name);
        else
            printf("%d\t%s\t%d\t%-40s ", i, type, num_values, name);
        tinymix_detail_control(mixer, name, 0, g_all_values);
    }
}

 void tinymix_print_enum(struct mixer_ctl *ctl, const char *space,
                               int print_all)
{
    unsigned int num_enums;
    unsigned int i;
    const char *string;
    int control_value = mixer_ctl_get_value(ctl, 0);

    if (print_all) {
        num_enums = mixer_ctl_get_num_enums(ctl);
        for (i = 0; i < num_enums; i++) {
            string = mixer_ctl_get_enum_string(ctl, i);
            printf("%s%s%s",
                   control_value == (int)i ? ">" : "", string,
                   (i < num_enums - 1) ? space : "");
        }
    }
    else {
        string = mixer_ctl_get_enum_string(ctl, control_value);
        printf("%s", string);
    }
}

 int tinymix_detail_control(struct mixer *mixer, const char *control,
                                  int prefix, int print_all)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_values;
    unsigned int i;
    int min, max;
    int ret;
    char *buf = NULL;
    size_t len;
    unsigned int tlv_header_size = 0;
    const char *space = g_tabs_only ? "\t" : " ";

    if (isnumber(control))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control: %s\n", control);
        return ENOENT;
    }

    type = mixer_ctl_get_type(ctl);
    num_values = mixer_ctl_get_num_values(ctl);

    if (type == MIXER_CTL_TYPE_BYTE) {
        if (mixer_ctl_is_access_tlv_rw(ctl)) {
            tlv_header_size = TLV_HEADER_SIZE;
        }
        buf = (char*) calloc(1, num_values + tlv_header_size);
        if (buf == NULL) {
            fprintf(stderr, "Failed to alloc mem for bytes %d\n", num_values);
            return ENOENT;
        }

        len = num_values;
        ret = mixer_ctl_get_array(ctl, buf, len + tlv_header_size);
        if (ret < 0) {
            fprintf(stderr, "Failed to mixer_ctl_get_array\n");
            free(buf);
            return ENOENT;
        }
    }

    if (prefix)
        printf("%s:%s", mixer_ctl_get_name(ctl), space);

    for (i = 0; i < num_values; i++) {
        switch (type)
        {
            case MIXER_CTL_TYPE_INT:
                printf("%d", mixer_ctl_get_value(ctl, i));
                break;
            case MIXER_CTL_TYPE_BOOL:
                printf("%s", mixer_ctl_get_value(ctl, i) ? "On" : "Off");
                break;
            case MIXER_CTL_TYPE_ENUM:
                tinymix_print_enum(ctl, space, print_all);
                break;
            case MIXER_CTL_TYPE_BYTE:
                /* skip printing TLV header if exists */
                printf(" %02x", buf[i + tlv_header_size]);
                break;
            default:
                printf("unknown");
                break;
        }

        if (i < num_values - 1)
            printf("%s", space);
    }

    if (print_all) {
        if (type == MIXER_CTL_TYPE_INT) {
            min = mixer_ctl_get_range_min(ctl);
            max = mixer_ctl_get_range_max(ctl);
            printf("%s(dsrange %d->%d)", space, min, max);
        }
    }

    free(buf);

    printf("\n");
    return 0;
}

 void tinymix_set_byte_ctl(struct mixer_ctl *ctl,
                                 char **values, unsigned int num_values)
{
    int ret;
    char *buf;
    char *end;
    unsigned int i;
    long n;
    unsigned int *tlv, tlv_size;
    unsigned int tlv_header_size = 0;

    if (mixer_ctl_is_access_tlv_rw(ctl)) {
        tlv_header_size = TLV_HEADER_SIZE;
    }

    tlv_size = num_values + tlv_header_size;

    buf = (char*) calloc(1, tlv_size);
    if (buf == NULL) {
        fprintf(stderr, "set_byte_ctl: Failed to alloc mem for bytes %d\n", num_values);
        exit(EXIT_FAILURE);
    }

    tlv = (unsigned int *)buf;
    tlv[0] = 0;
    tlv[1] = num_values;

    for (i = 0; i < num_values; i++) {
        errno = 0;
        n = strtol(values[i], &end, 0);
        if (*end) {
            fprintf(stderr, "%s not an integer\n", values[i]);
            goto fail;
        }
        if (errno) {
            fprintf(stderr, "strtol: %s: %s\n", values[i],
                    strerror(errno));
            goto fail;
        }
        if (n < 0 || n > 0xff) {
            fprintf(stderr, "%s should be between [0, 0xff]\n",
                    values[i]);
            goto fail;
        }
        /* start filling after the TLV header */
        buf[i + tlv_header_size] = n;
    }

    ret = mixer_ctl_set_array(ctl, buf, tlv_size);
    if (ret < 0) {
        fprintf(stderr, "Failed to set binary control\n");
        goto fail;
    }

    free(buf);
    return;

    fail:
    free(buf);
    exit(EXIT_FAILURE);
}

int tinymix_set_value(struct mixer *mixer, const char *control,
                             char **values, unsigned int num_values)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_ctl_values;
    unsigned int i;

    if (isnumber(control))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control: %s\n", control);
        return ENOENT;
    }

    type = mixer_ctl_get_type(ctl);
    num_ctl_values = mixer_ctl_get_num_values(ctl);

    if (type == MIXER_CTL_TYPE_BYTE) {
        tinymix_set_byte_ctl(ctl, values, num_values);
        return ENOENT;
    }

    if (isnumber(values[0])) {
        if (num_values == 1) {
            /* Set all values the same */
            int value = atoi(values[0]);

            for (i = 0; i < num_ctl_values; i++) {
                if (mixer_ctl_set_value(ctl, i, value)) {
                    fprintf(stderr, "Error: invalid value\n");
                    return EINVAL;
                }
            }
        } else {
            /* Set multiple values */
            if (num_values > num_ctl_values) {
                fprintf(stderr,
                        "Error: %u values given, but control only takes %u\n",
                        num_values, num_ctl_values);
                return EINVAL;
            }
            for (i = 0; i < num_values; i++) {
                if (mixer_ctl_set_value(ctl, i, atoi(values[i]))) {
                    fprintf(stderr, "Error: invalid value for index %d\n", i);
                    return EINVAL;
                }
            }
        }
    } else {
        if (type == MIXER_CTL_TYPE_ENUM) {
            if (num_values != 1) {
                fprintf(stderr, "Enclose strings in quotes and try again\n");
                return EINVAL;
            }
            if (mixer_ctl_set_enum_by_string(ctl, values[0])) {
                fprintf(stderr, "Error: invalid enum value\n");
                return EINVAL;
            }
        } else {
            fprintf(stderr, "Error: only enum types can be set with strings\n");
            return EINVAL;
        }
    }

    return 0;
}

