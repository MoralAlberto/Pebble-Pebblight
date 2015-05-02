#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

static BitmapLayer *image_layer;
static GBitmap *image;

//Receiving messages from the phone
//http://developer.getpebble.com/2/guides/app-phone-communication.html
enum {
  AKEY_NUMBER,
  AKEY_TEXT,
};
//Receiving messages from the phone

//Enabling AppMessage in the watchapp
//http://developer.getpebble.com/2/guides/app-phone-communication.html
void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
}


void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received

  //Receiving messages from the phone
  //http://developer.getpebble.com/2/guides/app-phone-communication.html
  // Check for fields you expect to receive
  Tuple *text_tuple = dict_find(received, AKEY_TEXT);

  // Act on the found fields received
  if (text_tuple) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Text: %s", text_tuple->value->cstring);
    text_layer_set_text(text_layer, text_tuple->value->cstring);
  }else{
    APP_LOG(APP_LOG_LEVEL_INFO, "Non Text Message");
  }
  //Receiving messages from the phone END

}

 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }
////Enabling AppMessage in the watchapp END

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Waiting iPhone...");

    //Sending messages to the phone
  //http://developer.getpebble.com/2/guides/app-phone-communication.html
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  //Tuplet value = TupletInteger(1, 42);
  Tuplet value = TupletCString(1, "torch");

  dict_write_tuplet(iter, &value);

  app_message_outbox_send();
  //Sending messages to the phone END
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select Button");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select Button");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // This needs to be deinited on app exit which is when the event loop ends
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGEN_TWITTER);

  // The bitmap layer holds the image for display
  image_layer = bitmap_layer_create((GRect){ .origin = { 90, 0 }, .size = {60,160} });
  bitmap_layer_set_bitmap(image_layer, image);
  bitmap_layer_set_alignment(image_layer, GAlignCenter);
  //bitmap_layer_set_compositing_mode(image_layer, GCompOpOr);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));

  text_layer = text_layer_create((GRect) { .origin = { 0, 60 }, .size = { 80, 80 } });
  text_layer_set_text(text_layer, "Turn\nON");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  //Enabling AppMessage in the watchapp
  //http://developer.getpebble.com/2/guides/app-phone-communication.html
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  //Enabling AppMessage in the watchapp END
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
