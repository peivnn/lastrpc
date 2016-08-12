/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Feng Qui  <peivnn@foxmail.com>                               |
  +----------------------------------------------------------------------+
*/
#ifndef PHP_LASTRPC_REQUEST_H
#define PHP_LASTRPC_REQUEST_H

#include "ext/standard/php_smart_str.h"

// 请求结构体
typedef struct _lastrpc_request
{
    double    rid;
    uint      method_len;
    zval     *arguments;
    smart_str request_body;
    smart_str response_body;
    void     *handle;
    char     *url;
    ulong     cntimeout;
    ulong     rwtimeout;

} lastrpc_request_t;


typedef struct _lastrpc_request_handle
{
    void   (*prepare) (struct _lastrpc_request *request TSRMLS_DC);
    void   (*exec)    (struct _lastrpc_request *request TSRMLS_DC);
    void   (*close)   (struct _lastrpc_request *request TSRMLS_DC);
} lastrpc_request_handle_t;


zval * lastrpc_request_call(char *url, ulong protocol,ulong cntimeout, ulong rwtimeout, zval *params TSRMLS_DC);
void php_lastrpc_request_close(TSRMLS_D);

extern lastrpc_request_t *request;

#endif /* PHP_LASTRPC_CONFIG_H */
