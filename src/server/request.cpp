#include "request.h"

#define DEMO_PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo!!</body></html>"


Request::Request( MHD_Connection *connection )
    : QObject()
    , mConnection( connection )
{

}

void Request::HandleRequest()
{
    SendResponse( DEMO_PAGE, "text/html", MHD_HTTP_OK );
}

int Request::SendResponse(
        const char *responseData
        , const char *responseType
        , int statusCode )
{
    int ret;
    struct MHD_Response *response;

    response = MHD_create_response_from_buffer(
                strlen( responseData )
                , (void*)( responseData )
                , MHD_RESPMEM_MUST_COPY );

    if( !response ) {
        return MHD_NO;
    }
    MHD_add_response_header( response
                             , MHD_HTTP_HEADER_CONTENT_TYPE
                             , responseType );
    ret = MHD_queue_response( mConnection, statusCode, response );
    MHD_destroy_response( response );

    return ret;
}

const char* Request::GetBody()
{
    return mBody.toLatin1().data();
}
