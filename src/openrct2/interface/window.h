#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "../common.h"
#include "../drawing/drawing.h"
#include "../management/Research.h"
#include "../peep/Peep.h"
#include "../ride/ride.h"
#include "../ride/TrackDesign.h"
#include "../ride/vehicle.h"
#include "../scenario/scenario.h"
#include "../world/park.h"
#include "colour.h"

struct rct_window;
union rct_window_event;
struct track_design_file_ref;
struct TitleSequence;
struct TextInputSession;
typedef struct scenario_index_entry scenario_index_entry;

#define TEXT_INPUT_SIZE 1024
#define TOP_TOOLBAR_HEIGHT 27

#ifdef __cplusplus
extern "C" {
#endif

extern uint16 TextInputDescriptionArgs[4];
extern char gTextBoxInput[TEXT_INPUT_SIZE];
extern sint32 gMaxTextBoxInputLength;
extern sint32 gTextBoxFrameNo;
extern bool gUsingWidgetTextBox;
extern struct TextInputSession * gTextInput;

typedef void wndproc(struct rct_window*, union rct_window_event*);

typedef uint8 rct_windowclass;
typedef uint16 rct_windownumber;
typedef sint16 rct_widgetindex;

typedef struct window_identifier {
    rct_windowclass classification;
    rct_windownumber number;
} window_identifier;

typedef struct widget_identifier {
    window_identifier window;
    rct_widgetindex widget_index;
} widget_identifier;

extern widget_identifier gCurrentTextBox;

/**
 * Widget structure
 * size: 0x10
 */
typedef struct rct_widget {
    uint8 type;                     // 0x00
    uint8 colour;                   // 0x01
    sint16 left;                    // 0x02
    sint16 right;                   // 0x04
    sint16 top;                     // 0x06
    sint16 bottom;                  // 0x08
    union {                         // 0x0A
        uint32 image;
        rct_string_id text;
        uint32 content;
        utf8 * string;
    };
    rct_string_id tooltip;          // 0x0E
} rct_widget;

/**
 * Viewport structure
 */
typedef struct rct_viewport {
    sint16 width;                   // 0x00
    sint16 height;                  // 0x02
    sint16 x;                       // 0x04
    sint16 y;                       // 0x06
    sint16 view_x;                  // 0x08
    sint16 view_y;                  // 0x0A
    sint16 view_width;              // 0x0C
    sint16 view_height;             // 0x0E
    uint32 flags;                   // 0x12
    uint8 zoom;                     // 0x10
    uint8 var_11;
    uint8 visibility;               // VISIBILITY_CACHE
} rct_viewport;

/**
 * Scroll structure
 * size: 0x12
 */
typedef struct rct_scroll {
    uint16 flags;               // 0x00
    uint16 h_left;              // 0x02
    uint16 h_right;             // 0x04
    uint16 h_thumb_left;        // 0x06
    uint16 h_thumb_right;       // 0x08
    uint16 v_top;               // 0x0A
    uint16 v_bottom;            // 0x0C
    uint16 v_thumb_top;         // 0x0E
    uint16 v_thumb_bottom;      // 0x10
} rct_scroll;

/**
 * Viewport focus structure.
 * size: 0xA
 * Use sprite.type to work out type.
 */
typedef struct coordinate_focus {
    sint16 var_480;
    sint16 x; //0x482
    sint16 y; //0x484 & VIEWPORT_FOCUS_Y_MASK
    sint16 z; //0x486
    uint8 rotation;//0x488
    uint8 zoom;//0x489
    sint16 width;
    sint16 height;
} coordinate_focus;

// Type is viewport_target_sprite_id & 0x80000000 != 0
typedef struct sprite_focus {
    sint16 var_480;
    uint16 sprite_id; //0x482
    uint8 pad_484;
    uint8 type; //0x485 & VIEWPORT_FOCUS_TYPE_MASK
    uint16 pad_486;
    uint8 rotation; //0x488
    uint8 zoom; //0x489
} sprite_focus;

#define VIEWPORT_FOCUS_TYPE_MASK 0xC0
enum{
    VIEWPORT_FOCUS_TYPE_COORDINATE = (1<<6),
    VIEWPORT_FOCUS_TYPE_SPRITE = (1<<7)
};
#define VIEWPORT_FOCUS_Y_MASK 0x3FFF

typedef struct rct_window_event_list {
    void (*close)(struct rct_window*);
    void (*mouse_up)(struct rct_window*, rct_widgetindex);
    void (*resize)(struct rct_window*);
    void (*mouse_down)(struct rct_window*, rct_widgetindex, rct_widget*);
    void (*dropdown)(struct rct_window*, rct_widgetindex, sint32);
    void (*unknown_05)(struct rct_window*);
    void (*update)(struct rct_window*);
    void (*unknown_07)(struct rct_window*);
    void (*unknown_08)(struct rct_window*);
    void (*tool_update)(struct rct_window*, rct_widgetindex, sint32, sint32);
    void (*tool_down)(struct rct_window*, rct_widgetindex, sint32, sint32);
    void (*tool_drag)(struct rct_window*, rct_widgetindex, sint32, sint32);
    void (*tool_up)(struct rct_window*, rct_widgetindex, sint32, sint32);
    void (*tool_abort)(struct rct_window*, rct_widgetindex);
    void (*unknown_0E)(struct rct_window*);
    void (*get_scroll_size)(struct rct_window*, sint32, sint32*, sint32*);
    void (*scroll_mousedown)(struct rct_window*, sint32, sint32, sint32);
    void (*scroll_mousedrag)(struct rct_window*, sint32, sint32, sint32);
    void (*scroll_mouseover)(struct rct_window*, sint32, sint32, sint32);
    void (*text_input)(struct rct_window*, rct_widgetindex, char*);
    void (*viewport_rotate)(struct rct_window*);
    void (*unknown_15)(struct rct_window*, sint32, sint32);
    void (*tooltip)(struct rct_window*, rct_widgetindex, rct_string_id*);
    void (*cursor)(struct rct_window*, rct_widgetindex, sint32, sint32, sint32*);
    void (*moved)(struct rct_window*, sint32, sint32);
    void (*invalidate)(struct rct_window*);
    void (*paint)(struct rct_window*, rct_drawpixelinfo*);
    void (*scroll_paint)(struct rct_window*, rct_drawpixelinfo*, sint32);
} rct_window_event_list;

typedef struct campaign_variables {
    sint16 campaign_type;
    sint16 no_weeks; //0x482
    uint16 ride_id; //0x484
    uint32 pad_486;
} campaign_variables;

typedef struct new_ride_variables {
    sint16 selected_ride_id; //0x480
    sint16 highlighted_ride_id; //0x482
    uint16 pad_484;
    uint16 pad_486;
    uint16 selected_ride_countdown; //488
} new_ride_variables;

typedef struct news_variables {
    sint16 var_480;
    sint16 var_482;
    uint16 var_484;
    uint16 var_486;
    uint16 var_488;
} news_variables;

typedef struct map_variables {
    sint16 rotation;
    sint16 var_482;
    uint16 var_484;
    uint16 var_486;
    uint16 var_488;
} map_variables;

typedef struct ride_variables {
    sint16 view;
    sint32 var_482;
    sint32 var_486;
} ride_variables;

typedef struct scenery_variables {
    sint16 selected_scenery_id;
    sint16 hover_counter;
} scenery_variables;

typedef struct track_list_variables {
    bool track_list_being_updated;
    bool reload_track_designs;
} track_list_variables;

typedef struct error_variables {
    uint16 var_480;
} error_variables;

/**
 * Window structure
 * size: 0x4C0
 */
typedef struct rct_window {
    rct_window_event_list* event_handlers;      // 0x000
    rct_viewport* viewport;     // 0x004
    uint64 enabled_widgets;     // 0x008
    uint64 disabled_widgets;    // 0x010
    uint64 pressed_widgets;     // 0x018
    uint64 hold_down_widgets;   // 0x020
    rct_widget* widgets;        // 0x028
    sint16 x;                   // 0x02C
    sint16 y;                   // 0x02E
    sint16 width;               // 0x030
    sint16 height;              // 0x032
    sint16 min_width;           // 0x034
    sint16 max_width;           // 0x036
    sint16 min_height;          // 0x038
    sint16 max_height;          // 0x03A
    rct_windownumber number;    // 0x03C
    uint16 flags;               // 0x03E
    rct_scroll scrolls[3];      // 0x040
    uint8 list_item_positions[1024];// 0x076
    uint16 no_list_items;           // 0x476 0 for no items
    sint16 pad_478;
    sint16 selected_list_item;      // 0x47A -1 for none selected
    sint16 pad_47C;
    sint16 pad_47E;
    union {
        coordinate_focus viewport_focus_coordinates;
        sprite_focus viewport_focus_sprite;
        campaign_variables campaign;
        new_ride_variables new_ride;
        news_variables news;
        map_variables map;
        ride_variables ride;
        scenery_variables scenery;
        track_list_variables track_list;
        error_variables error;
    };
    sint16 page;                    // 0x48A
    union {
        sint16 picked_peep_old_x;   // 0x48C staff/guest window: peep x gets set to 0x8000 on pickup, this is the old value
        sint16 vehicleIndex;        // 0x48C Ride window: selected car when setting vehicle colours
        sint16 numberOfStaff;       // 0x48C Used in park window.
        sint16 var_48C;
    };
    uint16 frame_no;                // 0x48E updated every tic for motion in windows sprites
    uint16 list_information_type;   // 0x490 0 for none, Used as current position of marquee in window_peep
    union {
        sint16 picked_peep_frame;   // 0x492 Animation frame of picked peep in staff window and guest window
        sint16 var_492;
    };
    union {                         // 0x494
        uint32 highlighted_item;
        uint16 ride_colour;
        rct_research_item* research_item;
        rct_object_entry* object_entry;
        const scenario_index_entry* highlighted_scenario;
        struct {
            uint16 var_494;
            uint16 var_496;
        };
    };
    uint8 var_498[0x14];
    sint16 selected_tab;            // 0x4AC
    sint16 var_4AE;
    uint16 viewport_target_sprite;  // 0x4B0 viewport target sprite
    sint16 saved_view_x;            // 0x4B2
    sint16 saved_view_y;            // 0x4B4
    rct_windowclass classification; // 0x4B6
    uint8 pad_4B7;
    sint8 var_4B8;
    sint8 var_4B9;
    uint8 colours[6];           // 0x4BA
    uint8 visibility;           // VISIBILITY_CACHE
} rct_window;

#define RCT_WINDOW_RIGHT(w) (w->x + w->width)
#define RCT_WINDOW_BOTTOM(w) (w->y + w->height)

typedef enum {
    WE_CLOSE = 0,
    WE_MOUSE_UP = 1,
    WE_RESIZE = 2,
    WE_MOUSE_DOWN = 3,
    WE_DROPDOWN = 4,
    WE_UNKNOWN_05 = 5,
    // Unknown 05: Used to update tabs that are not being animated
    // see window_peep. When the overview tab is not highlighted the
    // items being carried such as hats/balloons still need to be shown
    // and removed. Probably called after anything that affects items
    // being carried.
    WE_UPDATE = 6,
    WE_UNKNOWN_07 = 7,
    WE_UNKNOWN_08 = 8,
    WE_TOOL_UPDATE = 9,
    WE_TOOL_DOWN = 10,
    WE_TOOL_DRAG = 11,
    WE_TOOL_UP = 12,
    WE_TOOL_ABORT = 13,
    WE_UNKNOWN_0E = 14,
    WE_SCROLL_GETSIZE = 15,
    WE_SCROLL_MOUSEDOWN = 16,
    WE_SCROLL_MOUSEDRAG = 17,
    WE_SCROLL_MOUSEOVER = 18,
    WE_TEXT_INPUT = 19,
    WE_VIEWPORT_ROTATE = 20,
    WE_UNKNOWN_15 = 21, // scroll mouse move?
    WE_TOOLTIP = 22,
    WE_CURSOR = 23,
    WE_MOVED = 24,
    WE_INVALIDATE = 25,
    WE_PAINT = 26,
    WE_SCROLL_PAINT = 27,
} WINDOW_EVENTS;

typedef enum {
    /*
    WF_TIMEOUT_SHL = 0,
    WF_TIMEOUT_MASK = 7,
    WF_DRAGGING = 1 << 3,
    WF_SCROLLER_UP = 1 << 4,
    WF_SCROLLER_DOWN = 1 << 5,
    WF_SCROLLER_MIDDLE = 1 << 6,
    WF_DISABLE_VP_SCROLL = 1 << 9,
    */

    WF_STICK_TO_BACK = (1 << 0),
    WF_STICK_TO_FRONT = (1 << 1),
    WF_NO_SCROLLING = (1 << 2), // User is unable to scroll this viewport
    WF_SCROLLING_TO_LOCATION = (1 << 3),
    WF_TRANSPARENT = (1 << 4),
    WF_NO_BACKGROUND = (1 << 5), // Instead of half transparency, completely remove the window background
    WF_7 = (1 << 7),
    WF_RESIZABLE = (1 << 8),
    WF_NO_AUTO_CLOSE = (1 << 9), // Don't auto close this window if too many windows are open
    WF_10 = (1 << 10),
    WF_WHITE_BORDER_ONE = (1 << 12),
    WF_WHITE_BORDER_MASK = (1 << 12) | (1 << 13),

    WF_NO_SNAPPING = (1 << 15)
} WINDOW_FLAGS;

enum SCROLL_FLAGS {
    HSCROLLBAR_VISIBLE = (1 << 0),
    HSCROLLBAR_THUMB_PRESSED = (1 << 1),
    HSCROLLBAR_LEFT_PRESSED = (1 << 2),
    HSCROLLBAR_RIGHT_PRESSED = (1 << 3),
    VSCROLLBAR_VISIBLE = (1 << 4),
    VSCROLLBAR_THUMB_PRESSED = (1 << 5),
    VSCROLLBAR_UP_PRESSED = (1 << 6),
    VSCROLLBAR_DOWN_PRESSED = (1 << 7),
};

#define SCROLLBAR_SIZE          16

enum {
    SCROLL_PART_NONE = -1,
    SCROLL_PART_VIEW = 0,
    SCROLL_PART_HSCROLLBAR_LEFT = 1,
    SCROLL_PART_HSCROLLBAR_RIGHT = 2,
    SCROLL_PART_HSCROLLBAR_LEFT_TROUGH = 3,
    SCROLL_PART_HSCROLLBAR_RIGHT_TROUGH = 4,
    SCROLL_PART_HSCROLLBAR_THUMB = 5,
    SCROLL_PART_VSCROLLBAR_TOP = 6,
    SCROLL_PART_VSCROLLBAR_BOTTOM = 7,
    SCROLL_PART_VSCROLLBAR_TOP_TROUGH = 8,
    SCROLL_PART_VSCROLLBAR_BOTTOM_TROUGH = 9,
    SCROLL_PART_VSCROLLBAR_THUMB = 10,
};

enum {
    WC_MAIN_WINDOW = 0,
    WC_TOP_TOOLBAR = 1,
    WC_BOTTOM_TOOLBAR = 2,
    WC_TOOLTIP = 5,
    WC_DROPDOWN = 6,
    WC_ABOUT = 8,
    WC_PUBLISHER_CREDITS = 9,
    WC_MUSIC_CREDITS = 10,
    WC_ERROR = 11,
    WC_RIDE = 12,
    WC_RIDE_CONSTRUCTION = 13,
    WC_SAVE_PROMPT = 14,
    WC_RIDE_LIST = 15,
    WC_CONSTRUCT_RIDE = 16,
    WC_DEMOLISH_RIDE_PROMPT = 17,
    WC_SCENERY = 18,
    WC_OPTIONS = 19,
    WC_FOOTPATH = 20,
    WC_LAND = 21,
    WC_WATER = 22,
    WC_PEEP = 23,
    WC_GUEST_LIST = 24,
    WC_STAFF_LIST = 25,
    WC_FIRE_PROMPT = 26,
    WC_PARK_INFORMATION = 27,
    WC_FINANCES = 28,
    WC_TITLE_MENU = 29,
    WC_TITLE_EXIT = 30,
    WC_RECENT_NEWS = 31,
    WC_SCENARIO_SELECT = 32,
    WC_TRACK_DESIGN_LIST = 33,
    WC_TRACK_DESIGN_PLACE = 34,
    WC_NEW_CAMPAIGN = 35,
    WC_KEYBOARD_SHORTCUT_LIST = 36,
    WC_CHANGE_KEYBOARD_SHORTCUT = 37,
    WC_MAP = 38,
    WC_TITLE_LOGO = 39,
    WC_BANNER = 40,
    WC_MAP_TOOLTIP = 41,
    WC_EDITOR_OBJECT_SELECTION = 42,
    WC_EDITOR_INVENTION_LIST = 43,
    WC_EDITOR_INVENTION_LIST_DRAG = 44,
    WC_EDITOR_SCENARIO_OPTIONS = 45,
    WC_EDTIOR_OBJECTIVE_OPTIONS = 46,
    WC_MANAGE_TRACK_DESIGN = 47,
    WC_TRACK_DELETE_PROMPT = 48,
    WC_INSTALL_TRACK = 49,
    WC_CLEAR_SCENERY = 50,
    WC_NOTIFICATION_OPTIONS = 109,
    WC_CHEATS = 110,
    WC_RESEARCH = 111,
    WC_VIEWPORT = 112,
    WC_TEXTINPUT = 113,
    WC_MAPGEN = 114,
    WC_LOADSAVE = 115,
    WC_LOADSAVE_OVERWRITE_PROMPT = 116,
    WC_TITLE_OPTIONS = 117,
    WC_LAND_RIGHTS = 118,
    WC_THEMES = 119,
    WC_TILE_INSPECTOR = 120,
    WC_CHANGELOG = 121,
    WC_TITLE_EDITOR = 122,
    WC_TITLE_COMMAND_EDITOR = 123,
    WC_MULTIPLAYER = 124,
    WC_PLAYER = 125,
    WC_NETWORK_STATUS = 126,
    WC_SERVER_LIST = 127,
    WC_SERVER_START = 128,
    WC_CUSTOM_CURRENCY_CONFIG = 129,
    WC_DEBUG_PAINT = 130,
    WC_VIEW_CLIPPING = 131,
    WC_OBJECT_LOAD_ERROR = 132,

    // Only used for colour schemes
    WC_STAFF = 220,
    WC_EDITOR_TRACK_BOTTOM_TOOLBAR = 221,
    WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR = 222,
    WC_CHAT = 223,

    WC_NULL = 255,
};


enum {
    WV_PARK_AWARDS,
    WV_PARK_RATING,
    WV_PARK_OBJECTIVE,
    WV_PARK_GUESTS,
    WV_FINANCES_RESEARCH,
    WV_RIDE_RESEARCH,

};

enum {
    WD_BANNER,
    WD_NEW_CAMPAIGN,
    WD_DEMOLISH_RIDE,
    WD_SIGN,
    WD_SIGN_SMALL,

    WD_PLAYER,
};

#define validate_global_widx(wc, widx) \
    static_assert(widx == wc ## __ ## widx, "Global WIDX of " #widx " doesn't match actual value.")

#define WC_MAIN_WINDOW__0                               0
#define WC_TOP_TOOLBAR__WIDX_PAUSE                      0
#define WC_TOP_TOOLBAR__WIDX_LAND                       8
#define WC_TOP_TOOLBAR__WIDX_WATER                      9
#define WC_TOP_TOOLBAR__WIDX_SCENERY                    10
#define WC_TOP_TOOLBAR__WIDX_PATH                       11
#define WC_TOP_TOOLBAR__WIDX_CLEAR_SCENERY              17
#define WC_RIDE_CONSTRUCTION__WIDX_CONSTRUCT            23
#define WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE             29
#define WC_RIDE_CONSTRUCTION__WIDX_EXIT                 30
#define WC_RIDE_CONSTRUCTION__WIDX_ROTATE               32
#define WC_SCENERY__WIDX_SCENERY_TAB_1                  4
#define WC_SCENERY__WIDX_SCENERY_ROTATE_OBJECTS_BUTTON  25
#define WC_LAND__WIDX_PREVIEW                           5
#define WC_WATER__WIDX_PREVIEW                          3
#define WC_PEEP__WIDX_PATROL                            11
#define WC_PEEP__WIDX_ACTION_LBL                        12
#define WC_PEEP__WIDX_PICKUP                            13
#define WC_TRACK_DESIGN_LIST__WIDX_ROTATE               5
#define WC_TRACK_DESIGN_PLACE__WIDX_ROTATE              3
#define WC_MAP__WIDX_LAND_TOOL                          13
#define WC_MAP__WIDX_ROTATE_90                          20
#define WC_EDITOR_OBJECT_SELECTION__WIDX_TAB_1          4
#define WC_CLEAR_SCENERY__WIDX_PREVIEW                  3
#define WC_LAND_RIGHTS__WIDX_PREVIEW                    3
#define WC_STAFF__WIDX_PICKUP                           10

enum PROMPT_MODE {
    PM_SAVE_BEFORE_LOAD = 0,
    PM_SAVE_BEFORE_QUIT,
    PM_SAVE_BEFORE_QUIT2,
    PM_QUIT
};

typedef enum {
    BTM_TB_DIRTY_FLAG_MONEY = (1 << 0),
    BTM_TB_DIRTY_FLAG_DATE = (1 << 1),
    BTM_TB_DIRTY_FLAG_PEEP_COUNT = (1 << 2),
    BTM_TB_DIRTY_FLAG_CLIMATE = (1 << 3),
    BTM_TB_DIRTY_FLAG_PARK_RATING = (1 << 4)
} BTM_TOOLBAR_DIRTY_FLAGS;

// 000N_TTTL
enum {
    LOADSAVETYPE_LOAD = 0 << 0,
    LOADSAVETYPE_SAVE = 1 << 0,

    LOADSAVETYPE_GAME = 0 << 1,
    LOADSAVETYPE_LANDSCAPE = 1 << 1,
    LOADSAVETYPE_SCENARIO = 2 << 1,
    LOADSAVETYPE_TRACK = 3 << 1,
    LOADSAVETYPE_IMAGE = 4 << 1,
};

enum {
    MODAL_RESULT_FAIL = -1,
    MODAL_RESULT_CANCEL,
    MODAL_RESULT_OK
};

enum VISIBILITY_CACHE
{
    VC_UNKNOWN,
    VC_VISIBLE,
    VC_COVERED
};

enum GUEST_LIST_FILTER_TYPE
{
    GLFT_GUESTS_ON_RIDE,
    GLFT_GUESTS_IN_QUEUE,
    GLFT_GUESTS_THINKING_ABOUT_RIDE,
    GLFT_GUESTS_THINKING_X,
};

typedef enum {
    TOOL_ARROW = 0,
    TOOL_UP_ARROW = 2,
    TOOL_UP_DOWN_ARROW = 3,
    TOOL_PICKER = 7,
    TOOL_CROSSHAIR = 12,
    TOOL_PATH_DOWN = 17,
    TOOL_DIG_DOWN = 18,
    TOOL_WATER_DOWN = 19,
    TOOL_WALK_DOWN = 22,
    TOOL_PAINT_DOWN = 23,
    TOOL_ENTRANCE_DOWN = 24,
} TOOL_IDX;

typedef void (*modal_callback)(sint32 result);

typedef void (*close_callback)();

#define WINDOW_LIMIT_MIN 4
#define WINDOW_LIMIT_MAX 64
#define WINDOW_LIMIT_RESERVED 4 // Used to reserve room for the main viewport, toolbars, etc.

// rct2: 0x01420078
extern rct_window g_window_list[WINDOW_LIMIT_MAX + WINDOW_LIMIT_RESERVED];

extern rct_window * gWindowFirst;
extern rct_window * gWindowNextSlot;
extern rct_window * gWindowAudioExclusive;

// rct2: 0x00F635EE
extern ride_list_item _window_track_list_item;

extern uint16 gWindowUpdateTicks;
extern uint8 gToolbarDirtyFlags;
extern uint16 gWindowMapFlashingFlags;

extern colour_t gCurrentWindowColours[4];

extern bool gDisableErrorWindowSound;

void window_dispatch_update_all();
void window_update_all_viewports();
void window_update_all();

void window_set_window_limit(sint32 value);

rct_window *window_create(sint32 x, sint32 y, sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags);
rct_window *window_create_auto_pos(sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags);
rct_window *window_create_centred(sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags);
void window_close(rct_window *window);
void window_close_by_class(rct_windowclass cls);
void window_close_by_number(rct_windowclass cls, rct_windownumber number);
void window_close_top();
void window_close_all();
void window_close_all_except_class(rct_windowclass cls);
rct_window *window_find_by_class(rct_windowclass cls);
rct_window *window_find_by_number(rct_windowclass cls, rct_windownumber number);
rct_window *window_find_from_point(sint32 x, sint32 y);
rct_widgetindex window_find_widget_from_point(rct_window *w, sint32 x, sint32 y);
void window_invalidate(rct_window *window);
void window_invalidate_by_class(rct_windowclass cls);
void window_invalidate_by_number(rct_windowclass cls, rct_windownumber number);
void window_invalidate_all();
void widget_invalidate(rct_window *w, rct_widgetindex widgetIndex);
void widget_invalidate_by_class(rct_windowclass cls, rct_widgetindex widgetIndex);
void widget_invalidate_by_number(rct_windowclass cls, rct_windownumber number, rct_widgetindex widgetIndex);
void window_init_scroll_widgets(rct_window *w);
void window_update_scroll_widgets(rct_window *w);
sint32 window_get_scroll_data_index(rct_window *w, rct_widgetindex widget_index);

rct_window *window_bring_to_front(rct_window *w);
rct_window *window_bring_to_front_by_class(rct_windowclass cls);
rct_window *window_bring_to_front_by_class_with_flags(rct_windowclass cls, uint16 flags);
rct_window *window_bring_to_front_by_number(rct_windowclass cls, rct_windownumber number);

void window_push_others_right(rct_window *w);
void window_push_others_below(rct_window *w1);

rct_window *window_get_main();

void window_set_location(rct_window *w, sint32 x, sint32 y, sint32 z);
void window_scroll_to_viewport(rct_window *w);
void window_scroll_to_location(rct_window *w, sint32 x, sint32 y, sint32 z);
void window_rotate_camera(rct_window *w, sint32 direction);
void window_viewport_get_map_coords_by_cursor(rct_window *w, sint16 *map_x, sint16 *map_y, sint16 *offset_x, sint16 *offset_y);
void window_viewport_centre_tile_around_cursor(rct_window *w, sint16 map_x, sint16 map_y, sint16 offset_x, sint16 offset_y);
void window_zoom_set(rct_window *w, sint32 zoomLevel, bool atCursor);
void window_zoom_in(rct_window *w, bool atCursor);
void window_zoom_out(rct_window *w, bool atCursor);
void main_window_zoom(bool zoomIn, bool atCursor);

void window_show_textinput(rct_window *w, rct_widgetindex widgetIndex, uint16 title, uint16 text, sint32 value);
void window_text_input_key(rct_window* w, char keychar);

void window_draw_all(rct_drawpixelinfo *dpi, sint16 left, sint16 top, sint16 right, sint16 bottom);
void window_draw(rct_drawpixelinfo *dpi, rct_window *w, sint32 left, sint32 top, sint32 right, sint32 bottom);
void window_draw_widgets(rct_window *w, rct_drawpixelinfo *dpi);
void window_draw_viewport(rct_drawpixelinfo *dpi, rct_window *w);

void window_set_position(rct_window *w, sint32 x, sint32 y);
void window_move_position(rct_window *w, sint32 dx, sint32 dy);
void window_resize(rct_window *w, sint32 dw, sint32 dh);
void window_set_resize(rct_window *w, sint32 minWidth, sint32 minHeight, sint32 maxWidth, sint32 maxHeight);

bool tool_set(rct_window *w, rct_widgetindex widgetIndex, TOOL_IDX tool);
void tool_cancel();

void window_close_construction_windows();

void window_update_viewport_ride_music();

// Open window functions
void window_relocate_windows(sint32 width, sint32 height);
void window_resize_gui(sint32 width, sint32 height);
void window_resize_gui_scenario_editor(sint32 width, sint32 height);
void window_top_toolbar_open();
void window_game_bottom_toolbar_open();
void window_game_bottom_toolbar_invalidate_news_item();
void window_track_list_open(ride_list_item item);
rct_window *window_ride_main_open(sint32 rideIndex);
rct_window *window_ride_open_station(sint32 rideIndex, sint32 stationIndex);
rct_window *window_ride_open_track(rct_map_element *mapElement);
rct_window *window_ride_open_vehicle(rct_vehicle *vehicle);
void window_ride_construct(rct_window *w);
rct_window *window_ride_construction_open();
rct_window *window_maze_construction_open();
void ride_construction_toolupdate_entrance_exit(sint32 screenX, sint32 screenY);
void ride_construction_toolupdate_construct(sint32 screenX, sint32 screenY);
void ride_construction_tooldown_construct(sint32 screenX, sint32 screenY);


void window_maze_construction_update_pressed_widgets();
void window_network_status_open(const char* text, close_callback onClose);
void window_network_status_close();
void window_network_status_open_password();

void window_scenery_open();
void window_tile_inspector_open();
void window_tile_inspector_clear_clipboard();
void window_text_input_open(rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description, rct_string_id existing_text, uintptr_t existing_args, sint32 maxLength);
void window_text_input_raw_open(rct_window* call_w, rct_widgetindex call_widget, rct_string_id title, rct_string_id description, utf8string existing_text, sint32 maxLength);

rct_window * window_object_load_error_open(utf8 * path, size_t numMissingObjects, const rct_object_entry * missingObjects);

rct_window * window_editor_main_open();
void window_editor_bottom_toolbar_open();
void window_editor_object_selection_open();

void window_bubble_list_item(rct_window* w, sint32 item_position);

void window_align_tabs( rct_window *w, rct_widgetindex start_tab_id, rct_widgetindex end_tab_id );

void window_map_tooltip_update_visibility();

void window_staff_list_init_vars();

void game_command_callback_pickup_guest(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
void game_command_callback_pickup_staff(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);

void window_event_close_call(rct_window* w);
void window_event_mouse_up_call(rct_window* w, rct_widgetindex widgetIndex);
void window_event_resize_call(rct_window* w);
void window_event_mouse_down_call(rct_window *w, rct_widgetindex widgetIndex);
void window_event_dropdown_call(rct_window* w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
void window_event_unknown_05_call(rct_window* w);
void window_event_update_call(rct_window *w);
void window_event_unknown_07_call(rct_window* w);
void window_event_unknown_08_call(rct_window* w);
void window_event_tool_update_call(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
void window_event_tool_down_call(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
void window_event_tool_drag_call(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
void window_event_tool_up_call(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
void window_event_tool_abort_call(rct_window* w, rct_widgetindex widgetIndex);
void window_event_unknown_0E_call(rct_window* w);
void window_get_scroll_size(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
void window_event_scroll_mousedown_call(rct_window* w, sint32 scrollIndex, sint32 x, sint32 y);
void window_event_scroll_mousedrag_call(rct_window* w, sint32 scrollIndex, sint32 x, sint32 y);
void window_event_scroll_mouseover_call(rct_window* w, sint32 scrollIndex, sint32 x, sint32 y);
void window_event_textinput_call(rct_window *w, rct_widgetindex widgetIndex, char *text);
void window_event_viewport_rotate_call(rct_window* w);
void window_event_unknown_15_call(rct_window* w, sint32 scrollIndex, sint32 scrollAreaType);
rct_string_id window_event_tooltip_call(rct_window* w, rct_widgetindex widgetIndex);
sint32 window_event_cursor_call(rct_window* w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
void window_event_moved_call(rct_window* w, sint32 x, sint32 y);
void window_event_invalidate_call(rct_window* w);
void window_event_paint_call(rct_window* w, rct_drawpixelinfo *dpi);
void window_event_scroll_paint_call(rct_window* w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

void invalidate_all_windows_after_input();
void textinput_cancel();

void window_move_and_snap(rct_window *w, sint32 newWindowX, sint32 newWindowY, sint32 snapProximity);
sint32 window_can_resize(rct_window *w);

void window_start_textbox(rct_window *call_w, rct_widgetindex call_widget, rct_string_id existing_text, char *existing_args, sint32 maxLength);
void window_cancel_textbox();
void window_update_textbox_caret();
void window_update_textbox();

bool window_is_visible(rct_window* w);

bool land_tool_is_active();
bool water_tool_is_active();
bool clear_scenery_tool_is_active();
bool scenery_tool_is_active();

//Cheat: in-game land ownership editor
void toggle_ingame_land_ownership_editor();

void window_ride_construction_keyboard_shortcut_turn_left();
void window_ride_construction_keyboard_shortcut_turn_right();
void window_ride_construction_keyboard_shortcut_use_track_default();
void window_ride_construction_keyboard_shortcut_slope_down();
void window_ride_construction_keyboard_shortcut_slope_up();
void window_ride_construction_keyboard_shortcut_chain_lift_toggle();
void window_ride_construction_keyboard_shortcut_bank_left();
void window_ride_construction_keyboard_shortcut_bank_right();
void window_ride_construction_keyboard_shortcut_previous_track();
void window_ride_construction_keyboard_shortcut_next_track();
void window_ride_construction_keyboard_shortcut_build_current();
void window_ride_construction_keyboard_shortcut_demolish_current();

bool sub_6CA2DF(sint32 *trackType, sint32 *trackDirection, sint32 *rideIndex, sint32 *_liftHillAndAlternativeState, sint32 *x, sint32 *y, sint32 *z, sint32 *properties);
money32 place_provisional_track_piece(sint32 rideIndex, sint32 trackType, sint32 trackDirection, sint32 liftHillAndAlternativeState, sint32 x, sint32 y, sint32 z);


#ifdef __cplusplus
}
#endif

#endif
