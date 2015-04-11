

#include "SplashWindow.h"


enum {
    STATUS,
    HUMIDITY,
    TEMP,
    ICON,
    WIND,
    SOURCE,
    PLACE
};


static Window *window;
static Layer *window_layer;

static TextLayer *clock_layer;
static TextLayer *temp_layer;
static TextLayer *hum_layer;
static TextLayer *wind_layer;
static TextLayer *kmh_layer;
static TextLayer *source_layer;
static TextLayer *place_layer;


static BitmapLayer *iconbt_layer;
static BitmapLayer *iconch_layer;
static BitmapLayer *icon_layer;
static BitmapLayer *iconh_layer;
static BitmapLayer *iconw_layer;
//static BitmapLayer *icons_layer;

static GBitmap *iconbt;
static GBitmap *iconch;
static GBitmap *icon;
static GBitmap *iconh;
static GBitmap *iconw;
//static GBitmap *icons   =   NULL;

static GFont font_xsmall;
static GFont font_small;
static GFont font_medium;
static GFont font_large;

static char ora[] = "00:00";
static char timeFormat[5];

static uint16_t percent=-1;

static const uint32_t INBOUND_SIZE = 128; // Inbound app message size
static const uint32_t OUTBOUND_SIZE = 128; // Outbound app message size

static bool ready = false;

static char place_buff[10]= "\0";
static char humidity_buff[10] = "\0";
static char temp_buff[10] = "\0";
static char icon_buff[10] = "\0";
static char wind_buff[10] = "\0";
static char source_buff[20];

static time_t time_stamp = 0;
static const time_t INTERVAL = 1*60;

/**
 * Aggiorna i valori del meteo nei layer.
 */

static void update_values()
{
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Can't find icon: %s", icon_buff);
    if(icon)
        gbitmap_destroy(icon);
	// Set appropriate icon based on icon code
	if(strcmp(icon_buff, "01d") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_DAY);
    
    else if(strcmp(icon_buff, "01n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_CLEAR_NIGHT);
    
	else if(strcmp(icon_buff, "02d") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_FEW_CLOUDS_DAY);
	
    else if(strcmp(icon_buff, "02n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_FEW_CLOUDS_NIGHT);
    
	else if(strcmp(icon_buff, "03d") == 0 || strcmp(icon_buff, "04d") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_CLOUDS);
    
	else if(strcmp(icon_buff, "03n") == 0 || strcmp(icon_buff, "04n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_CLOUDS);
    
	else if(strcmp(icon_buff, "09d") == 0 || strcmp(icon_buff, "09n") == 0 ||
			strcmp(icon_buff, "10d") == 0 || strcmp(icon_buff, "10n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_RAIN);
    
	else if(strcmp(icon_buff, "11d") == 0 || strcmp(icon_buff, "11n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_THUNDERSTORM);
    
    else if(strcmp(icon_buff, "13d") == 0 || strcmp(icon_buff, "13n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_SNOW);
    
	else if(strcmp(icon_buff, "50d") == 0 || strcmp(icon_buff, "50n") == 0)
        icon = gbitmap_create_with_resource(RESOURCE_ID_CLOUDS);
    
	else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Can't find icon: %s", icon_buff);
		icon = NULL;
	}
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ICON OK");
	if(icon != NULL)
        bitmap_layer_set_bitmap(icon_layer, icon);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "ICON OK");
    text_layer_set_text(temp_layer, temp_buff);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "temp OK");
    text_layer_set_text(wind_layer, wind_buff);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "wind OK");
    text_layer_set_text(place_layer, place_buff);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "place OK");
    text_layer_set_text(hum_layer, humidity_buff);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "hum OK");
    text_layer_set_text(source_layer, source_buff);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "source OK");
}


/**
 * Richiede all'applicazione sul telefono le informazioni sul meteo.
 */
static void check_weather() {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletCString(STATUS, "retrieve");
    dict_write_tuplet(iter, &value);
    app_message_outbox_send();
    APP_LOG(APP_LOG_LEVEL_INFO, "Sent Retrive");
}

// Ricevuta di consegna avvenuta.
void out_send_handler (DictionaryIterator *sent, void *context)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "Message sent");
}

// ricevuta di consegna fallita.
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "ERROR ERRROR ERROR");
	if (reason==APP_MSG_APP_NOT_RUNNING)
		APP_LOG(APP_LOG_LEVEL_INFO, "Pebble app not running.");
}



// funzione che gestisce i messaggi ricevuti dal telefono

void in_receivede_handler(DictionaryIterator *received, void *context)
{
    char *status=(char*)dict_find(received, STATUS)->value;
    
    if(status!=NULL)
    {
    	APP_LOG(APP_LOG_LEVEL_INFO, "status ok");
    	if(strcmp(status, "ready") == 0)
    	{
        	

            //LOAD STORED DATA
            strcpy(temp_buff, (char*)dict_find(received, TEMP)->value);
            strcpy(humidity_buff, (char*)dict_find(received, HUMIDITY)->value);
            strcpy(icon_buff, (char*)dict_find(received, ICON)->value);
            strcpy(wind_buff, (char*)dict_find(received, WIND)->value);
            strcpy(place_buff, (char*)dict_find(received, PLACE)->value);
            strcpy(source_buff, "Loading...");
			ready = true;
			
            //update_values();
			APP_LOG(APP_LOG_LEVEL_INFO, "%s", icon_buff);
            time_stamp = time(NULL);
            check_weather();
    	}
        else if(strcmp(status, "configUpdated") == 0)
    	{
                time_stamp = time(NULL);
                check_weather();
    	}

        
    	else if(strcmp(status, "reporting") == 0) {
            
        	APP_LOG(APP_LOG_LEVEL_INFO, "Recieved status \"reporting\"");
            
        	// Copia i dati ricevuti nel messaggio nei buffer opportuni.
        	strcpy(temp_buff, (char*)dict_find(received, TEMP)->value);
        	strcpy(humidity_buff, (char*)dict_find(received, HUMIDITY)->value);
        	strcpy(icon_buff, (char*)dict_find(received, ICON)->value);
            strcpy(wind_buff, (char*)dict_find(received, WIND)->value);
            strcpy(place_buff, (char*)dict_find(received, PLACE)->value);
            strcpy(source_buff, (char*)dict_find(received, SOURCE)->value);

            update_values();
    	}
        else if(strcmp(status, "failed") == 0) {
        	strcpy(source_buff, "Connection\nFailed");
        }
    	else {
        	
    	}
	}
	else
	{
        APP_LOG(APP_LOG_LEVEL_INFO, "datas error");
	}
}


void in_dropped_handler(AppMessageResult reason, void *context)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "ERROR ERROR ERROR");

}


// Controlla se è tempo di aggiornare le informazioni.
static bool time_to_refresh() {
    time_t time_passed = time(NULL) - time_stamp;
    
    if(time_passed >= INTERVAL) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Time to refresh");
        return true;
    }
    
    else
    {
        return false;
    }
}

// Funzione che viene richiamata al cambiamento di orario (MINUTI)
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
   if(time_to_refresh() && ready && !layer_get_hidden( bitmap_layer_get_layer(iconbt_layer))) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Tutto a posto");
        time_stamp = time(NULL);
        check_weather();
    }
    strftime(ora, sizeof(ora),timeFormat, tick_time);  // STRTIME copia l'ora nell'array ora.
    text_layer_set_text(clock_layer, ora);
}

static void bluetooth_handler(bool connected)
{
    if(connected)
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "Connected");
		#ifdef PBL_BW
        	layer_set_hidden( bitmap_layer_get_layer(iconbt_layer), false);
		#else
			if(iconbt)
        		gbitmap_destroy(iconbt);
    		iconbt=gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH);
		#endif
    }
    else
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "Connection Lost");
		#ifdef PBL_BW
        	layer_set_hidden( bitmap_layer_get_layer(iconbt_layer), true);
        	vibes_short_pulse();
		#else
			if(iconbt)
        		gbitmap_destroy(iconbt);
    			iconbt=gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_OFF);
				vibes_short_pulse();
		#endif
    }
}

static void battery_handler(BatteryChargeState charge_state)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "Battery State ...");
    
	if(iconch)
        gbitmap_destroy(iconch);
        
    if (100>=charge_state.charge_percent && charge_state.charge_percent>=95)
            iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE100);
    else if (94>=charge_state.charge_percent && charge_state.charge_percent>=85)
            iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE90);
    else if (84>=charge_state.charge_percent && charge_state.charge_percent>=75)
		iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE80);
    else if (74>=charge_state.charge_percent && charge_state.charge_percent>=65)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE70);
    else if (64>=charge_state.charge_percent && charge_state.charge_percent>=55)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE60);
    else if (54>=charge_state.charge_percent && charge_state.charge_percent>=45)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE50);
    else if (44>=charge_state.charge_percent && charge_state.charge_percent>=35)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE40);
    else if (34>=charge_state.charge_percent && charge_state.charge_percent>=25)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE30);
    else if (24>=charge_state.charge_percent && charge_state.charge_percent>=15)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE20);
    else if (14>=charge_state.charge_percent && charge_state.charge_percent>=05)
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE10);
    else 
        iconch = gbitmap_create_with_resource(RESOURCE_ID_CHARGE0);
	
    bitmap_layer_set_bitmap(iconch_layer, iconch);
}
// carica la window con tutti i layer
static void window_load(Window *window) {
    window_layer = window_get_root_layer(window);
    
    
    font_xsmall = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_12));
    font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_22));
    font_medium	= fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_37));
    font_large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_43));
    
    // ------ Setting up source_layer ------
    source_layer = text_layer_create((GRect) { .origin = { 20, 0 }, .size = { 104, 27} });
    text_layer_set_text_color(source_layer, GColorWhite);
	#ifdef PBL_BW
    	text_layer_set_background_color(source_layer, GColorBlack);
	#else
		text_layer_set_background_color(source_layer, GColorVividCerulean   );
	#endif
    text_layer_set_font(source_layer, font_xsmall);
    text_layer_set_text_alignment(source_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(source_layer));
    text_layer_set_text(source_layer, "Loading...");
    
    
    // ------ Setting up clock_layer ------
    clock_layer = text_layer_create((GRect) { .origin = { 1, 120 }, .size = { 144, 50} });
    text_layer_set_text_color(clock_layer, GColorWhite);
	#ifdef PBL_BW
    	text_layer_set_text_color(clock_layer, GColorBlack);
	#else
		text_layer_set_text_color(clock_layer, GColorWhite );
	#endif
	#ifdef PBL_BW
    	text_layer_set_background_color(clock_layer, GColorBlack);
	#else
		text_layer_set_background_color(clock_layer, GColorVividCerulean  );
	#endif
    text_layer_set_font(clock_layer, font_large);
    text_layer_set_text_alignment(clock_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(clock_layer));
    
    
    // ------ Setting temp_layer ------
    temp_layer= text_layer_create((GRect) { .origin = { 47, 33 }, .size = { 92, 39} });
    text_layer_set_background_color(temp_layer, GColorWhite);
    text_layer_set_text_color(temp_layer, GColorBlack);
    text_layer_set_font(temp_layer, font_medium);
    text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(temp_layer));
    
    //------- Setting place_layer ------
    place_layer= text_layer_create((GRect) { .origin = {47 , 28 }, .size = {92 , 15} });
    text_layer_set_background_color(place_layer, GColorWhite);
    text_layer_set_text_color(place_layer, GColorBlack);
    text_layer_set_font(place_layer, font_xsmall);
    text_layer_set_text_alignment(place_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(place_layer));
    
    //------ Setting wind_layer ------
    wind_layer= text_layer_create((GRect) { .origin = {26 , 99 }, .size = {113 , 25} });
    text_layer_set_background_color(wind_layer, GColorWhite);
    text_layer_set_text_color(wind_layer, GColorBlack);
    text_layer_set_font(wind_layer, font_small);
    text_layer_set_text_alignment(wind_layer, GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(wind_layer));
    
    //------ Setting hum_layer ------
    hum_layer= text_layer_create((GRect) { .origin = {26 , 73 }, .size = { 113, 25} });
    text_layer_set_background_color(hum_layer, GColorWhite);
    text_layer_set_text_color(hum_layer, GColorBlack);
    text_layer_set_font(hum_layer, font_small);
    text_layer_set_text_alignment(hum_layer, GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(hum_layer));
    
    // ------ Setting up iconbt_layer ------
    iconbt_layer=bitmap_layer_create((GRect) { .origin = { 0, 5 }, .size = { 20, 20} });
    layer_set_bounds(bitmap_layer_get_layer(iconbt_layer), GRect(0, 0, 20, 20));
	if(iconbt)
        gbitmap_destroy(iconbt);
    iconbt=gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH);
    bitmap_layer_set_bitmap(iconbt_layer, iconbt);

	#ifdef PBL_BW
    	bitmap_layer_set_background_color(iconbt_layer, GColorBlack);
	#else
		bitmap_layer_set_background_color(iconbt_layer, GColorVividCerulean);
		bitmap_layer_set_compositing_mode(iconbt_layer, GCompOpSet);
	#endif
		
    layer_set_hidden( bitmap_layer_get_layer(iconbt_layer), true);
    layer_add_child(window_layer, bitmap_layer_get_layer(iconbt_layer));
    bluetooth_handler(bluetooth_connection_service_peek());
    
    // ------ Setting up iconch_layer ------
    
    iconch_layer=bitmap_layer_create((GRect) { .origin = { 124, 5 }, .size = { 20, 20} });
    layer_set_bounds(bitmap_layer_get_layer(iconch_layer), GRect(0, 0, 20, 20));
	
	#ifdef PBL_BW
    	bitmap_layer_set_background_color(iconch_layer, GColorBlack);
	#else
		bitmap_layer_set_background_color(iconch_layer, GColorVividCerulean   );
		bitmap_layer_set_compositing_mode(iconch_layer, GCompOpSet);
	#endif
		
    layer_add_child(window_layer, bitmap_layer_get_layer(iconch_layer));
    battery_handler(battery_state_service_peek());
    
    // ------ Setting up icon_layer ------
    icon_layer=bitmap_layer_create((GRect) { .origin = { 5, 28 }, .size = { 41, 44} });
    layer_set_bounds(bitmap_layer_get_layer(icon_layer), GRect(0, 0, 41, 44));
    bitmap_layer_set_background_color(icon_layer, GColorWhite);
    layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));
    
    // ------ Setting up iconh_layer ------
    iconh_layer=bitmap_layer_create((GRect) { .origin = { 5, 73 }, .size = { 25, 25} });
    layer_set_bounds(bitmap_layer_get_layer(iconh_layer), GRect(0, 0, 25, 25));
	if(iconh)
        gbitmap_destroy(iconh);
    iconh=gbitmap_create_with_resource(RESOURCE_ID_HUMIDITY);
    bitmap_layer_set_bitmap(iconh_layer, iconh);
    bitmap_layer_set_background_color(iconh_layer, GColorWhite);
    layer_add_child(window_layer, bitmap_layer_get_layer(iconh_layer));
    
    // ------ Setting up iconw_layer ------
    iconw_layer=bitmap_layer_create((GRect) { .origin = { 5, 99 }, .size = { 25, 25} });
    layer_set_bounds(bitmap_layer_get_layer(iconw_layer), GRect(0, 0, 25, 25));
	if(iconw)
        gbitmap_destroy(iconw);
    iconw=gbitmap_create_with_resource(RESOURCE_ID_WIND);
    bitmap_layer_set_bitmap(iconw_layer, iconw);
    bitmap_layer_set_background_color(iconw_layer, GColorWhite);
    layer_add_child(window_layer, bitmap_layer_get_layer(iconw_layer));
    
	APP_LOG(APP_LOG_LEVEL_INFO, "STO CARICANDO I LAYER");
	if(ready==true)
	{
		update_values();
	}
}

// richiamata alla chiusura deinit(), dealloca la memoria occupata dau layers
static void window_unload(Window *window) {
	
    text_layer_destroy(temp_layer);
    text_layer_destroy(clock_layer);
    text_layer_destroy(wind_layer);
    text_layer_destroy(hum_layer);
    text_layer_destroy(kmh_layer);
    
	if (iconbt)
		gbitmap_destroy(iconbt);
	if (icon)
		gbitmap_destroy(icon);
	if (iconh)
		gbitmap_destroy(iconh);
	if (iconw)
		gbitmap_destroy(iconw);
   
    bitmap_layer_destroy(iconbt_layer);
    bitmap_layer_destroy(icon_layer);
    bitmap_layer_destroy(iconh_layer);
    bitmap_layer_destroy(iconw_layer);
	
    fonts_unload_custom_font(font_xsmall);
    fonts_unload_custom_font(font_small);
    fonts_unload_custom_font(font_medium);
    fonts_unload_custom_font(font_large);
    
}
static void setHourStyle()
{
    if(clock_is_24h_style())
        strcpy(timeFormat, "%H:%M");
    else
        strcpy(timeFormat, "%I:%M");
    
}

static void init(void) {
    //INIT APP_MESSAGE
    app_message_register_inbox_received(in_receivede_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_send_handler);
    app_message_register_outbox_failed(out_failed_handler);
    app_message_open(INBOUND_SIZE, OUTBOUND_SIZE);

    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
	
    #ifdef PBL_BW
    	window_set_background_color(window, GColorBlack);
	#else
		window_set_background_color(window, GColorVividCerulean   );
	#endif
		
    window_stack_push(window, true);
    splashCreator();
    
    setHourStyle();
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

    bluetooth_connection_service_subscribe(&bluetooth_handler);
    battery_state_service_subscribe(&battery_handler);

    //144 × 168 pixel


    
}

static void deinit(void) {
    
    tick_timer_service_unsubscribe();

    app_message_deregister_callbacks();

    bluetooth_connection_service_unsubscribe();

    battery_state_service_unsubscribe();

    window_destroy(window);

}

int main(void) {
    init();
    
    APP_LOG(APP_LOG_LEVEL_INFO, "Done initializing, pushed window: %p", window);
    
    app_event_loop();
    deinit();
}


