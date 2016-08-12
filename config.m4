
PHP_ARG_WITH(curl, for curl support,
[  --with-curl[=DIR]             Include curl support])

PHP_ARG_ENABLE(lastrpc, whether to enable lastrpc support,
[  --enable-lastrpc             Enable lastrpc support],yes,no)

PHP_ARG_ENABLE(msgpack, whether to enable msgpack support,
[  --enable-msgpack             Enable msgpack support],no,no)


if test "$PHP_LASTRPC" != "no"; then

    AC_MSG_CHECKING([for cURL in path])

    for dir in $PHP_CURL /usr/include /usr/local/include; do
        if test -f "$dir/curl/easy.h"; then
            CURL_DIR=$dir
            break
        fi
    done

    if test -z "$CURL_DIR"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Please reinstall the libcurl distribution - easy.h should be in <curl-dir>/include/curl/])
    else
        AC_MSG_RESULT([found in $CURL_DIR])
    fi


    PHP_CHECK_LIBRARY(curl,curl_easy_init,
    [
        PHP_EVAL_INCLINE($CURL_DIR)

    ],[
        AC_MSG_ERROR([curl library cannot be used])
    ]
    )

    ifdef([PHP_ADD_EXTENDION_DEP],
    [
      PHP_ADD_EXTENSION_DEP(lastrpc, json, true)
    ])

    AC_MSG_CHECKING([for msgpack support])

    if test "$PHP_MSGPACK" != "no"; then

        if test ! `php -m | grep msgpack`;then
            AC_MSG_ERROR([Please install the msgpack lib])
        fi

        AC_MSG_RESULT([yes])
        AC_DEFINE(ENABLE_MSGPACK,1,[Enable msgpack support])
        ifdef([PHP_ADD_EXTENSION_DEP],
        [
        PHP_ADD_EXTENSION_DEP(lastrpc, msgpack, true)
        ])
    else
        AC_MSG_RESULT([no])
        AC_DEFINE(ENABLE_MSGPACK,0,[enable msgpack packager])
    fi

    PHP_SUBST(LASTRPC_SHARED_LIBADD)
    PHP_NEW_EXTENSION(lastrpc,
    lastrpc.c \
    last/lastrpc_client.c \
    last/lastrpc_server.c \
    last/lastrpc_packager.c \
    last/lastrpc_request.c \
    last/lastrpc_exception.c \
    packager/json.c \
    packager/msgpack.c \
    requester/curl.c,

    $ext_shared)

else
    AC_MSG_ERROR([Please Enable lastrpc support]);
fi
