# qoo-c

Quality of Outcome is a network performance metric.
QoO scores are calculated by comparing application requirements to network measurements.
The network measurements are Quality Attenuation measurements taken using the procedure described in TR-452.1 from the Broadband Forum.

This tool can:

* Compute Quality Attenuation summaries from latency and packet loss measurements
* Compute Quality of Outcome scores

How to use:

1. Create a simple_qoo_statistics data structure and add latency and packet loss samples.
2. Calculate RPM, QoO, or any of the other quality metrics.

Example use
---
    struct simple_qoo_statistics *stats = simple_qoo_statistics_create();
    
    //Add sample(s)
    struct timespec measured_delay;
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    simple_qoo_statistics_add_sample(stats, &measured_delay);
    
    //Compute statistics
    double rpm = simple_qoo_statistics_get_rpm(stats);

    //Clean up
    simple_qoo_statistics_destroy(stats);
---

## Acknowledgements
The authors would like to thank Neil Davies, Peter Thompson, Jonathan Newton, Gavin Young, Al Morton, Greg Mirksy and David Sinicrope for their work on QED in the Broadband Forum, and Hans Petter Dalsklev, Karl Kalvik, Knut Joar Strømmen and Len Ciavattone for their contributions to the code.