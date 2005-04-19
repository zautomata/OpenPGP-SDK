#include "packet.h"
#include "packet-parse.h"
#include "util.h"
#include "accumulate.h"
#include <unistd.h>
#include <string.h>

int main(int argc,char **argv)
    {
    ops_parse_options_t opt;
    ops_keyring_t keyring;
    ops_reader_fd_arg_t arg;

    ops_init();

    memset(&keyring,'\0',sizeof keyring);
    ops_parse_options_init(&opt);
    //    ops_parse_packet_options(&opt,OPS_PTAG_SS_ALL,OPS_PARSE_RAW);
    //    ops_parse_options(&opt,OPS_PTAG_SS_ALL,OPS_PARSE_PARSED);
    //    opt.cb=callback;
    arg.fd=0;
    opt.reader_arg=&arg;
    opt.reader=ops_reader_fd;
    ops_parse_and_accumulate(&keyring,&opt);

    ops_keyring_free(&keyring);

    ops_finish();

    return 0;
    }
