# qoo-c

Quality of Outcome is a network performance metric.
QoO scores are calculated by comparing application requirements to network measurements.
The network measurements are Quality Attenuation measurements as specified in [TR-452.1](https://www.broadband-forum.org/technical/download/TR-452.1.pdf) from the Broadband Forum, but excluding the computation of G, S, and V.

TR-452.1 can be found here: https://www.broadband-forum.org/technical/download/TR-452.1.pdf

This tool can:

* Compute Quality Attenuation summaries from latency and packet loss measurements
* Compute [Quality of Outcome](https://www.ietf.org/id/draft-olden-ippm-qoo-00.html) scores, and several other performance metrics such as [RPM](https://datatracker.ietf.org/doc/draft-ietf-ippm-responsiveness/)

How to use:

1. Create a sqa_stats data structure and add latency and packet loss samples.
2. Calculate RPM, QoO, or any of the other quality metrics.

Example use (see tests/unit/simple_qoo_test.c)
---
    struct sqa_stats *stats = sqa_stats_create(); //SQA is short for Simple Quality Attenuation
    
    //Add sample(s)
    struct timespec measured_delay;
    measured_delay.tv_sec = 0;
    measured_delay.tv_nsec = 200 * 1000 * 1000; // 200ms
    sqa_stats_add_sample(stats, &measured_delay);
    
    //Compute statistics
    struct simple_NR_list *nr = create_network_requirement();
    double qoo = sqa_stats_get_qoo(stats, nr);
    double rpm = sqa_stats_get_rpm(stats);

    //Clean up
    free(nr);
    sqa_stats_destroy(stats);
---

## Build instructions

Install compiler, build and test tools:

        apt install cmake
        apt install build-essential
        apt install libcmocka-dev

Clone the repository with submodules:

        git clone git@github.com:domoslabs/qoo-c.git --recurse-submodules

        (or, if you forget to use --recurse-submodules):

        git clone git@github.com:domoslabs/qoo-c.git
        git submodule update --init 
        
Build:

        cd qoo-c
        mkdir build
        cd build
        cmake ..
        make

## Acknowledgements
The authors would like to thank Neil Davies, Peter Thompson, Jonathan Newton, Gavin Young, Al Morton, Greg Mirksy and David Sinicrope for their work on QED in the Broadband Forum, and Hans Petter Dalsklev, Karl Kalvik, Knut Joar Strømmen and Len Ciavattone for their contributions to the code.