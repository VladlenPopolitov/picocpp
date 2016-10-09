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
    pc->VariableDefinePlatformVar( "EACCES", &pc->IntType, &EACCESValue, FALSE);
#endif

#ifdef EADDRINUSE
    pc->VariableDefinePlatformVar( "EADDRINUSE", &pc->IntType, &EADDRINUSEValue, FALSE);
#endif

#ifdef EADDRNOTAVAIL
    pc->VariableDefinePlatformVar( "EADDRNOTAVAIL", &pc->IntType, &EADDRNOTAVAILValue, FALSE);
#endif

#ifdef EAFNOSUPPORT
    pc->VariableDefinePlatformVar( "EAFNOSUPPORT", &pc->IntType, &EAFNOSUPPORTValue, FALSE);
#endif

#ifdef EAGAIN
    pc->VariableDefinePlatformVar( "EAGAIN", &pc->IntType, &EAGAINValue, FALSE);
#endif

#ifdef EALREADY
    pc->VariableDefinePlatformVar( "EALREADY", &pc->IntType, &EALREADYValue, FALSE);
#endif

#ifdef EBADF
    pc->VariableDefinePlatformVar( "EBADF", &pc->IntType, &EBADFValue, FALSE);
#endif

#ifdef EBADMSG
    pc->VariableDefinePlatformVar( "EBADMSG", &pc->IntType, &EBADMSGValue, FALSE);
#endif

#ifdef EBUSY
    pc->VariableDefinePlatformVar( "EBUSY", &pc->IntType, &EBUSYValue, FALSE);
#endif

#ifdef ECANCELED
    pc->VariableDefinePlatformVar( "ECANCELED", &pc->IntType, &ECANCELEDValue, FALSE);
#endif

#ifdef ECHILD
    pc->VariableDefinePlatformVar( "ECHILD", &pc->IntType, &ECHILDValue, FALSE);
#endif

#ifdef ECONNABORTED
    pc->VariableDefinePlatformVar( "ECONNABORTED", &pc->IntType, &ECONNABORTEDValue, FALSE);
#endif

#ifdef ECONNREFUSED
    pc->VariableDefinePlatformVar( "ECONNREFUSED", &pc->IntType, &ECONNREFUSEDValue, FALSE);
#endif

#ifdef ECONNRESET
    pc->VariableDefinePlatformVar( "ECONNRESET", &pc->IntType, &ECONNRESETValue, FALSE);
#endif

#ifdef EDEADLK
    pc->VariableDefinePlatformVar( "EDEADLK", &pc->IntType, &EDEADLKValue, FALSE);
#endif

#ifdef EDESTADDRREQ
    pc->VariableDefinePlatformVar( "EDESTADDRREQ", &pc->IntType, &EDESTADDRREQValue, FALSE);
#endif

#ifdef EDOM
    pc->VariableDefinePlatformVar( "EDOM", &pc->IntType, &EDOMValue, FALSE);
#endif

#ifdef EDQUOT
    pc->VariableDefinePlatformVar( "EDQUOT", &pc->IntType, &EDQUOTValue, FALSE);
#endif

#ifdef EEXIST
    pc->VariableDefinePlatformVar( "EEXIST", &pc->IntType, &EEXISTValue, FALSE);
#endif

#ifdef EFAULT
    pc->VariableDefinePlatformVar( "EFAULT", &pc->IntType, &EFAULTValue, FALSE);
#endif

#ifdef EFBIG
    pc->VariableDefinePlatformVar( "EFBIG", &pc->IntType, &EFBIGValue, FALSE);
#endif

#ifdef EHOSTUNREACH
    pc->VariableDefinePlatformVar( "EHOSTUNREACH", &pc->IntType, &EHOSTUNREACHValue, FALSE);
#endif

#ifdef EIDRM
    pc->VariableDefinePlatformVar( "EIDRM", &pc->IntType, &EIDRMValue, FALSE);
#endif

#ifdef EILSEQ
    pc->VariableDefinePlatformVar( "EILSEQ", &pc->IntType, &EILSEQValue, FALSE);
#endif

#ifdef EINPROGRESS
    pc->VariableDefinePlatformVar( "EINPROGRESS", &pc->IntType, &EINPROGRESSValue, FALSE);
#endif

#ifdef EINTR
    pc->VariableDefinePlatformVar( "EINTR", &pc->IntType, &EINTRValue, FALSE);
#endif

#ifdef EINVAL
    pc->VariableDefinePlatformVar( "EINVAL", &pc->IntType, &EINVALValue, FALSE);
#endif

#ifdef EIO
    pc->VariableDefinePlatformVar( "EIO", &pc->IntType, &EIOValue, FALSE);
#endif

#ifdef EISCONN
    pc->VariableDefinePlatformVar( "EISCONN", &pc->IntType, &EISCONNValue, FALSE);
#endif

#ifdef EISDIR
    pc->VariableDefinePlatformVar( "EISDIR", &pc->IntType, &EISDIRValue, FALSE);
#endif

#ifdef ELOOP
    pc->VariableDefinePlatformVar( "ELOOP", &pc->IntType, &ELOOPValue, FALSE);
#endif

#ifdef EMFILE
    pc->VariableDefinePlatformVar( "EMFILE", &pc->IntType, &EMFILEValue, FALSE);
#endif

#ifdef EMLINK
    pc->VariableDefinePlatformVar( "EMLINK", &pc->IntType, &EMLINKValue, FALSE);
#endif

#ifdef EMSGSIZE
    pc->VariableDefinePlatformVar( "EMSGSIZE", &pc->IntType, &EMSGSIZEValue, FALSE);
#endif

#ifdef EMULTIHOP
    pc->VariableDefinePlatformVar( "EMULTIHOP", &pc->IntType, &EMULTIHOPValue, FALSE);
#endif

#ifdef ENAMETOOLONG
    pc->VariableDefinePlatformVar( "ENAMETOOLONG", &pc->IntType, &ENAMETOOLONGValue, FALSE);
#endif

#ifdef ENETDOWN
    pc->VariableDefinePlatformVar( "ENETDOWN", &pc->IntType, &ENETDOWNValue, FALSE);
#endif

#ifdef ENETRESET
    pc->VariableDefinePlatformVar( "ENETRESET", &pc->IntType, &ENETRESETValue, FALSE);
#endif

#ifdef ENETUNREACH
    pc->VariableDefinePlatformVar( "ENETUNREACH", &pc->IntType, &ENETUNREACHValue, FALSE);
#endif

#ifdef ENFILE
    pc->VariableDefinePlatformVar( "ENFILE", &pc->IntType, &ENFILEValue, FALSE);
#endif

#ifdef ENOBUFS
    pc->VariableDefinePlatformVar( "ENOBUFS", &pc->IntType, &ENOBUFSValue, FALSE);
#endif

#ifdef ENODATA
    pc->VariableDefinePlatformVar( "ENODATA", &pc->IntType, &ENODATAValue, FALSE);
#endif

#ifdef ENODEV
    pc->VariableDefinePlatformVar( "ENODEV", &pc->IntType, &ENODEVValue, FALSE);
#endif

#ifdef ENOENT
    pc->VariableDefinePlatformVar( "ENOENT", &pc->IntType, &ENOENTValue, FALSE);
#endif

#ifdef ENOEXEC
    pc->VariableDefinePlatformVar( "ENOEXEC", &pc->IntType, &ENOEXECValue, FALSE);
#endif

#ifdef ENOLCK
    pc->VariableDefinePlatformVar( "ENOLCK", &pc->IntType, &ENOLCKValue, FALSE);
#endif

#ifdef ENOLINK
    pc->VariableDefinePlatformVar( "ENOLINK", &pc->IntType, &ENOLINKValue, FALSE);
#endif

#ifdef ENOMEM
    pc->VariableDefinePlatformVar( "ENOMEM", &pc->IntType, &ENOMEMValue, FALSE);
#endif

#ifdef ENOMSG
    pc->VariableDefinePlatformVar( "ENOMSG", &pc->IntType, &ENOMSGValue, FALSE);
#endif

#ifdef ENOPROTOOPT
    pc->VariableDefinePlatformVar( "ENOPROTOOPT", &pc->IntType, &ENOPROTOOPTValue, FALSE);
#endif

#ifdef ENOSPC
    pc->VariableDefinePlatformVar( "ENOSPC", &pc->IntType, &ENOSPCValue, FALSE);
#endif

#ifdef ENOSR
    pc->VariableDefinePlatformVar( "ENOSR", &pc->IntType, &ENOSRValue, FALSE);
#endif

#ifdef ENOSTR
    pc->VariableDefinePlatformVar( "ENOSTR", &pc->IntType, &ENOSTRValue, FALSE);
#endif

#ifdef ENOSYS
    pc->VariableDefinePlatformVar( "ENOSYS", &pc->IntType, &ENOSYSValue, FALSE);
#endif

#ifdef ENOTCONN
    pc->VariableDefinePlatformVar( "ENOTCONN", &pc->IntType, &ENOTCONNValue, FALSE);
#endif

#ifdef ENOTDIR
    pc->VariableDefinePlatformVar( "ENOTDIR", &pc->IntType, &ENOTDIRValue, FALSE);
#endif

#ifdef ENOTEMPTY
    pc->VariableDefinePlatformVar( "ENOTEMPTY", &pc->IntType, &ENOTEMPTYValue, FALSE);
#endif

#ifdef ENOTRECOVERABLE
    pc->VariableDefinePlatformVar( "ENOTRECOVERABLE", &pc->IntType, &ENOTRECOVERABLEValue, FALSE);
#endif

#ifdef ENOTSOCK
    pc->VariableDefinePlatformVar( "ENOTSOCK", &pc->IntType, &ENOTSOCKValue, FALSE);
#endif

#ifdef ENOTSUP
    pc->VariableDefinePlatformVar( "ENOTSUP", &pc->IntType, &ENOTSUPValue, FALSE);
#endif

#ifdef ENOTTY
    pc->VariableDefinePlatformVar( "ENOTTY", &pc->IntType, &ENOTTYValue, FALSE);
#endif

#ifdef ENXIO
    pc->VariableDefinePlatformVar( "ENXIO", &pc->IntType, &ENXIOValue, FALSE);
#endif

#ifdef EOPNOTSUPP
    pc->VariableDefinePlatformVar( "EOPNOTSUPP", &pc->IntType, &EOPNOTSUPPValue, FALSE);
#endif

#ifdef EOVERFLOW
    pc->VariableDefinePlatformVar( "EOVERFLOW", &pc->IntType, &EOVERFLOWValue, FALSE);
#endif

#ifdef EOWNERDEAD
    pc->VariableDefinePlatformVar( "EOWNERDEAD", &pc->IntType, &EOWNERDEADValue, FALSE);
#endif

#ifdef EPERM
    pc->VariableDefinePlatformVar( "EPERM", &pc->IntType, &EPERMValue, FALSE);
#endif

#ifdef EPIPE
    pc->VariableDefinePlatformVar( "EPIPE", &pc->IntType, &EPIPEValue, FALSE);
#endif

#ifdef EPROTO
    pc->VariableDefinePlatformVar( "EPROTO", &pc->IntType, &EPROTOValue, FALSE);
#endif

#ifdef EPROTONOSUPPORT
    pc->VariableDefinePlatformVar( "EPROTONOSUPPORT", &pc->IntType, &EPROTONOSUPPORTValue, FALSE);
#endif

#ifdef EPROTOTYPE
    pc->VariableDefinePlatformVar( "EPROTOTYPE", &pc->IntType, &EPROTOTYPEValue, FALSE);
#endif

#ifdef ERANGE
    pc->VariableDefinePlatformVar( "ERANGE", &pc->IntType, &ERANGEValue, FALSE);
#endif

#ifdef EROFS
    pc->VariableDefinePlatformVar( "EROFS", &pc->IntType, &EROFSValue, FALSE);
#endif

#ifdef ESPIPE
    pc->VariableDefinePlatformVar( "ESPIPE", &pc->IntType, &ESPIPEValue, FALSE);
#endif

#ifdef ESRCH
    pc->VariableDefinePlatformVar( "ESRCH", &pc->IntType, &ESRCHValue, FALSE);
#endif

#ifdef ESTALE
    pc->VariableDefinePlatformVar( "ESTALE", &pc->IntType, &ESTALEValue, FALSE);
#endif

#ifdef ETIME
    pc->VariableDefinePlatformVar( "ETIME", &pc->IntType, &ETIMEValue, FALSE);
#endif

#ifdef ETIMEDOUT
    pc->VariableDefinePlatformVar( "ETIMEDOUT", &pc->IntType, &ETIMEDOUTValue, FALSE);
#endif

#ifdef ETXTBSY
    pc->VariableDefinePlatformVar( "ETXTBSY", &pc->IntType, &ETXTBSYValue, FALSE);
#endif

#ifdef EWOULDBLOCK
    pc->VariableDefinePlatformVar( "EWOULDBLOCK", &pc->IntType, &EWOULDBLOCKValue, FALSE);
#endif

#ifdef EXDEV
    pc->VariableDefinePlatformVar( "EXDEV", &pc->IntType, &EXDEVValue, FALSE);
#endif

    pc->VariableDefinePlatformVar( "errno", &pc->IntType, &errno, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */
