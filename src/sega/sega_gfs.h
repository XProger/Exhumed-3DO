#ifndef SEGA_GFS_H
#define SEGA_GFS_H

#include <sega_cdc.h>

#define GFS_ERR_CDOPEN          0
#define GFS_ERR_CDRD            (-1)
#define GFS_ERR_FATAL           (-25)

enum GfsDirType {
    GFS_DIR_ID,
    GFS_DIR_NAME
};

typedef struct {
    sint32      fid;
    CdcFile     finfo;
    sint32      sctsz;
    sint32      nsct;
    sint32      lstrm;
} GfsFinfo;

typedef struct {
    sint32      bufno;
    sint32      sctsz;
    sint32      flt;
    CdcSubh     subh;
    sint32      fmode;
    sint32      puid;
    sint32      filepos;
    sint32      sctpos;
    sint32      sfad;
    sint32      efad;
} GfsCdRsrc;

typedef struct {
    sint32      fid;
    sint32      filepos;
    sint32      sctpos;
    sint32      sctnum;
} GfsScsiRsrc;

typedef struct {
    void        *data;
    sint32      filepos;
    sint32      sctpos;
    sint32      sctnum;
} GfsMemRsrc;

typedef struct {
    sint32      ftype;
    union {
        GfsCdRsrc       cd;
        GfsScsiRsrc     scsi;
        GfsMemRsrc      mem;
    } rsrc;
} GfsDtsrc;

typedef struct {
    GfsFinfo    finfo;
    GfsDtsrc    dtsrc;
    sint32      gmode;
    sint32      stat;
    sint32      sct;
    sint32      sctcnt;
    sint32      sctmax;
} GfsFlow;

typedef sint32 (*GfsTransFunc)(void *obj, sint32 nsct);

typedef struct {
    void        *data;
    sint32      adlt;
    sint32      len;
    sint32      nsct;
    sint32        use;
} GfsDataPack;

typedef GfsDataPack *GfdpHn;

typedef struct {
    void        *buf;
    sint32      size;
    sint32      wp;
    GfdpHn      dps;
    GfdpHn      dpd;
    sint32      tsctmax;
    sint32      tsct;
    sint32      tsctcnt;
    sint32      tbytcnt;
    void        *obj;
    GfsTransFunc tfunc;
    sint32      unit;
    sint32        active;
    sint32      stat;
    sint32      mode;
} GfsTrans;

typedef struct {
    sint32        used;
    sint32      amode;
    sint32      astat;
    GfsFlow     flow;
    GfsTrans    trans;
} GfsFile;

typedef GfsFile *GfsHn;

#define GFS_OPEN_MAX    24
#define GFS_FTYPE_NR    3
#define GFS_SCTBUF_SIZ  2048
#define GFS_FNAME_LEN   12
#define GFS_CDBBUF_NR   24
#define GFS_SELQ_MAX    24
#define GFS_FCONQ_MAX   24

#define GFS_TMODE_CPU   0


typedef struct {
    sint32 (*flowin)(GfsFlow *);
    void (*stopin)(GfsFlow *, sint32);
    sint32 (*seek)(GfsFlow *, sint32);
    sint32 (*tell)(GfsFlow *);
} GfsFileFunc;

typedef struct {
    GfsHn   access_file[GFS_OPEN_MAX];
    sint32  nfile;
} GfsSvr;

typedef struct {
    CdcFile     dirrec;
} GfsDirId;

typedef struct {
    CdcFile     dirrec;
    sint8       fname[GFS_FNAME_LEN];
} GfsDirName;

typedef struct {
    sint32      type;
    sint32      ndir;
    union {
        GfsDirId *dir_i;
        GfsDirName *dir_n;
    } dir;
} GfsDirTbl;

typedef struct {
    GfsDirTbl   dirtbl;
    sint32      nfile;
} GfsDirMng;

typedef struct {
    uint8 flt;
    uint8 fmode;
    CdcSubh subh;
    sint32 fad;
    sint32 snum;
} GfcdSelQu;

typedef struct {
    sint32 flt;
    sint32 buf;
    sint32 flnout;
} GfcdFconQu;  

typedef void (*GfsErrFunc)(void *obj, sint32 ec);

typedef struct {
    GfsErrFunc  func;
    void        *obj;
    sint32      code;
} GfsErrStat;

typedef struct {
    sint8 use_buf[GFS_CDBBUF_NR];
    sint8 use_filt[GFS_CDBBUF_NR];
    sint32 use_pu;
    sint32 tr_bufno;
    sint32 puid;
    sint32 timer;
    CdcStat stat;
    void (*func)(void *);
    void *obj;
    struct {
        sint32 len;
        sint32 stat;
        GfcdSelQu selq[GFS_SELQ_MAX];
    } tsk_setflt;
    struct {
        sint32 len;
        sint32 stat;
        GfcdFconQu fconq[GFS_FCONQ_MAX];
    } tsk_fltcon;
    struct {
        sint32 stat;
        sint32 flt;
    } tsk_setcon;
    struct {
        sint32 stat;
        sint32 bufno;
        sint32 spos;
        sint32 usct;
        sint32 cnt;
        sint32 *nsct;
        sint32 *nbyte;
    }tsk_getlen;
    struct {
        sint32 stat;
        sint32 bufno;
        sint32 sctpos;
        sint32 nsct;
    } tsk_reqdat;
    struct {
        sint32 stat;
        sint32 bufno;
        sint32 sctpos;
        sint32 nsct;
    } tsk_delsct;
    struct {
        sint32 stat;
        sint32 dst;
        sint32 src;
        sint32 spos;
        sint32 snum;
        sint32 fmode;
    } tsk_movsct;
    struct {
        sint32 stat;
        sint16 fid;
        sint16 work;
        sint32 *ndir;
    } tsk_chgdir;
} GfsCdbMng;

typedef struct {
    sint32      openmax;
    GfsFileFunc functbl[GFS_FTYPE_NR];
    GfsSvr      svr;
    GfsDirMng   curdir;
    GfsHn       pickup;
    sint32      sfad;
    sint32      efad;
    GfsHn       trans;
    GfsErrStat  error;
    uint32      flags;
    sint32      timer;
    GfsCdbMng   cdb;
    GfsDataPack srcpk;
    GfsDataPack dstpk;
    uint8       sect_buf[GFS_SCTBUF_SIZ];
    GfsFile     file[1];
} GfsMng;

#define GFS_WORK_SIZE(open_max) (sizeof(GfsMng) + ((open_max) - 1) * sizeof(GfsFile))

#define GFS_DIRTBL_TYPE(dirtbl)         ((dirtbl)->type)
#define GFS_DIRTBL_NDIR(dirtbl)         ((dirtbl)->ndir)
#define GFS_DIRTBL_DIRNAME(dirtbl)      ((dirtbl)->dir.dir_n)

void GFS_GetFileSize(GfsHn gfs, sint32 *sctsz, sint32 *nsct, sint32 *lstsz);

#if 0
void GFS_SetErrFunc(GfsErrFunc func, void *obj);
void GFS_Close(GfsHn gfs);
void GFS_NwGetStat(GfsHn gfs, sint32 *amode, sint32 *ndata);
sint32 GFS_Init(sint32 open_max, void *work, GfsDirTbl *dirtbl);
sint32 GFS_NameToId(sint8 *fname);
GfsHn GFS_Open(sint32 fid);
sint32 GFS_SetTransPara(GfsHn gfs, sint32 tsize);
sint32 GFS_SetTmode(GfsHn gfs, sint32 tmode);
sint32 GFS_NwCdRead(GfsHn gfs, sint32 nsct);
sint32 GFS_NwStop(GfsHn gfs);
sint32 GFS_Fread(GfsHn gfs, sint32 nsct, void *buf, sint32 bsize);
sint32 GFS_NwExecOne(GfsHn gfs);
#else
#define GFS_SetErrFunc(func,obj)
#define GFS_Close(gfs)
#define GFS_NwGetStat(gfs,amode,ndata)
#define GFS_Open(fid) NULL
#define GFS_Init(open_max,work,dirtbl) 0
#define GFS_NameToId(fname) 0
#define GFS_SetTransPara(gfs,tsize) 0
#define GFS_SetTmode(gfs,tmode) 0
#define GFS_NwCdRead(gfs,nsct) 0
#define GFS_NwStop(gfs) 0
#define GFS_Fread(gfs,nsct,buf,bsize) 0
#define GFS_NwExecOne(gfs) 0
#endif

#endif
