#ifndef MAVLINK_READER_H
#define MAVLINK_READER_H

#include <stdint.h>
#include <netinet/in.h>

#include "INIReader.h"
#include "util.h"

#include "mavlink_message_handler.h"

/* A mavlink packet should be limited to 6+255+2 = 263 bytes
   6 byte header, 255 max bytes in payload, 2 byte crc */
#define TELEM_PKT_MAX 512

class MAVLink_Reader {
public:
    MAVLink_Reader() :
        log_interval_us(10 * 1000000),
	next_message_handler(0),
	err_skipped(0),
	err_time_us(0),            /* last time we logged */
	err_interval_us(1000000)  /* once per second max */
    {
        uint64_t now_us = clock_gettime_us(CLOCK_MONOTONIC);
	next_tenthhz_time = now_us;
	next_1hz_time = now_us;
	next_10hz_time = now_us;
	next_100hz_time = now_us;
    }
    void run();
    void instantiate_message_handlers(INIReader config);

private:
    bool sane_telem_forwarder_packet(uint8_t *pkt, uint16_t pktlen);
    void handle_telem_forwarder_recv();
    void pack_telem_forwarder_sockaddr(INIReader config);
    int can_log_error();
    int create_and_bind();

    uint64_t next_tenthhz_time;
    uint64_t next_1hz_time;
    uint64_t next_10hz_time;
    uint64_t next_100hz_time;

    struct sockaddr_in sa_tf; /* solo's address */

    int fd_telem_forwarder;
    struct sockaddr_in sa;

    uint64_t log_interval_us; /* log stats this often */

#define MAX_MESSAGE_HANDLERS 10
    uint8_t next_message_handler;
    MAVLink_Message_Handler *message_handler[MAX_MESSAGE_HANDLERS];

    uint16_t err_skipped;
    uint64_t err_time_us;
    uint64_t err_interval_us;

    void do_idle_callbacks();
};


#endif
