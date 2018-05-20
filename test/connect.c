#include <stdio.h>
#include <unistd.h>
#include <freepoi/net/codec.h>
#include <freepoi/net/rtmp.h>

int
main(void)
{
    struct freepoi_connection conn;
    struct freepoi_connect_params params;
    struct freepoi_packet pkt;
    int error;

    memset(&conn, 0, sizeof(conn));
    conn.in.chunk_size = conn.out.chunk_size = 128;
    memset(&params, 0, sizeof(params));

    FREEPOI_CONNECT_PARAMS_INIT_NETCONNECTION_STATIC(
        &params,
        "gikopoi141_for",
        "WIN 21,0,0,213",
        "http://l4cs.jpn.org/gikopoi/flash/gikopoi141_for/gikopoi.swf",
        "rtmp://tswindy.serveftp.net:1937/gikopoi141_for",
        "http://l4cs.jpn.org/gikopoi/flash/gikopoi141_for/flash_gikopoi.html",
        "Anonymous",
        "",
        "dt303eml8858792281"
    );

    freepoi_initialize_host_info(&params.rtmp_host, "tswindy.serveftp.net",
                                 NULL, NULL, 1937);

    params.fpad = 0;
    params.capabilities = 239;
    params.audio_capabilities = 3575;
    params.video_capabilities = 252;
    params.video_function = 1;

#ifdef _WIN32
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
#endif
    error = freepoi_rtmp_connect(&conn, &params);

    if (error) {
        fprintf(stderr, "Can't do shit: %d", error);
        return -1;
    }

    while (1) {
        fflush(stderr);

        if (freepoi_read_packet(&conn, &pkt) != 0) {
            break;
        }

        switch (pkt.type) {
        case FREEPOI_COMMAND_PACKET:
            if (freepoi_handle_command(&conn, &pkt)) {
                break;
            } else {
                fprintf(stderr, "Done!");
                goto end;
            }

        default:
            continue;
        }

        sleep(1);
    };

end:
#ifdef _WIN32
    WSACleanup();

#endif
    return 0;
}
