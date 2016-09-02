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
    pc->VariableDefinePlatformVar( NULL, "EACCES", &pc->IntType, (UnionAnyValuePointer )&EACCESValue, FALSE);
#endif

#ifdef EADDRINUSE
    pc->VariableDefinePlatformVar( NULL, "EADDRINUSE", &pc->IntType, (UnionAnyValuePointer )&EADDRINUSEValue, FALSE);
#endif

#ifdef EADDRNOTAVAIL
    pc->VariableDefinePlatformVar( NULL, "EADDRNOTAVAIL", &pc->IntType, (UnionAnyValuePointer )&EADDRNOTAVAILValue, FALSE);
#endif

#ifdef EAFNOSUPPORT
    pc->VariableDefinePlatformVar( NULL, "EAFNOSUPPORT", &pc->IntType, (UnionAnyValuePointer )&EAFNOSUPPORTValue, FALSE);
#endif

#ifdef EAGAIN
    pc->VariableDefinePlatformVar( NULL, "EAGAIN", &pc->IntType, (UnionAnyValuePointer )&EAGAINValue, FALSE);
#endif

#ifdef EALREADY
    pc->VariableDefinePlatformVar( NULL, "EALREADY", &pc->IntType, (UnionAnyValuePointer )&EALREADYValue, FALSE);
#endif

#ifdef EBADF
    pc->VariableDefinePlatformVar( NULL, "EBADF", &pc->IntType, (UnionAnyValuePointer )&EBADFValue, FALSE);
#endif

#ifdef EBADMSG
    pc->VariableDefinePlatformVar( NULL, "EBADMSG", &pc->IntType, (UnionAnyValuePointer )&EBADMSGValue, FALSE);
#endif

#ifdef EBUSY
    pc->VariableDefinePlatformVar( NULL, "EBUSY", &pc->IntType, (UnionAnyValuePointer )&EBUSYValue, FALSE);
#endif

#ifdef ECANCELED
    pc->VariableDefinePlatformVar( NULL, "ECANCELED", &pc->IntType, (UnionAnyValuePointer )&ECANCELEDValue, FALSE);
#endif

#ifdef ECHILD
    pc->VariableDefinePlatformVar( NULL, "ECHILD", &pc->IntType, (UnionAnyValuePointer )&ECHILDValue, FALSE);
#endif

#ifdef ECONNABORTED
    pc->VariableDefinePlatformVar( NULL, "ECONNABORTED", &pc->IntType, (UnionAnyValuePointer )&ECONNABORTEDValue, FALSE);
#endif

#ifdef ECONNREFUSED
    pc->VariableDefinePlatformVar( NULL, "ECONNREFUSED", &pc->IntType, (UnionAnyValuePointer )&ECONNREFUSEDValue, FALSE);
#endif

#ifdef ECONNRESET
    pc->VariableDefinePlatformVar( NULL, "ECONNRESET", &pc->IntType, (UnionAnyValuePointer )&ECONNRESETValue, FALSE);
#endif

#ifdef EDEADLK
    pc->VariableDefinePlatformVar( NULL, "EDEADLK", &pc->IntType, (UnionAnyValuePointer )&EDEADLKValue, FALSE);
#endif

#ifdef EDESTADDRREQ
    pc->VariableDefinePlatformVar( NULL, "EDESTADDRREQ", &pc->IntType, (UnionAnyValuePointer )&EDESTADDRREQValue, FALSE);
#endif

#ifdef EDOM
    pc->VariableDefinePlatformVar( NULL, "EDOM", &pc->IntType, (UnionAnyValuePointer )&EDOMValue, FALSE);
#endif

#ifdef EDQUOT
    pc->VariableDefinePlatformVar( NULL, "EDQUOT", &pc->IntType, (UnionAnyValuePointer )&EDQUOTValue, FALSE);
#endif

#ifdef EEXIST
    pc->VariableDefinePlatformVar( NULL, "EEXIST", &pc->IntType, (UnionAnyValuePointer )&EEXISTValue, FALSE);
#endif

#ifdef EFAULT
    pc->VariableDefinePlatformVar( NULL, "EFAULT", &pc->IntType, (UnionAnyValuePointer )&EFAULTValue, FALSE);
#endif

#ifdef EFBIG
    pc->VariableDefinePlatformVar( NULL, "EFBIG", &pc->IntType, (UnionAnyValuePointer )&EFBIGValue, FALSE);
#endif

#ifdef EHOSTUNREACH
    pc->VariableDefinePlatformVar( NULL, "EHOSTUNREACH", &pc->IntType, (UnionAnyValuePointer )&EHOSTUNREACHValue, FALSE);
#endif

#ifdef EIDRM
    pc->VariableDefinePlatformVar( NULL, "EIDRM", &pc->IntType, (UnionAnyValuePointer )&EIDRMValue, FALSE);
#endif

#ifdef EILSEQ
    pc->VariableDefinePlatformVar( NULL, "EILSEQ", &pc->IntType, (UnionAnyValuePointer )&EILSEQValue, FALSE);
#endif

#ifdef EINPROGRESS
    pc->VariableDefinePlatformVar( NULL, "EINPROGRESS", &pc->IntType, (UnionAnyValuePointer )&EINPROGRESSValue, FALSE);
#endif

#ifdef EINTR
    pc->VariableDefinePlatformVar( NULL, "EINTR", &pc->IntType, (UnionAnyValuePointer )&EINTRValue, FALSE);
#endif

#ifdef EINVAL
    pc->VariableDefinePlatformVar( NULL, "EINVAL", &pc->IntType, (UnionAnyValuePointer )&EINVALValue, FALSE);
#endif

#ifdef EIO
    pc->VariableDefinePlatformVar( NULL, "EIO", &pc->IntType, (UnionAnyValuePointer )&EIOValue, FALSE);
#endif

#ifdef EISCONN
    pc->VariableDefinePlatformVar( NULL, "EISCONN", &pc->IntType, (UnionAnyValuePointer )&EISCONNValue, FALSE);
#endif

#ifdef EISDIR
    pc->VariableDefinePlatformVar( NULL, "EISDIR", &pc->IntType, (UnionAnyValuePointer )&EISDIRValue, FALSE);
#endif

#ifdef ELOOP
    pc->VariableDefinePlatformVar( NULL, "ELOOP", &pc->IntType, (UnionAnyValuePointer )&ELOOPValue, FALSE);
#endif

#ifdef EMFILE
    pc->VariableDefinePlatformVar( NULL, "EMFILE", &pc->IntType, (UnionAnyValuePointer )&EMFILEValue, FALSE);
#endif

#ifdef EMLINK
    pc->VariableDefinePlatformVar( NULL, "EMLINK", &pc->IntType, (UnionAnyValuePointer )&EMLINKValue, FALSE);
#endif

#ifdef EMSGSIZE
    pc->VariableDefinePlatformVar( NULL, "EMSGSIZE", &pc->IntType, (UnionAnyValuePointer )&EMSGSIZEValue, FALSE);
#endif

#ifdef EMULTIHOP
    pc->VariableDefinePlatformVar( NULL, "EMULTIHOP", &pc->IntType, (UnionAnyValuePointer )&EMULTIHOPValue, FALSE);
#endif

#ifdef ENAMETOOLONG
    pc->VariableDefinePlatformVar( NULL, "ENAMETOOLONG", &pc->IntType, (UnionAnyValuePointer )&ENAMETOOLONGValue, FALSE);
#endif

#ifdef ENETDOWN
    pc->VariableDefinePlatformVar( NULL, "ENETDOWN", &pc->IntType, (UnionAnyValuePointer )&ENETDOWNValue, FALSE);
#endif

#ifdef ENETRESET
    pc->VariableDefinePlatformVar( NULL, "ENETRESET", &pc->IntType, (UnionAnyValuePointer )&ENETRESETValue, FALSE);
#endif

#ifdef ENETUNREACH
    pc->VariableDefinePlatformVar( NULL, "ENETUNREACH", &pc->IntType, (UnionAnyValuePointer )&ENETUNREACHValue, FALSE);
#endif

#ifdef ENFILE
    pc->VariableDefinePlatformVar( NULL, "ENFILE", &pc->IntType, (UnionAnyValuePointer )&ENFILEValue, FALSE);
#endif

#ifdef ENOBUFS
    pc->VariableDefinePlatformVar( NULL, "ENOBUFS", &pc->IntType, (UnionAnyValuePointer )&ENOBUFSValue, FALSE);
#endif

#ifdef ENODATA
    pc->VariableDefinePlatformVar( NULL, "ENODATA", &pc->IntType, (UnionAnyValuePointer )&ENODATAValue, FALSE);
#endif

#ifdef ENODEV
    pc->VariableDefinePlatformVar( NULL, "ENODEV", &pc->IntType, (UnionAnyValuePointer )&ENODEVValue, FALSE);
#endif

#ifdef ENOENT
    pc->VariableDefinePlatformVar( NULL, "ENOENT", &pc->IntType, (UnionAnyValuePointer )&ENOENTValue, FALSE);
#endif

#ifdef ENOEXEC
    pc->VariableDefinePlatformVar( NULL, "ENOEXEC", &pc->IntType, (UnionAnyValuePointer )&ENOEXECValue, FALSE);
#endif

#ifdef ENOLCK
    pc->VariableDefinePlatformVar( NULL, "ENOLCK", &pc->IntType, (UnionAnyValuePointer )&ENOLCKValue, FALSE);
#endif

#ifdef ENOLINK
    pc->VariableDefinePlatformVar( NULL, "ENOLINK", &pc->IntType, (UnionAnyValuePointer )&ENOLINKValue, FALSE);
#endif

#ifdef ENOMEM
    pc->VariableDefinePlatformVar( NULL, "ENOMEM", &pc->IntType, (UnionAnyValuePointer )&ENOMEMValue, FALSE);
#endif

#ifdef ENOMSG
    pc->VariableDefinePlatformVar( NULL, "ENOMSG", &pc->IntType, (UnionAnyValuePointer )&ENOMSGValue, FALSE);
#endif

#ifdef ENOPROTOOPT
    pc->VariableDefinePlatformVar( NULL, "ENOPROTOOPT", &pc->IntType, (UnionAnyValuePointer )&ENOPROTOOPTValue, FALSE);
#endif

#ifdef ENOSPC
    pc->VariableDefinePlatformVar( NULL, "ENOSPC", &pc->IntType, (UnionAnyValuePointer )&ENOSPCValue, FALSE);
#endif

#ifdef ENOSR
    pc->VariableDefinePlatformVar( NULL, "ENOSR", &pc->IntType, (UnionAnyValuePointer )&ENOSRValue, FALSE);
#endif

#ifdef ENOSTR
    pc->VariableDefinePlatformVar( NULL, "ENOSTR", &pc->IntType, (UnionAnyValuePointer )&ENOSTRValue, FALSE);
#endif

#ifdef ENOSYS
    pc->VariableDefinePlatformVar( NULL, "ENOSYS", &pc->IntType, (UnionAnyValuePointer )&ENOSYSValue, FALSE);
#endif

#ifdef ENOTCONN
    pc->VariableDefinePlatformVar( NULL, "ENOTCONN", &pc->IntType, (UnionAnyValuePointer )&ENOTCONNValue, FALSE);
#endif

#ifdef ENOTDIR
    pc->VariableDefinePlatformVar( NULL, "ENOTDIR", &pc->IntType, (UnionAnyValuePointer )&ENOTDIRValue, FALSE);
#endif

#ifdef ENOTEMPTY
    pc->VariableDefinePlatformVar( NULL, "ENOTEMPTY", &pc->IntType, (UnionAnyValuePointer )&ENOTEMPTYValue, FALSE);
#endif

#ifdef ENOTRECOVERABLE
    pc->VariableDefinePlatformVar( NULL, "ENOTRECOVERABLE", &pc->IntType, (UnionAnyValuePointer )&ENOTRECOVERABLEValue, FALSE);
#endif

#ifdef ENOTSOCK
    pc->VariableDefinePlatformVar( NULL, "ENOTSOCK", &pc->IntType, (UnionAnyValuePointer )&ENOTSOCKValue, FALSE);
#endif

#ifdef ENOTSUP
    pc->VariableDefinePlatformVar( NULL, "ENOTSUP", &pc->IntType, (UnionAnyValuePointer )&ENOTSUPValue, FALSE);
#endif

#ifdef ENOTTY
    pc->VariableDefinePlatformVar( NULL, "ENOTTY", &pc->IntType, (UnionAnyValuePointer )&ENOTTYValue, FALSE);
#endif

#ifdef ENXIO
    pc->VariableDefinePlatformVar( NULL, "ENXIO", &pc->IntType, (UnionAnyValuePointer )&ENXIOValue, FALSE);
#endif

#ifdef EOPNOTSUPP
    pc->VariableDefinePlatformVar( NULL, "EOPNOTSUPP", &pc->IntType, (UnionAnyValuePointer )&EOPNOTSUPPValue, FALSE);
#endif

#ifdef EOVERFLOW
    pc->VariableDefinePlatformVar( NULL, "EOVERFLOW", &pc->IntType, (UnionAnyValuePointer )&EOVERFLOWValue, FALSE);
#endif

#ifdef EOWNERDEAD
    pc->VariableDefinePlatformVar( NULL, "EOWNERDEAD", &pc->IntType, (UnionAnyValuePointer )&EOWNERDEADValue, FALSE);
#endif

#ifdef EPERM
    pc->VariableDefinePlatformVar( NULL, "EPERM", &pc->IntType, (UnionAnyValuePointer )&EPERMValue, FALSE);
#endif

#ifdef EPIPE
    pc->VariableDefinePlatformVar( NULL, "EPIPE", &pc->IntType, (UnionAnyValuePointer )&EPIPEValue, FALSE);
#endif

#ifdef EPROTO
    pc->VariableDefinePlatformVar( NULL, "EPROTO", &pc->IntType, (UnionAnyValuePointer )&EPROTOValue, FALSE);
#endif

#ifdef EPROTONOSUPPORT
    pc->VariableDefinePlatformVar( NULL, "EPROTONOSUPPORT", &pc->IntType, (UnionAnyValuePointer )&EPROTONOSUPPORTValue, FALSE);
#endif

#ifdef EPROTOTYPE
    pc->VariableDefinePlatformVar( NULL, "EPROTOTYPE", &pc->IntType, (UnionAnyValuePointer )&EPROTOTYPEValue, FALSE);
#endif

#ifdef ERANGE
    pc->VariableDefinePlatformVar( NULL, "ERANGE", &pc->IntType, (UnionAnyValuePointer )&ERANGEValue, FALSE);
#endif

#ifdef EROFS
    pc->VariableDefinePlatformVar( NULL, "EROFS", &pc->IntType, (UnionAnyValuePointer )&EROFSValue, FALSE);
#endif

#ifdef ESPIPE
    pc->VariableDefinePlatformVar( NULL, "ESPIPE", &pc->IntType, (UnionAnyValuePointer )&ESPIPEValue, FALSE);
#endif

#ifdef ESRCH
    pc->VariableDefinePlatformVar( NULL, "ESRCH", &pc->IntType, (UnionAnyValuePointer )&ESRCHValue, FALSE);
#endif

#ifdef ESTALE
    pc->VariableDefinePlatformVar( NULL, "ESTALE", &pc->IntType, (UnionAnyValuePointer )&ESTALEValue, FALSE);
#endif

#ifdef ETIME
    pc->VariableDefinePlatformVar( NULL, "ETIME", &pc->IntType, (UnionAnyValuePointer )&ETIMEValue, FALSE);
#endif

#ifdef ETIMEDOUT
    pc->VariableDefinePlatformVar( NULL, "ETIMEDOUT", &pc->IntType, (UnionAnyValuePointer )&ETIMEDOUTValue, FALSE);
#endif

#ifdef ETXTBSY
    pc->VariableDefinePlatformVar( NULL, "ETXTBSY", &pc->IntType, (UnionAnyValuePointer )&ETXTBSYValue, FALSE);
#endif

#ifdef EWOULDBLOCK
    pc->VariableDefinePlatformVar( NULL, "EWOULDBLOCK", &pc->IntType, (UnionAnyValuePointer )&EWOULDBLOCKValue, FALSE);
#endif

#ifdef EXDEV
    pc->VariableDefinePlatformVar( NULL, "EXDEV", &pc->IntType, (UnionAnyValuePointer )&EXDEVValue, FALSE);
#endif

    pc->VariableDefinePlatformVar( NULL, "errno", &pc->IntType, (UnionAnyValuePointer )&errno, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */
