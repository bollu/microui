#include <SDL2/SDL.h>
#include <stdio.h>
#include <assert.h>

#include "microui-header.h"
#include "renderer.h"

static char logbuf[64000];
static int logbuf_updated = 0;
static float bg[3] = {90, 95, 100};

static void write_log(const char *text) {
    if (logbuf[0]) {
        strcat(logbuf, "\n");
    }
    strcat(logbuf, text);
    logbuf_updated = 1;
}

static void test_window(mu_Context *ctx) {
    /* do window */
    if (mu_begin_window(ctx, "Demo Window", mu_rect(40, 40, 300, 450))) {
        mu_Container *win = mu_get_current_container(ctx);
        win->rect.w = mu_max(win->rect.w, 240);
        win->rect.h = mu_max(win->rect.h, 300);

        /* window info */
        if (mu_header(ctx, "Window Info")) {
            mu_Container *win = mu_get_current_container(ctx);
            char buf[64];
            mu_layout_row(ctx, 2, (int[]){54, -1}, 0);
            mu_label(ctx, "Position:");
            sprintf(buf, "%d, %d", win->rect.x, win->rect.y);
            mu_label(ctx, buf);
            mu_label(ctx, "Size:");
            sprintf(buf, "%d, %d", win->rect.w, win->rect.h);
            mu_label(ctx, buf);
        }

        /* labels + buttons */
        if (mu_header_ex(ctx, "Test Buttons", MU_OPT_EXPANDED)) {
            mu_layout_row(ctx, 3, (int[]){86, -110, -1}, 0);
            mu_label(ctx, "Test buttons 1:");
            if (mu_button(ctx, "Button 1")) {
                write_log("Pressed button 1");
            }
            if (mu_button(ctx, "Button 2")) {
                write_log("Pressed button 2");
            }
            mu_label(ctx, "Test buttons 2:");
            if (mu_button(ctx, "Button 3")) {
                write_log("Pressed button 3");
            }

            // on button PRESS, open a PERSISTENT popup (that disappears if clicked OUTSIDE)
            if (mu_button(ctx, "Popup")) {
                mu_open_popup(ctx, "Test Popup");
            }

            // if POPUP OPEN
            // Without push_id, clicking on the final 9 button registers clicks for ALL buttons AT ONCE.
            if (mu_begin_popup(ctx, "Test Popup")) {
                mu_layout_row(ctx, 5, (int[]){10, 20, 30, 40, 50}, 0);
                for (int i = 0; i < 5; i++) {
                    // mu_push_id(ctx, &i, sizeof(i));
                    if (mu_button(ctx, "x")) {
                        printf("Pressed button %d\n", i);
                    }
                    // mu_pop_id(ctx);
                }
                mu_button(ctx, "Hello");
                mu_button(ctx, "World");
                mu_end_popup(ctx);
            } // end popup
        } // end Test Buttons
        mu_end_window(ctx);
    }  // end window.
}

static void log_window(mu_Context *ctx) {
    if (mu_begin_window(ctx, "Log Window", mu_rect(350, 40, 300, 200))) {
        /* output text panel */
        mu_layout_row(ctx, 1, (int[]){-1}, -25);
        mu_begin_panel(ctx, "Log Output");
        mu_Container *panel = mu_get_current_container(ctx);
        mu_layout_row(ctx, 1, (int[]){-1}, -1);
        mu_text(ctx, logbuf);
        mu_end_panel(ctx);
        if (logbuf_updated) {
            panel->scroll.y = panel->content_size.y;
            logbuf_updated = 0;
        }

        /* input textbox + submit button */
        static char buf[128];
        int submitted = 0;
        mu_layout_row(ctx, 2, (int[]){-70, -1}, 0);
        if (mu_textbox(ctx, buf, sizeof(buf)) & MU_RES_SUBMIT) {
            mu_set_focus(ctx, ctx->last_id);
            submitted = 1;
        }
        if (mu_button(ctx, "Submit")) {
            submitted = 1;
        }
        if (submitted) {
            write_log(buf);
            buf[0] = '\0';
        }

        mu_end_window(ctx);
    }
}

static void style_window(mu_Context *ctx) {
    static struct {
        const char *label;
        int idx;
    } colors[] = {{"text:", MU_COLOR_TEXT},
                  {"border:", MU_COLOR_BORDER},
                  {"windowbg:", MU_COLOR_WINDOWBG},
                  {"titlebg:", MU_COLOR_TITLEBG},
                  {"titletext:", MU_COLOR_TITLETEXT},
                  {"panelbg:", MU_COLOR_PANELBG},
                  {"button:", MU_COLOR_BUTTON},
                  {"buttonhover:", MU_COLOR_BUTTONHOVER},
                  {"buttonfocus:", MU_COLOR_BUTTONFOCUS},
                  {"base:", MU_COLOR_BASE},
                  {"basehover:", MU_COLOR_BASEHOVER},
                  {"basefocus:", MU_COLOR_BASEFOCUS},
                  {"scrollbase:", MU_COLOR_SCROLLBASE},
                  {"scrollthumb:", MU_COLOR_SCROLLTHUMB},
                  {NULL}};

    if (mu_begin_window(ctx, "Style Editor", mu_rect(350, 250, 300, 240))) {
        int sw = mu_get_current_container(ctx)->body.w * 0.14;
        mu_layout_row(ctx, 6, (int[]){80, sw, sw, sw, sw, -1}, 0);
        for (int i = 0; colors[i].label; i++) {
            mu_label(ctx, colors[i].label);
            uint8_slider(ctx, &ctx->style->colors[i].r, 0, 255);
            uint8_slider(ctx, &ctx->style->colors[i].g, 0, 255);
            uint8_slider(ctx, &ctx->style->colors[i].b, 0, 255);
            uint8_slider(ctx, &ctx->style->colors[i].a, 0, 255);
            mu_draw_rect(ctx, mu_layout_next(ctx), ctx->style->colors[i]);
        }
        mu_end_window(ctx);
    }
}


static const char button_map[256] = {
    [SDL_BUTTON_LEFT & 0xff] = MU_MOUSE_LEFT,
    [SDL_BUTTON_RIGHT & 0xff] = MU_MOUSE_RIGHT,
    [SDL_BUTTON_MIDDLE & 0xff] = MU_MOUSE_MIDDLE,
};

static const char key_map[256] = {
    [SDLK_LSHIFT & 0xff] = MU_KEY_SHIFT,
    [SDLK_RSHIFT & 0xff] = MU_KEY_SHIFT,
    [SDLK_LCTRL & 0xff] = MU_KEY_CTRL,
    [SDLK_RCTRL & 0xff] = MU_KEY_CTRL,
    [SDLK_LALT & 0xff] = MU_KEY_ALT,
    [SDLK_RALT & 0xff] = MU_KEY_ALT,
    [SDLK_RETURN & 0xff] = MU_KEY_RETURN,
    [SDLK_BACKSPACE & 0xff] = MU_KEY_BACKSPACE,
};

static int text_width(mu_Font font, const char *text, int len) {
    if (len == -1) {
        len = strlen(text);
    }
    return r_get_text_width(text, len);
}

static int text_height(mu_Font font) { return r_get_text_height(); }

int main(int argc, char **argv) {
    /* init SDL and renderer */
    SDL_Init(SDL_INIT_EVERYTHING);
    r_init();

    /* 1. init microui */
    mu_Context *ctx = malloc(sizeof(mu_Context));
    mu_init(ctx, text_width, text_height);

    /* 2. main loop */
    for (;;) {

        /* 2.a handle SDL events */
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    exit(EXIT_SUCCESS);
                    break;
                case SDL_MOUSEMOTION:
                    mu_input_mousemove(ctx, e.motion.x, e.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    mu_input_scroll(ctx, 0, e.wheel.y * -30);
                    break;
                case SDL_TEXTINPUT:
                    mu_input_text(ctx, e.text.text);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {
                    int b = button_map[e.button.button & 0xff];
                    if (b && e.type == SDL_MOUSEBUTTONDOWN) {
                        mu_input_mousedown(ctx, e.button.x, e.button.y, b);
                    }
                    if (b && e.type == SDL_MOUSEBUTTONUP) {
                        mu_input_mouseup(ctx, e.button.x, e.button.y, b);
                    }
                    break;
                }

                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    int c = key_map[e.key.keysym.sym & 0xff];
                    if (c && e.type == SDL_KEYDOWN) {
                        mu_input_keydown(ctx, c);
                    }
                    if (c && e.type == SDL_KEYUP) {
                        mu_input_keyup(ctx, c);
                    }
                    break;
                }
            }
        }

        /* 2.b update frame by informing microui about the logical structure of
         * our frames */
        mu_finalize_events_begin_draw(ctx);
        // style_window(ctx);
        // log_window(ctx);
        test_window(ctx);
        mu_end(ctx);


        /* 2.c render */
        r_clear(mu_color(bg[0], bg[1], bg[2], 255));
        mu_Command *cmd = NULL;
        // mu fills in the command data structure.
        while (mu_next_command(ctx, &cmd)) {
            switch (cmd->type) {
                case MU_COMMAND_TEXT:
                    r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
                    break;
                case MU_COMMAND_RECT:
                    r_draw_rect(cmd->rect.rect, cmd->rect.color);
                    break;
                case MU_COMMAND_ICON:
                    r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
                    break;
                case MU_COMMAND_CLIP:
                    r_set_clip_rect(cmd->clip.rect);
                    break;
                case MU_COMMAND_JUMP:
                    assert(0 && "unhandled error");
            }
        }
        r_present();
    }

    return 0;
}

