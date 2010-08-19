-- Test creating a pattern from a GdkPixbuf and drawing it onto a
-- GtkDrawingArea widget.  I've provided controls for tweaking some of the
-- options controlling how the drawing is done, and clicking on the drawing
-- area sets the position to translate to.  There's a gradient drawn behind
-- the image so that the alpha blending can be more easily seen.

local Cairo = require "oocairo"

if not pcall(require, "gtk") then
    error("This example requires the 'gtk' module (Lua-Gnome) to be installed")
end

local IMAGE_FILENAME = "examples/images/smiley.xpm"
local MAX_SCALE, SCALE_STEP = 20, 0.1
local FILTERS = { "fast", "good", "best", "nearest", "bilinear", "gaussian" }
local EXTENDMODES = { "none", "repeat", "reflect", "pad" }

-- Options describing the way the image should be drawn.
local canvas_wd, canvas_ht = 300, 300
local image_x, image_y = canvas_wd/2, canvas_ht/2
local scale_x, scale_y = 1, 1
local filter, extendmode = "best", "none"
local opacity = 1

local canvas    -- drawing area widget, created below
local bg_grad   -- background surface

-- Event handler callbacks for the controls, bound automatically by name in
-- the .ui file.
local event_handlers = {
    x_scaling_changed = function (widget)
        scale_x = widget:get_value()
        canvas:queue_draw()
    end,

    y_scaling_changed = function (widget)
        scale_y = widget:get_value()
        canvas:queue_draw()
    end,

    opacity_changed = function (widget)
        opacity = widget:get_value()
        canvas:queue_draw()
    end,

    filter_changed = function (widget)
        filter = FILTERS[widget:get_active() + 1]
        canvas:queue_draw()
    end,

    extend_changed = function (widget)
        extendmode = EXTENDMODES[widget:get_active() + 1]
        canvas:queue_draw()
    end,

    canvas_clicked = function (widget, event)
        local click = event.button
        image_x, image_y = click.x, click.y
        canvas:queue_draw()
    end,

    -- This is triggered when the drawing area gets resized, in which case the
    -- background gradient needs to be recreated to match.  Also, this is
    -- called manually once during initialization.
    canvas_size_set = function (widget, size)
        canvas_wd, canvas_ht = size.width, size.height
        bg_grad = Cairo.pattern_create_linear(0, 0, canvas_wd, canvas_ht)
        bg_grad:add_color_stop_rgb(0, 1, 0, 0)  -- from red
        bg_grad:add_color_stop_rgb(1, 0, 0, 1)  -- to blue
    end,
}

local function initialize_canvas_widget ()
    local imgpattern
    local function handle_expose (widget, event)
        local window = widget.window
        local cr = Cairo.context_create_gdk(window)

        -- Draw gradient background (without this we get whatever the Gtk+
        -- theme says the background should be).
        cr:set_source(bg_grad)
        cr:paint()

        -- Position and scale image.
        cr:translate(image_x, image_y)
        cr:scale(scale_x, scale_y)

        -- Use the GdkPixbuf object as the source image.
        if not imgpattern then
            local img = gtk.gdk_pixbuf_new_from_file(IMAGE_FILENAME, nil)
            assert(img, "error loading image file")
            -- Create a pattern for drawing the image with.  This only has to
            -- be done once, but we wait until the first expose event to do it
            -- so that there's a suitable context object on hand.  The X and Y
            -- coordinates should give the pattern an origin at its center.
            cr:set_source_pixbuf(img, -img:get_width()/2, -img:get_height()/2)
            imgpattern = cr:get_source()
        else
            cr:set_source(imgpattern)
        end

        -- Paint the image using current drawing settings.
        imgpattern:set_filter(filter)
        imgpattern:set_extend(extendmode)
        cr:paint_with_alpha(opacity)

        return false
    end

    -- Tell Gtk+ not to bother clearing the background of the drawing area,
    -- because we're going to draw everything ourselves.  Don't do this if
    -- you're only drawing on part of the GdkWindow, and want the normal
    -- Gtk+ background to show through.
    canvas:set_app_paintable(true)

    event_handlers.canvas_size_set(canvas, {width=canvas_wd, height=canvas_ht})
    canvas:connect("expose-event", handle_expose)
end

local function create_app_window ()
    -- Load GUI window layout.
    local filename = arg[0]:gsub("%.lua$", ".ui", 1)
    local builder = gtk.builder_new()
    local rc, err = builder:add_from_file(filename, nil)
    if rc == 0 then error(err.message) end

    canvas = builder:get_object("canvas")
    initialize_canvas_widget()

    -- Set controls to default values.
    builder:get_object("x_scale"):set_value(scale_x)
    builder:get_object("y_scale"):set_value(scale_y)
    builder:get_object("opacity"):set_value(opacity)
    builder:get_object("filter_choice"):set_active(2)
    builder:get_object("extend_choice"):set_active(0)

    builder:connect_signals_full(event_handlers)
    return builder:get_object("appwindow")
end

local window = create_app_window()
window:connect("destroy", gtk.main_quit)
window:show()
gtk.main()

-- vi:ts=4 sw=4 expandtab
