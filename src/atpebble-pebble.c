#include <pebble.h>

enum {
    KEY_STOPNAME_0 = 0x0,
    KEY_STOPNAME_1 = 0x1,
    KEY_BUSID_0 = 0x2,
    KEY_BUSID_1 = 0x3,
    KEY_ERROR = 0x4
}; 
    
static Window *window;
static TextLayer *stopname_layer_0;
static TextLayer *stopname_layer_1;
static TextLayer *stoptime_layer_0;
static TextLayer *stoptime_layer_1;
static TextLayer *refreshing_layer;

static void set_refreshing_layer(bool from, bool to) {
    layer_set_hidden((Layer *)stopname_layer_0, to);
    layer_set_hidden((Layer *)stopname_layer_1, to);
    layer_set_hidden((Layer *)stoptime_layer_0, to);
    layer_set_hidden((Layer *)stoptime_layer_1, to);
    layer_set_hidden((Layer *)refreshing_layer, from);
}

static void update_data() {
    set_refreshing_layer(false, true);
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Refreshing");
    update_data();
}

static void config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
    // check accel here
    APP_LOG(APP_LOG_LEVEL_INFO, "Pebble was shaked");
    update_data();
}

static void main_window_load(Window *window) {
    window_set_background_color(window, GColorBlack);

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Origin = coordinate of upper-lefthand corner of rectangle
    // Size = size of rectangle
    stopname_layer_0 = text_layer_create((GRect) { .origin = { 0, -3 }, .size = { bounds.size.w, 22 } });
    text_layer_set_background_color(stopname_layer_0, GColorBlack);
    text_layer_set_text_color(stopname_layer_0, GColorWhite);
    text_layer_set_text(stopname_layer_0, "");
    text_layer_set_text_alignment(stopname_layer_0, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stopname_layer_0));
    
    stoptime_layer_0 = text_layer_create((GRect) { .origin = { 0, 14  }, .size = { bounds.size.w, 60 } });
    text_layer_set_background_color(stoptime_layer_0, GColorClear);
    text_layer_set_text_color(stoptime_layer_0, GColorWhite);
    text_layer_set_text(stoptime_layer_0, "");
    text_layer_set_text_alignment(stoptime_layer_0, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stoptime_layer_0));

    stopname_layer_1 = text_layer_create((GRect) { .origin = { 0, 70 }, .size = { bounds.size.w, 22 } });
    text_layer_set_background_color(stopname_layer_1, GColorBlack);
    text_layer_set_text_color(stopname_layer_1, GColorWhite);
    text_layer_set_text(stopname_layer_1, "");
    text_layer_set_text_alignment(stopname_layer_1, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stopname_layer_1));
    
    stoptime_layer_1 = text_layer_create((GRect) { .origin = { 0, 88 }, .size = { bounds.size.w, 60 } });
    text_layer_set_background_color(stoptime_layer_1, GColorClear);
    text_layer_set_text_color(stoptime_layer_1, GColorWhite);
    text_layer_set_text(stoptime_layer_1, "");
    text_layer_set_text_alignment(stoptime_layer_1, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stoptime_layer_1));

    refreshing_layer = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w, 100 } });
    text_layer_set_background_color(refreshing_layer, GColorClear);
    text_layer_set_text_color(refreshing_layer, GColorWhite);
    text_layer_set_text(refreshing_layer, "Refreshing...");
    text_layer_set_text_alignment(refreshing_layer, GTextAlignmentCenter);
    text_layer_set_font(refreshing_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(refreshing_layer));
}

static void main_window_unload(Window *window) {
    text_layer_destroy(stopname_layer_0);
    text_layer_destroy(stopname_layer_1);
    text_layer_destroy(stoptime_layer_0);
    text_layer_destroy(stoptime_layer_1);
    text_layer_destroy(refreshing_layer);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message recieved from JS");
    
    static char name_buffer0[18];
    static char name_buffer1[18];
    static char time_buffer0[63];
    static char time_buffer1[63];
    
    Tuple *t = dict_read_first(iter);

    while (t != NULL) {
        switch (t->key) {
        case KEY_STOPNAME_0:
            snprintf(name_buffer0, sizeof(name_buffer0), "%s", t->value->cstring);
            break;
        case KEY_STOPNAME_1:
            snprintf(name_buffer1, sizeof(name_buffer1), "%s", t->value->cstring);
            break;
        case KEY_BUSID_0:
            snprintf(time_buffer0, sizeof(time_buffer0), "%s", t->value->cstring);
            break;
        case KEY_BUSID_1:
            snprintf(time_buffer1, sizeof(time_buffer1), "%s", t->value->cstring);
            APP_LOG(APP_LOG_LEVEL_INFO, time_buffer1);
            break; 
        case KEY_ERROR:
            snprintf(name_buffer0, sizeof(name_buffer0), "%s", t->value->cstring);
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
            break;
        }
        t = dict_read_next(iter);
    }
    
    text_layer_set_text(stopname_layer_0, name_buffer0);
    text_layer_set_font(stopname_layer_0, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

    text_layer_set_text(stopname_layer_1, name_buffer1);
    text_layer_set_font(stopname_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    
    text_layer_set_text(stoptime_layer_0, time_buffer0);
    text_layer_set_font(stoptime_layer_0, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    
    text_layer_set_text(stoptime_layer_1, time_buffer1);
    text_layer_set_font(stoptime_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    
    set_refreshing_layer(true, false);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Inbox message dropped");
}

static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason,void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox could not send");
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox sent successfully to JS");
}

static void init(void) {
    window = window_create();

    window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
    });
    
    const bool animated = true;
    window_stack_push(window, animated);
    
    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

    accel_tap_service_subscribe(&accel_tap_handler);
}

static void deinit(void) {
    window_destroy(window);
    accel_tap_service_unsubscribe();
}

int main(void) {
    init();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
    app_event_loop();
    deinit();
}
