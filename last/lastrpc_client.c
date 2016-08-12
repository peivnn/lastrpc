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

#include "lastrpc_client.h"
#include "lastrpc_request.h"

zend_class_entry *lastrpc_client_zce;


static zval * lastrpc_client_handle(zval *client, char *method, zval *arguments TSRMLS_DC)
{

    // url, 数据传输格式, 传输协议, 返回值
    zval *url, *protocol, *retval, *params;

    url      = zend_read_property(lastrpc_client_zce, client, ZEND_STRL("url"),        0 TSRMLS_CC);
    protocol = zend_read_property(lastrpc_client_zce, client, ZEND_STRL("protocol")  , 0 TSRMLS_CC);

    ALLOC_INIT_ZVAL(params);
    array_init(params);
    add_assoc_string(params, "m", method, 1);
    add_assoc_zval(params,   "p", arguments);

    retval = lastrpc_request_call(Z_STRVAL_P(url), Z_LVAL_P(protocol), LASTRPC_G(cntimeout), LASTRPC_G(rwtimeout), params TSRMLS_CC);

    return retval;
}


PHP_METHOD(client,__construct)
{
    char *url;
    int url_len;
    ulong protocol = 0;
    zval *protocol_p;
    char *strurl;


    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &url, &url_len, &protocol) == FAILURE)
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "URL must be a string");
		return;
	}

    strurl = malloc(url_len);

    if ((strurl = strstr(url,"https://")) != NULL)
    {
        url = strurl;
    }
    else if((strurl = strstr(url,"http://")) != NULL)
    {
        url = strurl;
    }
    else if((strurl = strstr(url,"tcp://")) != NULL)
    {
        // 暂时不支持TCP协议
        // url = strurl;
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "temporary does not support TCP protocol");
    }
    else
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "unsupported protocol url '%s'", url);
    }

    zend_update_property_stringl(lastrpc_client_zce,getThis(),ZEND_STRL("url"),url,url_len TSRMLS_CC);

    switch (protocol) {
        case 0:
            protocol_p = zend_read_property(lastrpc_client_zce, getThis(), ZEND_STRL("protocol"), 0 TSRMLS_CC);
            protocol   = Z_LVAL_P(protocol_p);
            break;
        case LASTRPC_PROTOCOL_HTTP:
        case LASTRPC_PROTOCOL_TCP:
            break;
        default:
            protocol = LASTRPC_PROTOCOL_HTTP;
    }
    zend_update_property_long(lastrpc_client_zce,getThis(),ZEND_STRL("protocol"),protocol TSRMLS_CC);
}

PHP_METHOD(client,__destruct)
{
    if (zend_parse_parameters_none() == FAILURE)
    {
        return;
    }
    php_lastrpc_request_close(TSRMLS_C);
}

PHP_METHOD(client,__call)
{
    char *method;
    int method_len;
    zval *arguments, *result, *protocol;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &method, &method_len, &arguments) == FAILURE)
    {
		return;
	}

    if (result = lastrpc_client_handle(getThis(), method, arguments TSRMLS_CC))
    {
        RETURN_ZVAL(result, 1, 0);
    }

    RETURN_FALSE;
}

PHP_METHOD(client,_debug)
{
    if (zend_parse_parameters_none() == FAILURE)
    {
        return;
    }

    zval *resdata;

    ALLOC_INIT_ZVAL(resdata);
    array_init(resdata);
    add_assoc_double(resdata, "rid",           request->rid);
    add_assoc_zval(resdata,   "arguments",     request->arguments);
    add_assoc_string(resdata, "request_body",  request->request_body.c,  1);
    add_assoc_string(resdata, "response_body", request->response_body.c, 1);

    RETURN_ZVAL(resdata, 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arguments_call, 0, 0, 2)
	ZEND_ARG_INFO(0, method)
    ZEND_ARG_INFO(0, arguments)
ZEND_END_ARG_INFO()

static zend_function_entry lastrpc_client_methods[] = {
    // ZEND_ACC_CTOR和ZEND_ACC_DTOR为特殊标记, 分别表示构造函数与析构函数
    // 1. 类名  2. 方法名  3. 参数声明绑定名称  4.方法类型
	PHP_ME(client, __construct,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR | ZEND_ACC_FINAL)
    PHP_ME(client, __destruct,       NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR | ZEND_ACC_FINAL)
    PHP_ME(client, __call, arguments_call, ZEND_ACC_PUBLIC)
    PHP_ME(client, _debug,           NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};


void lastrpc_client_init(TSRMLS_D)
{
    zend_class_entry zce;

    INIT_CLASS_ENTRY(zce,"lastrpc\\Client",lastrpc_client_methods);

    lastrpc_client_zce = zend_register_internal_class(&zce TSRMLS_CC);

    zend_declare_property_long(lastrpc_client_zce,ZEND_STRL("protocol"),LASTRPC_PROTOCOL_HTTP,ZEND_ACC_PUBLIC TSRMLS_CC);

    zend_declare_property_string(lastrpc_client_zce,ZEND_STRL("url"),"",ZEND_ACC_PROTECTED TSRMLS_CC);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
