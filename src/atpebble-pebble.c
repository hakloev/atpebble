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

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
    // check accel here
    APP_LOG(APP_LOG_LEVEL_INFO, "Pebble was shaked");
    
    //text_layer_set_text(stopname_layer_berg, "Laster info...");

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
}

static void main_window_load(Window *window) {
    window_set_background_color(window, GColorBlack);

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Origin = coordinate of upper-lefthand corner of rectangle
    // Size = size of rectangle
    stopname_layer_0 = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 18 } });
    text_layer_set_background_color(stopname_layer_0, GColorClear);
    text_layer_set_text_color(stopname_layer_0, GColorWhite);
    text_layer_set_text(stopname_layer_0, "");
    text_layer_set_text_alignment(stopname_layer_0, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stopname_layer_0));
    
    stoptime_layer_0 = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 70 } });
    text_layer_set_background_color(stoptime_layer_0, GColorClear);
    text_layer_set_text_color(stoptime_layer_0, GColorWhite);
    text_layer_set_text(stoptime_layer_0, "");
    text_layer_set_text_alignment(stoptime_layer_0, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stoptime_layer_0));

    stopname_layer_1 = text_layer_create((GRect) { .origin = { 0, 73 }, .size = { bounds.size.w, 18 } });
    text_layer_set_background_color(stopname_layer_1, GColorClear);
    text_layer_set_text_color(stopname_layer_1, GColorWhite);
    text_layer_set_text(stopname_layer_1, "");
    text_layer_set_text_alignment(stopname_layer_1, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stopname_layer_1));
    
    stoptime_layer_1 = text_layer_create((GRect) { .origin = { 0, 91 }, .size = { bounds.size.w, 70 } });
    text_layer_set_background_color(stoptime_layer_1, GColorClear);
    text_layer_set_text_color(stoptime_layer_1, GColorWhite);
    text_layer_set_text(stoptime_layer_1, "");
    text_layer_set_text_alignment(stoptime_layer_1, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(stoptime_layer_1));

}

static void main_window_unload(Window *window) {
    text_layer_destroy(stopname_layer_0);
    text_layer_destroy(stopname_layer_1);
    text_layer_destroy(stoptime_layer_0);
    text_layer_destroy(stoptime_layer_1);
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message recieved from JS");
    
    static char name_buffer0[12];
    static char name_buffer1[12];
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
