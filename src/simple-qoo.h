#ifndef _SIMPLEQOO_H
#define _SIMPLEQOO_H

#include <time.h>
#include "tdigest.h"
#include "network-reqs.h"
#include "udpst_common.h"

// Simple Quality Attenuation (SQA) statistics
// Implements a quality attenuation value estimator following the specification of TR-452.1,
// but excluding the G, S, and V decomposition.
struct sqa_stats
{
    int number_of_samples, number_of_lost_packets;
    // Use timespec for the exact measurements:
    struct timespec min, max, delay_eq_loss_threshold;
    // Use double for the approximations:
    double shifted_sum, shifted_sum_of_squares, offset;
    td_histogram_t *empirical_distribution;
};

struct sqa_stats *sqa_stats_create();
void sqa_stats_destroy(struct sqa_stats *statistics);
void sqa_stats_add_sample(struct sqa_stats *statistics, struct timespec *delay);

//Get the various statistics
int sqa_stats_get_number_of_samples(struct sqa_stats *statistics);
int sqa_stats_get_number_of_lost_packets(struct sqa_stats *statistics);
double sqa_stats_get_loss_percentage(struct sqa_stats *statistics);
struct timespec *sqa_stats_get_min(struct sqa_stats *statistics);
struct timespec *sqa_stats_get_max(struct sqa_stats *statistics);
struct timespec *sqa_stats_get_delay_eq_loss_threshold(struct sqa_stats *statistics);
double sqa_stats_get_sum(struct sqa_stats *statistics);
double sqa_stats_get_mean(struct sqa_stats *statistics);
double sqa_stats_get_trimmed_mean(struct sqa_stats *statistics, double lower_cutoff, double upper_cutoff);
double sqa_stats_get_variance(struct sqa_stats *statistics);
double sqa_stats_get_standard_deviation(struct sqa_stats *statistics);
double sqa_stats_get_median(struct sqa_stats *statistics);
double sqa_stats_get_percentile(struct sqa_stats *statistics, double percentile);
double sqa_stats_get_qoo(struct sqa_stats *statistics, struct simple_NR_list *nr, struct simple_QTA *offset_cdf);
double sqa_stats_get_rpm(struct sqa_stats *statistics);

#endif