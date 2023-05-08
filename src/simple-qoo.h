#ifndef _SIMPLEQOO_H
#define _SIMPLEQOO_H

#include <time.h>
#include "tdigest.h"
#include "network-reqs.h"
#include "udpst_common.h"

struct simple_qoo_statistics
{
    int number_of_samples, number_of_lost_packets;
    // Use timespec for the exact measurements:
    struct timespec min, max, delay_eq_loss_threshold;
    // Use double for the approximations:
    double shifted_sum, shifted_sum_of_squares, offset;
    td_histogram_t *empirical_distribution;
};

struct simple_qoo_statistics *simple_qoo_statistics_create();
void simple_qoo_statistics_destroy(struct simple_qoo_statistics *statistics);
void simple_qoo_statistics_add_sample(struct simple_qoo_statistics *statistics, struct timespec *delay);

//Get the various statistics
int simple_qoo_statistics_get_number_of_samples(struct simple_qoo_statistics *statistics);
int simple_qoo_statistics_get_number_of_lost_packets(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_loss_percentage(struct simple_qoo_statistics *statistics);
struct timespec *simple_qoo_statistics_get_min(struct simple_qoo_statistics *statistics);
struct timespec *simple_qoo_statistics_get_max(struct simple_qoo_statistics *statistics);
struct timespec *simple_qoo_statistics_get_delay_eq_loss_threshold(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_sum(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_mean(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_trimmed_mean(struct simple_qoo_statistics *statistics, double lower_cutoff, double upper_cutoff);
double simple_qoo_statistics_get_variance(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_standard_deviation(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_median(struct simple_qoo_statistics *statistics);
double simple_qoo_statistics_get_percentile(struct simple_qoo_statistics *statistics, double percentile);
double simple_qoo_statistics_get_qoo(struct simple_qoo_statistics *statistics, struct simple_NR_list *nr, struct simple_QTA *offset_cdf);
double simple_qoo_statistics_get_rpm(struct simple_qoo_statistics *statistics);

#endif