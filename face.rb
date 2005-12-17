require 'gnomecanvas2'

class FacePart
  attr_reader :type, :offset, :filename, :canvas_items, :scale, :rotation
  
  def initialize(root, type, filename, offset)
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

    case @type
    when :eye, :ear, :eyebrow
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
  
  def toggle()
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
    puts "Setting: #{filename}"
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
  def initialize(root, filename = nil)
    @root = root
    @parts  = {
      :head    => FacePart.new(root, :head,    "data/head/0000.png",    Point.new(0, 0)),
      :eye     => FacePart.new(root, :eye,     "data/eye/0000.png",     Point.new(35, -10)),
      :eyebrow => FacePart.new(root, :eyebrow, "data/eyebrow/0000.png", Point.new(45, -30)),
      :glasses => FacePart.new(root, :glasses, "data/glasses/0000.png", Point.new(0, -10)),
      :ear     => FacePart.new(root, :ear,     "data/ear/0000.png",     Point.new(90, 0)),
      :nose    => FacePart.new(root, :nose,    "data/nose/0000.png",    Point.new(0, 30)),
      :mouth   => FacePart.new(root, :mouth,   "data/mouth/0000.png",   Point.new(0, 75)),
      :beard   => FacePart.new(root, :beard,   "data/beard/0000.png",   Point.new(0, 75)),
      :hair    => FacePart.new(root, :hair,    "data/hair/0000.png",    Point.new(0, -20))
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
    
  end
end

# EOF #
