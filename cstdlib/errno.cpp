/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <errno.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

#ifdef EACCES
static int EACCESValue = EACCES;
#endif

#ifdef EADDRINUSE
static int EADDRINUSEValue = EADDRINUSE;
#endif

#ifdef EADDRNOTAVAIL
static int EADDRNOTAVAILValue = EADDRNOTAVAIL;
#endif

#ifdef EAFNOSUPPORT
static int EAFNOSUPPORTValue = EAFNOSUPPORT;
#endif

#ifdef EAGAIN
static int EAGAINValue = EAGAIN;
#endif

#ifdef EALREADY
static int EALREADYValue = EALREADY;
#endif

#ifdef EBADF
static int EBADFValue = EBADF;
#endif

#ifdef EBADMSG
static int EBADMSGValue = EBADMSG;
#endif

#ifdef EBUSY
static int EBUSYValue = EBUSY;
#endif

#ifdef ECANCELED
static int ECANCELEDValue = ECANCELED;
#endif

#ifdef ECHILD
static int ECHILDValue = ECHILD;
#endif

#ifdef ECONNABORTED
static int ECONNABORTEDValue = ECONNABORTED;
#endif

#ifdef ECONNREFUSED
static int ECONNREFUSEDValue = ECONNREFUSED;
#endif

#ifdef ECONNRESET
static int ECONNRESETValue = ECONNRESET;
#endif

#ifdef EDEADLK
static int EDEADLKValue = EDEADLK;
#endif

#ifdef EDESTADDRREQ
static int EDESTADDRREQValue = EDESTADDRREQ;
#endif

#ifdef EDOM
static int EDOMValue = EDOM;
#endif

#ifdef EDQUOT
static int EDQUOTValue = EDQUOT;
#endif

#ifdef EEXIST
static int EEXISTValue = EEXIST;
#endif

#ifdef EFAULT
static int EFAULTValue = EFAULT;
#endif

#ifdef EFBIG
static int EFBIGValue = EFBIG;
#endif

#ifdef EHOSTUNREACH
static int EHOSTUNREACHValue = EHOSTUNREACH;
#endif

#ifdef EIDRM
static int EIDRMValue = EIDRM;
#endif

#ifdef EILSEQ
static int EILSEQValue = EILSEQ;
#endif

#ifdef EINPROGRESS
static int EINPROGRESSValue = EINPROGRESS;
#endif

#ifdef EINTR
static int EINTRValue = EINTR;
#endif

#ifdef EINVAL
static int EINVALValue = EINVAL;
#endif

#ifdef EIO
static int EIOValue = EIO;
#endif

#ifdef EISCONN
static int EISCONNValue = EISCONN;
#endif

#ifdef EISDIR
static int EISDIRValue = EISDIR;
#endif

#ifdef ELOOP
static int ELOOPValue = ELOOP;
#endif

#ifdef EMFILE
static int EMFILEValue = EMFILE;
#endif

#ifdef EMLINK
static int EMLINKValue = EMLINK;
#endif

#ifdef EMSGSIZE
static int EMSGSIZEValue = EMSGSIZE;
#endif

#ifdef EMULTIHOP
static int EMULTIHOPValue = EMULTIHOP;
#endif

#ifdef ENAMETOOLONG
static int ENAMETOOLONGValue = ENAMETOOLONG;
#endif

#ifdef ENETDOWN
static int ENETDOWNValue = ENETDOWN;
#endif

#ifdef ENETRESET
static int ENETRESETValue = ENETRESET;
#endif

#ifdef ENETUNREACH
static int ENETUNREACHValue = ENETUNREACH;
#endif

#ifdef ENFILE
static int ENFILEValue = ENFILE;
#endif

#ifdef ENOBUFS
static int ENOBUFSValue = ENOBUFS;
#endif

#ifdef ENODATA
static int ENODATAValue = ENODATA;
#endif

#ifdef ENODEV
static int ENODEVValue = ENODEV;
#endif

#ifdef ENOENT
static int ENOENTValue = ENOENT;
#endif

#ifdef ENOEXEC
static int ENOEXECValue = ENOEXEC;
#endif

#ifdef ENOLCK
static int ENOLCKValue = ENOLCK;
#endif

#ifdef ENOLINK
static int ENOLINKValue = ENOLINK;
#endif

#ifdef ENOMEM
static int ENOMEMValue = ENOMEM;
#endif

#ifdef ENOMSG
static int ENOMSGValue = ENOMSG;
#endif

#ifdef ENOPROTOOPT
static int ENOPROTOOPTValue = ENOPROTOOPT;
#endif

#ifdef ENOSPC
static int ENOSPCValue = ENOSPC;
#endif

#ifdef ENOSR
static int ENOSRValue = ENOSR;
#endif

#ifdef ENOSTR
static int ENOSTRValue = ENOSTR;
#endif

#ifdef ENOSYS
static int ENOSYSValue = ENOSYS;
#endif

#ifdef ENOTCONN
static int ENOTCONNValue = ENOTCONN;
#endif

#ifdef ENOTDIR
static int ENOTDIRValue = ENOTDIR;
#endif

#ifdef ENOTEMPTY
static int ENOTEMPTYValue = ENOTEMPTY;
#endif

#ifdef ENOTRECOVERABLE
static int ENOTRECOVERABLEValue = ENOTRECOVERABLE;
#endif

#ifdef ENOTSOCK
static int ENOTSOCKValue = ENOTSOCK;
#endif

#ifdef ENOTSUP
static int ENOTSUPValue = ENOTSUP;
#endif

#ifdef ENOTTY
static int ENOTTYValue = ENOTTY;
#endif

#ifdef ENXIO
static int ENXIOValue = ENXIO;
#endif

#ifdef EOPNOTSUPP
static int EOPNOTSUPPValue = EOPNOTSUPP;
#endif

#ifdef EOVERFLOW
static int EOVERFLOWValue = EOVERFLOW;
#endif

#ifdef EOWNERDEAD
static int EOWNERDEADValue = EOWNERDEAD;
#endif

#ifdef EPERM
static int EPERMValue = EPERM;
#endif

#ifdef EPIPE
static int EPIPEValue = EPIPE;
#endif

#ifdef EPROTO
static int EPROTOValue = EPROTO;
#endif

#ifdef EPROTONOSUPPORT
static int EPROTONOSUPPORTValue = EPROTONOSUPPORT;
#endif

#ifdef EPROTOTYPE
static int EPROTOTYPEValue = EPROTOTYPE;
#endif

#ifdef ERANGE
static int ERANGEValue = ERANGE;
#endif

#ifdef EROFS
static int EROFSValue = EROFS;
#endif

#ifdef ESPIPE
static int ESPIPEValue = ESPIPE;
#endif

#ifdef ESRCH
static int ESRCHValue = ESRCH;
#endif

#ifdef ESTALE
static int ESTALEValue = ESTALE;
#endif

#ifdef ETIME
static int ETIMEValue = ETIME;
#endif

#ifdef ETIMEDOUT
static int ETIMEDOUTValue = ETIMEDOUT;
#endif

#ifdef ETXTBSY
static int ETXTBSYValue = ETXTBSY;
#endif

#ifdef EWOULDBLOCK
static int EWOULDBLOCKValue = EWOULDBLOCK;
#endif

#ifdef EXDEV
static int EXDEVValue = EXDEV;
#endif


/* creates various system-dependent definitions */
void StdErrnoSetupFunc(Picoc *pc)
{
    /* defines */
#ifdef EACCES
    pc->VariableDefinePlatformVar( NULL, "EACCES", &pc->IntType, (UnionAnyValue *)&EACCESValue, FALSE);
#endif

#ifdef EADDRINUSE
    pc->VariableDefinePlatformVar( NULL, "EADDRINUSE", &pc->IntType, (UnionAnyValue *)&EADDRINUSEValue, FALSE);
#endif

#ifdef EADDRNOTAVAIL
    pc->VariableDefinePlatformVar( NULL, "EADDRNOTAVAIL", &pc->IntType, (UnionAnyValue *)&EADDRNOTAVAILValue, FALSE);
#endif

#ifdef EAFNOSUPPORT
    pc->VariableDefinePlatformVar( NULL, "EAFNOSUPPORT", &pc->IntType, (UnionAnyValue *)&EAFNOSUPPORTValue, FALSE);
#endif

#ifdef EAGAIN
    pc->VariableDefinePlatformVar( NULL, "EAGAIN", &pc->IntType, (UnionAnyValue *)&EAGAINValue, FALSE);
#endif

#ifdef EALREADY
    pc->VariableDefinePlatformVar( NULL, "EALREADY", &pc->IntType, (UnionAnyValue *)&EALREADYValue, FALSE);
#endif

#ifdef EBADF
    pc->VariableDefinePlatformVar( NULL, "EBADF", &pc->IntType, (UnionAnyValue *)&EBADFValue, FALSE);
#endif

#ifdef EBADMSG
    pc->VariableDefinePlatformVar( NULL, "EBADMSG", &pc->IntType, (UnionAnyValue *)&EBADMSGValue, FALSE);
#endif

#ifdef EBUSY
    pc->VariableDefinePlatformVar( NULL, "EBUSY", &pc->IntType, (UnionAnyValue *)&EBUSYValue, FALSE);
#endif

#ifdef ECANCELED
    pc->VariableDefinePlatformVar( NULL, "ECANCELED", &pc->IntType, (UnionAnyValue *)&ECANCELEDValue, FALSE);
#endif

#ifdef ECHILD
    pc->VariableDefinePlatformVar( NULL, "ECHILD", &pc->IntType, (UnionAnyValue *)&ECHILDValue, FALSE);
#endif

#ifdef ECONNABORTED
    pc->VariableDefinePlatformVar( NULL, "ECONNABORTED", &pc->IntType, (UnionAnyValue *)&ECONNABORTEDValue, FALSE);
#endif

#ifdef ECONNREFUSED
    pc->VariableDefinePlatformVar( NULL, "ECONNREFUSED", &pc->IntType, (UnionAnyValue *)&ECONNREFUSEDValue, FALSE);
#endif

#ifdef ECONNRESET
    pc->VariableDefinePlatformVar( NULL, "ECONNRESET", &pc->IntType, (UnionAnyValue *)&ECONNRESETValue, FALSE);
#endif

#ifdef EDEADLK
    pc->VariableDefinePlatformVar( NULL, "EDEADLK", &pc->IntType, (UnionAnyValue *)&EDEADLKValue, FALSE);
#endif

#ifdef EDESTADDRREQ
    pc->VariableDefinePlatformVar( NULL, "EDESTADDRREQ", &pc->IntType, (UnionAnyValue *)&EDESTADDRREQValue, FALSE);
#endif

#ifdef EDOM
    pc->VariableDefinePlatformVar( NULL, "EDOM", &pc->IntType, (UnionAnyValue *)&EDOMValue, FALSE);
#endif

#ifdef EDQUOT
    pc->VariableDefinePlatformVar( NULL, "EDQUOT", &pc->IntType, (UnionAnyValue *)&EDQUOTValue, FALSE);
#endif

#ifdef EEXIST
    pc->VariableDefinePlatformVar( NULL, "EEXIST", &pc->IntType, (UnionAnyValue *)&EEXISTValue, FALSE);
#endif

#ifdef EFAULT
    pc->VariableDefinePlatformVar( NULL, "EFAULT", &pc->IntType, (UnionAnyValue *)&EFAULTValue, FALSE);
#endif

#ifdef EFBIG
    pc->VariableDefinePlatformVar( NULL, "EFBIG", &pc->IntType, (UnionAnyValue *)&EFBIGValue, FALSE);
#endif

#ifdef EHOSTUNREACH
    pc->VariableDefinePlatformVar( NULL, "EHOSTUNREACH", &pc->IntType, (UnionAnyValue *)&EHOSTUNREACHValue, FALSE);
#endif

#ifdef EIDRM
    pc->VariableDefinePlatformVar( NULL, "EIDRM", &pc->IntType, (UnionAnyValue *)&EIDRMValue, FALSE);
#endif

#ifdef EILSEQ
    pc->VariableDefinePlatformVar( NULL, "EILSEQ", &pc->IntType, (UnionAnyValue *)&EILSEQValue, FALSE);
#endif

#ifdef EINPROGRESS
    pc->VariableDefinePlatformVar( NULL, "EINPROGRESS", &pc->IntType, (UnionAnyValue *)&EINPROGRESSValue, FALSE);
#endif

#ifdef EINTR
    pc->VariableDefinePlatformVar( NULL, "EINTR", &pc->IntType, (UnionAnyValue *)&EINTRValue, FALSE);
#endif

#ifdef EINVAL
    pc->VariableDefinePlatformVar( NULL, "EINVAL", &pc->IntType, (UnionAnyValue *)&EINVALValue, FALSE);
#endif

#ifdef EIO
    pc->VariableDefinePlatformVar( NULL, "EIO", &pc->IntType, (UnionAnyValue *)&EIOValue, FALSE);
#endif

#ifdef EISCONN
    pc->VariableDefinePlatformVar( NULL, "EISCONN", &pc->IntType, (UnionAnyValue *)&EISCONNValue, FALSE);
#endif

#ifdef EISDIR
    pc->VariableDefinePlatformVar( NULL, "EISDIR", &pc->IntType, (UnionAnyValue *)&EISDIRValue, FALSE);
#endif

#ifdef ELOOP
    pc->VariableDefinePlatformVar( NULL, "ELOOP", &pc->IntType, (UnionAnyValue *)&ELOOPValue, FALSE);
#endif

#ifdef EMFILE
    pc->VariableDefinePlatformVar( NULL, "EMFILE", &pc->IntType, (UnionAnyValue *)&EMFILEValue, FALSE);
#endif

#ifdef EMLINK
    pc->VariableDefinePlatformVar( NULL, "EMLINK", &pc->IntType, (UnionAnyValue *)&EMLINKValue, FALSE);
#endif

#ifdef EMSGSIZE
    pc->VariableDefinePlatformVar( NULL, "EMSGSIZE", &pc->IntType, (UnionAnyValue *)&EMSGSIZEValue, FALSE);
#endif

#ifdef EMULTIHOP
    pc->VariableDefinePlatformVar( NULL, "EMULTIHOP", &pc->IntType, (UnionAnyValue *)&EMULTIHOPValue, FALSE);
#endif

#ifdef ENAMETOOLONG
    pc->VariableDefinePlatformVar( NULL, "ENAMETOOLONG", &pc->IntType, (UnionAnyValue *)&ENAMETOOLONGValue, FALSE);
#endif

#ifdef ENETDOWN
    pc->VariableDefinePlatformVar( NULL, "ENETDOWN", &pc->IntType, (UnionAnyValue *)&ENETDOWNValue, FALSE);
#endif

#ifdef ENETRESET
    pc->VariableDefinePlatformVar( NULL, "ENETRESET", &pc->IntType, (UnionAnyValue *)&ENETRESETValue, FALSE);
#endif

#ifdef ENETUNREACH
    pc->VariableDefinePlatformVar( NULL, "ENETUNREACH", &pc->IntType, (UnionAnyValue *)&ENETUNREACHValue, FALSE);
#endif

#ifdef ENFILE
    pc->VariableDefinePlatformVar( NULL, "ENFILE", &pc->IntType, (UnionAnyValue *)&ENFILEValue, FALSE);
#endif

#ifdef ENOBUFS
    pc->VariableDefinePlatformVar( NULL, "ENOBUFS", &pc->IntType, (UnionAnyValue *)&ENOBUFSValue, FALSE);
#endif

#ifdef ENODATA
    pc->VariableDefinePlatformVar( NULL, "ENODATA", &pc->IntType, (UnionAnyValue *)&ENODATAValue, FALSE);
#endif

#ifdef ENODEV
    pc->VariableDefinePlatformVar( NULL, "ENODEV", &pc->IntType, (UnionAnyValue *)&ENODEVValue, FALSE);
#endif

#ifdef ENOENT
    pc->VariableDefinePlatformVar( NULL, "ENOENT", &pc->IntType, (UnionAnyValue *)&ENOENTValue, FALSE);
#endif

#ifdef ENOEXEC
    pc->VariableDefinePlatformVar( NULL, "ENOEXEC", &pc->IntType, (UnionAnyValue *)&ENOEXECValue, FALSE);
#endif

#ifdef ENOLCK
    pc->VariableDefinePlatformVar( NULL, "ENOLCK", &pc->IntType, (UnionAnyValue *)&ENOLCKValue, FALSE);
#endif

#ifdef ENOLINK
    pc->VariableDefinePlatformVar( NULL, "ENOLINK", &pc->IntType, (UnionAnyValue *)&ENOLINKValue, FALSE);
#endif

#ifdef ENOMEM
    pc->VariableDefinePlatformVar( NULL, "ENOMEM", &pc->IntType, (UnionAnyValue *)&ENOMEMValue, FALSE);
#endif

#ifdef ENOMSG
    pc->VariableDefinePlatformVar( NULL, "ENOMSG", &pc->IntType, (UnionAnyValue *)&ENOMSGValue, FALSE);
#endif

#ifdef ENOPROTOOPT
    pc->VariableDefinePlatformVar( NULL, "ENOPROTOOPT", &pc->IntType, (UnionAnyValue *)&ENOPROTOOPTValue, FALSE);
#endif

#ifdef ENOSPC
    pc->VariableDefinePlatformVar( NULL, "ENOSPC", &pc->IntType, (UnionAnyValue *)&ENOSPCValue, FALSE);
#endif

#ifdef ENOSR
    pc->VariableDefinePlatformVar( NULL, "ENOSR", &pc->IntType, (UnionAnyValue *)&ENOSRValue, FALSE);
#endif

#ifdef ENOSTR
    pc->VariableDefinePlatformVar( NULL, "ENOSTR", &pc->IntType, (UnionAnyValue *)&ENOSTRValue, FALSE);
#endif

#ifdef ENOSYS
    pc->VariableDefinePlatformVar( NULL, "ENOSYS", &pc->IntType, (UnionAnyValue *)&ENOSYSValue, FALSE);
#endif

#ifdef ENOTCONN
    pc->VariableDefinePlatformVar( NULL, "ENOTCONN", &pc->IntType, (UnionAnyValue *)&ENOTCONNValue, FALSE);
#endif

#ifdef ENOTDIR
    pc->VariableDefinePlatformVar( NULL, "ENOTDIR", &pc->IntType, (UnionAnyValue *)&ENOTDIRValue, FALSE);
#endif

#ifdef ENOTEMPTY
    pc->VariableDefinePlatformVar( NULL, "ENOTEMPTY", &pc->IntType, (UnionAnyValue *)&ENOTEMPTYValue, FALSE);
#endif

#ifdef ENOTRECOVERABLE
    pc->VariableDefinePlatformVar( NULL, "ENOTRECOVERABLE", &pc->IntType, (UnionAnyValue *)&ENOTRECOVERABLEValue, FALSE);
#endif

#ifdef ENOTSOCK
    pc->VariableDefinePlatformVar( NULL, "ENOTSOCK", &pc->IntType, (UnionAnyValue *)&ENOTSOCKValue, FALSE);
#endif

#ifdef ENOTSUP
    pc->VariableDefinePlatformVar( NULL, "ENOTSUP", &pc->IntType, (UnionAnyValue *)&ENOTSUPValue, FALSE);
#endif

#ifdef ENOTTY
    pc->VariableDefinePlatformVar( NULL, "ENOTTY", &pc->IntType, (UnionAnyValue *)&ENOTTYValue, FALSE);
#endif

#ifdef ENXIO
    pc->VariableDefinePlatformVar( NULL, "ENXIO", &pc->IntType, (UnionAnyValue *)&ENXIOValue, FALSE);
#endif

#ifdef EOPNOTSUPP
    pc->VariableDefinePlatformVar( NULL, "EOPNOTSUPP", &pc->IntType, (UnionAnyValue *)&EOPNOTSUPPValue, FALSE);
#endif

#ifdef EOVERFLOW
    pc->VariableDefinePlatformVar( NULL, "EOVERFLOW", &pc->IntType, (UnionAnyValue *)&EOVERFLOWValue, FALSE);
#endif

#ifdef EOWNERDEAD
    pc->VariableDefinePlatformVar( NULL, "EOWNERDEAD", &pc->IntType, (UnionAnyValue *)&EOWNERDEADValue, FALSE);
#endif

#ifdef EPERM
    pc->VariableDefinePlatformVar( NULL, "EPERM", &pc->IntType, (UnionAnyValue *)&EPERMValue, FALSE);
#endif

#ifdef EPIPE
    pc->VariableDefinePlatformVar( NULL, "EPIPE", &pc->IntType, (UnionAnyValue *)&EPIPEValue, FALSE);
#endif

#ifdef EPROTO
    pc->VariableDefinePlatformVar( NULL, "EPROTO", &pc->IntType, (UnionAnyValue *)&EPROTOValue, FALSE);
#endif

#ifdef EPROTONOSUPPORT
    pc->VariableDefinePlatformVar( NULL, "EPROTONOSUPPORT", &pc->IntType, (UnionAnyValue *)&EPROTONOSUPPORTValue, FALSE);
#endif

#ifdef EPROTOTYPE
    pc->VariableDefinePlatformVar( NULL, "EPROTOTYPE", &pc->IntType, (UnionAnyValue *)&EPROTOTYPEValue, FALSE);
#endif

#ifdef ERANGE
    pc->VariableDefinePlatformVar( NULL, "ERANGE", &pc->IntType, (UnionAnyValue *)&ERANGEValue, FALSE);
#endif

#ifdef EROFS
    pc->VariableDefinePlatformVar( NULL, "EROFS", &pc->IntType, (UnionAnyValue *)&EROFSValue, FALSE);
#endif

#ifdef ESPIPE
    pc->VariableDefinePlatformVar( NULL, "ESPIPE", &pc->IntType, (UnionAnyValue *)&ESPIPEValue, FALSE);
#endif

#ifdef ESRCH
    pc->VariableDefinePlatformVar( NULL, "ESRCH", &pc->IntType, (UnionAnyValue *)&ESRCHValue, FALSE);
#endif

#ifdef ESTALE
    pc->VariableDefinePlatformVar( NULL, "ESTALE", &pc->IntType, (UnionAnyValue *)&ESTALEValue, FALSE);
#endif

#ifdef ETIME
    pc->VariableDefinePlatformVar( NULL, "ETIME", &pc->IntType, (UnionAnyValue *)&ETIMEValue, FALSE);
#endif

#ifdef ETIMEDOUT
    pc->VariableDefinePlatformVar( NULL, "ETIMEDOUT", &pc->IntType, (UnionAnyValue *)&ETIMEDOUTValue, FALSE);
#endif

#ifdef ETXTBSY
    pc->VariableDefinePlatformVar( NULL, "ETXTBSY", &pc->IntType, (UnionAnyValue *)&ETXTBSYValue, FALSE);
#endif

#ifdef EWOULDBLOCK
    pc->VariableDefinePlatformVar( NULL, "EWOULDBLOCK", &pc->IntType, (UnionAnyValue *)&EWOULDBLOCKValue, FALSE);
#endif

#ifdef EXDEV
    pc->VariableDefinePlatformVar( NULL, "EXDEV", &pc->IntType, (UnionAnyValue *)&EXDEVValue, FALSE);
#endif

    pc->VariableDefinePlatformVar( NULL, "errno", &pc->IntType, (UnionAnyValue *)&errno, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */
