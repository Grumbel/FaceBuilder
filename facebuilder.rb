#!/usr/bin/env ruby

require "gnomecanvas2"
require "gtk2"
require "face.rb"

Point = Struct.new("Point", :x, :y)
class Point
  def to_p
    [x,y]
  end
  def +(p)
    [x+p.x, y+p.y]
  end
  def -(p)
    [x-p.x, y-p.y]
  end
end

class FaceBuilder < Gtk::VBox
  def add_image(root, filename, x, y, anchor, affine = nil)
    im = Gdk::Pixbuf.new(filename)

    unless im.nil?
      image = Gnome::CanvasPixbuf.new(root,
                                      :pixbuf => im,
                                      :x => x,
                                      :y => y,
                                      :width => im.width,
                                      :height => im.height,
                                      :anchor => anchor)

      image.signal_connect("event") { |item,event|
        puts "Bla: ", item, event
      }
      # puts image.i2w_affine
      # puts image.i2c_affine

      if affine then
        image.affine_absolute(affine)
      end

      image.signal_connect("destroy", im) { |item, im| }
      return image
    end
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
    return [left, right]
  end

  def setup_menu()
    accel_group = Gtk::AccelGroup.new
    item_factory = Gtk::ItemFactory.new(Gtk::ItemFactory::TYPE_MENU_BAR,
                                        '<main>', accel_group)
    
    @window.add_accel_group(accel_group)
    @window.set_border_width(0)

    ifactory_cb = proc do |data, widget|
      puts "ItemFactory: activated \"#{Gtk::ItemFactory.path_from_widget(widget)}\""
    end
    
    menu_items = 
      [
       ['/_File'],
#       ['/File/tearoff1', '<Tearoff>', nil, nil, ifactory_cb],
       ['/File/_Open',
        '<Item>', '<control>O', nil, method(:open_file)],
       ['/File/Save _As...',
        '<Item>', '<control>S', nil, method(:save_file)],
       ['/File/Quit',
        '<Item>', '<control>Q', nil, method(:quit)],
       ['/Help/About',
        '<Item>', nil, nil, method(:about_dialog)]
      ]
    item_factory.create_items(menu_items)
    
    return item_factory.get_widget('<main>')
  end

  def about_dialog(data, widget)
#     Gtk::AboutDialog.set_email_hook {|about, link|
#       p "email_hook"
#       p link
#     }
#     Gtk::AboutDialog.set_url_hook {|about, link|
#       p "url_hook"
#       p link
#     }

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
  
  def quit(data, widget)
    Gtk.main_quit
  end

  def open_file(data, widget)
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

  def save_file(data, widget)
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

  def set_current_part(type)
    @parts.each_with_index() { |p, i|
      if type == p then
        if @current_part != i
          @treeview.columns[0].title = type.to_s.capitalize
                                                          
          @list.clear()
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

            # Add keep of aspect ratio
            iter[0] = pixbuf
            iter[1] = filename
          }

          @current_part = i
        end       
      end
    }

    
  end

  def initialize(window)
    super()

    @window = window
    
    @box = Gtk::EventBox.new
    @menu = setup_menu()
    pack_start(@menu, false, false, 0)
    @hbox = Gtk::HBox.new(false, 5)
    pack_start(@hbox)
    @hbox.pack_start(@box)

    @label = Gtk::Label.new
    @label.show
    pack_end(@label,false,false,0)
    set_border_width(@pad = 2)
    set_size_request((@width = 48)+(@pad*2), (@height = 48)+(@pad*2))
    @canvas = Gnome::Canvas.new(true)
    # @box.border_width = 5
    @box.add(@canvas)
    
    @list = Gtk::ListStore.new(Gdk::Pixbuf, String)
    
    @scrolled_win = Gtk::ScrolledWindow.new
    @scrolled_win.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC)
    # @scrolled_win.width = 64

    @treeview = Gtk::TreeView.new(@list)
    @treeview.headers_visible = false
    @treeview.append_column(Gtk::TreeViewColumn.new("Stuff",
                                                    Gtk::CellRendererPixbuf.new, 
                                                    {:pixbuf => 0}))
    @treeview.selection.set_mode(Gtk::SELECTION_SINGLE)
    # scrolled_win.add_with_viewport(treeview)
    
    @treeview.signal_connect("cursor-changed") { |treeview|
      row, column = treeview.cursor()
      puts @list.methods()

      puts "Selected something #{@list.get_value(@list.get_iter(row), 1)}"
      filename = @list.get_value(@list.get_iter(row), 1)
      @face.get_part(@parts[@current_part]).filename = filename
    }
    
    @scrolled_win.set_size_request(96,64)
    @hbox.pack_end(@scrolled_win, false, false, 0)
    @scrolled_win.add_with_viewport(@treeview)

    @canvas.signal_connect("button-press-event") { |item,event|
      # Some throuble with alpha
      # puts "Bla: ", @canvas.get_item_at(event.x, event.y)
    }

    # puts
    @canvas.modify_bg(Gtk::STATE_NORMAL, Gdk::Color.new(65535, 65535, 65535))

    @canvas.root.affine_absolute(Art::Affine.translate(192, 192))
    @face = Face.new(@canvas.root)
    
    setup_controls(@canvas.root, Point.new(0, -200),  :hat)
    setup_controls(@canvas.root, Point.new(-200, -140),   :forehead)
    setup_controls(@canvas.root, Point.new(0, 200),   :head)
    setup_controls(@canvas.root, Point.new(200, -140), :hair)

    setup_controls(@canvas.root, Point.new(-200, 0), :glasses)

    setup_controls(@canvas.root, Point.new(200, -40), :eyebrow)
    setup_controls(@canvas.root, Point.new(200, 0),   :eye)
    setup_controls(@canvas.root, Point.new(200, 40),  :nose)
    setup_controls(@canvas.root, Point.new(200, 80),  :mouth)
    setup_controls(@canvas.root, Point.new(-200, 80),  :mouthfold)

    Gnome::CanvasText.new(@canvas.root,
                          {:text => 
                            "PgUp, PgDown:\tscale\n" +
                            "Home, End:\t\trotate\n" +
                            "Cursorkeys:\t\tmove\n",
                            :x => -180,
                            :y => 180,
                            :font => "Sans 10",
                            :anchor => Gtk::ANCHOR_N,
                            :fill_color => "black"})

    @parts = [:eye, :eyebrow, :glasses, :ear, :mouth, :mouthfold, :beard, :nose, :head, :forehead, :hair, :hat]
    @current_part = 0

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
    
    #     @box.signal_connect('size-allocate') { |w,e,*b| 
    #       @width, @height = [e.width,e.height].collect{|i|i - (@pad*2)}
    #       @size = [@width,@height].min
    #       @radius = @size / 2
    #       @canvas.set_size(@width,@height)
    #       @canvas.set_scroll_region(0,0,@width,@height)
    #       false
    #     }

    @canvas.set_size(384, 384)
    @canvas.set_scroll_region(0,0,384,384)

    signal_connect_after('show') {|w,e| }
    signal_connect_after('hide') {|w,e| }

    @canvas.show()
    @scrolled_win.show()
    @treeview.show()
    @menu.show()
    @box.show()
    @hbox.show()
    show()
  end
end


if $0 == __FILE__

  class Viewer < Gtk::Window
    def initialize()
      super()
      set_title("Face Builder")
      signal_connect("delete_event") { |i,a| Gtk::main_quit }
      $facebuilder = FaceBuilder.new(self)
      add($facebuilder)
      set_default_size(640, 512)
      # set_resizable(false)
      show()
    end
  end

  Gtk.init()

  view = Viewer.new
  view.show

  Gtk.main()
  
end

# EOF #
