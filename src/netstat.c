/*
* netstat.h
* Auther: Mohammad Elwakeel
* Date: 12-12-2017
*/

#include <stdio.h>
#include <curl/curl.h>
#include "netstat.h"
#include <string.h>

static CURL *handle = NULL;


static Netstat_result curl_err_to_netstat_Err(CURLcode curl_err)
{
    Netstat_result ret;
    switch(curl_err)
    {
        case CURLE_OK:
        ret =  NETSTAT_SUCCESS;
        break;

        case CURLE_URL_MALFORMAT:
        ret = NETSTAT_INVALID_URL_FORMAT;
        break;

        case CURLE_COULDNT_RESOLVE_PROXY:
        ret = NETSTAT_FAILED_TO_RESOLVE_PROXY;
        break;

        case CURLE_COULDNT_RESOLVE_HOST:
        ret = NETSTAT_FAILED_TO_RESOLVE_HOST;
        break;

        case CURLE_COULDNT_CONNECT:
        ret = NETSTAT_FAILED_TO_CONNECT;
        break;

        case CURLE_REMOTE_ACCESS_DENIED:
        ret = NETSTAT_REMOTE_ACCESS_DENIED;
        break;

        default:
        ret = NETSTAT_FAIL;
        break;
    }

    return ret;
}

static size_t response_data_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    // empty callback to avoid storming stdout with response string
    return size * nmemb;
}

static void fill_metrics_data(Metrics *result)
{
    // server IP
    char *temp = NULL;

    curl_easy_getinfo(handle, CURLINFO_PRIMARY_IP, &temp);
    if(temp)
    {
        size_t len = ( (strlen(temp) > IP_BUFFER_SIZE) ? (IP_BUFFER_SIZE - 1):(strlen(temp) + 1) );
        memset(result->server_ip, 0, IP_BUFFER_SIZE);
        memcpy(result->server_ip, temp, len);
    }

    // effective URL
    temp = NULL;
    curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &temp);
    if(temp)
    {
        size_t len = ( (strlen(temp) > EFFECTIVE_URL_MAX_LENGTH) ? (EFFECTIVE_URL_MAX_LENGTH - 1):(strlen(temp) + 1) );
        memset(result->effective_url, 0, EFFECTIVE_URL_MAX_LENGTH);
        memcpy(result->effective_url, temp, len);
    }

    // http response code
    result->http_response_code = -1;
    curl_easy_getinfo(handle, CURLINFO_HTTP_CODE, &(result->http_response_code));

    // name lookup time
    result->name_lookup_time = -1;
    curl_easy_getinfo(handle, CURLINFO_NAMELOOKUP_TIME, &(result->name_lookup_time));

    // connect time
    result->connect_time = -1;
    curl_easy_getinfo(handle, CURLINFO_CONNECT_TIME, &(result->connect_time));

    // start transfer time
    result->start_transfer_time = -1;
    curl_easy_getinfo(handle, CURLINFO_STARTTRANSFER_TIME, &(result->start_transfer_time));

    // total time
    result->total_time = -1;
    curl_easy_getinfo(handle, CURLINFO_TOTAL_TIME, &(result->total_time));
}


Netstat_result netstat_init(void)
{
    if(handle != NULL)
    {
        // previous initialization took place. clean up first
        curl_easy_cleanup(handle);
        handle = NULL;
    }

    handle = curl_easy_init();
    if(handle == NULL)
    {
        return NETSTAT_FAIL;
    }
    else
    {
        return NETSTAT_SUCCESS;
    }
}

Netstat_result netstat_set_test_url(const char *url)
{
    if(!url)
    {
        return NETSTAT_INVALID_ARGS;
    }

    if(!handle)
    {
        return NETSTAT_NOT_INITIALIZED;
    }

    return curl_err_to_netstat_Err(curl_easy_setopt(handle, CURLOPT_URL, url));
}

Netstat_result netstat_add_http_header(HEADER *header_info, const char *header_string)
{
    struct curl_slist *list;

    if(!header_string)
    {
        return NETSTAT_INVALID_ARGS;
    }

    list = (struct curl_slist *)(*header_info);

    list = curl_slist_append(list, header_string);

    *header_info = (HEADER)list;

    return NETSTAT_SUCCESS;
}

Netstat_result netstat_clear_http_headers(HEADER *header_info)
{
    if((!header_info) || (!(*header_info)))
    {
        return NETSTAT_INVALID_ARGS;
    }

    curl_slist_free_all((struct curl_slist *)*header_info);
    *header_info = (HEADER)NULL;

    return NETSTAT_SUCCESS;
}

Netstat_result netstat_test_link(const char *url, HEADER *header_info, Metrics *result)
{
    CURLcode ret_val;
    if(!url)
    {
        return NETSTAT_INVALID_ARGS;
    }

    if(!result)
    {
        return NETSTAT_INVALID_ARGS;
    }

    if(!handle)
    {
        return NETSTAT_NOT_INITIALIZED;
    }



    ret_val = curl_easy_setopt(handle, CURLOPT_URL, url);
    if(ret_val != CURLE_OK)
    {
        return curl_err_to_netstat_Err(ret_val);
    }

    if((header_info) && (*header_info))
    {
        ret_val = curl_easy_setopt(handle, CURLOPT_HTTPHEADER, *header_info);
        if(ret_val != CURLE_OK)
        {
            return curl_err_to_netstat_Err(ret_val);
        }
    }

    ret_val = curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    if(ret_val != CURLE_OK)
    {
        return curl_err_to_netstat_Err(ret_val);
    }

    ret_val = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, response_data_callback);
    if(ret_val != CURLE_OK)
    {
        return curl_err_to_netstat_Err(ret_val);
    }

    ret_val = curl_easy_perform(handle);
    if(ret_val != CURLE_OK)
    {
        return curl_err_to_netstat_Err(ret_val);
    }

    // if all good so far, parse statistics
    fill_metrics_data(result);

    return NETSTAT_SUCCESS;

}

Netstat_result netstat_deinit(void)
{
    if(!handle)
    {
        return NETSTAT_NOT_INITIALIZED;
    }

    curl_easy_cleanup(handle);

	handle = NULL;

    return NETSTAT_SUCCESS;
}
