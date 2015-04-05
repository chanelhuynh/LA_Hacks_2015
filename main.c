/*
 * main.c
 * Sets up the Window, ClickConfigProvider and ClickHandlers.
 */
 
#include <pebble.h>

static Window *s_main_window_i;    // Input
static Window *s_main_window_t;   // Notes + Countdown

static TextLayer *s_output_layer_i; 
static TextLayer *s_output_layer_t;

static AppTimer *s_timer_half;
static AppTimer *s_timer_min;

static char s_uptime_buffer[32];

// Nandini
int counter = 0;
static Window *s_main_window_n;
static TextLayer *s_output_layer_n;

// Alex
static void f_init();
static void update_text();
#define REPEAT_INTERVAL_MS 50

// You can define defaults for values in persistent storage
#define NUM_FIDGETS_DEFAULT 0

static Window *s_main_window;

static TextLayer *s_header_layer, *s_body_layer, *s_label_layer;
static GBitmap *s_icon_plus, *s_icon_minus;

static int s_num_fidgets = NUM_FIDGETS_DEFAULT;

// End Alex
static int min = 0;
static int sec = 0;    // make sure it doesnt go over 60
static int saved_min = 0;
static int saved_sec = 0;
static int millis = 0;
static int warn_half = 0;
static int warn_min = 0;

static int state = 0;  // Like a state machine

// Notecards
void update_Notecard(int counter) {
  switch(counter) {
    case 0:
      text_layer_set_text(s_output_layer_n, "Hello, hackers, judges, and friends..." );
      break;
    case 1:
      text_layer_set_text(s_output_layer_n, "This is a one-stop app to make your next presentation a hit...");
      break;
    case 2:
      text_layer_set_text(s_output_layer_n, "Coded at LA HACKS 2015!");
      break;
    case 3:
      text_layer_set_text(s_output_layer_n, "Face your fears. Starting with this one.");
      break;
  }
}

static void main_window_load_n(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  // Create output textLayer
  s_output_layer_n = text_layer_create(GRect(5, 0, window_bounds.size.w-10, window_bounds.size.h/2));
  text_layer_set_overflow_mode(s_output_layer_n, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer_n));
}

static void main_window_unload_n(Window *window) {
  // Destroy output textLayer
  text_layer_destroy(s_output_layer_n);
}

void down_single_click_handler_n(ClickRecognizerRef recognizer, void *context) {
  update_Notecard(counter);
  if (counter == 4)
    counter = 3;
  counter++;
}

void up_single_click_handler_n(ClickRecognizerRef recognizer, void *context) {
  update_Notecard(counter);
    if (counter <0)
    counter = 0;
  counter--;

}

void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler_n);
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler_n);
}

// Input
static void printInput() {
  snprintf(s_uptime_buffer, sizeof(s_uptime_buffer), "Input the timer...\n%.2d:%.2d", min, sec);
  text_layer_set_text(s_output_layer_i, s_uptime_buffer);
}

static void printMinSec() {
  snprintf(s_uptime_buffer, sizeof(s_uptime_buffer), "%.2d:%.2d", min, sec);
  text_layer_set_text(s_output_layer_t, s_uptime_buffer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  (state) ? sec++ : min++;
  printInput();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(!state) state = 1;
    else state = 0;
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {

  // Add 10
  if (state) 
    sec = sec + 10;
  else 
    min = min + 10;
  
  if (sec >= 59) sec = 59;
  if (min >= 59) min = 59;
  printInput();
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  if (state) // Sub 10
    sec = sec - 10;
  else 
    min = min - 10;
  
  if (sec < 0) sec = 0;
  if (min < 0) min = 0;  
  printInput();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  (state) ? sec-- : min--;
  if (sec < 0) sec = 0;
  if (min < 0) min = 0;
  printInput();
}

static void timer_callback_half(void *data) {
  vibes_double_pulse();
  text_layer_set_text(s_output_layer_t, "Buzz!");
}

static void timer_callback_min(void *data) {
  vibes_double_pulse();
  vibes_double_pulse();
  text_layer_set_text(s_output_layer_t, "1 minute left!");
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
 
  if (min == 0 && sec == 0) {
    tick_timer_service_unsubscribe();
    light_enable(true);
    vibes_long_pulse();
    psleep(2000); // Pebble guy says to use apptimer instead if it messes up other stuff
    text_layer_set_text(s_output_layer_t, "Time's up!");
    psleep(2000);
    window_stack_pop(true);
    
    f_init();
  }
   
  else {
    sec--;
  
    if (sec < 0) { 
      sec = 59; 
      min--;
    }
    
    printMinSec();  
  }
}

// Alex
static void tap_handler(AccelAxisType axis, int32_t direction) {
  s_num_fidgets++;
}
static void main_window_load_f(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  int width = 120;
  //Layer for header
  s_header_layer = text_layer_create(GRect(4, 0, width, 60));
  text_layer_set_font(s_header_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(s_header_layer, GColorClear);
  text_layer_set_text(s_header_layer, "Fidget Counter");
  layer_add_child(window_layer, text_layer_get_layer(s_header_layer));

  //Layer for text
  s_body_layer = text_layer_create(GRect(4, 44, width, 60));
  text_layer_set_font(s_body_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_background_color(s_body_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_body_layer));

  //Layer for subtext
  s_label_layer = text_layer_create(GRect(4, 44 + 28, width, 60));
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text(s_label_layer, "fidgets in your presentation");
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));
  
  update_text();
}

static void update_text() {
  static char s_body_text[18];
  snprintf(s_body_text, sizeof(s_body_text), "You had %u", s_num_fidgets/2);
  text_layer_set_text(s_body_layer, s_body_text);
}
// End Alex

static void main_window_load_t(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  // Add timer layer
  s_output_layer_t = text_layer_create(GRect(5, 5, window_bounds.size.w - 10 ,window_bounds.size.h/4));
  text_layer_set_text(s_output_layer_t, "Starting timer...");
  text_layer_set_font(s_output_layer_t, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_output_layer_t, GTextAlignmentCenter);
  text_layer_set_background_color(s_output_layer_t, GColorBlack);
  text_layer_set_text_color(s_output_layer_t, GColorWhite);
  text_layer_set_text_alignment(s_output_layer_t, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer_t));

  // Add notecard layer
  s_output_layer_n = text_layer_create(GRect(5, window_bounds.size.h/4, window_bounds.size.w-10, window_bounds.size.h));
  text_layer_set_overflow_mode(s_output_layer_n, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer_n));
   
  // Add timer 
  s_timer_half = app_timer_register(warn_half, timer_callback_half, NULL);
  s_timer_min = app_timer_register(warn_min, timer_callback_min, NULL);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_output_layer_t); // Destroy output textLayer
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Set status
  state = 2;
  saved_min = min;
  saved_sec = sec;
  
  // Set up values for timer stage
  millis = 1000 * (min * 60 + sec);  // Timer time in milliseconds
  warn_half = millis / 2;            // Timer warning at half
  warn_min = millis - 60000;         // Timer warning at 1 min left
  
  // Alex put this here
  accel_tap_service_subscribe(tap_handler);
  
  // Push second window
  window_set_fullscreen(s_main_window_t, true);
  window_stack_push(s_main_window_t, true);
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers - Short click
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, NULL, select_long_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 0, NULL, up_long_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 0, NULL, down_long_click_handler);
}

static void main_window_load_i(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create output textLayer
  s_output_layer_i = text_layer_create(GRect(5, 0, window_bounds.size.w - 10, window_bounds.size.h));
  text_layer_set_font(s_output_layer_i, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  printInput();

  text_layer_set_overflow_mode(s_output_layer_i, GTextOverflowModeWordWrap);
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer_i));
}

static void main_input_init() {
  min = saved_min;
  sec = saved_sec;
  printInput();
}

// Alex
static void f_init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load_f,
    .unload = main_window_unload,
  });
  // Subscribe to taps
  
  window_stack_push(s_main_window, true);
}

static void init() {
  // Create input window
  s_main_window_i = window_create();
  window_set_window_handlers(s_main_window_i, (WindowHandlers) {
    .load = main_window_load_i,
    .unload = main_window_unload,
    .appear = main_input_init
  });
  
  window_set_click_config_provider(s_main_window_i, click_config_provider);
  
  // Second set up window
  s_main_window_t = window_create();
  window_set_window_handlers(s_main_window_t, (WindowHandlers) {
    .load = main_window_load_t,
    .unload = main_window_unload,
  });  
  window_set_click_config_provider(s_main_window_t, config_provider);
  
  window_stack_push(s_main_window_i, true);
}

static void deinit() {
  window_destroy(s_main_window);
  window_destroy(s_main_window_i);
  window_destroy(s_main_window_t);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
