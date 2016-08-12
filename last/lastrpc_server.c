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
#include "SAPI.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_lcg.h"

#include "php_lastrpc.h"

#include "lastrpc_packager.h"
#include "lastrpc_server.h"
#include "lastrpc_exception.h"

zend_class_entry *lastrpc_server_zce;


lastrpc_server_data_t * php_lastrpc_server_hash_get(zval *postzval TSRMLS_DC)
{
    lastrpc_server_data_t *server_data;
	zval **ppzval;
	HashTable *ht;

	server_data = (lastrpc_server_data_t *)ecalloc(sizeof(lastrpc_server_data_t), 1);

    if (Z_TYPE_P(postzval) != IS_ARRAY)
    {
        return server_data;
    }

    ht = Z_ARRVAL_P(postzval);
    if (zend_hash_find(ht, "m", sizeof("m"), (void**)&ppzval) == SUCCESS)
    {
		if (IS_STRING != Z_TYPE_PP(ppzval))
        {
			convert_to_string(*ppzval);
		}
		server_data->method = Z_STRVAL_PP(ppzval);
        server_data->method_len = Z_STRLEN_PP(ppzval);
        ZVAL_NULL(*ppzval);
	}

    if (zend_hash_find(ht, "p", sizeof("p"), (void**)&ppzval) == SUCCESS)
    {
		if (IS_ARRAY != Z_TYPE_PP(ppzval))
        {
			convert_to_array(*ppzval);
		}
		Z_ADDREF_P(*ppzval);
		server_data->params = *ppzval;
	}

    return server_data;
}


static void lastrpc_server_response(lastrpc_server_data_t *response_data TSRMLS_DC)
{

    sapi_header_line ctr = {0};
	char header_line[512];

	ctr.line_len = snprintf(header_line, sizeof(header_line), "Content-Length: %ld", response_data->response_len);
	ctr.line = header_line;
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);

	ctr.line_len = snprintf(header_line, sizeof(header_line), "Cache-Control: no-cache");
	ctr.line = header_line;
	sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
    php_header(TSRMLS_C);

    PHPWRITE(response_data->response, response_data->response_len);

    efree(response_data);
}


static void lastrpc_server_handle(zval *object TSRMLS_DC)
{
    zval *post_data = NULL;
    zend_class_entry *zce;
    char method[256], *post_body, *response_body;
    size_t post_body_len;
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 5
	php_stream *post_stream;
	smart_str st_post_data = {0};
#endif

    lastrpc_server_data_t *server_data = NULL;

    lastrpc_data_t *packager = php_lastrpc_packager_get(LASTRPC_G(packager) TSRMLS_CC);
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 5
    post_stream = SG(request_info).request_body;

	if (!post_stream || FAILURE == php_stream_rewind(post_stream))
    {
        zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "request is empty");
	}

	while (!php_stream_eof(post_stream))
    {
		char buf[512];

		size_t post_stream_len = php_stream_read(post_stream, buf, sizeof(buf));

		if (post_stream_len && post_stream_len != (size_t) -1)
        {
			smart_str_appendl(&st_post_data, buf, post_stream_len);
		}
	}
    post_body = st_post_data.c;
    post_body_len = st_post_data.len;
#else
    post_body = SG(request_info).raw_post_data;
	post_body_len = SG(request_info).raw_post_data_length;
#endif

    post_data = packager->decode(post_body, post_body_len TSRMLS_CC);

    server_data = php_lastrpc_server_hash_get(post_data TSRMLS_CC);

    if (!server_data->method)
    {
        zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "request method is empty");
        return;
    }

    if (!server_data->params)
    {
        zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "request params is empty");
        return ;
    }

    zce = Z_OBJCE_P(object);
    zend_str_tolower_copy(method, server_data->method, server_data->method_len);

    if (!zend_hash_exists(&zce->function_table, method, strlen(method) + 1))
    {
        zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "call to undefined api %s::%s()", zce->name, server_data->method);
        return ;
    }

    zend_bool call_fault = 0;
    zval output, func;

    server_data->response = "";
    server_data->response_len = 0;

    zend_try
    {
		uint count;
		zval ***func_params;
		zval *return_val = NULL;
		HashTable *fp_ht;

        INIT_ZVAL(output);

        fp_ht = Z_ARRVAL_P(server_data->params);
		count = zend_hash_num_elements(fp_ht);

		if (count)
        {
			uint i = 0;
			func_params = emalloc(sizeof(zval **) * count);
			for
            (
                zend_hash_internal_pointer_reset(fp_ht);
				zend_hash_get_current_data(fp_ht, (void **) &func_params[i++]) == SUCCESS;
				zend_hash_move_forward(fp_ht)
			);
		}
        else
        {
			func_params = NULL;
		}

		ZVAL_STRINGL(&func, server_data->method, server_data->method_len, 0);

		if (call_user_function_ex(NULL, &object, &func, &return_val, count, func_params, 0, NULL TSRMLS_CC) != SUCCESS)
        {
			if (func_params)
            {
				efree(func_params);
			}
		    zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "call to api %s::%s() failed", zce->name, server_data->method);
		}

		if (func_params)
        {
			efree(func_params);
		}

		if (return_val)
        {
            smart_str response_buf = {0};
            packager->encode(return_val, &response_buf TSRMLS_CC);
            server_data->response = response_buf.c;
            server_data->response_len = response_buf.len;
		}
    }
    zend_catch
    {
		call_fault = 1;
	}
    zend_end_try();

    lastrpc_server_response(server_data TSRMLS_CC);

    if (call_fault)
    {
		zend_bailout();
	}
}





PHP_METHOD(server,__construct)
{
    zval *object;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE)
    {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "handle not the object");
		return;
	}

    zend_update_property(lastrpc_server_zce, getThis(), "_handler", sizeof("_handler")-1, object TSRMLS_CC);
}

PHP_METHOD(server,handle)
{
    zval *handler;
    const char *method;

    handler = zend_read_property(lastrpc_server_zce, getThis(), ZEND_STRL("_handler"), 0 TSRMLS_CC);

    if (!handler || Z_TYPE_P(handler) != IS_OBJECT)
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "handler not the object");
        RETURN_FALSE;
    }

    method = SG(request_info).request_method;

    if (method && strcmp(method,"POST") == 0)
    {
        lastrpc_server_handle(handler TSRMLS_CC);
    }
    // zend_throw_exception_ex(lastrpc_exception_zce, 0 TSRMLS_CC, "request not the POST [%s]", method);
    RETURN_TRUE;
}

ZEND_BEGIN_ARG_INFO_EX(arguments_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static zend_function_entry lastrpc_server_methods[] = {
	PHP_ME(server, __construct, arguments_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR | ZEND_ACC_FINAL)
    PHP_ME(server, handle,      NULL,                ZEND_ACC_PUBLIC)
	PHP_FE_END
};

void lastrpc_server_init(TSRMLS_D)
{
    zend_class_entry zce;

    INIT_CLASS_ENTRY(zce,"lastrpc\\Server",lastrpc_server_methods);

    lastrpc_server_zce = zend_register_internal_class(&zce TSRMLS_CC);

    zend_declare_property_null(lastrpc_server_zce,ZEND_STRL("_handler"), ZEND_ACC_PROTECTED TSRMLS_CC);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
