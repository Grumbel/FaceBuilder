#!/usr/bin/env ruby
#
# This file is gererated by ruby-glade-create-template 1.1.3.
#
require 'libglade2'
require 'face.rb'

Point = Struct.new("Point", :x, :y)

class FacebuilderGlade
  include GetText

  attr :glade
  attr :canvas

  # Creates menu hints.
  def create_uiinfo_menus(name)
    @app = @glade['FaceBuilder']
    tips = @glade.get_tooltips(@app.toplevel)
    callback_dummy = Proc.new{} #Dummy 
    uiinfos = [
               Gnome::UIInfo::menu_new_item('_New', nil, callback_dummy, nil),
               Gnome::UIInfo::menu_open_item(callback_dummy, nil),
               Gnome::UIInfo::menu_save_item(callback_dummy, nil),
               Gnome::UIInfo::menu_save_as_item(callback_dummy, nil),
               Gnome::UIInfo::menu_quit_item(callback_dummy, nil),
               Gnome::UIInfo::menu_copy_item(callback_dummy, nil),
               Gnome::UIInfo::menu_paste_item(callback_dummy, nil),
               Gnome::UIInfo::menu_properties_item(callback_dummy, nil),
               Gnome::UIInfo::menu_preferences_item(callback_dummy, nil),
               Gnome::UIInfo::menu_about_item(callback_dummy, nil),
              ]
    uiinfos[0][9]  = @glade['new1']
    uiinfos[1][9]  = @glade['open1']
    uiinfos[2][9]  = @glade['save1']
    uiinfos[3][9]  = @glade['save_as1']
    uiinfos[4][9]  = @glade['quit1']
    uiinfos[5][9]  = @glade['copy1']
    uiinfos[6][9]  = @glade['paste1']
    uiinfos[7][9]  = @glade['properties1']
    uiinfos[8][9] = @glade['preferences1']
    uiinfos[9][9] = @glade['about1']
    @app.install_menu_hints(uiinfos)
  end

  # Creates tooltips.
  def create_tooltips
    @tooltip = Gtk::Tooltips.new
    @glade['toolbutton1'].set_tooltip(@tooltip, _('New File'))
    @glade['toolbutton_open_file'].set_tooltip(@tooltip, _('Open File'))
    @glade['toolbutton_save_file'].set_tooltip(@tooltip, _('Save File'))

    @glade['toolbutton_undo'].signal_connect("clicked") { |*params| self.on_undo() }
    @glade['toolbutton_redo'].signal_connect("clicked") { |*params| self.on_redo() }

    @glade['toolbutton_open_file'].signal_connect("clicked") { |*params| on_open1_activate(nil) }
    @glade['toolbutton_save_file'].signal_connect("clicked") { |*params| on_save_as1_activate(nil) }
    @glade['toolbutton_save_as_file'].signal_connect("clicked") { |*params| on_save_as1_activate(nil) }

    @glade['toolbutton_reload'].signal_connect("clicked") { |*params| 
      @face.reload()
    }

    @glade['toolbutton_undo'].set_sensitive(false)
    @glade['toolbutton_redo'].set_sensitive(false)

    @glade['toolbutton_copy'].signal_connect("clicked")  { |*params| on_copy1_activate(nil) }
    @glade['toolbutton_paste'].signal_connect("clicked") { |*params| on_paste1_activate(nil) }

    @glade['toolbutton_size_minus'].signal_connect("clicked") { |*params| 
      part = @face.get_part(@parts[@current_part])
      part.scale=(part.scale / 1.02) if part
    }
    @glade['toolbutton_size_plus'].signal_connect("clicked")  { |*params| 
      part = @face.get_part(@parts[@current_part])
      part.scale=(part.scale * 1.02) if part
    }

    @glade['toolbutton_rotate_left'].signal_connect("clicked")  { |*params|
      part = @face.get_part(@parts[@current_part])
      part.rotation=(part.rotation - 1.0) if part
    }
    @glade['toolbutton_rotate_right'].signal_connect("clicked") { |*params| 
      part = @face.get_part(@parts[@current_part])
      part.rotation=(part.rotation + 1.0) if part
    }

    @glade['toolbutton_reset_properties'].signal_connect("clicked") {|*params|
      part = @face.get_part(@parts[@current_part])
      part.rotation=(0) if part
      part.scale=(1.0) if part
    }      
  end

  def initialize(path_or_data, root = nil, domain = nil, localedir = nil, flag = GladeXML::FILE)
    bindtextdomain(domain, localedir, nil, "UTF-8")
    @glade = GladeXML.new(path_or_data, root, domain, localedir, flag) {|handler| method(handler)}
    
    create_uiinfo_menus(domain)
    create_tooltips

    # Non Glade stuff
    @parts = [:eye, :eyebrow, :glasses, :ear, :mouth, :mouthfold, :beard, :nose, :head, :forehead, :hair, :hat]
    @current_part = 0

    @canvas = @glade['FaceCanvas']
    @face = Face.new(@canvas.root)
    @face.load("examples/pirate.xml")
    @canvas.modify_bg(Gtk::STATE_NORMAL, Gdk::Color.new(65535, 65535, 65535))

    setup_faceparts()
    setup_partselector()

    @canvas_controls = []
    group = Gnome::CanvasGroup.new(@canvas.root, {:x => 0.0, :y => 0.0 })

    setup_controls(group, Point.new(0, -200),  :hat)
    setup_controls(group, Point.new(-200, -140),   :forehead)
    setup_controls(group, Point.new(0, 200),   :head)
    setup_controls(group, Point.new(200, -140), :hair)
    
    setup_controls(group, Point.new(-200, 0), :glasses)
    
    setup_controls(group, Point.new(200, -40), :eyebrow)
    setup_controls(group, Point.new(200, 0),   :eye)
    setup_controls(group, Point.new(200, 40),  :nose)
    setup_controls(group, Point.new(200, 80),  :mouth)
    setup_controls(group, Point.new(-200, 80),  :mouthfold)

    hide_controls()

    Gnome::CanvasText.new(@canvas.root,
                          {:text => 
                            "PgUp, PgDown:\tscale\n" +
                            "Home, End:\t\trotate\n" +
                            "Cursorkeys:\t\tmove\n" +
                            "Cursor+Shift:\tmove vertical",
                            :x => -256,
                            :y => 256,
                            :font => "Sans 10",
                            :anchor => Gtk::ANCHOR_SW,
                            :fill_color => "black"})

    @canvas.signal_connect_after("button-press-event") { |widget, event|
      widget.grab_focus()
    }

    @canvas.signal_connect("key-press-event") { |widget, event|
      part = @face.get_part(@parts[@current_part])
      case event.keyval
      when Gdk::Keyval::GDK_c
        part.offset = Point.new(0, part.offset.y) if part

      when Gdk::Keyval::GDK_e
        @current_part -= 1
        if @current_part < 0 then
          @current_part = @parts.length - 1
        end
        puts @parts[@current_part]

      when Gdk::Keyval::GDK_o
        @current_part += 1
        if @current_part >= @parts.length then
          @current_part = 0
        end
        puts @parts[@current_part]

      when Gdk::Keyval::GDK_r
        @face.reload()

      when Gdk::Keyval::GDK_Page_Down
        part.scale=(part.scale / 1.02) if part

      when Gdk::Keyval::GDK_Page_Up
        part.scale=(part.scale * 1.02) if part

      when Gdk::Keyval::GDK_End
        part.rotation=(part.rotation - 1.0) if part

      when Gdk::Keyval::GDK_Home
        part.rotation=(part.rotation + 1.0) if part

      when Gdk::Keyval::GDK_a
        part.next_item if part
        
      when Gdk::Keyval::GDK_Up
        part.offset = Point.new(part.offset.x, part.offset.y - 1) if part
        
      when Gdk::Keyval::GDK_Down
        part.offset = Point.new(part.offset.x, part.offset.y + 1) if part

      when Gdk::Keyval::GDK_Left
        part.offset = Point.new(part.offset.x - 1, part.offset.y) if part

      when Gdk::Keyval::GDK_Right
        part.offset = Point.new(part.offset.x + 1, part.offset.y) if part
      end
    }

    @glade['toolbutton_copy'].set_sensitive(false)
    @glade['toolbutton_paste'].set_sensitive(false)
    @glade['copy1'].set_sensitive(false)
    @glade['paste1'].set_sensitive(false)
    @glade['properties1'].set_sensitive(false)
    @glade['preferences1'].set_sensitive(false)
  end

  def on_undo()
    @face.do_undo()
    update_undo()
  end

  def on_redo()
    @face.do_redo()
    update_undo()
  end
  
  def on_open1_activate(widget)
    dialog =  Gtk::FileChooserDialog.new("Gtk::FileChooser sample", nil,
                                         Gtk::FileChooser::ACTION_OPEN,
                                         "gnome-vfs",
                                         [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
                                         [Gtk::Stock::OPEN,   Gtk::Dialog::RESPONSE_ACCEPT]
                                         )
    dialog.run { |response|
      if response == Gtk::Dialog::RESPONSE_ACCEPT then
        @face.load(dialog.filename)
      end
      
      dialog.destroy
    }
  end

  def on_export_as_svg1_activate(widget)
    dialog =  Gtk::FileChooserDialog.new("FaceBuilder - Export face as SVG", nil,
                                         Gtk::FileChooser::ACTION_SAVE,
                                         "gnome-vfs",
                                         [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
                                         [Gtk::Stock::SAVE,   Gtk::Dialog::RESPONSE_ACCEPT]
                                         )
    dialog.run { |response|
      if response == Gtk::Dialog::RESPONSE_ACCEPT then
        @face.save_as_svg(dialog.filename)
      end
      
      dialog.destroy
    }       
  end

  def on_export_as_png1_activate(widget)
    dialog =  Gtk::FileChooserDialog.new("FaceBuilder - Export face as PNG", nil,
                                         Gtk::FileChooser::ACTION_SAVE,
                                         "gnome-vfs",
                                         [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
                                         [Gtk::Stock::SAVE,   Gtk::Dialog::RESPONSE_ACCEPT]
                                         )
    dialog.run { |response|
      if response == Gtk::Dialog::RESPONSE_ACCEPT then
        @face.save_as_png(dialog.filename)
      end
      
      dialog.destroy
    }    
  end

  def on_paste1_activate(widget)
    puts "on_paste1_activate() is not implemented yet."

    clipboard = @app.get_clipboard(Gdk::Selection::CLIPBOARD)

    clipboard.request_text{|clipboard, text| 
      puts "GOt: >>#{text}<<"
    }

    clipboard.request_contents(Gdk::Atom.intern("CLIPBOARD", false)) { |clipboard, selection_data| 
      puts selection_data.type, selection_data.text
    }

  end

  def on_save_as1_activate(widget)
    dialog =  Gtk::FileChooserDialog.new("Gtk::FileChooser sample", nil,
                                         Gtk::FileChooser::ACTION_SAVE,
                                         "gnome-vfs",
                                         [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
                                         [Gtk::Stock::SAVE,   Gtk::Dialog::RESPONSE_ACCEPT]
                                         )
    dialog.run { |response|
      if response == Gtk::Dialog::RESPONSE_ACCEPT then
        @face.save(dialog.filename)
      end
      
      dialog.destroy
    }
  end
  def on_about1_activate(widget)
    Gtk::AboutDialog.show(nil,
                          # "artists" => File.new("AUTHORS").readlines(),
                          "authors" => File.new("AUTHORS").readlines(),
                          "comments" => "A simple tool to construct faces",
                          "copyright" => "Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>",
                          # "documenters" => ["Documenter 1 <no1@foo.bar.com>", "Documenter 2 <no2@foo.bar.com>"],
                          "license" => File.new("COPYING").read(),
                          # "logo_icon_name" => "gtk-home",
                          "logo" => Gdk::Pixbuf.new("data/logo.png"),
                          "name" => "Face Builder",
                          "version" => "0.0.1",
                          "website" => "http://windstille.berlios.de/facebuilder/",
                          "website_label" => "Face Builder"
                          )
  end
  def on_preferences1_activate(widget)
    puts "on_preferences1_activate() is not implemented yet."
  end

  def on_copy1_activate(widget)
    puts "on_copy1_activate() is not implemented yet."
    clipboard = @canvas.get_clipboard(Gdk::Selection::CLIPBOARD)
    # puts clipboard.text
  end
  def on_new1_activate(widget)
    puts "on_new1_activate() is not implemented yet."
  end
  def on_clear1_activate(widget)
    puts "on_clear1_activate() is not implemented yet."
  end
  def on_cut1_activate(widget)
    puts "on_cut1_activate() is not implemented yet."
  end
  def on_save1_activate(widget)
    puts "on_save1_activate() is not implemented yet."
  end
  def on_properties1_activate(widget)
    puts "on_properties1_activate() is not implemented yet."
  end
  def on_show_controls1_activate(widget)
    if widget.active? then
      show_controls()
    else
      hide_controls()
    end
  end

  def on_quit1_activate(widget)
    Gtk.main_quit
  end

  def hide_controls()
    @canvas_controls.each {|item| item.hide() }
  end

  def show_controls()
    @canvas_controls.each {|item| item.show() }
  end

  def setup_controls(root, pos, facepart)
    left = Gtk::Button.new("<-")
    Gnome::CanvasWidget.new(root,
                            {:widget => left,
                              :x => pos.x,
                              :y => pos.y,
                              :width =>  32.0,
                              :height => 32.0,
                              :anchor => Gtk::ANCHOR_EAST,
                              :size_pixels => FALSE})
    left.show()
    left.signal_connect("clicked") {
      @face.get_part(facepart).previous_item()
      set_current_part(facepart)
    }

    right = Gtk::Button.new("->")
    Gnome::CanvasWidget.new(root,
                            {:widget => right,
                              :x => pos.x,
                              :y => pos.y,
                              :width =>  32.0,
                              :height => 32.0,
                              :anchor => Gtk::ANCHOR_WEST,
                              :size_pixels => FALSE})
    right.show()
    right.signal_connect("clicked") {
      @face.get_part(facepart).next_item()
      set_current_part(facepart)
    }
    @canvas_controls += [left, right]

    return [left, right]
  end

  def setup_faceparts()
    faceparts = @glade['FaceParts']
    @list = Gtk::ListStore.new(String, Gdk::Pixbuf)
    faceparts.model = @list
    faceparts.set_property('selection-mode', Gtk::SELECTION_BROWSE)

    faceparts.signal_connect("selection-changed") { |iconview|
      row = iconview.selected_items()[0]
      if row then # when does this get nil?
        filename = iconview.model.get_value(@list.get_iter(row), 0)
        # puts filename.inspect
        @face.get_part(@parts[@current_part]).filename = filename
        # puts "Selected something #{iconview.model.get_value(@list.get_iter(row), 1)}"
      end
    }

    faceparts.text_column   = -1
    faceparts.pixbuf_column =  1
  end

  def setup_partselector()
    partselector = @glade['PartSelector']

    #partselector.model = Gtk::ListStore.new(String)
    
    model = Gtk::ListStore.new(Gdk::Pixbuf, String)

    partselector.model = model
    renderer = Gtk::CellRendererPixbuf.new
    partselector.pack_start(renderer, false)
    partselector.set_attributes(renderer, :pixbuf => 0)
    
    renderer = Gtk::CellRendererText.new
    partselector.pack_start(renderer, true)
    partselector.set_attributes(renderer, :text => 1)

    partselector.signal_connect("changed") { |partselector|
      set_current_part(@parts[partselector.active])
    }

    @parts.each {|part|
      #puts part.to_s
      #iter = partselector.model.append()
      #iter[0] = "bla" # part.to_s
      # partselector.append_text part.to_s

      iter = model.append
      iter[0] = @glade['FaceBuilder'].render_icon(Gtk::Stock::OK, Gtk::IconSize::MENU, "icon")
      iter[1] = part.to_s
    }
  end

  def set_current_part(type)
    @parts.each_with_index() { |p, i|
      if type == p then
        @glade['PartSelector'].active = i

        if @current_part != i then
          @list.clear()

          # Add empty item
          iter = @list.append()
          iter[0] = nil
          iter[1] = Gdk::Pixbuf.new('data/empty.png')

          # Add other items
          Dir.new("data/#{type}/").grep(/\.png$/).each{|v|
            filename = "data/#{type}/#{v}"
            iter = @list.append()
            pixbuf = Gdk::Pixbuf.new(filename)

            # scale, while keeping aspect
            if pixbuf.width > 64 or pixbuf.height > 64 then # need scaling, since larger then 64x64
              aspect = pixbuf.width.to_f / pixbuf.height
              if pixbuf.width > pixbuf.height then
                pixbuf = pixbuf.scale(64, 64 / aspect)
              else
                pixbuf = pixbuf.scale(64 * aspect, 64)
              end
            end

            whitepixbuf = Gdk::Pixbuf.new(Gdk::Pixbuf::COLORSPACE_RGB, true, 8, 64, 64)
            whitepixbuf.fill!(0xf5f5f5ff)

            whitepixbuf.composite!(pixbuf, 
                                   (whitepixbuf.width - pixbuf.width)/2, (whitepixbuf.height - pixbuf.height)/2,
                                   pixbuf.width, pixbuf.height,
                                   (whitepixbuf.width - pixbuf.width)/2, (whitepixbuf.height - pixbuf.height)/2,
                                   1.0, 1.0,
                                   Gdk::Pixbuf::INTERP_BILINEAR,
                                   255)            

            # Add keep of aspect ratio
            iter[1] = whitepixbuf
            iter[0] = filename
          }

          @current_part = i
        end       
      end
    }   
  end

  def update_undo()
    @glade['toolbutton_undo'].set_sensitive(@face.has_undo_stack?)
    @glade['toolbutton_redo'].set_sensitive(@face.has_redo_stack?)
  end
end

# Main program
if __FILE__ == $0
  # Set values as your own application. 
  PROG_PATH = "facebuilder.glade"
  PROG_NAME = "FaceBuilder"
  PROG_VERSION = "0.1.0"
  Gnome::Program.new(PROG_NAME, PROG_VERSION)
  #If you use Ruby/GTK2 widgets only, call Gtk.init not Gnome::Program.new here.
  #Gtk.init
  $facebuilder = FacebuilderGlade.new(PROG_PATH, nil, PROG_NAME)
  Gtk.main
end

# EOF #
