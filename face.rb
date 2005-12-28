require "rexml/document"
require "base64.rb"
require 'gnomecanvas2'

class FaceCommand
  def initialize(undo_, redo_)
    @undo = undo_
    @redo = redo_    
  end

  def do_undo()
    @undo.call()
  end
  
  def do_redo()
    @redo.call()
  end
end

class FacePart
  attr_reader :type, :offset, :filename, :canvas_items, :scale, :rotation
  
  def item_event(item, event)
    case event.event_type
    when Gdk::Event::BUTTON_PRESS
      case event.button
      when 1
        # Record the click position in item-space
        @x, @y = item.parent.w2i(event.x, event.y)

        # Record the item position before the drag
        @old_offset = @offset.clone()

        fleur = Gdk::Cursor.new(Gdk::Cursor::FLEUR)
        item.grab(Gdk::Event::POINTER_MOTION_MASK | Gdk::Event::BUTTON_RELEASE_MASK,
                  fleur,
                  event.time)
        
        @dragging = true

        $facebuilder.set_current_part(@type)
      end

    when Gdk::Event::MOTION_NOTIFY
      item_x, item_y = item.parent.w2i(event.x, event.y)
      if @dragging && (event.state & Gdk::Window::BUTTON1_MASK == Gdk::Window::BUTTON1_MASK)
        # Calculate the new position

        if event.state & Gdk::Window::SHIFT_MASK == Gdk::Window::SHIFT_MASK
          @offset.x = @old_offset.x
        else
          @offset.x = @old_offset.x + item_x - @x
        end
        @offset.y = @old_offset.y + item_y - @y       

        update_items()
      end

    when Gdk::Event::BUTTON_RELEASE
      item.ungrab(event.time)
      @dragging = false;

      if @old_offset != @offset then
        old_offset = @old_offset.clone()
        offset     = @offset.clone()
        
        @parent.add_to_undo_stack(FaceCommand.new(proc{ @parent.get_part(@type).offset = old_offset },
                                                  proc{ @parent.get_part(@type).offset = offset     }))
        $facebuilder.update_undo() if $facebuilder
      end
    end
  end

  def initialize(parent, root, type, offset)
    @parent = parent
    @type     = type
    
    @offset   = offset
    @filename = nil
    @scale    = 1.0
    @rotation = 0

    @canvas_items = []

    im = Gdk::Pixbuf.new("data/empty.png")
    @canvas_items << Gnome::CanvasPixbuf.new(root,
                                             :pixbuf => im,
                                             :x => 0, # offset.x,
                                             :y => 0, # offset.y,
                                             :width =>  im.width,
                                             :height => im.height,
                                             :anchor => Gtk::ANCHOR_CENTER)

    case @type
    when :eye, :ear, :eyebrow, :mouthfold
      @canvas_items << Gnome::CanvasPixbuf.new(root,
                                               :pixbuf => im,
                                               :x => 0, # offset.x,
                                               :y => 0, # offset.y,
                                               :width =>  im.width,
                                               :height => im.height,
                                               :anchor => Gtk::ANCHOR_CENTER)
    end
    
    @canvas_items.each {|i|
      i.signal_connect("event") { |item, event|
        item_event(item, event)
      }
    }

    self.filename=(nil)
    update_items()
  end

  def reload()
    self.filename=(@filename)
  end
  
  def next_item()
    pathname = "data/" + type.to_s + "/"
    files = Dir.new(pathname).to_a[2..-1].grep(/\.png$/)
    
    file = File::basename(@filename)
    index = 0

    files.each_with_index { |obj, i|
      if obj == file then
        index = i
        break
      end
    }
    index += 1
    
    if index >= files.length then
      index = 0
    end

    self.filename=(pathname + files[index])
  end

  def previous_item()
    pathname = "data/" + type.to_s + "/"
    files = Dir.new(pathname).to_a[2..-1].grep(/\.png$/)
    
    file = File::basename(@filename)
    index = 0

    files.each_with_index { |obj, i|
      if obj == file then
        index = i
        break
      end
    }
    index -= 1
    
    if index < 0 then
      index = files.length - 1
    end

    self.filename=(pathname + files[index])
  end

  def update_items()
    if @canvas_items.length == 1 then
      @canvas_items[0].affine_absolute(Art::Affine.translate(@offset.x, @offset.y) *
                                       Art::Affine.rotate(@rotation) *
                                       Art::Affine.scale(@scale, @scale))
    elsif @canvas_items.length == 2 then
      @canvas_items[0].affine_absolute(Art::Affine.translate(@offset.x, @offset.y) *
                                       Art::Affine.rotate(@rotation) *
                                       Art::Affine.scale(@scale, @scale))
      @canvas_items[1].affine_absolute(Art::Affine.scale(-1.0, 1.0) * 
                                       Art::Affine.translate(@offset.x, @offset.y) *
                                       Art::Affine.rotate(@rotation) *
                                       Art::Affine.scale(@scale, @scale))
    else
      raise "Unhandled @canvas_items.length: #{@canvas_items.length}"
    end
  end

  def rotation=(rotation)
    @rotation = rotation
    update_items()
  end

  def scale=(scale)
    @scale = scale
    update_items()
  end

  def filename=(filename)
    if @parent.use_undo() and filename != @filename then
      old_filename = @filename.clone() if @filename
      new_filename = filename.clone()  if filename
      @parent.add_to_undo_stack(FaceCommand.new(proc{ @parent.without_undo {
                                                    @parent.get_part(@type).filename=(old_filename) }},
                                                proc{ @parent.without_undo {
                                                    @parent.get_part(@type).filename=(new_filename) }}))
      $facebuilder.update_undo() if $facebuilder
    end
    
    if filename and ! filename.empty? then
      @canvas_items.each{ |item| item.show() }
      
      @filename = filename    
      pixbuf = Gdk::Pixbuf.new(@filename)
      @canvas_items.each {|item|
        item.pixbuf = pixbuf
      }
    else
      @filename = nil
      @canvas_items.each{ |item| item.hide() }
    end
  end

  def offset=(offset)
    @offset = offset
    update_items()
  end

  def offset()
    return @offset
  end

  def save_svg(out)
    if @filename then
      pixbuf = Gdk::Pixbuf.new(@filename)

      out.write("\n\n<!-- #{@type.to_s} -->\n")
      out.write("<g transform=\"")
      out.write("translate(#{pixbuf.width/2},#{pixbuf.height/2}) ")
      out.write("translate(#{-pixbuf.width/2 + 256},#{-pixbuf.height/2 + 256}) ")
      out.write("translate(#{offset.x},#{offset.y}) ")
      out.write("rotate(#{@rotation}) ")
      out.write("scale(#{@scale}) ")
      out.write("translate(#{-pixbuf.width/2},#{-pixbuf.height/2}) ")

      out.write("\">")
      out.write("<image height=\"#{pixbuf.height}\" width=\"#{pixbuf.width}\" ")
      out.write("xlink:href=\"data:image/png;base64,#{Base64.encode64(File.new(@filename).read())}\" ")
      out.write("x=\"0\" y=\"0\" />")
      out.write("</g>\n")

      case @type
      when :eye, :ear, :eyebrow, :mouthfold
        out.write("<g transform=\"")
        out.write("translate(#{pixbuf.width/2},#{pixbuf.height/2}) ")
        out.write("scale(-1.0, 1.0) ")
        out.write("translate(#{pixbuf.width/2 - 256},#{-pixbuf.height/2 + 256}) ")
        out.write("translate(#{offset.x},#{offset.y}) ")
        out.write("rotate(#{@rotation}) ")
        out.write("scale(#{@scale}) ")
        out.write("translate(#{-pixbuf.width/2},#{-pixbuf.height/2}) ")

        out.write("\">")
        out.write("<image height=\"#{pixbuf.height}\" width=\"#{pixbuf.width}\" ")
        out.write("xlink:href=\"data:image/png;base64,#{Base64.encode64(File.new(@filename).read())}\" ")
        out.write("x=\"0\" y=\"0\" />")
        out.write("</g>\n")
      end
    end
  end

  def save(out)
    if @filename then
      out << "  <#{type}>\n"
      out << "    <filename>#{@filename}</filename>\n"
      out << "    <offset><x>#{@offset.x}</x><y>#{@offset.y}</y></offset>\n"
      out << "    <scale>#{@scale}</scale>\n"
      out << "    <rotation>#{@rotation}</rotation>\n"
      out << "  </#{type}>\n"
    end
  end
end

class Face
  attr_accessor :redo_stack, :parts
  attr_reader   :use_undo, :undo_stack

  def initialize(root, filename = nil)
    @undo_stack = []
    @redo_stack = []
    @use_undo   = true

    @root = root
    @parts  = {
      :head      => FacePart.new(self, root, :head,      Point.new( 0,   0)),
      :ear       => FacePart.new(self, root, :ear,       Point.new(90,   0)),
      :forehead  => FacePart.new(self, root, :forehead,  Point.new( 0, -75)),
      :beard     => FacePart.new(self, root, :beard,     Point.new( 0,  75)),
      :eye       => FacePart.new(self, root, :eye,       Point.new(35, -10)),
      :eyebrow   => FacePart.new(self, root, :eyebrow,   Point.new(45, -30)),
      :nose      => FacePart.new(self, root, :nose,      Point.new( 0,  30)),
      :mouth     => FacePart.new(self, root, :mouth,     Point.new( 0,  75)),
      :mouthfold => FacePart.new(self, root, :mouthfold, Point.new(40,  75)),
      :glasses   => FacePart.new(self, root, :glasses,   Point.new( 0, -10)),
      :hair      => FacePart.new(self, root, :hair,      Point.new( 0, -20)),
      :hat       => FacePart.new(self, root, :hat,       Point.new( 0, -50))
    }
  end

  def without_undo()
    disable_undo()
    yield()
    enable_undo()
  end

  def disable_undo()
    @use_undo = false
  end

  def enable_undo()
    @use_undo = true
  end

  def get_part(type)
    return @parts[type]
  end 

  def reload()
    @parts.each{|key, val| val.reload() }
  end

  def save_as_png(filename)
    # FIXME: Only primitive export, only copies from framebuffer and doesn't work with overlap
    puts $facebuilder.canvas.window
    window = $facebuilder.canvas.window
    pixbuf = Gdk::Pixbuf.from_drawable(window.colormap, window, 0, 0, 512, 512)
    pixbuf.save(filename,"png")
  end

  def save_as_svg(filename)
    f = File.new(filename, "w")
    f.write <<EOF
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!-- Created with Inkscape (http://www.inkscape.org/) -->
<svg
   xmlns="http://www.w3.org/2000/svg" version="1.1"
   xmlns:xlink="http://www.w3.org/1999/xlink"
   width="512"
   height="512">
EOF
    
    @parts.each{ |key, val| 
      val.save_svg(f)
    }
    f.write "</svg>"
    f.close()    
  end

  def save(filename)
    out = File.new(filename, "w")
    out << "<face>\n"
    @parts.each{|key, val| val.save(out)}
    out << "</face>\n"
    out.close()
  end
  
  def load(filename)
    file = File.new( filename )
    doc = REXML::Document.new(file)
    @parts.each { |key, part|
      part.rotation = 0.0
      part.scale    = 1.0
      part.filename = nil
      part.offset   = Point.new(0, 0)
    }

    doc.elements.each("face/*") { |element|
      part = element.name.to_sym
      @parts[part].rotation = element.elements['rotation'].text.to_f
      @parts[part].scale    = element.elements['scale'].text.to_f
      @parts[part].filename = element.elements['filename'].text
      @parts[part].offset   = Point.new(element.elements['offset/x/'].text.to_f, 
                                        element.elements['offset/y'].text.to_f)
    }
  end

  def do_undo()
    if ! @undo_stack.empty? then
      cmd = @undo_stack.pop
      cmd.do_undo()
      @redo_stack << cmd
    end
  end

  def do_redo()
    if ! @redo_stack.empty? then
      cmd = @redo_stack.pop
      cmd.do_redo()
      @undo_stack << cmd
    end    
  end

  def add_to_undo_stack(item)
    @redo_stack.clear()
    @undo_stack << item
  end

  def has_undo_stack?()
    return ! @undo_stack.empty?
  end

  def has_redo_stack?()
    return ! @redo_stack.empty?
  end

  def on_change()
    
  end
end

# EOF #
