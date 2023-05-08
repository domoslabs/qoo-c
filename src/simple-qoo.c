#include <math.h>
#include "simple-qoo.h"

struct simple_qoo_statistics *simple_qoo_statistics_create(){
    struct simple_qoo_statistics *stats = malloc(sizeof(struct simple_qoo_statistics));
    stats->number_of_samples = 0;
    stats->number_of_lost_packets = 0;
    stats->min.tv_sec = 0;
    stats->min.tv_nsec = 0;
    stats->max.tv_sec = 0;
    stats->max.tv_nsec = 0;
    stats->delay_eq_loss_threshold.tv_sec = 15; // 15 seconds as default
    stats->delay_eq_loss_threshold.tv_nsec = 0;
    stats->shifted_sum = 0;
    stats->shifted_sum_of_squares = 0;
    stats->offset = 0.1; // 0.1 seconds as default. Just need something in the expected range of the samples.
    stats->empirical_distribution = td_new(50.0);
    return stats;
}

void simple_qoo_statistics_destroy(struct simple_qoo_statistics *statistics){
    td_free(statistics->empirical_distribution);
    free(statistics);
}

void simple_qoo_statistics_add_sample(struct simple_qoo_statistics *stats, struct timespec *delay){
    //Sanity check
    if (delay->tv_sec <= 0 && delay->tv_nsec <= 0){
        printf("Error: Zero or Negative delay\n");
        return;
    }
    stats->number_of_samples++;
    // A sample is either a lost packet or a packet with a delay
    if (delay->tv_sec > stats->delay_eq_loss_threshold.tv_sec || 
    (delay->tv_sec == stats->delay_eq_loss_threshold.tv_sec && delay->tv_nsec > stats->delay_eq_loss_threshold.tv_nsec)) {
        // If loss
        stats->number_of_lost_packets++;
    } else {
        // If delay
        if (stats->max.tv_sec == 0 && stats->max.tv_nsec == 0){
            stats->min = *delay;
            stats->max = *delay;
        }
        else{
            // New min?
            if (delay->tv_sec < stats->min.tv_sec || (delay->tv_sec == stats->min.tv_sec && delay->tv_nsec < stats->min.tv_nsec)){
                stats->min = *delay;
            }
            // New max?
            if (delay->tv_sec > stats->max.tv_sec || (delay->tv_sec == stats->max.tv_sec && delay->tv_nsec > stats->max.tv_nsec)){
                stats->max = *delay;
            }
        }
        // Use double precision for the approximations:
        double delay_in_seconds = tspecusec(delay) / 1000000.0;

        stats->shifted_sum += delay_in_seconds - stats->offset;
        stats->shifted_sum_of_squares += pow(delay_in_seconds - stats->offset, 2.0);
        td_add(stats->empirical_distribution, delay_in_seconds, 1);
    }
}

//Get functions for various statistics
int simple_qoo_statistics_get_number_of_samples(struct simple_qoo_statistics *statistics){
    return statistics->number_of_samples;
}

int simple_qoo_statistics_get_number_of_lost_packets(struct simple_qoo_statistics *statistics){
    return statistics->number_of_lost_packets;
}

double simple_qoo_statistics_get_loss_percentage(struct simple_qoo_statistics *statistics){
    return (double)100.0 * statistics->number_of_lost_packets / statistics->number_of_samples;
}

struct timespec *simple_qoo_statistics_get_min(struct simple_qoo_statistics *statistics){
    return &statistics->min;
}

struct timespec *simple_qoo_statistics_get_max(struct simple_qoo_statistics *statistics){
    return &statistics->max;
}

double simple_qoo_statistics_get_sum(struct simple_qoo_statistics *statistics){
    return statistics->shifted_sum + statistics->offset * statistics->number_of_samples;
}

struct timespec *simple_qoo_statistics_get_delay_eq_loss_threshold(struct simple_qoo_statistics *statistics){
    return &statistics->delay_eq_loss_threshold;
}

double simple_qoo_statistics_get_mean(struct simple_qoo_statistics *statistics){
    return statistics->offset + (statistics->shifted_sum / (statistics->number_of_samples - statistics->number_of_lost_packets));
}

double simple_qoo_statistics_get_trimmed_mean(struct simple_qoo_statistics *statistics, double lower_cutoff, double upper_cutoff){
    return td_trimmed_mean(statistics->empirical_distribution, lower_cutoff/100.0, upper_cutoff/100.0);
}

double simple_qoo_statistics_get_variance(struct simple_qoo_statistics *statistics){
    // Use the offset values of sum and sum_of_squares for numerical stability
    int number_of_latency_samples = statistics->number_of_samples - statistics->number_of_lost_packets;
    return (statistics->shifted_sum_of_squares - pow(statistics->shifted_sum, 2.0) / number_of_latency_samples) / (number_of_latency_samples);
}

double simple_qoo_statistics_get_standard_deviation(struct simple_qoo_statistics *statistics){
    return sqrt(simple_qoo_statistics_get_variance(statistics));
}

double simple_qoo_statistics_get_median(struct simple_qoo_statistics *statistics){
    return td_quantile(statistics->empirical_distribution, 0.5);
}

double simple_qoo_statistics_get_percentile(struct simple_qoo_statistics *statistics, double percentile){
    return td_quantile(statistics->empirical_distribution, percentile/100.0);
}

double simple_qoo_statistics_get_qoo(struct simple_qoo_statistics *statistics, struct simple_NR_list *nr, struct simple_QTA *offset_cdf){
    double qoo = 100;
    for (int i = 0; i < nr->nrp.num_percentiles; i++) {
        double perc = nr->nrp.percentiles[i];
        double mes_lat = td_quantile(statistics->empirical_distribution, (double)perc/100);
        double nrp_lat = nr->nrp.latencies[i];
        double nrpou_lat = nr->nrpou.latencies[i];
        //Find two closest values in offset
        double offset = 0;
        for (int j = 0; j < offset_cdf->num_percentiles-1; j++) {
            double perc_offset1 = offset_cdf->percentiles[j];
            double perc_offset2  = offset_cdf->percentiles[j+1];
            if (perc_offset1 < perc && perc <= perc_offset2) {
                double lat_offset1 = offset_cdf->latencies[j];
                double lat_offset2  = offset_cdf->latencies[j+1];
                offset = domosm_linear_interpolation_between_percentiles(perc, lat_offset1, lat_offset2, perc_offset1, perc_offset2);
                //log_info("Offset: %.6f, lat_offset1: %.6f, lat_offset2: %.6f,", offset, lat_offset1, lat_offset2);
                break;
            }
        }
        double qoo_part = (1 - (((mes_lat+offset) - nrp_lat) / (nrpou_lat - nrp_lat)))*100;
        if (qoo_part < 0) {
            qoo_part = 0;
        }
        if (qoo_part < qoo) {
            qoo = qoo_part;
        }
    }
    return qoo;
}

double simple_qoo_statistics_get_rpm(struct simple_qoo_statistics *statistics){
    // This is a simplified RPM metric. The full version needs latency measurements for TCP, TLS and HTTP for "foreign" flows
    // and samples taken on a saturated flow. See: https://datatracker.ietf.org/doc/draft-ietf-ippm-responsiveness/
    return 60.0/simple_qoo_statistics_get_mean(statistics);
}