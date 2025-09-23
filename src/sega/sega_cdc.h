#ifndef SEGA_CDC_H
#define SEGA_CDC_H

#include <sega_xpt.h>

typedef struct {
    sint32  fad;
    sint32  size;
    uint8   unit;
    uint8   gap;
    uint8   fn;
    uint8   atr;
} CdcFile;

typedef struct {
    uint8   fn;
    uint8   cn;
    uint8   smmsk;
    uint8   smval;
    uint8   cimsk;
    uint8   cival;
} CdcSubh;

typedef struct {
    uint8   status;
    struct {
        uint8   flgrep;
        uint8   ctladr;
        uint8   tno;
        uint8   idx;
        sint32  fad;
    } report;
} CdcStat;

typedef struct {
    sint32 ptype;
    union {
        sint32 fad;
        struct {
            uint8 tno;
            uint8 idx;
        } trkidx;
    } pbody;
} CdcPos;

typedef struct {
    CdcPos  start;
    CdcPos  end;
    uint8   pmode;
} CdcPly;

enum CdcPosType {
    CDC_PTYPE_DFL,
    CDC_PTYPE_FAD,
    CDC_PTYPE_TNO,
    CDC_PTYPE_NOCHG,
    CDC_PTYPE_END
};

enum CdcErrCode {
    CDC_ERR_OK = OK,

    CDC_ERR_CMDBUSY=-1,
    CDC_ERR_CMDNG  =-2,
    CDC_ERR_TMOUT  =-3,
    CDC_ERR_PUT    =-4,
    CDC_ERR_REJECT =-5,
    CDC_ERR_WAIT   =-6,
    CDC_ERR_TRNS   =-7,
    CDC_ERR_PERI   =-8
};

#define CDC_STC_MSK     0x0f

#define CDC_ST_PAUSE    0x01

#define CDC_POS_PTYPE(pos)          ((pos)->ptype)
#define CDC_POS_FAD(pos)            ((pos)->pbody.fad)
#define CDC_POS_TNO(pos)            ((pos)->pbody.trkidx.tno)
#define CDC_POS_IDX(pos)            ((pos)->pbody.trkidx.idx)

#define CDC_PLY_START(ply)          ((ply)->start)
#define CDC_PLY_END(ply)            ((ply)->end)
#define CDC_PLY_PMODE(ply)          ((ply)->pmode)

#define CDC_PLY_STYPE(ply)          CDC_POS_PTYPE(&CDC_PLY_START(ply))
#define CDC_PLY_SFAD(ply)           CDC_POS_FAD(&CDC_PLY_START(ply))
#define CDC_PLY_STNO(ply)           CDC_POS_TNO(&CDC_PLY_START(ply))
#define CDC_PLY_SIDX(ply)           CDC_POS_IDX(&CDC_PLY_START(ply))

#define CDC_PLY_ETYPE(ply)          CDC_POS_PTYPE(&CDC_PLY_END(ply))
#define CDC_PLY_EFAS(ply)           CDC_POS_FAD(&CDC_PLY_END(ply))
#define CDC_PLY_ETNO(ply)           CDC_POS_TNO(&CDC_PLY_END(ply))
#define CDC_PLY_EIDX(ply)           CDC_POS_IDX(&CDC_PLY_END(ply))

#define CDC_STAT_STATUS(stat)       ((stat)->status)
#define CDC_STAT_FAD(stat)          ((stat)->report.fad)

#define CDC_GET_STC(stat)           (CDC_STAT_STATUS(stat) & CDC_STC_MSK)

#define CDC_HIRQ_DCHG       0x0020
#define CDC_GetHirqReq()    0

sint32  CDC_GetPeriStat(CdcStat *stat);

#if 0
sint32  CDC_CdPlay(CdcPly *ply);
sint32  CDC_CdSeek(CdcPos *pos);
sint32  CDC_TgetToc(uint32 *toc);
sint32  CDC_GetSctNum(sint32 bufno, sint32 *snum);
sint32  CDC_CdGetLastBuf(sint32 *bufno);
#else
#define CDC_CdPlay(ply) 0
#define CDC_CdSeek(pos) 0
#define CDC_TgetToc(toc) 0
#define CDC_GetSctNum(bufno, snum) 0
#define CDC_CdGetLastBuf(bufno) 0
#endif

#endif
