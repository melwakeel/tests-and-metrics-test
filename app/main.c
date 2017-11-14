#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "netstat.h"


static void print_help(void)
{
    printf("Usage: netstat -U \"server url\" -H \"Header-name: Header-value\"  -n <integer>\n");
    printf("This app makes use of netstat lib to make HTTP GET request to a remote server and retrieve information about connection quality\n");
    printf("Opthions: \n\t-U pass the URL of the target server. If no URL is passed, default will be www.google.com\n");
    printf("\t-H (optional): specify an extra HTTP header to add to the GET request.  Can be used multiple times\n");
    printf("\t-n number of HTTP requests to make (i.e. the number of samples to take the median of). If not set, default will be 1 sample\n");
	printf("\t-h print help\n");
    printf("example: netstat -U \"www.google.com\" -H \"Accept:\" -n 10\n");
}

static void print_formatted_result(Metrics *data)
{
	printf("SKTEST;");
	printf("%s;", data->server_ip);
	printf("%ld;", data->http_response_code);
	printf("%.1f;", data->name_lookup_time);
	printf("%.1f;", data->connect_time);
	printf("%.1f;", data->start_transfer_time);
	printf("%.1f\n", data->total_time);
}

int main(int argc, char *argv[])
{
    int i;

    HEADER header_info = NULL;
    char *url=NULL;
    char default_url[] = "www.google.com";
    int n_iterations = 1;
    Metrics result, total;

    if(argc%2 == 0)
    {
        //incorrect number of arguments
        print_help();
        return 0;
    }

    if(netstat_init() != NETSTAT_SUCCESS)
    {
        return 0;
    }

    for(i=1;i<argc;i++)
    {
        switch(*(argv[i] + 1))
        {
            case 'U':
            i++;
            url = argv[i];
            break;

            case 'H':
            i++;
            netstat_add_http_header(&header_info, argv[i]);
            break;

            case 'n':
            i++;
            n_iterations = (int) atoi(argv[i]);
            if(n_iterations < 1)
                n_iterations = 1;
            break;

			case 'h':
            default:
            // we should not be here unless passed arguments are incorrect
            netstat_deinit();
            print_help();
            return 0;
            break;

        }
    }

    if(url == NULL)
    {
        //defaulting URL
        url = default_url;
    }

    memset(&result, 0, sizeof(result));
    memset(&total, 0, sizeof(total));

    for(i=0;i<n_iterations;i++)
    {
        if(netstat_test_link(url, &header_info, &result) == NETSTAT_SUCCESS)
        {
            total.name_lookup_time += result.name_lookup_time;
            total.connect_time += result.connect_time;
            total.start_transfer_time += result.start_transfer_time;
            total.total_time += result.total_time;
        }
    }

    total.name_lookup_time /= n_iterations;
    total.connect_time /= n_iterations;
    total.start_transfer_time /= n_iterations;
    total.total_time /= n_iterations;
    strncpy(total.server_ip, result.server_ip, IP_BUFFER_SIZE);
    strncpy(total.effective_url, result.effective_url, EFFECTIVE_URL_MAX_LENGTH);
    total.http_response_code = result.http_response_code;

    print_formatted_result(&total);

    //printf("Statistics:\neffective_url: %s\nserver_ip: %s\nresponse code:%d\nname_lookup_time: %.1f\nconnect_time: %.1f\nstart_transfer_time: %.1f\ntotal_time: %.1f\n", result.effective_url, result.server_ip, (int)result.http_response_code, result.name_lookup_time, result.connect_time, result.start_transfer_time, result.total_time);


    if(header_info != NULL)
    {
        netstat_clear_http_headers(&header_info);
    }
    netstat_deinit();
    return 0;
}


