/* GEM Worm
 * Copyright (C) 2009 Jeffrey Armstrong
 * http://jeff.rainbow-100.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * A full version of the license terms is available in LICENSE.txt.
 */

#include <stdio.h>
/*#include <stdlib.h>*/

#ifdef __GNUC__

#include <gem.h>

#define HIWORD(x)  ((int16_t)((uint32_t)x >> 16))
#define LOWORD(x)  ((int16_t)((uint32_t)x & 0xFFFF)) 

#else

#include <portab.h>
#include <aes.h>
#include <vdi.h>

#endif /* __GNUC__ */

/* The proper resource header is dependent upon system */
#ifdef PCGEM
#include "wormpc.h"
#else
#include "wormst.h"
#endif

#include "field.h"
#include "player.h"

/* Sometimes you may need to compile in some missing global
 * arrays used within the gem libraries
 */
#ifdef PCGEM
#ifdef NEEDGSX
GLOBAL WORD   contrl[11];        /* control inputs                            */
GLOBAL WORD   intin[80];         /* max string length                         */
GLOBAL WORD   ptsin[256];        /* polygon fill points                       */
GLOBAL WORD   intout[45];        /* open workstation output                   */
GLOBAL WORD   ptsout[12];
#endif
#else
#define FAR
#endif

WORD    app_wflags = NAME | CLOSER | MOVER | SIZER;
WORD    app_accid;
WORD    app_appid;

WORD    app_wh = -1;    /* Border window handle */
WORD    app_vh;         /* VDI handle */

char    *app_title = "Worm";
GRECT   app_wdw;            /* xywh of working area */

OBJECT FAR *app_menu;
OBJECT FAR *about_box;

#ifndef WF_WXYWH
#ifdef WF_WORKXYWH
#define WF_WXYWH    WF_WORKXYWH
#endif
#endif

#ifndef WF_CXYWH
#ifdef WF_CURRXYWH
#define WF_CXYWH    WF_CURRXYWH
#endif
#endif

#define max(x,y)    x>y ? x : y
#define min(x,y)    x<y ? x : y

#ifdef PCGEM
#define RCS_FILE    "wormpc.rsc"
#else
#define RCS_FILE    "wormst.rsc"
#endif

#define MOVEDELAY   300

#define STARTWIDTH  640
#define STARTHEIGHT 480

#ifndef MGEMLIB
int rc_intersect(GRECT *one, GRECT *two)
{
WORD tx,ty,tw,th;

    tw = min(one->g_x+one->g_w,two->g_x+two->g_w);
    th = min(one->g_y+one->g_h,two->g_y+two->g_h);
    tx = max(one->g_x,two->g_x);
    ty = max(one->g_y,two->g_y);
    two->g_x = tx;
    two->g_y = ty;
    two->g_w = tw-tx;
    two->g_h = th-ty;
    return ( (tw > tx) && (th > ty) );
}
#endif

void hndl_about()
{
GRECT box,origin;

    /* ob_xywh(app_menu, MDESK, &origin); */

    form_center(about_box, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
    objc_draw(about_box,0,2,box.g_x, box.g_y, box.g_w, box.g_h);
    form_do(about_box,0);
    form_dial(FMD_FINISH,0,0,0,0,box.g_x, box.g_y, box.g_w, box.g_h);



    return;
}

int open_window(int new)
{
    graf_mouse(M_OFF,NULL);
    wind_update(BEG_UPDATE);

    app_wh = wind_create(app_wflags, 0, 0, 200, 200);
    if(app_wh<=0)
        return -1;

#ifdef PCGEM
    wind_set(app_wh, WF_NAME, FPOFF(app_title), FPSEG(app_title), 0, 0);
#else

#ifdef MGEMLIB
    wind_set(app_wh, WF_NAME, HIWORD(app_title), LOWORD(app_title), 0, 0);
#else
    wind_set(app_wh, WF_NAME, app_title);
#endif
#endif

    if(new == 1) {
        /* Get the desktop */
        wind_get(0, WF_WXYWH, &app_wdw.g_x,
                             &app_wdw.g_y,
                             &app_wdw.g_w,
                             &app_wdw.g_h);
    
        app_wdw.g_w = min(STARTWIDTH,app_wdw.g_w);
        app_wdw.g_h = min(STARTHEIGHT,app_wdw.g_h);
    }

    wind_open(app_wh, app_wdw.g_x, app_wdw.g_y, app_wdw.g_w, app_wdw.g_h);
    wind_get(app_wh, WF_WXYWH, &app_wdw.g_x,
                               &app_wdw.g_y,
                               &app_wdw.g_w,
                               &app_wdw.g_h);


    wind_update(END_UPDATE);
    graf_mouse(M_ON,NULL);
    return 1;
}

int init_app()
{
WORD work_in[11];
WORD work_out[57];

WORD ww;

WORD i;
WORD txtwidth,txtheight;

    app_accid = appl_init();

    /* VDI handle... */
    app_vh = graf_handle(&txtwidth,&txtheight,&i,&i);
    for(i=0;i<10;i++) work_in[i] = 1;
    work_in[10] = 2;
    v_opnvwk(work_in,&app_vh,work_out);

    /* RSC load */
    if(rsrc_load(RCS_FILE) == 0) {
        form_alert(1,"[3][The resource file was not found][Exit]");
        return -1;
    }

    wind_update(BEG_UPDATE);
    rsrc_gaddr(R_TREE,RCMENU,&app_menu);
    menu_bar(app_menu,1);
    wind_update(END_UPDATE);

    rsrc_gaddr(R_TREE,ABOUT,&about_box);

    return open_window(1);
}

/* Not necessary yet... */
void do_redraw()
{
GRECT box, me;

WORD pts[4];

#ifdef DEBUG
    printf("REDRAW!\n");
#endif

    graf_mouse(M_OFF,NULL);
    wind_update(BEG_UPDATE);

    wind_get(app_wh, WF_WXYWH, &me.g_x, &me.g_y, &me.g_w, &me.g_h);
    wind_get(app_wh, WF_FIRSTXYWH, &box.g_x,
                                   &box.g_y,
                                   &box.g_w,
                                   &box.g_h);

    while(box.g_w || box.g_h) {
        if(rc_intersect(&me, &box)) {
            pts[0] = box.g_x;
            pts[1] = box.g_y;
            pts[2] = box.g_w+box.g_x-1;
            pts[3] = box.g_h+box.g_y-1;
            vs_clip(app_vh, 1, pts);
            draw_field(app_vh, &app_wdw);
        }
        wind_get(app_wh, WF_NEXTXYWH, &box.g_x,
                                     &box.g_y,
                                     &box.g_w,
                                     &box.g_h);
    }

    /* Turn off clipping */
    vs_clip(app_vh, 0, pts);
    wind_update(END_UPDATE);
    graf_mouse(M_ON,NULL);
}

void do_window_change(GRECT *rect)
{
    wind_set(app_wh, WF_CXYWH, rect->g_x, rect->g_y, rect->g_w, rect->g_h);
    wind_get(app_wh, WF_WXYWH, &app_wdw.g_x, &app_wdw.g_y, &app_wdw.g_w, &app_wdw.g_h);
}

void hndl_keys(WORD key, WPLAYER *player)
{
char asc;

    asc = key & 127;
    switch(asc) {
        case 'w':
        case 'W':
            player->dir = WUP;
            break;
        case 's':
        case 'S':
            player->dir = WDOWN;
            break;
        case 'a':
        case 'A':
            player->dir = WLEFT;
            break;
        case 'd':
        case 'D':
            player->dir = WRIGHT;
            break;
    }

}

int main(int argc, char *argv[])
{
WORD ret;
WORD msg[8];
int done;
WORD top;
WORD dummy;

WORD special_keys;
WORD key;

WPLAYER *player;

int playing = 0;
int palive, falive;

int tailx, taily;

#ifdef __GEMLIB__
EVMULT_IN evin;
EVMULT_OUT evout;
#endif

    srand(time(NULL));

    if(init_app() > 0) {
        done = 0;

#ifdef DEBUG
        printf("AppID: %d\n",app_accid);
#endif
        player = init_player();
        field_init();
        food_init();

#ifdef DEBUG
        printf("Game init complete\n");
#endif

#ifdef __GEMLIB__
        memset(&evin, 0, sizeof(EVMULT_IN));
        evin.emi_flags = MU_KEYBD | MU_MESAG | MU_TIMER;
        evin.emi_tlow = (int16_t)MOVEDELAY;
#endif

        msg[0] = AC_OPEN;
        msg[4] = app_accid;
        ret = MU_MESAG;
        done = -2;
        while(done <=0) {

            if(done == -2)
                done = -1;
            else {
#ifdef __GEMLIB__
                ret = evnt_multi_fast(&evin, msg, &evout);
                key = evout.emo_kreturn;
#else
                ret = evnt_multi(MU_KEYBD | MU_MESAG | MU_TIMER, /* Accept keypresses, messages, and timers */
                         0, 0, 0,                                            /* Mouse clicks, which buttons, and states */
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,               /* Mouse movement info */
                         msg,                                                    /* 16-byte message array */
                         (WORD)MOVEDELAY, 0,                                 /* The low and high word of the timer in ms */
                         &dummy, &dummy,                                     /* Mouse position @ mouse event */
                         &dummy,                                                 /* Mouse button status */
                         &special_keys,                                  /* "Special" key statuses */
                         &key,                                               /* Key pressed scancode */
                         &dummy);                                            /* # of mouse button presses */
#endif
            }
#ifdef DEBUG
            printf("maybe MSG: %d\n",msg[0]);
#endif
            /* Key presses */
            if((ret & MU_KEYBD) && playing > 0) {
                hndl_keys(key, player);

            /* Timer - update game */
            } else if((ret & MU_TIMER) && playing > 0) {
                wind_update(BEG_UPDATE);
                palive = update_player(player, &tailx, &taily);
                falive = update_field(player);

                food_check(app_vh, &app_wdw, player);

                /* form_alert(1,"[1][Update][Ok]"); */
                /* draw_field(app_vh, &app_wdw); */
                incremental_draw(app_vh, &app_wdw, player->head->c_x, player->head->c_y);
                incremental_draw(app_vh, &app_wdw, tailx, taily);

                wind_update(END_UPDATE);

                if(palive == DEAD || falive == DEAD) {
                    form_alert(1,"[3][You've Died!|Good try, though...][Reset]");
                    reset_player(player);
                    playing = 0;
                }

            /* Messages (menu, resizes, etc.) */
            } else if(ret & MU_MESAG) {
#ifdef DEBUG
                printf("MSG: %d\n",msg[0]);
#endif
                switch(msg[0]) {
                    case AC_OPEN:
                        if(msg[4] != app_accid)
                            break;
                        if(app_wh > 0) {
                            wind_get(0,WF_TOP, &top, &dummy, &dummy, &dummy);
                            if(top != app_wh) {
                                wind_set(app_wh, WF_TOP, app_wh, 0, 0, 0);
                                wind_set(app_wh, WF_CXYWH, 30,30,100,100);
                            }
                        } else
                            open_window(0);
                        break;

                    case AC_CLOSE:
                        if(msg[3] == app_accid)
                            app_wh = -1;
                        break;

                    case WM_CLOSED:
                        if(msg[3] != app_wh || app_wh < 0)
                            break;
                        graf_mouse(M_OFF,NULL);
                        wind_close(app_wh);
                        wind_delete(app_wh);
                        graf_mouse(M_ON,NULL);
                        app_wh = -1;
                        done = 1;
                        break;

                    case WM_TOPPED:
                        wind_set(app_wh,WF_TOP,app_wh,0,0,0);
                        break;

                    case WM_MOVED:
                    case WM_SIZED:
                        do_window_change((GRECT *)&msg[4]);
                    case WM_REDRAW:
                        update_field(player);
                        do_redraw();
                        break;

                    case MN_SELECTED:
                        switch(msg[4]) {
                            case MQUIT:
                                msg[0] = WM_CLOSED;
                                msg[3] = app_wh;
                                appl_write(app_accid,sizeof(msg),msg);
                                break;
                            case MNEW:
                               update_field(player);
                               draw_field(app_vh, &app_wdw);
                                playing = 1;
                                break;
                            case MABOUT:
                                hndl_about();
                                break;
                        }
                        break;

                } /* switch */

            } /* if */

            if(app_wh <= 0 )
                continue;

            /* done = 0; */

        } /* while */

        v_clsvwk(app_vh);
        /* wind_close(app_wh); */
        /* wind_delete(app_wh); */
        appl_exit();

    }

    return 0;
}
