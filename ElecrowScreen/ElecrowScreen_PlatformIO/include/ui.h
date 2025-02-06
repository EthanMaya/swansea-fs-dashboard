// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 8.3.6
// Project name: demo1

#ifndef _DEMO1_UI_H
#define _DEMO1_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_comp.h"
#include "ui_comp_hook.h"
#include "ui_events.h"

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t * ui_Screen1;
void ui_event_Bar2(lv_event_t * e);
extern lv_obj_t * ui_Bar2;
extern lv_obj_t * ui_Label3;
extern lv_obj_t * ui_Label4;
extern lv_obj_t * ui_Label5;
extern lv_obj_t * ui_Label6;
extern lv_obj_t * ui_Label7;
extern lv_obj_t * ui_Label1;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_Label8;
// CUSTOM VARIABLES

// EVENTS
extern lv_obj_t * ui____initial_actions0;

// FONTS
LV_FONT_DECLARE(ui_font_Font1);
LV_FONT_DECLARE(ui_font_Font100);
LV_FONT_DECLARE(ui_font_Font150);
LV_FONT_DECLARE(ui_font_Font250);

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
