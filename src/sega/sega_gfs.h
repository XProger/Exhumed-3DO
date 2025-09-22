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
    Sint32      fid;
    CdcFile     finfo;
    Sint32      sctsz;
    Sint32      nsct;
    Sint32      lstrm;
} GfsFinfo;

typedef struct {
    Sint32      bufno;
    Sint32      sctsz;
    Sint32      flt;
    CdcSubh     subh;
    Sint32      fmode;
    Sint32      puid;
    Sint32      filepos;
    Sint32      sctpos;
    Sint32      sfad;
    Sint32      efad;
} GfsCdRsrc;

typedef struct {
    Sint32      fid;
    Sint32      filepos;
    Sint32      sctpos;
    Sint32      sctnum;
} GfsScsiRsrc;

typedef struct {
    void        *data;
    Sint32      filepos;
    Sint32      sctpos;
    Sint32      sctnum;
} GfsMemRsrc;

typedef struct {
    Sint32      ftype;
    union {
        GfsCdRsrc       cd;
        GfsScsiRsrc     scsi;
        GfsMemRsrc      mem;
    } rsrc;
} GfsDtsrc;

typedef struct {
    GfsFinfo    finfo;
    GfsDtsrc    dtsrc;
    Sint32      gmode;
    Sint32      stat;
    Sint32      sct;
    Sint32      sctcnt;
    Sint32      sctmax;
} GfsFlow;

typedef Sint32 (*GfsTransFunc)(void *obj, Sint32 nsct);

typedef struct {
    void        *data;
    Sint32      adlt;
    Sint32      len;
    Sint32      nsct;
    Bool        use;
} GfsDataPack;

typedef GfsDataPack *GfdpHn;

typedef struct {
    void        *buf;
    Sint32      size;
    Sint32      wp;
    GfdpHn      dps;
    GfdpHn      dpd;
    Sint32      tsctmax;
    Sint32      tsct;
    Sint32      tsctcnt;
    Sint32      tbytcnt;
    void        *obj;
    GfsTransFunc tfunc;
    Sint32      unit;
    Bool        active;
    Sint32      stat;
    Sint32      mode;
} GfsTrans;

typedef struct {
    Bool        used;
    Sint32      amode;
    Sint32      astat;
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
    Sint32 (*flowin)(GfsFlow *);
    void (*stopin)(GfsFlow *, Bool);
    Sint32 (*seek)(GfsFlow *, Sint32);
    Sint32 (*tell)(GfsFlow *);
} GfsFileFunc;

typedef struct {
    GfsHn   access_file[GFS_OPEN_MAX];
    Sint32  nfile;
} GfsSvr;

typedef struct {
    CdcFile     dirrec;
} GfsDirId;

typedef struct {
    CdcFile     dirrec;
    Sint8       fname[GFS_FNAME_LEN];
} GfsDirName;

typedef struct {
    Sint32      type;
    Sint32      ndir;
    union {
        GfsDirId *dir_i;
        GfsDirName *dir_n;
    } dir;
} GfsDirTbl;

typedef struct {
    GfsDirTbl   dirtbl;
    Sint32      nfile;
} GfsDirMng;

typedef struct {
    Uint8 flt;
    Uint8 fmode;
    CdcSubh subh;
    Sint32 fad;
    Sint32 snum;
} GfcdSelQu;

typedef struct {
    Sint32 flt;
    Sint32 buf;
    Sint32 flnout;
} GfcdFconQu;  

typedef void (*GfsErrFunc)(void *obj, Sint32 ec);

typedef struct {
    GfsErrFunc  func;
    void        *obj;
    Sint32      code;
} GfsErrStat;

typedef struct {
    Sint8 use_buf[GFS_CDBBUF_NR];
    Sint8 use_filt[GFS_CDBBUF_NR];
    Bool use_pu;
    Sint32 tr_bufno;
    Sint32 puid;
    Sint32 timer;
    CdcStat stat;
    void (*func)(void *);
    void *obj;
    struct {
        Sint32 len;
        Sint32 stat;
        GfcdSelQu selq[GFS_SELQ_MAX];
    } tsk_setflt;
    struct {
        Sint32 len;
        Sint32 stat;
        GfcdFconQu fconq[GFS_FCONQ_MAX];
    } tsk_fltcon;
    struct {
        Sint32 stat;
        Sint32 flt;
    } tsk_setcon;
    struct {
        Sint32 stat;
        Sint32 bufno;
        Sint32 spos;
        Sint32 usct;
        Sint32 cnt;
        Sint32 *nsct;
        Sint32 *nbyte;
    }tsk_getlen;
    struct {
        Sint32 stat;
        Sint32 bufno;
        Sint32 sctpos;
        Sint32 nsct;
    } tsk_reqdat;
    struct {
        Sint32 stat;
        Sint32 bufno;
        Sint32 sctpos;
        Sint32 nsct;
    } tsk_delsct;
    struct {
        Sint32 stat;
        Sint32 dst;
        Sint32 src;
        Sint32 spos;
        Sint32 snum;
        Sint32 fmode;
    } tsk_movsct;
    struct {
        Sint32 stat;
        Sint16 fid;
        Sint16 work;
        Sint32 *ndir;
    } tsk_chgdir;
} GfsCdbMng;

typedef struct {
    Sint32      openmax;
    GfsFileFunc functbl[GFS_FTYPE_NR];
    GfsSvr      svr;
    GfsDirMng   curdir;
    GfsHn       pickup;
    Sint32      sfad;
    Sint32      efad;
    GfsHn       trans;
    GfsErrStat  error;
    Uint32      flags;
    Sint32      timer;
    GfsCdbMng   cdb;
    GfsDataPack srcpk;
    GfsDataPack dstpk;
    Uint8       sect_buf[GFS_SCTBUF_SIZ];
    GfsFile     file[1];
} GfsMng;

#define GFS_WORK_SIZE(open_max) (sizeof(GfsMng) + ((open_max) - 1) * sizeof(GfsFile))

#define GFS_DIRTBL_TYPE(dirtbl)         ((dirtbl)->type)
#define GFS_DIRTBL_NDIR(dirtbl)         ((dirtbl)->ndir)
#define GFS_DIRTBL_DIRNAME(dirtbl)      ((dirtbl)->dir.dir_n)

void GFS_GetFileSize(GfsHn gfs, Sint32 *sctsz, Sint32 *nsct, Sint32 *lstsz);

#if 0
void GFS_SetErrFunc(GfsErrFunc func, void *obj);
void GFS_Close(GfsHn gfs);
void GFS_NwGetStat(GfsHn gfs, Sint32 *amode, Sint32 *ndata);
Sint32 GFS_Init(Sint32 open_max, void *work, GfsDirTbl *dirtbl);
Sint32 GFS_NameToId(Sint8 *fname);
GfsHn GFS_Open(Sint32 fid);
Sint32 GFS_SetTransPara(GfsHn gfs, Sint32 tsize);
Sint32 GFS_SetTmode(GfsHn gfs, Sint32 tmode);
Sint32 GFS_NwCdRead(GfsHn gfs, Sint32 nsct);
Sint32 GFS_NwStop(GfsHn gfs);
Sint32 GFS_Fread(GfsHn gfs, Sint32 nsct, void *buf, Sint32 bsize);
Sint32 GFS_NwExecOne(GfsHn gfs);
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
