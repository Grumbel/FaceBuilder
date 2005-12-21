#!/usr/bin/env ruby

require "gnomecanvas2"
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
       ['/File/tearoff1', '<Tearoff>', nil, nil, ifactory_cb],
       ['/File/_New',
        '<Item>', '<control>N', nil, ifactory_cb],
       ['/File/_Open',
        '<Item>', '<control>O', nil, method(:open_file)],
       ['/File/_Save',
        '<Item>', '<control>S', nil, method(:save_file)],
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
    Gtk::AboutDialog.set_email_hook {|about, link|
      p "email_hook"
      p link
    }
    Gtk::AboutDialog.set_url_hook {|about, link|
      p "url_hook"
      p link
    }

    Gtk::AboutDialog.show(nil,
                          "artists" => ["Ingo Ruhnke <grumbel@gmx.de>"],
                          "authors" => ["Ingo Ruhnke <grumbel@gmx.de>"],
                          "comments" => "A simple tool to construct faces",
                          "copyright" => "Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>",
                          # "documenters" => ["Documenter 1 <no1@foo.bar.com>", "Documenter 2 <no2@foo.bar.com>"],
                          "license" => "This program is licenced under the GNU GPL.",
                          "logo_icon_name" => "gtk-home",
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
        @current_part = i
      end
    }
  end

  def initialize(window)
    super()

    @window = window

    @box = Gtk::EventBox.new
    @menu = setup_menu()
    pack_start(@menu, false, false, 0)
    pack_start(@box)

    @label = Gtk::Label.new
    @label.show
    pack_end(@label,false,false,0)
    set_border_width(@pad = 2)
    set_size_request((@width = 48)+(@pad*2), (@height = 48)+(@pad*2))
    @canvas = Gnome::Canvas.new(true)
    @box.add(@canvas)
    
    @canvas.signal_connect("button-press-event") { |item,event|
      # Some throuble with alpha
      # puts "Bla: ", @canvas.get_item_at(event.x, event.y)
    }

    puts @canvas.modify_bg(Gtk::STATE_NORMAL, Gdk::Color.new(65535, 65535, 65535))

    @canvas.root.affine_absolute(Art::Affine.translate(192, 192))
    @face = Face.new(@canvas.root)
    
    setup_controls(@canvas.root, Point.new(0, -200),  :hat)
    setup_controls(@canvas.root, Point.new(0, 200),   :head)
    setup_controls(@canvas.root, Point.new(200, -140), :hair)

    setup_controls(@canvas.root, Point.new(200, -40), :eyebrow)
    setup_controls(@canvas.root, Point.new(200, 0),   :eye)
    setup_controls(@canvas.root, Point.new(200, 40),  :nose)
    setup_controls(@canvas.root, Point.new(200, 80),  :mouth)

    @parts = [:eye, :eyebrow, :glasses, :ear, :mouth, :beard, :nose, :head, :hair, :hat]
    @current_part = 0

    @canvas.signal_connect("key-press-event") { |widget, event|
      part = @face.get_part(@parts[@current_part])
      case event.keyval
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

      when Gdk::Keyval::GDK_p
        part.scale=(part.scale / 1.02) if part

      when Gdk::Keyval::GDK_u
        part.scale=(part.scale * 1.02) if part

      when Gdk::Keyval::GDK_y
        part.rotation=(part.rotation - 1.0) if part

      when Gdk::Keyval::GDK_i
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
    @menu.show()
    @box.show()
    show()
  end
end


if $0 == __FILE__

  class Viewer < Gtk::Window
    def initialize()
      super()
      set_title("Face Builder")
      signal_connect("delete_event") { |i,a| Gtk::main_quit }
      add(FaceBuilder.new(self))
      set_default_size(512, 512)
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
