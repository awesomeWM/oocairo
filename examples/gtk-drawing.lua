-- This example was intended to show some simple Cairo drawing in a
-- GtkDrawingArea widget, but it ended up being more of a demonstration
-- of some Gtk+ techniques.  Search for 'Cairo' in the code to find the
-- bits which directly use the Lua-oocairo library.
--
-- Quite accidentally this program also demonstrates how to create a
-- GdkPixbuf from a Cairo image surface, which is a useful way of rendering
-- icons with Cairo for display in Gtk+.  With a GdkPixmap you can draw
-- directly onto the image with Cairo, but pixmaps don't allow transparency.
-- For the line style icons shown in the menus in this example I wanted the
-- natural Gtk+ background to show through, so transparency is essential
-- (particularly when the icons are highlighted by being displayed against
-- a different background colour).

local Cairo = require "oocairo"

if not pcall(require, "gtk") then
    error("This example requires the 'gtk' module (Lua-Gnome) to be installed")
end

local LINE_CAP = { "butt", "square", "round" }
local LINE_JOIN = { "miter", "bevel", "round" }

local ui = {}
local canvas_wd, canvas_ht = 300, 300

local function refresh_canvas () ui.canvas:queue_draw() end
local event_handlers = {
    line_width_changed = refresh_canvas,
    line_cap_changed = refresh_canvas,
    line_join_changed = refresh_canvas,
    fill_color_changed = refresh_canvas,
    stroke_color_changed = refresh_canvas,

    canvas_size_set = function (widget, size)
        canvas_wd, canvas_ht = size.width, size.height
    end,
}

local function initialize_canvas_widget (canvas)
    local function handle_expose (widget, event)
        local window = widget.window
        local cr = Cairo.context_create_gdk(window)

        -- It's easier to define the path with a different scaling, so that
        -- the positions don't depend on the size or aspect ratio of the
        -- canvas, but we have to restore the original CTM before drawing or
        -- the stroke pen won't be circular.
        cr:save()
        cr:scale(canvas_wd, canvas_ht)
        cr:new_path()
        cr:move_to(.05, .95)
        cr:line_to(.5, .05)
        cr:curve_to(1.33, .3, .9, .8, .4, .8)
        cr:restore()

        local c = gtk.new"GdkColor"
        ui.fill_color:get_color(c)
        cr:set_source_gdk_color(c, ui.fill_color:get_alpha())
        cr:fill_preserve()

        ui.stroke_color:get_color(c)
        cr:set_source_gdk_color(c, ui.stroke_color:get_alpha())
        cr:set_line_width(ui.line_width:get_value())
        cr:set_line_cap(LINE_CAP[ui.line_cap:get_active() + 1])
        cr:set_line_join(LINE_JOIN[ui.line_join:get_active() + 1])
        cr:stroke_preserve()

        return false
    end

    canvas:set_size_request(canvas_wd, canvas_ht)
    canvas:connect("expose-event", handle_expose)

    return canvas
end

local function initialize_line_width (widget)
    widget:set_value(20)
    return widget
end

local function make_line_cap_icon (mode, height)
    local width = height * 4
    local surface = Cairo.image_surface_create("argb32", width, height)
    local cr = Cairo.context_create(surface)

    local halfline = height / 2
    cr:move_to(halfline, halfline)
    cr:line_to(width - halfline, halfline)

    cr:set_line_width(height)
    cr:set_line_cap(mode)
    cr:set_source_rgb(0, 0, 0)
    cr:stroke_preserve()

    local _, twopixels = cr:device_to_user_distance(2, 2)
    cr:set_line_width(twopixels)
    cr:set_line_cap("round")
    cr:set_source_rgb(1, 0, 0)
    cr:stroke()

    return surface:get_gdk_pixbuf()
end

local function make_line_join_icon (mode, height)
    local width = height * 4
    height = height * 2
    local linewd = height / 2
    local halfline = linewd / 2
    local surface = Cairo.image_surface_create("argb32", width, height)
    local cr = Cairo.context_create(surface)

    cr:move_to(halfline, height - halfline)
    cr:line_to(width / 2, halfline)
    cr:line_to(width - halfline, height - halfline)

    cr:set_line_width(linewd)
    cr:set_line_cap("round")
    cr:set_line_join(mode)
    cr:set_source_rgb(0, 0, 0)
    cr:stroke_preserve()

    local _, twopixels = cr:device_to_user_distance(2, 2)
    cr:set_line_width(twopixels)
    cr:set_line_cap("round")
    cr:set_source_rgb(1, 0, 0)
    cr:stroke()

    return surface:get_gdk_pixbuf()
end

local function make_icon_combo (combo, items, default_item, make_icon)
    -- Try to decide on a suitable height for the icons, based on the size
    -- of the whole combo box.  This is a bit arbitrary, but seems to come
    -- out as a good match for the font size.
    combo:map()     -- size won't be right until a real window is created
    local size = gtk.new"GtkRequisition"
    combo:size_request(size)
    local icon_height = size.height

    -- Create a list store to contain the icons and labels, and add them.
    local store = gtk.list_store_new(2, gtk.gdk_pixbuf_get_type(),
                                     gtk.G_TYPE_STRING)
    local iter = gtk.new("GtkTreeIter")
    local default_item_idx
    for i, style in ipairs(items) do
        store:append(iter)
        store:set(iter, 0, make_icon(style, icon_height), 1, style, -1)
        if style == default_item then default_item_idx = i - 1 end
    end
    combo:set_model(store)

    -- Set up renderers to display the icon next to the label.
    local renderer = gtk.cell_renderer_pixbuf_new()
    renderer:set_property("xpad", 6)
    renderer:set_property("ypad", 3)
    combo:pack_start(renderer, false)
    combo:set_attributes(renderer, "pixbuf", 0, nil)

    renderer = gtk.cell_renderer_text_new()
    combo:pack_start(renderer, true)
    combo:set_attributes(renderer, "text", 1, nil)

    combo:set_active(default_item_idx)
    return combo
end

local function initialize_line_cap_combo (combo)
    return make_icon_combo(combo, LINE_CAP, "butt", make_line_cap_icon)
end

local function initialize_line_join_combo (combo)
    return make_icon_combo(combo, LINE_JOIN, "miter", make_line_join_icon)
end

local function initialize_color (widget, r, g, b, a)
    local c = gtk.new"GdkColor"
    c.red   = r * 0xFFFF
    c.green = g * 0xFFFF
    c.blue  = b * 0xFFFF
    widget:set_color(c)
    widget:set_alpha(a * 0xFFFF)
    return widget
end

local function create_app_window ()
    -- Load GUI window layout.
    local filename = arg[0]:gsub("%.lua$", ".ui", 1)
    local builder = gtk.builder_new()
    local rc, err = builder:add_from_file(filename, nil)
    if rc == 0 then error(err.message) end

    ui.canvas = initialize_canvas_widget(builder:get_object("canvas"))
    ui.line_width = initialize_line_width(builder:get_object("line_width"))
    ui.line_cap = initialize_line_cap_combo(builder:get_object("line_cap"))
    ui.line_join = initialize_line_join_combo(builder:get_object("line_join"))
    ui.fill_color = initialize_color(builder:get_object("fill_color"),
                                     1, 1, 0, 1)
    ui.stroke_color = initialize_color(builder:get_object("stroke_color"),
                                       .5, 0, .5, 0.6)

    builder:connect_signals_full(event_handlers)
    return builder:get_object("appwindow")
end

local window = create_app_window()
window:connect("destroy", gtk.main_quit)
window:show()
gtk.main()

-- vi:ts=4 sw=4 expandtab
