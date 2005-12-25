require "rexml/document"
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

        @parent.undo_stack << FaceCommand.new(proc{ @parent.get_part(@type).offset = old_offset },
                                              proc{ @parent.get_part(@type).offset = offset     })
      end
    end
  end

  def initialize(parent, root, type, filename, offset)
    @parent = parent
    @type     = type
    @offset   = offset
    @filename = filename
    @scale    = 1.0
    @rotation = 0

    @canvas_items = []

    im = Gdk::Pixbuf.new(@filename)
    if ! im then 
      raise "#{@filename}: file not found: #{im.inspect}"
    end

    @canvas_items << Gnome::CanvasPixbuf.new(root,
                                             :pixbuf => im,
                                             :x => 0, # offset.x,
                                             :y => 0, # offset.y,
                                             :width =>  im.width,
                                             :height => im.height,
                                             :anchor => Gtk::ANCHOR_CENTER)

    @canvas_items.last.signal_connect("event") { |item, event|
      item_event(item, event)
    }

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
    $facebuilder.update_undo() if $facebuilder

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
    @filename = filename    
    pixbuf = Gdk::Pixbuf.new(@filename)
    @canvas_items.each {|item|
      item.pixbuf = pixbuf
    }
  end

  def offset=(offset)
    @offset = offset
    update_items()
  end

  def offset()
    return @offset
  end

  def save(out)
    out << "  <#{type}>\n"
    out << "    <filename>#{@filename}</filename>\n"
    out << "    <offset><x>#{@offset.x}</x><y>#{@offset.y}</y></offset>\n"
    out << "    <scale>#{@scale}</scale>\n"
    out << "    <rotation>#{@rotation}</rotation>\n"
    out << "  </#{type}>\n"
  end
end

class Face
  attr_accessor :undo_stack, :redo_stack, :parts

  def initialize(root, filename = nil)
    @undo_stack = []
    @redo_stack = []

    @root = root
    @parts  = {
      :head      => FacePart.new(self, root, :head,      "data/head/0000.png",      Point.new( 0,   0)),
      :forehead  => FacePart.new(self, root, :forehead,  "data/forehead/0000.png",  Point.new( 0, -75)),
      :eye       => FacePart.new(self, root, :eye,       "data/eye/0000.png",       Point.new(35, -10)),
      :eyebrow   => FacePart.new(self, root, :eyebrow,   "data/eyebrow/0000.png",   Point.new(45, -30)),
      :ear       => FacePart.new(self, root, :ear,       "data/ear/0000.png",       Point.new(90,   0)),
      :nose      => FacePart.new(self, root, :nose,      "data/nose/0000.png",      Point.new( 0,  30)),
      :mouth     => FacePart.new(self, root, :mouth,     "data/mouth/0000.png",     Point.new( 0,  75)),
      :mouthfold => FacePart.new(self, root, :mouthfold, "data/mouthfold/0000.png", Point.new(40,  75)),
      :beard     => FacePart.new(self, root, :beard,     "data/beard/0000.png",     Point.new( 0,  75)),
      :glasses   => FacePart.new(self, root, :glasses,   "data/glasses/0000.png",   Point.new( 0, -10)),
      :hair      => FacePart.new(self, root, :hair,      "data/hair/0000.png",      Point.new( 0, -20)),
      :hat       => FacePart.new(self, root, :hat,       "data/hat/0000.png",       Point.new( 0, -50))
    }
  end

  def get_part(type)
    return @parts[type]
  end 

  def reload()
    @parts.each{|key, val| val.reload() }
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
