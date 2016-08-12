/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Feng Qiu  <peivnn@foxmail.com>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php_lastrpc.h"

#include "ext/standard/php_var.h" /* for serialize */
#include "ext/standard/php_smart_str.h" /* for smart string */
#include "ext/standard/url.h" /* for php_url */
#include <zend_exceptions.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include "last/lastrpc_packager.h"
#include "last/lastrpc_client.h"
#include "last/lastrpc_request.h"
#include "last/lastrpc_exception.h"

typedef struct _lastrpc_curl
{
    CURL *curl;
} lastrpc_curl_t;


size_t php_lastrpc_curl_response_wite(char *resbody, size_t size, size_t nmemb, lastrpc_request_t *request)
{
    size_t len = size * nmemb;
    smart_str_free(&request->response_body);
    smart_str_appendl(&request->response_body.c, resbody, len);
    return len;
}


void php_lastrpc_curl_prepare(lastrpc_request_t *request TSRMLS_DC)
{
    lastrpc_curl_t *handle = (lastrpc_curl_t *) request->handle;
    CURL *curl = handle->curl;

    if (!curl)
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "curl missing [prepare]");
    }

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,     request->request_body.c);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,  request->request_body.len);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  php_lastrpc_curl_response_wite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      request);
    curl_easy_setopt(curl, CURLOPT_POST,           1);
    curl_easy_setopt(curl, CURLOPT_VERBOSE,        0);
    curl_easy_setopt(curl, CURLOPT_HEADER,         0);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);

    // 设置小于1000MS 出错的BUG
    if (LIBCURL_VERSION_NUM > 0x070A00)
    {
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    }
    // 超时设置毫秒
    if(LIBCURL_VERSION_NUM > 0x071002)
    {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 2000);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 1000);
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
    }

}

void php_lastrpc_curl_exec(lastrpc_request_t *request TSRMLS_DC)
{
    CURLcode curl_error = CURLE_OK;
    lastrpc_curl_t *handle = (lastrpc_curl_t *) request->handle;
    CURL *curl = handle->curl;
    if (!curl)
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "curl missing [exec]");
    }

    curl_error = curl_easy_perform(curl);

    if (curl_error != CURLE_OK)
    {
        zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "curl exec error, errcode:%ld\n", curl_error);
    }
    else
    {
        long http_code;
        if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code) == CURLE_OK && http_code != 200)
        {
            zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "server responsed non-200 code:%ld\n", http_code);
        }
    }
}

void php_lastrpc_curl_close(lastrpc_request_t *request TSRMLS_DC)
{
    lastrpc_curl_t *handle = (lastrpc_curl_t *) request->handle;
    CURL *curl = handle->curl;
    curl_easy_cleanup(curl);

    smart_str_free(&request->request_body);
	smart_str_free(&request->response_body);
    efree(request);
}


void php_lastrpc_curl_init(lastrpc_request_handle_t *request_handle, lastrpc_request_t *request TSRMLS_DC)
{
    CURLcode curl_error = CURLE_OK;
    struct curl_slist *headers;

    lastrpc_curl_t *handle = ecalloc(1, sizeof(lastrpc_curl_t));
    CURL *dcurl = curl_easy_init();

    if (!dcurl)
    {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "curl init failed");
	}

    curl_error = curl_easy_setopt(dcurl, CURLOPT_URL, request->url);

    if (curl_error != CURLE_OK)
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, curl_easy_strerror(curl_error));
    }

    headers = curl_slist_append(headers, "User-Agent: PHP LASTRPC - " PHP_LASTRPC_VERSION);

    curl_easy_setopt(dcurl, CURLOPT_HTTPHEADER, headers);

    handle->curl = dcurl;
    request->handle          = handle;
    request_handle->prepare  = php_lastrpc_curl_prepare;
    request_handle->exec     = php_lastrpc_curl_exec;
    request_handle->close    = php_lastrpc_curl_close;
}








/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
