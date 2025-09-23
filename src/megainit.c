#include <sega_sys.h>
#include <sega_int.h>

static void vd1Comfil(void); /* ＶＤＰ１ クリッピング初期化    */
static void vd2Ramfil(void); /* ＶＤＰ２ ＶＲＡＭクリア        */
static void colRamfil(void); /* カラーＲＡＭクリア             */
static void sndRamfil(sint32); /* サウンドＲＡＭクリア           */
static void scuDspInit(void); /* ＳＣＵ ＤＳＰ 初期化           */
static void msh2PeriInit(void); /* マスタＳＨ周辺モジュール初期化 */
static void sndDspInit(void); /* サウンドＤＳＰクリア           */

static void vbIrtn(void); /* ＶＢ-Ｉｎ 割込み処理           */
static void vbOrtn(void); /* ＶＢ-Ｏｕｔ 割込み処理         */
static void syncVbI(void); /* ＶＢ-Ｉｎ 同期処理用           */
static void memset_w(sint16*, sint16, sint32);
/* ワード ｍｅｍｓｅｔ            */
static void memcpy_w(sint16*, sint16*, sint32);
/* ワード ｍｅｍｃｐｙ            */
static sint16* blkmfil_w(sint16*, sint16, sint32);
/* ワード・ブロックｆｉｌｌ       */
static sint32* blkmfil_l(sint32*, sint32, sint32);
/* ロングワード・ブロックｆｉｌｌ */

/* 現時点の(ライセンス画面表示中)画面サイズに関する情報 */
#define XRES (320) /* ライセンス画面の水平サイズ     */
#define SCLIP_UX (XRES - 1) /*             〃                 */
#define SCLIP_UY_N (224 - 1) /*   〃   (ＮＴＳＣの場合)        */
#define SCLIP_UY_P (256 - 1) /*   〃   (ＰＡＬの場合)          */

/* 処理対象デバイスのベースアドレス */
#define SND_RAM ((sint32*)0x25a00000)
#define VD1_VRAM ((sint16*)0x25c00000)
#define VD1_REG ((sint16*)0x25d00000)
/* VD2_VRAM は、ライセンス表示で使用中のＶＲＡＭ領域を除く    */
#define VD2_VRAM ((sint32*)0x25e08004)
/* COL_RAM は、ライセンス表示で使用中のカラーＲＡＭ領域を除く */
#define COL_RAM ((sint16*)0x25f00020)
#define VD2_REG ((sint16*)0x25f80000)
#define SCSP_DSP_RAM ((sint16*)0x25b00800)

/* ＳＭＰＣレジスタ */
#define SMPC_REG(ofs) (*(uint8*)(0x20100000 + ofs))

/* ＳＣＵレジスタ */
#define DSP_PGM_CTRL_PORT (*(sint32*)0x25fe0080)
#define DSP_PGM_RAM_PORT (*(sint32*)0x25fe0084)
#define DSP_DATA_RAM_ADRS_PORT (*(sint32*)0x25fe0088)
#define DSP_DATA_RAM_DATA_PORT (*(sint32*)0x25fe008c)

/* ＳＣＳＰ サウンドＲＡＭサイズレジスタ */
#define SCSP_SNDRAMSZ (*(sint8*)0x25b00400)

/* ＳＨ２周辺モジュールレジスタ */
#define MSH2_DMAC_SAR(ofs) (*(sint32*)(0xffffff80 + ofs))
#define MSH2_DMAC_DAR(ofs) (*(sint32*)(0xffffff84 + ofs))
#define MSH2_DMAC_TCR(ofs) (*(sint32*)(0xffffff88 + ofs))
#define MSH2_DMAC_CHCR(ofs) (*(sint32*)(0xffffff8c + ofs))
#define MSH2_DMAC_DRCR(sel) (*(sint8*)(0xfffffe71 + sel))
#define MSH2_DMAC_DMAOR (*(sint32*)(0xffffffb0))
#define MSH2_DIVU_CONT (*(sint32*)(0xffffffb8))

#define MSETDIV (4)
#define BLKMSK_VD2_VRAM (0x1fffc)
#define BLKMSK_COL_RAM (0x001fe)

#define M68000_VECTBLSZ (0x00400 / sizeof(sint32))
#define BLKMSK_SND_RAM (0x003fc)

#define SCSP_DSP_RAMSZ (0x00400)

#define VBI_NUM (0x40)
#define VBO_NUM (0x41)
#define VB_MASK (0x0003)

static sint16 yBottom, ewBotRight;
static sint16 vdp1cmds[48];
static sint16 vbIcnt = 0;
static sint16 sequence = 0;
static sint32* vramptr = VD2_VRAM;
static sint16* cramptr = COL_RAM;

#define POKE_W(adr, data) (*((uint16*)(adr + 0x05e00000)) = ((uint16)(data)))

void megaInit(void)
{
#ifdef TODO // mega
    yBottom = (VD2_REG[2] & 1) ? SCLIP_UY_P : SCLIP_UY_N;
    ewBotRight = ((XRES / 8) << 9) + (yBottom);
    SYS_SETUINT(VBI_NUM, vbIrtn);
    SYS_SETUINT(VBO_NUM, vbOrtn);
    SYS_CHGSCUIM(~VB_MASK, 0);

    vd1Comfil();
    for (sequence = 0; sequence < MSETDIV; sequence++)
    {
        syncVbI();
        colRamfil();
        vd2Ramfil();
        sndRamfil(sequence);
    }

    scuDspInit();
    msh2PeriInit();
    sndDspInit();

    SYS_CHGSCUIM(-1, VB_MASK);
    SYS_SETUINT(VBI_NUM, (void (*)())0);
    SYS_SETUINT(VBO_NUM, (void (*)())0);
#endif
}

static void memset_w(sint16* buf, sint16 pattern, sint32 size)
{
    register sint32 i;

    for (i = 0; i < size; i += sizeof(sint16))
    {
        *buf++ = pattern;
    }
}

static void memcpy_w(sint16* dst, sint16* src, sint32 size)
{
    register sint32 i;

    for (i = 0; i < size; i += sizeof(sint16))
    {
        *dst++ = *src++;
    }
}

static sint16* blkmfil_w(sint16* buf, sint16 pattern, sint32 brkmsk)
{
    register sint32 i;

    i = (sint32)buf & brkmsk;
    for (; i <= brkmsk; i += sizeof(sint16))
    {
        *buf++ = pattern;
    }
    return (buf);
}
static sint32* blkmfil_l(sint32* buf, sint32 pattern, sint32 brkmsk)
{
    register sint32 i;

    i = (sint32)buf & brkmsk;
    for (; i <= brkmsk; i += sizeof(sint32))
    {
        *buf++ = pattern;
    }
    return (buf);
}

static void vbIrtn(void)
{
    vbIcnt++;
}

static void vbOrtn(void)
{
    sint16* vdp1r;
    /* イレースライトでフレームバッファをクリア */
    vdp1r = VD1_REG;
    *vdp1r++ = 0x0; /* １／６０秒自動描画モード */
    *vdp1r++ = 0x0;
    *vdp1r++ = 0x2;
    *vdp1r++ = 0x0; /* イレースライトは透明色   */
    *vdp1r++ = 0x0; /*  〃   左上座標   */
    *vdp1r = ewBotRight; /*  〃   右下座標   */
}

static void syncVbI(void)
{
    register sint32 cur_cnt_value;

    cur_cnt_value = vbIcnt;
    while (cur_cnt_value == vbIcnt)
        ;
}

/* ＶＤＰ１に、システムクリッピングとローカル座標を読ませる */
static void vd1Comfil(void)
{
    register sint16* cmdbuf;

    memset_w((cmdbuf = vdp1cmds), 0, sizeof(vdp1cmds));
    cmdbuf[0] = 0x0009;
    cmdbuf[10] = SCLIP_UX;
    cmdbuf[11] = yBottom;
    cmdbuf[16] = 0x000a;
    cmdbuf[32] = 0x8000;
    memcpy_w(VD1_VRAM, vdp1cmds, sizeof(vdp1cmds));
}

static void vd2Ramfil(void)
{
    vramptr = blkmfil_l(vramptr, 0, BLKMSK_VD2_VRAM);
}

static void colRamfil(void)
{
    cramptr = blkmfil_w(cramptr, 0, BLKMSK_COL_RAM);
}

static void sndRamfil(sint32 initstep)
{
#ifdef TODO
    register sint32* memptr;

    switch (initstep)
    {
        case 0:
            SMPC_REG(31) = 7; /* Ｍ６８０００を停止          */
            break;
        case 1:
            SCSP_SNDRAMSZ = 2; /* サウンドＲＡＭサイズ設定    */
            /* サウンドＲＡＭ先頭４００Ｈ  */
            memptr = SND_RAM; /* (ベクタ)に４００Ｈをフィル  */
            blkmfil_l(memptr, 0x400, BLKMSK_SND_RAM);
            *memptr = 0x0007fffc; /* ＳＰ初期値をセット          */
            memptr += M68000_VECTBLSZ;
            *memptr = 0x4e7160fc; /* アドレス４００Ｈに ＮＯＰと */
            /* ＢＲＡ ＠－２ 命令を書込み  */
            break;
        case 2:
            SMPC_REG(31) = 6; /* Ｍ６８０００起動(無限待ち)  */
            break;
            /* 備考： １イントの間がある   */
    } /* ため、ＳＭＰＣステータスの  */
#endif
} /* セット／チェックを省略      */

static void msh2PeriInit(void)
{
#ifdef TODO
    register sint32 i, ofs, dummy;

    ofs = 0;
    for (i = 0; i < 2; i++)
    { /* ＤＭＡＣ各レジスタを初期化  */
        MSH2_DMAC_SAR(ofs) = 0x00000000;
        MSH2_DMAC_DAR(ofs) = 0x00000000;
        MSH2_DMAC_TCR(ofs) = 0x00000001;
        dummy = MSH2_DMAC_CHCR(ofs);
        MSH2_DMAC_CHCR(ofs) = 0x00000000;
        MSH2_DMAC_DRCR(i) = 0x00;
        ofs = 0x10;
    }
    dummy = MSH2_DMAC_DMAOR;
    MSH2_DMAC_DMAOR = 0x00000000;
    /* ＤＩＶＵ割込みを不許可      */
    MSH2_DIVU_CONT = 0x00000000;
#endif
}

static void scuDspInit(void)
{
    register sint32 i;

    DSP_PGM_CTRL_PORT = 0x0; /* ＤＳＰ停止           */

    for (i = 0; i < 256; i++)
        DSP_PGM_RAM_PORT = 0xf0000000; /* ＥＮＤ命令フィル     */

    for (i = 0; i < 256; i++)
    { /* ＤＳＰ ＲＡＭクリア  */
        DSP_DATA_RAM_ADRS_PORT = i;
        DSP_DATA_RAM_DATA_PORT = 0x0;
    }
}

static void sndDspInit(void)
{
    memset_w(SCSP_DSP_RAM, 0, SCSP_DSP_RAMSZ);
    /* サウンドＤＳＰ       */
} /* プログラム領域クリア */

