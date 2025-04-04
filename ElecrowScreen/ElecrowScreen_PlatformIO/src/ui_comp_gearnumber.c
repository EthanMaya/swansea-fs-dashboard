// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 8.3.6
// Project name: demo1

#include "ui.h"


// COMPONENT GearNumber

lv_obj_t * ui_GearNumber_create(lv_obj_t * comp_parent)
{

    lv_obj_t * cui_GearNumber;
    cui_GearNumber = lv_textarea_create(comp_parent);
    lv_obj_set_width(cui_GearNumber, lv_pct(20));
    lv_obj_set_height(cui_GearNumber, LV_SIZE_CONTENT);    /// 4
    lv_obj_set_x(cui_GearNumber, lv_pct(30));
    lv_obj_set_y(cui_GearNumber, lv_pct(-23));
    lv_obj_set_align(cui_GearNumber, LV_ALIGN_CENTER);
    lv_textarea_set_text(cui_GearNumber, "5");
    lv_textarea_set_placeholder_text(cui_GearNumber, "Placeholder...");
    lv_obj_set_style_text_font(cui_GearNumber, &ui_font_Font250, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cui_GearNumber, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cui_GearNumber, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(cui_GearNumber, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cui_GearNumber, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_GEARNUMBER_NUM);
    children[UI_COMP_GEARNUMBER_GEARNUMBER] = cui_GearNumber;
    lv_obj_add_event_cb(cui_GearNumber, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_GearNumber, del_component_child_event_cb, LV_EVENT_DELETE, children);
    ui_comp_GearNumber_create_hook(cui_GearNumber);
    return cui_GearNumber;
}

