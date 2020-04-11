//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_DOTS_CODE_H
#define DOTS_CLIENT_DOTS_CODE_H
#define RequestGet      1
#define RequestPost     2
#define RequestPut      3
#define RequestDelete   4

#define ResponseCreated   65
#define ResponseDeleted   66
#define ResponseValid     67
#define ResponseChanged   68
#define ResponseContent   69

#define ResponseLimit2xx         100

#define ResponseBadRequest           128
#define ResponseUnauthorized         129
#define ResponseBadOption            130
#define ResponseForbidden            131
#define ResponseNotFound             132
#define ResponseMethodNotAllowed     133
#define ResponseNotAcceptable        134
#define ResponseConflict             137
#define ResponsePreconditionFailed   140

#define RequestEntityTooLarge          141
#define ResponseUnsupportedMediaType   143

#define ResponseUnprocessableEntity    150

#define ResponseInternalServerError   160
#define ResponseNotImplemented        161
#define ResponseServiceUnavailable    163

#define ResponseBadGateway             162
#define ResponseGatewayTimeout         164
#define ResponseProxyingNotSupported   165

#endif //DOTS_CLIENT_DOTS_CODE_H
