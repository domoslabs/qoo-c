# qoo-c

Quality of Outcome is a network performance metric. 
QoO scores are calculated by comparing application requirements to network measurements.
The network measurements are Quality Attenuation measurements taken using the procedure described in TR-452.1 from the Broadband Forum.

This tool can:

* Compute Quality Attenuation summaries from latency and packet loss measurements
* Compute Quality of Outcome scores

How to use:

1. Create a link_monitor and add latency and packet loss samples. Samples can be added in bulk or one-by-one.
2. Calculate QoO

Example use
---

I want session statistics at the end of a test:


I want periodic statistics without G, S and V decomposition:


I want periodic statistics with G, S and V decomposition:

    //Make the link monitor
    struct qed_link_monitor *link_monitor = qed_link_monitor_create_default();

    // Add the samples
    qed_link_monitor_add_sample(link_monitor, 60, 1, 0.2); //Packet size, timestamp, measured_delay

    // Make the QTA
    struct domos_QTA *offset_cdf = make_QTA();

    // Get an aggregate for the interval you care about
    struct qed_aggregated_samples *aggregate = create_qed_aggregate_for_interval(link_monitor, 60);

    // Calculate QoO
    double qoo = get_quality_of_outcome_from_aggregated_samples(aggregate, nr, offset_cdf);

    // Caculate RPM

    // Calculate average latency, jitter, 

    // Calculate AIM
---

## Acknowledgements
The authors would like to thank Neil Davies, Peter Thompson, Jonathan Newton, Gavin Young, Al Morton, Greg Mirksy and David Sinicrope for their work on QED in the Broadband Forum.
Hans Petter Dalsklev, Karl Kalvik and Knut Joar Strømmen for their contributions to the code.