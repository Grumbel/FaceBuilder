
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
  
  
  def next_item(n = 1)
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
    index += n
    index %= files.length

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
      if filename != @filename
        @canvas_items.each{ |item| item.show() }

        begin
          pixbuf = Gdk::Pixbuf.new(filename)
          @filename = filename    
          @canvas_items.each {|item|
            item.pixbuf = pixbuf
          }
        rescue GLib::FileError => err
          puts err
        end
      end
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

# EOF #
