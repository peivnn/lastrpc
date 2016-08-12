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
#include <zend_exceptions.h>

#include "php_lastrpc.h"

#include "lastrpc_packager.h"
#include "lastrpc_client.h"
#include "lastrpc_request.h"
#include "lastrpc_exception.h"

lastrpc_request_handle_t *request_handle;
lastrpc_request_t *request;

zval * lastrpc_request_call(char *url, ulong protocol, ulong cntimeout, ulong rwtimeout, zval *params TSRMLS_DC)
{
    request = ecalloc(1, sizeof(lastrpc_request_t));
    request_handle = ecalloc(1, sizeof(lastrpc_request_handle_t));
    // 构建请求ID
    struct timeval tp = {0};
    gettimeofday(&tp, NULL);
    request->rid       = (double)(tp.tv_sec + tp.tv_usec/1000000.000000);
    request->url       = url;
    request->cntimeout = cntimeout;
    request->rwtimeout = rwtimeout;

    lastrpc_data_t * packager = php_lastrpc_packager_get(LASTRPC_G(packager) TSRMLS_CC);

    if (params)
    {
        smart_str buf = {0};
        request->arguments = params;
        packager->encode(params, &buf TSRMLS_CC);
        request->request_body = buf;
    }

    if (!request->handle)
    {
        // 当前只支持HTTP协议
        php_lastrpc_curl_init(request_handle, request TSRMLS_CC);
        // switch (protocol)
        // {
        //     case LASTRPC_PROTOCOL_HTTP:
        //         php_lastrpc_curl_init(request_handle, request TSRMLS_CC);
        //         break;
        //     case LASTRPC_PROTOCOL_TCP:
        //         php_lastrpc_tcp_init(request_handle, request TSRMLS_CC);
        //         break;
        //     default:
        //         return NULL;
        // }
    }

    request_handle->prepare(request TSRMLS_CC);
    request_handle->exec(request TSRMLS_CC);

    zval * resval = packager->decode(request->response_body.c, request->response_body.len TSRMLS_CC);

    return resval;
}

void php_lastrpc_request_close(TSRMLS_D)
{
    if (request_handle)
    {
        request_handle->close(request TSRMLS_CC);
    }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
