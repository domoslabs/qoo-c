#ifndef _DOMOS_NETREQ_H
#define _DOMOS_NETREQ_H

struct simple_QTA
{
    int num_percentiles;
    float percentiles[32];
    int num_latencies;
    float latencies[32];
};

struct simple_NR_list
{
    char type[32];
    int traffic_type;
    int report_session;
    char report_name[32];
    int MinThroughputRx;
    int MinThroughputTx;
    struct simple_QTA nrp;
    struct simple_QTA nrpou;
};

#endif