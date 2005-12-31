require "rexml/document"
require "base64.rb"
require 'gnomecanvas2'
require 'face_part.rb'

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

  def center()
    offset = @parts[:head].offset
    @parts.each{|key, val| 
      val.offset = Point.new(val.offset.x - offset.x, val.offset.y - offset.y)
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
    pixbuf.save(filename, "png")
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
