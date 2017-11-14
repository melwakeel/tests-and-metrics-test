#ifndef __NET_STAT_H__
#define __NET_STAT_H__
/*
* netstat.h
* Auther: Mohammad Elwakeel
* Date: 12-12-2017
*/

/**
 * @file netstat.h
 * @author Mohammad Elwakeel
 * @date 12 Nov 2017
 * @brief File contains public APIs and structures used by library APIs
 *
 */

 /*! \mainpage Netstat library API user manual
 *
 * \section About About
 *
 * Netstat library is a very simple to use library that can be used to monitor link quality to
 * a remote server. it can retrieve various information such as connection establishment time and http response code.
 * For a full list of information available, check @ref Metrics.
 *
 * \section Start Getting Started
 * The library offers a simple interface to acess the features. The following code example shows how to
 * get started with the APIs
 * @code
 * Metrics result;
 * memset(&result, 0, sizeof(Metrics));
 *
 * // initalize library
 * netstat_init();
 *
 * // test and retrieve link metrics
 * netstat_test_link("http://www.google.com", NULL, &result);
 *
 * // print info
 * printf("Statistics:\neffective_url: %s\nserver_ip: %s\nresponse code:%d\nname_lookup_time: %.1f\nconnect_time: %.1f\nstart_transfer_time: %.1f\ntotal_time: %.1f\n",
 *	result.effective_url,
 *	result.server_ip,
 *	(int)result.http_response_code,
 *	result.name_lookup_time,
 *	result.connect_time,
 *	result.start_transfer_time,
 *	result.total_time);
 *
 * // cleanup resources
 * netstat_deinit();
 *@endcode
 *
 */


// Macro Definitions

/**
 * @brief max IP buffer size
 *
 * maximum buffer length assigned to server IP string
 */
#define IP_BUFFER_SIZE 50

/**
 * @brief max URL buffer size
 *
 * max buffer length assigned to effective_url string
 */
#define EFFECTIVE_URL_MAX_LENGTH 2000

// types and stru definitions

/**
 * @brief Netstat error codes
 *
 * Error codes returned by netstat library API calls
 */
typedef enum {

NETSTAT_SUCCESS = 0,		/**< call to function succeeded. */
NETSTAT_NOT_INITIALIZED,	/**< Library not initialized prior to call. */
NETSTAT_FAIL,				/**< Generic Failure. */
NETSTAT_INVALID_ARGS,		/**< invalid argument values passed to function. */
NETSTAT_INVALID_URL_FORMAT,	/**< invalid URL format requested. */
NETSTAT_FAILED_TO_RESOLVE_PROXY,/**< Proxy was not resolved. */
NETSTAT_FAILED_TO_RESOLVE_HOST,	/**< Couldn't resolve host from DNS server. */
NETSTAT_FAILED_TO_CONNECT,		/**< Couldn't establish connection with remote server. */
NETSTAT_REMOTE_ACCESS_DENIED,	/**< Remote Server denied access. */
NETSTAT_HTTP_RETURNED_ERROR,	/**< HTTP error code returned. */
NETSTAT_ERROR_END				/**< Should never be used or returned. used for boundary checking only. */
}Netstat_result;

/**
 * @brief Link Metrics
 *
 * This structure defines different measurements obtained from the GET response.
 */
typedef struct{
char server_ip[IP_BUFFER_SIZE];	/**< zero terminated string for server IP Metrics#server_ip. */
char effective_url[EFFECTIVE_URL_MAX_LENGTH]; /**< zero terminated string for effective url Metrics#effective_url. */
long http_response_code;	/**< http response code Metrics#http_response_code. */
double name_lookup_time;	/**< name lookup time. Metrics#name_lookup_time. */
double connect_time;		/**< connect time. Metrics#connect_time. */
double start_transfer_time;	/**< start transfer time. Metrics#start_transfer_time. */
double total_time;			/**< total time. Metrics#total_time. */
}Metrics;

/**
 * @brief HTTP extra header info
 *
 * This type defines the information handle for extra http headers added to the request.
 * @see netstat_add_http_header
 * @see netstat_clear_http_headers
 */
typedef void* HEADER;


/**
 * @brief Initialize netstat library
 * this function initializes all internal states and resources
 * used during library calls
 * @return @ref NETSTAT_SUCCESS if succeeded, @ref Netstat_result error code if failed.
 * @note after completing all library calls, @ref netstat_deinit should be called to free any used resources
 * @warning This API must be called before any other library API
 */
Netstat_result netstat_init(void);

/**
 * @brief add extra header to http request
 * adds an extra http header to the next request
 * @param header_info: pointer to header information handle. this handle will be used to append new headers
 *        or send a request with additional headers. it must be a valid pointer and handler must be initalized to null before first call.
 *        see example below:
 * @code
 * HEADER header_info = NULL;
 * // first call to add new header
 * netstat_add_http_header(&header_info, "accept:");
 *
 * // second call to add new header
 * netstat_add_http_header(&header_info, "Shoesize: 10");
 * // now both headers are stored within header_info handle
 * @endcode
 * @param header_string is the string for header name and value.
 * @return @ref NETSTAT_SUCCESS if succeeded, @ref Netstat_result error code if failed.
 * @note you must initialize the header handle to NULL before first call!
 * @warning after completing all requests, you must call @ref netstat_clear_http_headers to free resources used by the headers.
 */
Netstat_result netstat_add_http_header(HEADER *header_info, const char *header_string);

/**
 * @brief free extra headers and their resources
 * this API is called to free resources allocated by @ref netstat_add_http_header()
 * @param header_info: pointer to header information handle.
 * @return @ref NETSTAT_SUCCESS if succeeded, @ref Netstat_result error code if failed.
 * @note after calling this API, all pre-allocated headers for this handle will be lost.
 */
Netstat_result netstat_clear_http_headers(HEADER *header_info);

/**
 * @brief Test link to server
 * This API sends an HTTP GET request to server URL and returns information about link quality
 * @param url: string of servers URL. Must be valid URL format! Can't be NULL
 * @param header_info handle for extra headers to append to the request. if no additional headers are required, this param can be NULL.
 * @param result pointer to Metrics struct that shall be filled with retrieved information. Can't be NULL
 * @return @ref NETSTAT_SUCCESS if succeeded, @ref Netstat_result error code if failed.
 */
Netstat_result netstat_test_link(const char *url, HEADER *header_info, Metrics *result);

/**
 * @brief clean up lib resources
 * this API is used to free any library internal resources and clean up properly.
 * @return @ref NETSTAT_SUCCESS if succeeded, @ref Netstat_result error code if failed.
 * @note This API must be called at the end of the application to ensure proper cleanup.
 */
Netstat_result netstat_deinit(void);

#endif
