//
//  SplashWindow.c
//  first
//
//  Created by Francesco Di Carlo on 26/01/14.
//  Copyright (c) 2014 francescodicarlo. All rights reserved.
//

#include "SplashWindow.h"

static Window *splash;
static Layer *splash_layer;
static BitmapLayer *logo_layer;
static GBitmap *logo = NULL;

static void timer_callback(void *contest)
{
    window_stack_remove(splash, true);
}

void splashCreator()
{
    splash = window_create();
	window_set_background_color(splash, GColorBlack);
    window_set_window_handlers(splash, (WindowHandlers) {
        .load = splashLoad,
        .unload = splashUnload,
    });
    window_stack_push(splash, false);
    
    const uint32_t timeout_ms = 1000;
    APP_LOG(APP_LOG_LEVEL_INFO, "Done initializing, pushed window: %p", splash);
    app_timer_register(timeout_ms, timer_callback, NULL);
}
void splashLoad(Window *splash)
{
    splash_layer = window_get_root_layer(splash);
    logo_layer=bitmap_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 168} });
    layer_set_bounds(bitmap_layer_get_layer(logo_layer), GRect(0, 0, 144, 168));
    logo = gbitmap_create_with_resource(RESOURCE_ID_LOGO);
    bitmap_layer_set_bitmap(logo_layer, logo);
    layer_add_child(splash_layer, bitmap_layer_get_layer(logo_layer));
}
void splashUnload(Window *splash)
{
    layer_destroy(splash_layer);
    bitmap_layer_destroy(logo_layer);
    gbitmap_destroy(logo);
}











