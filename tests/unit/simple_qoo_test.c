#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cmocka.h>
#include <time.h>
#include <sys/time.h>

#include "../../src/simple-qoo.h"
#include "../../src/qed-standard-format.h"

char *testfilepath = NULL;

static void test_sqa_stats_single_sample(void **state)
{
    struct sqa_stats *stats = sqa_stats_create();
    struct timespec measured_delay;
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    sqa_stats_add_sample(stats, &measured_delay);
    assert_int_equal(sqa_stats_get_number_of_samples(stats), 1);
    struct timespec *tmp = sqa_stats_get_max(stats);
    assert_true((tmp->tv_sec == 0) && (tmp->tv_nsec == 200 * 1000 * 1000));
    tmp = sqa_stats_get_min(stats);
    assert_true((tmp->tv_sec == 0) && (tmp->tv_nsec == 200 * 1000 * 1000));
    double tmp2 = sqa_stats_get_sum(stats);
    assert_in_range(tmp2, 0, 99999999999);
    assert_float_equal(tmp2, 0.2, 0.000001);
    assert_true(sqa_stats_get_number_of_lost_packets(stats) == 0);
    assert_true(sqa_stats_get_delay_eq_loss_threshold(stats)->tv_sec == 15);
    assert_true(sqa_stats_get_delay_eq_loss_threshold(stats)->tv_nsec == 0);
    tmp2 = sqa_stats_get_mean(stats);
    assert_in_range(tmp2, 0, 99999999999);
    assert_float_equal(tmp2, 0.2, 0.000001);
    assert_true(sqa_stats_get_variance(stats) == 0.0);
    assert_true(sqa_stats_get_standard_deviation(stats) == 0.0);
    assert_true(sqa_stats_get_median(stats) == 0.2);
    assert_true(sqa_stats_get_percentile(stats, 0.5) == 0.2);
    assert_true(sqa_stats_get_percentile(stats, 0.9) == 0.2);
    assert_true(sqa_stats_get_percentile(stats, 0.99) == 0.2);
    sqa_stats_destroy(stats);
}

static void test_sqa_stats_multiple_samples(void **state)
{
    struct sqa_stats *stats = sqa_stats_create();
    struct timespec measured_delay;
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    sqa_stats_add_sample(stats, &measured_delay);
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 100 * 1000 * 1000; // 100ms
    sqa_stats_add_sample(stats, &measured_delay);
    assert_int_equal(sqa_stats_get_number_of_samples(stats), 2);
    struct timespec *tmp = sqa_stats_get_max(stats);
    assert_true((tmp->tv_sec == 0) && (tmp->tv_nsec == 200 * 1000 * 1000));
    tmp = sqa_stats_get_min(stats);
    assert_true((tmp->tv_sec == 0) && (tmp->tv_nsec == 100 * 1000 * 1000));
    assert_in_range(sqa_stats_get_sum(stats), 0, 9999999);
    assert_float_equal(sqa_stats_get_sum(stats), 0.3, 0.0000001);
    assert_true(sqa_stats_get_number_of_lost_packets(stats) == 0);
    assert_float_equal(sqa_stats_get_mean(stats), 0.15, 0.0000001);
    assert_float_equal(sqa_stats_get_standard_deviation(stats), 0.05, 0.0000001);
    assert_float_equal(sqa_stats_get_variance(stats), 0.0025, 0.0000001);
    assert_float_equal(sqa_stats_get_median(stats), 0.2, 0.0000001);
    assert_float_equal(sqa_stats_get_percentile(stats, 50), 0.2, 0.0000001);
    assert_float_equal(sqa_stats_get_percentile(stats, 99), 0.2, 0.0000001);
    assert_float_equal(sqa_stats_get_percentile(stats, 99.9), 0.2, 0.0000001);
    sqa_stats_destroy(stats);
}

static void sqa_stats_test_loss(void **state)
{
    struct sqa_stats *stats = sqa_stats_create();
    struct timespec measured_delay;
    measured_delay.tv_sec = 15;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    sqa_stats_add_sample(stats, &measured_delay);
    assert_int_equal(sqa_stats_get_number_of_samples(stats), 1);
    assert_int_equal(sqa_stats_get_number_of_lost_packets(stats), 1);
    assert_in_range(sqa_stats_get_loss_percentage(stats), 0, 100);
    assert_float_equal(sqa_stats_get_loss_percentage(stats), 100, 0.0000001);
    sqa_stats_destroy(stats);
}

float get_elapsed_time(struct timespec *start)
{
    struct timespec end = {0, 0};
    clock_gettime(CLOCK_REALTIME, &end);
    return (end.tv_sec - start->tv_sec) + (end.tv_nsec - start->tv_nsec) / 1000000000.0;
}

static void sqa_stats_test_1M_samples(void **state)
{
    struct timespec start = {0, 0};
    clock_gettime(CLOCK_REALTIME, &start);
    struct sqa_stats *stats = sqa_stats_create();
    struct timespec measured_delay;
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    for (int i = 0; i < 1000000; i++)
    {
        sqa_stats_add_sample(stats, &measured_delay);
    }
    assert_int_equal(sqa_stats_get_number_of_samples(stats), 1000000);
    assert_int_equal(sqa_stats_get_number_of_lost_packets(stats), 0);
    assert_float_equal(sqa_stats_get_loss_percentage(stats), 0, 0.0000001);
    assert_float_equal(sqa_stats_get_mean(stats), 0.2, 0.0000001);
    assert_float_equal(sqa_stats_get_standard_deviation(stats), 0, 0.0001);
    sqa_stats_destroy(stats);
    assert_true(get_elapsed_time(&start) < 0.2);
}

static void test_example_from_readme(void **state)
{
    struct sqa_stats *stats = sqa_stats_create();
    
    //Add sample(s)
    struct timespec measured_delay;
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    sqa_stats_add_sample(stats, &measured_delay);
    
    //Compute statistics
    double rpm = sqa_stats_get_rpm(stats);
    assert_float_equal(rpm, 300, 0.000001);
    //Clean up
    sqa_stats_destroy(stats);
}

int main(int argc, char *argv[])
{   
    testfilepath = "./testfiles";
    if (argc==1) {
        printf("no testfile path given as argument\n");
        testfilepath = "./testfiles";
        char cwd[1024];
        // getcwd(cwd, sizeof(cwd));
        // printf("Current working dir: %s\n", cwd);
        // exit(1);
    } else if (argc==2) {
        testfilepath = argv[1];
    }
    const struct CMUnitTest qed_standard_format_tests[] = {
        cmocka_unit_test(test_sqa_stats_single_sample),
        cmocka_unit_test(test_sqa_stats_multiple_samples),
        cmocka_unit_test(sqa_stats_test_loss),
        cmocka_unit_test(sqa_stats_test_1M_samples),
        cmocka_unit_test(test_example_from_readme),
        };
    return cmocka_run_group_tests(qed_standard_format_tests, NULL, NULL); //move list init to setup and teardown
}